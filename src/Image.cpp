#include <Foo/graphics/Image.h>

#pragma warning (push, 0)
#pragma warning( disable: 26451 )
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_GIF
//#define STBI_NO_STDIO
#include "stb_image.h"
#pragma warning (pop)

#include <filesystem>
#include <iostream>
#include <fstream>
#include <cstdlib>

namespace {

    stbi_io_callbacks callbacks_from_istream() {
        stbi_io_callbacks cb;
        cb.read = [](void* user, char* data, int size) -> int {
            auto& in = *static_cast<std::istream*>(user);
            in.read(data, size);
            return static_cast<int>(in.gcount());
        };

        cb.skip = [](void* user, int n) -> void {
            auto& in = *static_cast<std::istream*>(user);
            if (n > 0) {
                in.ignore(n);
            }
            else {
                for (int i = n; i < 0; ++i) in.unget();
            }
        };

        cb.eof = [](void* user) -> int {
            auto& in = *static_cast<std::istream*>(user);
            return in.eof();
        };
        return cb;
    }

    // About 32k
    static inline constexpr unsigned max_image_size = (1 << 24) / 1000 * 2;

    template <class T> 
    [[nodiscard]] T* uninitialized_alloc(size_t n) {
        auto p = std::malloc(sizeof(T) * n);
        return static_cast<T*>(p);
    }

    void dealloc(uint8_t* ptr) {
        std::free(ptr);
    }

}

namespace foo {

    glm::ivec2 Image::valid_image_size(unsigned width, unsigned height) {
        width = std::clamp(width, 0U, max_image_size);
        height = std::clamp(height, 0U, max_image_size);
        return { width, height };
    }

    std::optional<Image> Image::load(const fs::path& path, int components, bool flip) {
        //auto canonpath = fs::canonical(path);
        //std::ifstream in{ path };
        //if (!in) {
        //    std::cerr << "Failed to open image file " << path.string() << std::endl;
        //    return {};
        //}

        int x, y, comp;

        stbi_set_flip_vertically_on_load(flip);

        //auto cb = callbacks_from_istream();
        //auto data = stbi_load_from_callbacks(&cb, &in, &x, &y, &comp, components);
        auto str = path.string();
        auto data = stbi_load(str.c_str(),  & x, &y, &comp, components);
        //auto data = stbi_load(fuckingpathstring.c_str(), &x, &y, &comp, components);
        if (!data) {
            std::cerr << "Failed to load image " << path.string() << " : " << stbi_failure_reason() << std::endl;
            return {};
        }

        return Image{ static_cast<unsigned>(x), static_cast<unsigned>(y), components, data };
    }

    Image::Image(unsigned width, unsigned height, int components)
        : size_{ valid_image_size(width, height) }
        , components_{ std::clamp(components, 0, 4) }
        , from_file_{ false }
        , data_{ uninitialized_alloc<uint8_t>(size_.x * size_.y * static_cast<unsigned>(components_)) } {

    }

    Image::Image(unsigned width, unsigned height, int components, uint8_t* data)
        : size_{ width, height }
        , components_{ components }
        , from_file_{ true }
        , data_{ data } {
    
    }

    Image::Image() noexcept
        : size_{ 0, 0 }
        , components_{ 0 }
        , from_file_{ false }
        , data_{ nullptr } {

    }

    Image::~Image() {
        if (from_file_) {
            stbi_image_free(data_);
        }
        else {
            if (data_) dealloc(data_);
        }
    }

    Image::Image(const Image& copy)
        : Image(copy.size_.x, copy.size_.y, copy.components_) {
        std::uninitialized_copy(copy.begin(), copy.end(), begin());
    }

    Image::Image(Image&& r) noexcept
        : size_{ std::move(r.size_) }
        , components_{ std::exchange(r.components_, 0) }
        , from_file_{ std::exchange(r.from_file_, false) }
        , data_{ std::exchange(r.data_, nullptr) } {

    }

    Image& Image::operator=(Image copy) {
        if (this != &copy) {
            using std::swap;
            swap(*this, copy);
        }
        return *this;
    }


    glm::uvec2 Image::size() const noexcept {
        return size_;
    }

    auto Image::width() const noexcept -> unsigned {
        return size_.x;
    }

    auto Image::height() const noexcept -> unsigned {
        return size_.y;
    }

    uint8_t* Image::data() noexcept {
        return data_;
    }

    const uint8_t* Image::data() const noexcept {
        return data_;
    }

    uint8_t* Image::begin() noexcept {
        return data_;
    }

    const uint8_t* Image::begin() const noexcept {
        return data_;
    }

    uint8_t* Image::end() noexcept {
        return data_ + size_.x * size_.y * components_;
    }

    const uint8_t* Image::end() const noexcept {
        return data_ + size_.x * size_.y * components_;
    }

    size_t Image::size_bytes() const noexcept {
        return sizeof(uint8_t) * size_.x * size_.y * components_;
    }

    int Image::components() const noexcept {
        return components_;
    }

    Image::operator bool() const noexcept {
        return data_;
    }

}