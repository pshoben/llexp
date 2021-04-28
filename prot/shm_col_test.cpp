#include "shm_col.h"

using namespace lldb;

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int
main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s /shm-path\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *shmpath = argv[1];

//    /* Create shared memory object and set its size to the size
//       of our structure. */
//printf("shm_open\n");
//    int fd = shm_open(shmpath, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
//    if (fd == -1)
//        errExit("shm_open");
//
//printf("ftruncate shm segment to size %u bytes\n", SHM_SEGMENT_SIZE);
//    if (ftruncate(fd, SHM_SEGMENT_SIZE) == -1)
//        errExit("ftruncate");
//
//    /* Map the object into the caller's address space. */
//printf("shm_mmap\n");
//    Chunk * shmp = (Chunk *) mmap(NULL, sizeof(*shmp),
//                               PROT_READ | PROT_WRITE,
//                               MAP_SHARED, fd, 0);
//    if (shmp == MAP_FAILED)
//        errExit("mmap");
//
//printf("shm_unlink\n");
//    shm_unlink( shmpath);
//
printf("SHM_SEGMENT_SIZE = %u\n", SHM_SEGMENT_SIZE );

printf("sizeof(ValueAddress) = %lu\n",sizeof(ValueAddress));
printf("sizeof(RegionAddress) = %lu\n",sizeof(RegionAddress));
printf("sizeof(Chunk) = %lu\n",sizeof(Chunk));
printf("sizeof(ChunkLocation) = %lu\n",sizeof(ChunkLocation));
printf("sizeof(Chunk_KV) = %lu\n",sizeof(Chunk_KV));
printf("sizeof(ControlBlock) = %lu\n",sizeof(ControlBlock));
printf("sizeof(Column) = %lu\n",sizeof(Column));
printf("sizeof(Region) = %lu\n",sizeof(Region));

printf("sizeof(Block<char>) = %lu\n",sizeof(Block<char>));
printf("sizeof(Block<short>) = %lu\n",sizeof(Block<short>));
printf("sizeof(Block<int>) = %lu\n",sizeof(Block<int>));
printf("sizeof(Block<long>) = %lu\n",sizeof(Block<long>));
printf("sizeof(Block<int128_t>) = %lu (%lu)\n",sizeof(Block<int128_t>), sizeof(int128_t));

printf("sizeof(Block<time_t>) = %lu (%lu)\n",sizeof(Block<time_t>), sizeof(time_t));
printf("sizeof(Block<TimePair>) = %lu (%lu)\n",sizeof(Block<TimePair>), sizeof(TimePair));
printf("sizeof(Block<TimedSample1>) = %lu (%lu)\n",sizeof(Block<TimedSample1>), sizeof(TimedSample1));
printf("sizeof(Block<TimedSample2>) = %lu (%lu)\n",sizeof(Block<TimedSample2>), sizeof(TimedSample2));

    g_shm = Shm( shmpath );

    exit( EXIT_SUCCESS); 
}


