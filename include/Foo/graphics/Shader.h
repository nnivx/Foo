#pragma once 

#include <Foo/graphics/gltypes.h>

#include <glm/fwd.hpp>

#include <string>
#include <string_view>
//#include <optional>
#include <map>

#include <filesystem>

namespace fs = std::filesystem;

namespace foo {

	class Shader {
	public:

		static Shader from_source(std::string_view vert, std::string_view frag);

		static Shader from_file(const fs::path& vert_path, const fs::path& frag_path);

		//Shader(std::string_view vert, std::string_view frag);

	public:

		int uniform_location(const std::string& name) const;

		void set_uniform(int location, int v0);
		void set_uniform(int location, int v0, int v1);
		void set_uniform(int location, int v0, int v1, int v2);
		void set_uniform(int location, int v0, int v1, int v2, int v3);

		void set_uniform(int location, float v0);
		void set_uniform(int location, float v0, float v1);
		void set_uniform(int location, float v0, float v1, float v2);
		void set_uniform(int location, float v0, float v1, float v2, float v3);

		void set_uniform(int location, unsigned v0);
		void set_uniform(int location, unsigned v0, unsigned v1);
		void set_uniform(int location, unsigned v0, unsigned v1, unsigned v2);
		void set_uniform(int location, unsigned v0, unsigned v1, unsigned v2, unsigned v3);

		template <class T, glm::qualifier Q>
		void set_uniform(int location, const glm::vec<2, T, Q>& v) {
			set_uniform(location, v.x, v.y);
		}

		template <class T, glm::qualifier Q>
		void set_uniform(int location, const glm::vec<3, T, Q>& v) {
			set_uniform(location, v.x, v.y, v.z);
		}

		template <class T, glm::qualifier Q>
		void set_uniform(int location, const glm::vec<4, T, Q>& v) {
			set_uniform(location, v.x, v.y, v.z, v.w);
		}

		void set_uniform(int location, const glm::mat4& m);

		void bind() const;

		gl::object_id id() const;

	private:

		Shader() = default;

		gl::program program_;
		std::map<std::string, int> uniforms_;

	};


}