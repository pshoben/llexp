
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <locale.h>
#include <iostream>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h> // conflict msr
#include <limits.h> // conflict msr
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include <string>
using std::string;

#define HOST    "127.0.0.1"
#define LISTEN_PORT   6565
#define RELAY_PORT    7575

#define MAX_CONN        16
#define MAX_EVENTS      32
#define BUF_SIZE        16
#define MAX_LINE        256
#define MAX_FILE_SIZE 1024 * 1024 * 1024 // 1GB

static bool verbose = false;
static bool relay = false;  // if true, relay mode else echo mode
static uint16_t listen_port = LISTEN_PORT;
static uint16_t relay_to_port = RELAY_PORT;

char relay_to_host[HOST_NAME_MAX+1]=HOST; // TODO nyi
char out_prefix[PATH_MAX+1]="tcp_relay_out";
bool dump_bytes = false;
uint64_t max_file_size = MAX_FILE_SIZE;

static FILE * fp_fwd = nullptr ;
static FILE * fp_rev = nullptr ;

static int conn_sock_fd = 0;
static int relay_sock_fd = 0 ;

#ifndef __always_inline
# define __always_inline	inline __attribute__((always_inline))
#endif

inline uint32_t rdtsc32() {
    uint32_t low, high;
    __asm__ volatile (
    "rdtsc" /*  "lfence\n\trdtsc\n\tlfence" */
    : "=a"(low), "=d"(high)
    : "a"(0)
    : "%ecx");
    return low;
}

/*
 * register events of fd to epfd
 */
static void epoll_ctl_add(int epfd, int fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        perror("epoll_ctl()\n");
        exit(1);
    }
}

