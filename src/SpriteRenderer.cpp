#include "SpriteRenderer.h"
#include "Sprite.h"

#include <Foo/graphics/Texture.h>
#include <Foo/graphics/gltypes.h>
#include <Foo/graphics/Shader.h>

#include <glm/gtx/transform.hpp>

#include <stdexcept>

namespace foo {

	// Buffer

	Sprite* SpriteStorage::create() {
		const auto index = size();

		vertices_.insert(vertices_.end(), vertex_per_quad, {});

		const auto base_index = static_cast<index_type>(size() * 4);
		auto idx = indices_.insert(indices_.end(), index_per_quad, base_index);
		idx[0] += 0;
		idx[1] += 1;
		idx[2] += 2;
		idx[3] += 2;
		idx[4] += 1;
		idx[5] += 3;
		return &sprites_.emplace_back(
				nullptr,
				irect{ 0, 0, 1, 1 },
				glm::fvec2{ 0.5f, 0.5f },
				glm::fvec2{ 0.0f, 0.0f },
				glm::fvec2{ 1.0f, 1.0f },
				glm::fvec3{ 1.0f, 1.0f, 1.0f },
				0.0f,
				0,
				true,
				Sprite::readonly_data{ index }
			);
	}

	void SpriteStorage::destroy(Sprite* sprite) {
		if (!sprite) return;

		auto it = std::find_if(sprites_.cbegin(), sprites_.cend(), [&](const Sprite& other) {
			return &other == sprite;
			});

		if (it == sprites_.end()) return;

		const auto back_index = size() - 1;
		const auto recycled_index = sprite->readonly.index;

		sprites_.erase(it);

		if (back_index != recycled_index) {
			auto back = at(back_index);

			back->readonly.index = recycled_index;

			const auto p1 = vertices(back_index);
			std::copy(p1, p1 + 4, vertices(recycled_index));

			const auto p2 = indices(back_index);
			std::copy(p2, p2 + 6, indices(recycled_index));
		}

		vertices_.erase(vertices_.end() - vertex_per_quad, vertices_.end());
		indices_.erase(indices_.end() - index_per_quad, indices_.end());
	}

	void SpriteStorage::clear() {
		sprites_.clear();
		vertices_.clear();
		indices_.clear();
	}

	bool SpriteStorage::empty() const {
		return sprites_.empty();
	}

	size_t SpriteStorage::size() const {
		return sprites_.size();
	}

	const Sprite* SpriteStorage::at(size_t index) const {
		// it's more likely to find it at the end for the first few deletions
		for (auto it = sprites_.rbegin(); it != sprites_.rend(); ++it) {
			if (it->readonly.index == index) return &*it;
		}

		return nullptr;
	}

	const SpriteStorage::vertex_type* SpriteStorage::vertices(size_t index) const {
		return &vertices_[index * vertex_per_quad];
	}

	const SpriteStorage::index_type* SpriteStorage::indices(size_t index) const {
		return &indices_[index * index_per_quad];
	}

	// Renderer


	SpriteRenderer::SpriteRenderer(size_t buffer_size)
		: buffer_size_{ buffer_size }
		, vbo_(buffer_size)
		, shader_{ Shader::from_file("res/shaders/SpriteVS.glsl", "res/shaders/SpriteFS.glsl") }
		, loc_mvp_{ shader_.uniform_location("mvp") }
		, loc_col_{ shader_.uniform_location("col") }
		, loc_tex_{ shader_.uniform_location("tex") } {

		glEnableVertexArrayAttrib(vao_, 0);
		glVertexArrayAttribFormat(vao_, 0, 4, GL_FLOAT, false, offsetof(SpriteVertex, pos));
		glVertexArrayAttribBinding(vao_, 0, 0);

		glEnableVertexArrayAttrib(vao_, 1);
		glVertexArrayAttribFormat(vao_, 1, 2, GL_FLOAT, false, offsetof(SpriteVertex, uv));
		glVertexArrayAttribBinding(vao_, 1, 0);

		glVertexArrayVertexBuffer(vao_, 0, vbo_.id(), 0, sizeof(SpriteVertex));

	}

