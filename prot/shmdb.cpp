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

    // create new 
    managed_shared_memory segment(create_only,"SharedMemoryName",65536);

    //Note that map<Key, MappedType>'s value_type is std::pair<const Key, MappedType>,
    //so the allocator must allocate that pair.
    typedef int    KeyType;
    typedef float  MappedType;
    typedef std::pair<const int, float> ValueType;

    //allocator of for the map.
    typedef allocator<ValueType, managed_shared_memory::segment_manager> ShmemAllocator;

    //third parameter argument is the ordering function is used to compare the keys.
    typedef map<KeyType, MappedType, std::less<KeyType>, ShmemAllocator> MySHMMap;

    //Initialize the shared memory STL-compatible allocator
    ShmemAllocator alloc_inst (segment.get_segment_manager());

    // offset ptr within SHM for map 
    offset_ptr<MySHMMap> m_pmap = segment.construct<MySHMMap>("MySHMMapName")(std::less<int>(), alloc_inst);

    //Insert data in the map
    for(int i = 0; i < 10; ++i)
    {
            m_pmap->insert(std::pair<const int, float>(i, (float)i));
    }

    return 0;
}
