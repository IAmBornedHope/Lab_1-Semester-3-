#pragma once
#include <cstddef>

template<typename T>
class ICollection {
public:
    virtual ~ICollection() = default;
    virtual bool insert(const T& value) = 0;
    virtual bool erase(const T& value) = 0;
    virtual bool contains(const T& value) const = 0;
    virtual size_t size() const = 0;
    virtual bool is_empty() const = 0;
    virtual void clear() = 0;
};