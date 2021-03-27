#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <functional>
#include <utility>

int main ()
{
    using namespace boost::interprocess;

    // remove earlier existing SHM
    shared_memory_object::remove("SharedMemoryName");

    return 0;
}

