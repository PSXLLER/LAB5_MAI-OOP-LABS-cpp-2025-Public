#include "custom_memory_resource.hpp"
#include <new>
#include <algorithm>

void* VectorTrackingMemoryResource::do_allocate(std::size_t bytes, std::size_t alignment) 
{
    void* p = ::operator new(bytes, std::align_val_t(alignment));
    allocated_.push_back(p);
    return p;
}

void VectorTrackingMemoryResource::do_deallocate(void* p, std::size_t bytes, std::size_t alignment) 
{
    ::operator delete(p, bytes, std::align_val_t(alignment));
    auto it = std::find(allocated_.begin(), allocated_.end(), p);
    if (it != allocated_.end())
        allocated_.erase(it);
}

bool VectorTrackingMemoryResource::do_is_equal(const std::pmr::memory_resource& other) const noexcept 
{
    return this == &other;
}

VectorTrackingMemoryResource::~VectorTrackingMemoryResource() 
{
    for (void* p : allocated_)
        ::operator delete(p);
    allocated_.clear();
}

size_t VectorTrackingMemoryResource::allocated_count() const 
{
    return allocated_.size();
}
