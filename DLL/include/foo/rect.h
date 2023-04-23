#pragma once

#include <Foo/tmp.h>

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>

#include <algorithm>

namespace foo {


	/// @brief Simple class for handling rects. 
	template <class T, Vector2 VT>
		requires std::same_as<typename VT::value_type, T>
	struct rect {

		using value_type = T;
		using vector_type = VT;

		T x, y;
		T w, h;


		/// @brief Default constructor.
		constexpr rect() noexcept = default;


		/// @brief Constructs a rect with a given size.
		explicit constexpr rect(const vector_type& size) noexcept
			: rect(size.x, size.y) {

		}


		/// @brief Constructs a rect with a given size. 
		constexpr rect(auto w, auto h) noexcept
			: rect(0, 0, static_cast<T>(w), static_cast<T>(h)) {

		}


		/// @brief Construct rect with given pos and size. 
		constexpr rect(auto x, auto y, auto w, auto h) noexcept
			: x{ static_cast<T>(x) }
			, y{ static_cast<T>(y) }
			, w{ static_cast<T>(w) }
			, h{ static_cast<T>(h) } {

		}


		/// @brief Construct rect with pos and size.
		constexpr rect(const vector_type& pos, const vector_type& size) noexcept
			: x{ pos.x }
			, y{ pos.y }
			, w{ size.x }
			, h{ size.y } {

		}


		/// @brief Returns the min extents (x1, y1) of this rect. 
		constexpr auto min() const noexcept
			-> vector_type {
			if constexpr (std::is_unsigned<T>::value) {
				return { x, y };
			}
			else {
				return { std::min(x, x + w), std::min(y, y + h) };
			}
		}


		/// @brief Returns the max extents (x2, y2) of this rect.
		constexpr auto max() const noexcept
			-> vector_type {
			if constexpr (std::is_unsigned<T>::value) {
				return { x + w, y + h };
			}
			else {
				return { std::max(x, x + w), std::max(y, y + h) };
			}
		}


		/// @brief Returns a new rect such that size >= 0 and (x1, y1) <= (x2, y2).
		constexpr auto abs() const noexcept
			-> rect {
			if constexpr (std::is_unsigned<T>::value) {
				return *this;
			}
			else {
				return {
					std::min(x, x + w),
					std::min(y, y + h),
					std::abs(w),
					std::abs(h),
				};
			}
		}


		/// @brief Returns true if point is in this rect.
		template <bool Inclusive = true>
		constexpr auto contains(const vector_type& point) const noexcept
			-> bool {
			return contains<Inclusive>(point.x, point.y);
		}


		/// @brief Returns true if this rect contains a point. 
		template <bool Inclusive = true>
		constexpr auto contains(T px, T py) const noexcept
			-> bool {
			const auto [x1, y1] = min();
			const auto [x2, y2] = max();
			if constexpr (Inclusive) {
				return (x1 <= px) && (px <= x2) && (x1 <= px) && (py <= x2);
			}
			else {
				return (x1 < px) && (px < x2) && (x1 < px) && (py < x2);
			}
		}


		/// @brief Returns true if this rect fully contains another rect. 
		template <bool Inclusive = true>
		constexpr auto contains(const rect& rhs) const noexcept
			-> bool {
			const auto [ax1, ay1] = min();
			const auto [ax2, ay2] = max();
			const auto [bx1, by1] = rhs.min();
			const auto [bx2, by2] = rhs.max();
			if constexpr (Inclusive) {
				return (bx2 <= ax2) && (ax1 <= bx1) && (ay1 <= by1) && (by2 <= ay2);
			}
			else {
				return (bx2 < ax2) && (ax1 < bx1) && (ay1 < by1) && (by2 < ay2);
			}
		}


		/// @brief Returns true if any point of another rect is in this rect.
		template <bool Inclusive = true>
		constexpr auto intersects(const rect& rhs) const noexcept
			-> bool {
			const auto [ax1, ay1] = min();
			const auto [ax2, ay2] = max();
			const auto [bx1, by1] = rhs.min();
			const auto [bx2, by2] = rhs.max();
			if constexpr (Inclusive) {
				return (ax1 <= bx2) && (bx1 <= ax2) && (ay1 <= by2) && (by1 <= ay2);
			}
			else {
				return (ax1 < bx2) && (bx1 < ax2) && (ay1 < by2) && (by1 < ay2);
			}
		}


	};


	/// @brief Returns true if a given value is in range. 
	/// TODO move function to another file
	template <template <class> class UpperBound = std::less, class T>
	constexpr auto in_range(T v, T min, T max) noexcept
		-> bool {
		return !(v < min) && UpperBound<T>{}(v, max);
	}


	/// @brief Returns the intersection of two rects.
	template <class T, Vector2 VT>
	constexpr auto intersection(const rect<T, VT>& a, const rect<T, VT>& b)
		-> rect<T, VT> {
		auto [ax1, ay1] = a.min();
		auto [ax2, ay2] = a.max();
		auto [bx1, by1] = b.min();
		auto [bx2, by2] = b.max();

		T x = std::max(ax1, bx1);
		T y = std::max(ay1, by1);
		T w = std::min(ax2, bx2) - x;
		T h = std::min(ay2, by2) - y;

		return { x, y, w, h };
	}


	/// @brief Rect with floating point values.
	using frect = rect<glm::fvec2::value_type, glm::fvec2>;


	/// @brief Rect with integer values.
	using irect = rect<glm::ivec2::value_type, glm::ivec2>;


	/// @brief Transforms integer values rect to range (0.. 1).
	/// Doesn't check if values are outside range.
	inline frect normalize(const irect& rect, const glm::ivec2& size) {
		return { rect.x / static_cast<float>(size.x),
				 rect.y / static_cast<float>(size.y),
				 rect.w / static_cast<float>(size.x),
				 rect.h / static_cast<float>(size.y), };
	}


	/// @brief Expands normalized values to rect of given size. 
	inline irect expand(const frect& rect, const glm::ivec2& size) {
		return { rect.x * size.x,
				 rect.y * size.y,
				 rect.w * size.x,
				 rect.h * size.y, };
	}

}
