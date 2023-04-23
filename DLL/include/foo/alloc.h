#pragma once

#include <jemalloc/jemalloc.h>

#include <new>

#pragma warning (push)
#pragma warning (disable: 28251)
#pragma warning (disable: 28252)
#pragma warning (disable: 28253)
[[nodiscard]]
void* operator new(size_t size);

[[nodiscard]]
void* operator new(size_t size, std::nothrow_t) noexcept;

[[nodiscard]]
void* operator new(size_t size, std::align_val_t align);

[[nodiscard]]
void* operator new(size_t size, std::align_val_t align, std::nothrow_t) noexcept;

void operator delete(void* p) noexcept;

void operator delete(void* p, size_t) noexcept;

void operator delete(void* p, std::nothrow_t) noexcept;
#pragma warning (pop)

namespace foo {

	void* realloc(void* p, size_t);

}
