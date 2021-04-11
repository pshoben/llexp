
add_executable ( harness_prot harness_prot.cpp )
target_compile_options( harness_prot PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( harness_prot PRIVATE pthread )

add_executable ( server server.cpp )
target_compile_options( server PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( server PRIVATE pthread ${Boost_LIBRARIES} rt)

add_executable ( client client.cpp )
target_compile_options( client PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( client PRIVATE pthread ${Boost_LIBRARIES} rt)


add_executable ( boost_ipc_vector boost_ipc_vector.cpp )
target_compile_options( boost_ipc_vector PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( boost_ipc_vector PRIVATE pthread ${Boost_LIBRARIES} rt)

add_executable ( scoped_alloc scoped_alloc.cpp )
target_compile_options( scoped_alloc PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( scoped_alloc PRIVATE pthread ${Boost_LIBRARIES} rt)

add_executable ( lldb lldb.cpp )
target_compile_options( lldb PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( lldb PRIVATE pthread ${Boost_LIBRARIES} rt)

add_executable ( pshm_ucase_bounce pshm_ucase_bounce.c )
target_compile_options( pshm_ucase_bounce PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( pshm_ucase_bounce PRIVATE pthread ${Boost_LIBRARIES} rt)

add_executable ( pshm_ucase_send pshm_ucase_send.c )
target_compile_options( pshm_ucase_send PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( pshm_ucase_send PRIVATE pthread ${Boost_LIBRARIES} rt)

#add_executable ( shm_col_test shm_col_test.cpp )
#target_compile_options( shm_col_test PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
#target_link_libraries( shm_col_test PRIVATE pthread ${Boost_LIBRARIES} rt)

add_executable ( create create.cpp )
target_compile_options( create PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( create PRIVATE pthread ${Boost_LIBRARIES} rt)

add_executable ( create_col create_col.cpp )
target_compile_options( create_col PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( create_col PRIVATE pthread ${Boost_LIBRARIES} rt)


add_executable ( destroy destroy.cpp )
target_compile_options( destroy PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( destroy PRIVATE pthread ${Boost_LIBRARIES} rt)

add_executable ( set set.cpp )
target_compile_options( set PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( set PRIVATE pthread ${Boost_LIBRARIES} rt)

add_executable ( get get.cpp )
target_compile_options( get PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( get PRIVATE pthread ${Boost_LIBRARIES} rt)

add_executable ( fill fill.cpp )
target_compile_options( fill PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( fill PRIVATE pthread ${Boost_LIBRARIES} rt)

add_executable ( fill_col fill_col.cpp )
target_compile_options( fill_col PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( fill_col PRIVATE pthread ${Boost_LIBRARIES} rt)


add_executable ( consume consume.cpp )
target_compile_options( consume PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( consume PRIVATE pthread ${Boost_LIBRARIES} rt)




