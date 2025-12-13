#pragma once

#include <memory_resource>
#include <vector>
#include <cstddef>

class VectorTrackingMemoryResource : public std::pmr::memory_resource 
{
private:
    std::vector<void*> allocated_;

protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override;
    void  do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override;
    bool  do_is_equal(const std::pmr::memory_resource& other) const noexcept override;

public:
    ~VectorTrackingMemoryResource();

    size_t allocated_count() const;
};
