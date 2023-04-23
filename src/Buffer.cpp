#include <Foo/graphics/Buffer.h>

#include <glad/glad.h>

namespace foo {

	BufferBase::BufferBase(size_t size)
		: size_{ size } {
		glNamedBufferStorage(id(), size, nullptr, GL_DYNAMIC_STORAGE_BIT);
	}

	void BufferBase::update(size_t offset, size_t size, const void* data) {
		glNamedBufferSubData(id(), offset, size, data);
	}

	size_t BufferBase::size_bytes() const {
		return size_;
	}


}