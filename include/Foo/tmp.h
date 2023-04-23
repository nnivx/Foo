#pragma once

#include <concepts>

namespace foo {

	template <class T>
	concept Vector2 = requires (T t) {
		t.x;
		t.y;
	};

	template <class T>
	concept Vector3 = requires (T t) {
		t.x;
		t.y;
		t.z;
	};

	template <class T>
	concept Vector4 = requires (T t) {
		t.x;
		t.y;
		t.z;
		t.w;
	};

	template <class T>
	concept Rect = requires (T t) {
		t.x;
		t.y;
		t.w;
		t.h;
	};

}