#pragma once

// Type extensions for gltypes
#include <Foo/graphics/gltypes.h>

namespace foo {

	template <class T>
	concept dsa_aware = requires {
		T::dsa_aware::value;
	};

	template <class T> 
	class GLObject {
	public:

		using dsa_aware = std::false_type;

		template <class... Args>
		GLObject(Args&&... args)
			: object_(std::forward<Args>(args)...) {

		}

		gl::object_id id() const {
			return object_;
		}

	private:

		T object_;

	};

}