	Sprite* SpriteRenderer::create(Texture* texture, const frect& rect) {
		auto p = create();
		if (p) {
			p->texture = texture;
			p->texture_rect = expand(rect, texture->size());
			p->size = texture->size();
		}
		return p;
	}

	void SpriteRenderer::update() {
		int num_dirty = 0;
		for (auto& sprite: store_) {
			if (sprite.dirty) ++num_dirty;
		}

		const float dirty_ratio = static_cast<float>(num_dirty) / store_.size();
		const float min_single_flush_ratio = 0.20f;

		// TODO what's the magic number between many flush()
		// and one big potentially wasteful call?
		if (dirty_ratio <= min_single_flush_ratio && num_dirty < 6) {
			for (auto& sprite : store_) {
				if (sprite.dirty) {
					update_quad(sprite);
					flush(sprite.readonly.index, 1);
					sprite.dirty = false;
				}
			}
		}
		else {
			for (auto& sprite : store_) {
				if (sprite.dirty) {
					update_quad(sprite);
					sprite.dirty = false;
				}
			}

			flush(0, store_.size());
		}
	}


	size_t SpriteRenderer::buffer_size() const {
		return buffer_size_;
	}

	void SpriteRenderer::update_quad(const Sprite& s) {
		const glm::fvec2 origin = s.origin * s.size;

		// Recalculate matrix
		glm::mat4 model{ 1.0f };
		model = glm::translate(model, glm::fvec3(s.position - origin, s.layer));

		model = glm::translate(model, glm::fvec3(origin, 0.0f));
		model = glm::rotate(model, s.rotation, glm::fvec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::fvec3(-origin, 0.0f));

		model = glm::scale(model, glm::fvec3(s.size, 1.0f));

		// Recalculate Texture
		const auto p0 = normalize_uv(*s.texture, { s.texture_rect.x, s.texture_rect.y });
		const auto p1 = normalize_uv(*s.texture, { s.texture_rect.x + s.texture_rect.w, s.texture_rect.y + s.texture_rect.h });

		// Update vertices
		auto quad = store_.vertices(s.readonly.index);
		quad[0].pos = model * glm::fvec4(0, 1, 0, 1);
		quad[0].uv = { p0.x, p1.y };

		quad[1].pos = model * glm::fvec4(0, 0, 0, 1);
		quad[1].uv = { p0.x, p0.y };

		quad[2].pos = model * glm::fvec4(1, 1, 0, 1);
		quad[2].uv = { p1.x, p1.y };

		quad[3].pos = model * glm::fvec4(1, 0, 0, 1);
		quad[3].uv = { p1.x, p0.y };

		// Indices don't need updating
	}

	void SpriteRenderer::begin_draw(const glm::mat4& mvp) {
		shader_.bind();
		glBindVertexArray(vao_);
		shader_.set_uniform(loc_mvp_, mvp);
	}

	void SpriteRenderer::draw() {
		const Texture* prev = nullptr;

		for (const auto& sprite : store_) {
			// TODO tex shouldn't be null, but lets use a default one anyway
			const auto tex = sprite.texture ? sprite.texture : nullptr;
			if (tex != prev) {
				tex->bind(0);
				prev = tex;
			}

			shader_.set_uniform(loc_col_, sprite.color);

			draw(sprite.readonly.index, 1);
		}
	}

	void SpriteRenderer::flush(size_t index, size_t n) {
		vbo_.update(store_.vertices(index), n * SpriteStorage::vertex_per_quad, index);
		// TIL offset *is* aware of stride so no need to calculate as byte size...
		//glNamedBufferSubData(vbo_.id(), index, bytes_sizeof<SpriteVertex>(n) * 4, store_.vertices(index));
	}

	void SpriteRenderer::draw(size_t index, size_t n) {
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(n) * SpriteStorage::index_per_quad, GL_UNSIGNED_SHORT, store_.indices(index));
	}


}
