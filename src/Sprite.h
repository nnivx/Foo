#pragma once

#include <Foo/rect.h>

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace foo {
	
	class Texture;

	static constexpr inline glm::fvec2 Center{ 0.5f, 0.5f };
	constexpr inline glm::fvec2 BottomLeft{ 0.0f, 0.0f };
	constexpr inline glm::fvec2 TopLeft{ 0.0f, 1.0f };
	constexpr inline glm::fvec2 BottomRight{ 1.0f, 0.0f };
	constexpr inline glm::fvec2 TopRight{ 1.0f, 1.0f };

	struct Sprite {
		Texture* texture;
		irect texture_rect;

		glm::fvec2 origin;
		glm::fvec2 position;
		glm::fvec2 size;
		glm::fvec3 color;
		float rotation;

		int layer;
		bool dirty;

		struct readonly_data {
			size_t index;
		} readonly;
	};

}

