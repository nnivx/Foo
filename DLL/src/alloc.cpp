#include <foo/alloc.h>

#include <stdexcept>

#pragma warning (push)
#pragma warning (disable: 28251)
#pragma warning (disable: 28252)
#pragma warning (disable: 28253)

void* operator new(size_t size) {
	auto p = je_malloc(size);
	if (!p) throw std::bad_alloc();
	return p;
}

void* operator new(size_t size, std::nothrow_t) noexcept {
	return je_malloc(size);
}

void* operator new(size_t size, std::align_val_t align) {
	auto p = je_aligned_alloc(static_cast<size_t>(align), size);
	if (!p) throw std::bad_alloc();
	return p;
}

void* operator new(size_t size, std::align_val_t align, std::nothrow_t) noexcept {
	return je_aligned_alloc(static_cast<size_t>(align), size);
}

void operator delete(void* p) noexcept {
	je_free(p);
}

void operator delete(void* p, size_t) noexcept {
	je_free(p);
}

void operator delete(void* p, std::nothrow_t) noexcept {
	je_free(p);
}
#pragma warning (pop)

namespace foo {

	void* realloc(void* p, size_t size) {
		return je_realloc(p, size);
	}

}