static void set_sockaddr(struct sockaddr_in *addr)
{
    bzero((char *)addr, sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(listen_port);
    printf("set_sockaddr port %hu\n",listen_port); fflush(stdout);

}

bool set_blocking(int fd, bool blocking)
{
    if (fd < 0) return false;

#ifdef _WIN32
    unsigned long mode = blocking ? 0 : 1;
   return (ioctlsocket(fd, FIONBIO, &mode) == 0) ? true : false;
#else
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return false;
    flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
#endif
}

static void set_relaysockaddr(struct sockaddr_in *addr)
{
    bzero((char *)addr, sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(relay_to_port);
}


void connect_fwd()
{
    struct sockaddr_in relay_addr;
    relay_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    set_relaysockaddr(&relay_addr);

    if (connect(relay_sock_fd, (struct sockaddr *)&relay_addr, sizeof(relay_addr)) < 0) {
        perror("connect() to relay target");
        exit(1);
    }
    set_blocking(relay_sock_fd, false);
}

/*
 * epoll echo server
 */
void start_svr() {
    struct timespec ts_timestamp;
    int i;
    int n;
    int epfd;
    int nfds;
    int listen_sock;
    socklen_t socklen;
    char read_buf[BUF_SIZE];
    //char write_buf[BUF_SIZE];

    struct sockaddr_in srv_addr;
    struct sockaddr_in cli_addr;
    struct epoll_event events[MAX_EVENTS];

    listen_sock = socket(AF_INET, SOCK_STREAM, 0);

    set_sockaddr(&srv_addr);
    bind(listen_sock, (struct sockaddr *) &srv_addr, sizeof(srv_addr));

    set_blocking(listen_sock, false);

    listen(listen_sock, MAX_CONN);

    epfd = epoll_create(1);

    epoll_ctl_add(epfd, listen_sock, EPOLLIN | EPOLLOUT | EPOLLET);
    if (relay && relay_sock_fd != 0) {
        epoll_ctl_add(epfd, relay_sock_fd, EPOLLIN | EPOLLOUT | EPOLLET);
    }

    socklen = sizeof(cli_addr);
    uint64_t raw_offset = 0;

    for (;;) {
        //printf("[+] before epoll_wait\n");
        nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        clock_gettime(CLOCK_MONOTONIC, &ts_timestamp);

        if (verbose) {
            printf("at timestamp [%ld.%ld]:\n", ts_timestamp.tv_sec, ts_timestamp.tv_nsec);
        }

        for (i = 0; i < nfds; i++) {
            if (events[i].data.fd == listen_sock) {
                /* handle new connection */
                conn_sock_fd =
                        accept(listen_sock,
                               (struct sockaddr *) &cli_addr,
                               &socklen);

                inet_ntop(AF_INET, (char *) &(cli_addr.sin_addr),
                          read_buf, sizeof(cli_addr));
                printf("[+] connected with %s:%d\n", read_buf,
                       ntohs(cli_addr.sin_port));
                set_blocking(conn_sock_fd, false);
                epoll_ctl_add(epfd, conn_sock_fd,
                              EPOLLPRI | EPOLLIN | EPOLLET | EPOLLRDHUP |
                              EPOLLHUP);

            } else if (events[i].events & EPOLLIN) {
                /* handle EPOLLIN event */
                int read_from_fd = events[i].data.fd;
                for (;;) {
                    bzero(read_buf, sizeof(read_buf));
                    //printf("[+] before read fd %d\n", read_from_fd);
                    int remaining = read(read_from_fd, read_buf, sizeof(read_buf));
                    //printf("[+] read returned : %d [%.*s]\n", remaining, remaining, read_buf);

                    if (remaining <= 0 /* || errno == EAGAIN */ ) {
                        break;
                    } else {
                        char *p = read_buf;
                        FILE *dump_fp = 0;
                        int write_to_fd = 0;
                        // print raw bytes
                        if (verbose) {
                            char tmpstr[8192];
                            memset(tmpstr, 0, sizeof(tmpstr));
                            for (int j = 0; j < remaining; j++) {
                                char c = ((char *) p)[j];
                                tmpstr[j] = (c < '0') ? '*' : c;
                            }
                            printf("%s : %s %04d BYTES : \"%.*s\"\n", __FUNCTION__,
                                   (events[i].data.fd == relay_sock_fd) ? "REV" : "FWD",
                                   remaining, remaining, tmpstr);
                        }
                        if (events[i].data.fd == relay_sock_fd) {
                            // data received from remote, relay it back to user
                            write_to_fd = conn_sock_fd;
                            dump_fp = fp_rev;
                            if (verbose)
                                printf("[+] response: [%s] %lu %d\n", read_buf, strlen(read_buf), remaining);
                        } else {
                            write_to_fd = relay_sock_fd;
                            dump_fp = fp_fwd;
                            if (verbose)
                                printf("[+] relaying: [%s] %lu %d\n", read_buf, strlen(read_buf), remaining);
                        }
                        // dump to file
                        if (dump_bytes && raw_offset < max_file_size) {
                            fwrite(p, remaining, 1, dump_fp);
                            raw_offset += remaining;
                        }
                        // write to socket
                        while (remaining > 0) {
                            //printf("[+] before write [%.*s] fd %d (remaining = %d)\n", remaining,  p, write_to_fd, remaining);
                            n = write(write_to_fd, p, remaining);
                            //printf("[+] after write fd %d (wrote %d)\n", write_to_fd, n);
                            if (n > 0) {
                                p += n;
                                remaining -= n;
                            }
                        }

                    }
                }

            } else {
                printf("[+] unexpected\n");
            }
            /* check if the connection is closing */
            if (events[i].events & (EPOLLRDHUP | EPOLLHUP)) {
                printf("[+] connection closed\n");
                epoll_ctl(epfd, EPOLL_CTL_DEL,
                          events[i].data.fd, NULL);
                close(events[i].data.fd);
                continue;
            }
        }
    }
    if( dump_bytes ) {
        fclose( fp_fwd );
        fclose( fp_rev );
    }
}

void sihandler( int s )
{
    printf("caught signal %d\n", s);
    // calls fclose and flushes
    exit(0);
}

int main(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "vo:m:h:p:l:")) != -1) {
        switch (opt) {
            case 'v':
                verbose = true;
                printf("verbose mode (-v) active\n");
                break;
            case 'o':
            {
                memset( out_prefix, 0, sizeof( out_prefix ));
                strncpy( out_prefix, optarg, sizeof( out_prefix )-1);
                dump_bytes = true;

                string prefix = out_prefix;
                fp_rev = fopen( string( prefix + string("_rev.raw")).c_str(), "w");
                fp_fwd = fopen( string( prefix + string("_fwd.raw")).c_str(), "w");
                if( !fp_rev || !fp_fwd ) {
                    printf("failed to open dump files with prefix \"%s\"  for write\n", out_prefix);
                    exit( 1 );
                }
                printf("dump to files active : using output file with prefix \"%s\"\n", out_prefix);
                break;
            }
            case 'm':
                max_file_size = (uint64_t) atoi(optarg);
                printf("using max output file size = %lu bytes (%fGB)\n", max_file_size, ((double)max_file_size)/1048576.0);
                break;
            case 'h':
                memset(relay_to_host,0,sizeof(relay_to_host));
                strncpy(relay_to_host,optarg, HOST_NAME_MAX);
                break;
            case 'p':
                relay_to_port = atoi(optarg);
                break;
            case 'l':
                listen_port = atoi(optarg);
                break;
            default:
                printf("usage: %s [-v (verbose)] [-o output-file-prefix] [-m max-output-file-size (in bytes)] [-h relay-to-host] [-p relay-to-port] [-l listen-port]\n", argv[0]);
                exit(1);
        }
    }

    printf( "relay server listening on port %hu\n", listen_port);
    connect_fwd();
    start_svr();
    return 0;
}

