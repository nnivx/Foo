#pragma once

#include <glm/vec2.hpp>

#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

namespace foo {

	class Image {
	public:

		/// @brief Returns a valid image size.
		static glm::ivec2 valid_image_size(unsigned w, unsigned h);


		/// @brief Load image from path.
		static std::optional<Image> load(const fs::path& path, int components, bool flip = false);

	public:

		/// @brief Creates an uninitialized(!!) image buffer.
		Image(unsigned width, unsigned height, int components);

		Image() noexcept;

		~Image();

		Image(const Image& copy);

		Image(Image&& r) noexcept;

		Image& operator=(Image copy);

	public:

		auto size() const noexcept -> glm::uvec2;
		auto width() const noexcept -> unsigned;
		auto height() const noexcept -> unsigned;

		auto data() noexcept -> uint8_t*;
		auto data() const noexcept -> const uint8_t*;

		auto begin() noexcept -> uint8_t*;
		auto begin() const noexcept -> const uint8_t*;

		auto end() noexcept -> uint8_t*;
		auto end() const noexcept -> const uint8_t*;

		auto size_bytes() const noexcept -> size_t;

		auto components() const noexcept -> int;

		explicit operator bool() const noexcept;

		friend void swap(Image& a, Image& b) {
			using std::swap;
			swap(a.size_, b.size_);
			swap(a.components_, b.components_);
			swap(a.from_file_, b.from_file_);
			swap(a.data_, b.data_);
		}

	private:

		Image(unsigned width, unsigned height, int components, uint8_t* data);

		glm::uvec2 size_;
		int		   components_;
		bool	   from_file_;
		uint8_t*   data_;

		static_assert(std::is_same<unsigned char, uint8_t>::value, "unsigned char != uint8_t");

	};

}