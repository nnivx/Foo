#pragma once

#include <Foo/graphics/Buffer.h>
#include <Foo/graphics/Shader.h>
#include <Foo/rect.h>

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <vector>
#include <list>

namespace foo {

	struct Sprite;
	class Texture;

	/// @brief Basic sprite vertex.
	struct SpriteVertex {
		glm::fvec4 pos;
		glm::fvec2 uv;
	};


	/// @brief Storage for sprite and buffer data.
	class SpriteStorage {
	public:

		using vertex_type = SpriteVertex;
		using index_type = unsigned short;

		static constexpr inline size_t vertex_per_quad = 4;	// 4 vertices per quad
		static constexpr inline size_t index_per_quad  = 6;	// 2 triangles per quad

	public:

		/// @brief Create a new empty sprite. 
		Sprite* create();

		/// @brief Destroy sprite.
		void destroy(Sprite* sprite);

		/// @brief Clear storage and invalidate all pointers.
		void clear();

		/// @brief Returns true if storage is empty. 
		bool empty() const;

		/// @brief Returns the number of sprites in storage. 
		size_t size() const;

		/// @brief Begin iterator of sprites.
		auto begin() {
			return sprites_.begin();
		}

		/// @brief Begin iterator of sprites.
		auto begin() const {
			return sprites_.begin();
		}

		/// @brief End iterator of sprites.
		auto end() {
			return sprites_.end();
		}

		/// @brief End iterator of sprites.
		auto end() const {
			return sprites_.end();
		}

		/// @brief Sort sprites in the storage.
		template <class Pred>
			requires (requires (Pred p) {
					{ p(std::declval<Sprite>(), std::declval<Sprite>()) } -> std::same_as<bool>;
				})
		void sort_sprites(Pred pred) {
			sprites_.sort(pred);
		}

		/// @brief Returns sprite at index.
		Sprite* at(size_t index) {
			return const_cast<Sprite*>(static_cast<const SpriteStorage*>(this)->at(index));
		}

		/// @brief Returns sprite at index.
		const Sprite* at(size_t index) const;

		/// @brief Returns vertices at index. 
		vertex_type* vertices(size_t index) {
			return const_cast<vertex_type*>(static_cast<const SpriteStorage*>(this)->vertices(index));
		}

		/// @brief Returns vertices at index.
		const vertex_type* vertices(size_t index) const;

		/// @brief Returns indices at index.
		index_type* indices(size_t index) {
			return const_cast<index_type*>(static_cast<const SpriteStorage*>(this)->indices(index));
		}

		/// @brief Returns indices at index.
		const index_type* indices(size_t index) const;

	private:

		std::list<Sprite> sprites_;

		std::vector<vertex_type> vertices_;
		std::vector<index_type> indices_;

	};


	/// @brief Sprite Renderer class.
	class SpriteRenderer {
	public:

		explicit SpriteRenderer(size_t buffer_size);

		Sprite* create() {
			return store_.create();
		}

		Sprite* create(Texture* texture, const frect& rect = { 1, 1 });

		void destroy(Sprite* sprite) {
			store_.destroy(sprite);
		}

		SpriteStorage& store() {
			return store_;
		}

		void update();

		void begin_draw(const glm::mat4& mvp);

		void draw();

		size_t buffer_size() const;

	private:

		void update_quad(const Sprite& s);

		void flush(size_t index, size_t n);

		void draw(size_t index, size_t n);

		SpriteStorage store_;

		size_t buffer_size_;
		gl::vertex_array	 vao_;
		Buffer<SpriteVertex> vbo_;

		Shader shader_;
		int loc_mvp_;
		int loc_col_;
		int loc_tex_;

	};

}
