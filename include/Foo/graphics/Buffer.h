#pragma once

#include <Foo/graphics/gltypes_ext.h>

#include <vector>

namespace foo {


	/// @brief Base class for Buffer
	class BufferBase: public GLObject<gl::buffer> {
	public:

		explicit BufferBase(size_t size);

		void update(size_t offset, size_t size, const void* data);

		size_t size_bytes() const;

	private:
		
		size_t size_;

	};


	/// @brief Returns the number of bytes given n objects.
	template <class T>
	constexpr size_t bytes_sizeof(size_t n) noexcept {
		return sizeof(T) * n;
	}


	/// @brief Returns the number of bytes between two pointers.
	template <class T>
	constexpr size_t bytes_sizeof(const T* begin, const T* end) noexcept {
		return sizeof(T) * (end - begin);
	}


	/// @brief Basic Buffer Object.
	template <class T>
	class Buffer: public BufferBase {
	public:

		explicit Buffer(size_t max_size)
			: BufferBase(bytes_sizeof<T>(max_size)) {

		}

		void update(const T* begin, const T* end, size_t offset = 0) {
			update(begin, end - begin, offset);
		}

		void update(const T* p, const size_t n, size_t offset = 0) {
			BufferBase::update(offset, bytes_sizeof<T>(n), p);
			// buffer is aware of stride for offset??
			//BufferBase::update(bytes_sizeof<T>(offset), bytes_sizeof<T>(n), p);
		}

		size_t size() const {
			return size_bytes() / sizeof(T);
		}

	};


	/// @brief Buffer Object with cpu-side buffer.
	template <class T>
	class BufferedBuffer : public Buffer<T> {
	public:

		explicit BufferedBuffer(size_t max_size)
			: Buffer<T>(max_size)
			, buffer_(max_size) {

		}

		auto begin() {
			return buffer_.begin();
		}

		auto end() {
			return buffer_.end();
		}

		auto begin() const {
			return buffer_.begin();
		}

		auto end() const {
			return buffer_.end();
		}

		auto data() {
			return buffer_.data();
		}

		auto data() const {
			return buffer_.data();
		}

		decltype(auto) operator[](size_t index) {
			return buffer_[index];
		}

		decltype(auto) operator[](size_t index) const {
			return buffer_[index];
		}

		void flush() {
			update(begin(), end(), 0);
		}

	private:

		std::vector<T> buffer_;

	};


	/// @brief Helper function for creating VBO. 
	template <template <class> class BufferType = Buffer, class T>
	BufferType<T> make_buffer(const T* begin, const T* end) {
		BufferType<T> b{ end - begin };
		b.update(begin, end);
		return b;
	}


	/// @brief Helper function for creating VBO. 
	template <template <class> class BufferType = Buffer, class T>
	BufferType<T> make_buffer(const T* begin, size_t n) {
		BufferType<T> b{ n };
		b.update(begin, n);
		return b;
	}


	/// @brief Helper function for creating VBO. 
	template <class T, template <class> class BufferType = Buffer>
		requires std::is_trivially_constructible_v<T>
	BufferType<T> make_buffer(size_t n) {
		return { n };
	}

}