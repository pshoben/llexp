
add_executable ( harness_prot harness_prot.cpp )
target_compile_options( harness_prot PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( harness_prot PRIVATE pthread )

add_executable ( server server.cpp )
target_compile_options( server PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( server PRIVATE pthread ${Boost_LIBRARIES} rt)

add_executable ( client client.cpp )
target_compile_options( client PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( client PRIVATE pthread ${Boost_LIBRARIES} rt)

add_executable ( destroy destroy.cpp )
target_compile_options( destroy PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( destroy PRIVATE pthread ${Boost_LIBRARIES} rt)

add_executable ( boost_ipc_vector boost_ipc_vector.cpp )
target_compile_options( boost_ipc_vector PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( boost_ipc_vector PRIVATE pthread ${Boost_LIBRARIES} rt)

add_executable ( scoped_alloc scoped_alloc.cpp )
target_compile_options( scoped_alloc PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( scoped_alloc PRIVATE pthread ${Boost_LIBRARIES} rt)

add_executable ( lldb lldb.cpp )
target_compile_options( lldb PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps )
target_link_libraries( lldb PRIVATE pthread ${Boost_LIBRARIES} rt)


