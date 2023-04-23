#pragma once

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <glm/ext/matrix_clip_space.hpp>

namespace foo {

	// TODO tidy interface; should be
	// proj * inverse view * model
	class Camera {
	public:

		Camera& position(const glm::fvec3& pos) {
			pos_ = pos;
			return *this;
		}

		Camera& look_at(const glm::fvec3& target, const glm::fvec3& up = glm::fvec3(0, 1, 0)) {
			const auto f = glm::normalize(target - pos_);
			const auto r = glm::normalize(glm::cross(f, up));
			const auto u = glm::cross(r, f);

			view_[0].x = r.x;
			view_[1].x = r.y;
			view_[2].x = r.z;

			view_[0].y = u.x;
			view_[1].y = u.y;
			view_[2].y = u.z;

			view_[0].z = -f.x;
			view_[1].z = -f.y;
			view_[2].z = -f.z;

			view_[0].w = 0;
			view_[1].w = 0;
			view_[2].w = 0;

			view_[3].x = -glm::dot(r, pos_);
			view_[3].y = -glm::dot(u, pos_);
			view_[3].z = glm::dot(f, pos_);
			view_[3].w = 1;

			vp_ = proj_ * view_;
			return *this;
		}

		Camera& perspective(float fov, float width, float height, float znear, float zfar) {
			proj_ = glm::perspective(fov, width / height, znear, zfar);
			vp_ = proj_ * view_;
			return *this;
		}

		Camera& ortho(int width, int height, int znear, int zfar) {
#if 1
			float w = width / 2.0f;
			float h = height / 2.0f;
			proj_ = glm::ortho(-w, w, -h, h, static_cast<float>(znear), static_cast<float>(zfar));
#else
			float aspect_2 = width / height / 2;
			proj_ = glm::ortho(-aspect_2, aspect_2, -0.5f, 0.5f, znear, zfar);
#endif
			vp_ = proj_ * view_;
			return *this;
		}

		glm::mat4 view() const {
			return view_;
		}

		glm::mat4 projection() const {
			return proj_;
		}

		glm::mat4 matrix() const {
			return vp_;
		}

	private:

		glm::fvec3 pos_ = { 0, 0, 0 };
		glm::mat4 view_{ 1 };
		glm::mat4 proj_{ 1 };
		glm::mat4 vp_{ 1 };

	};

}