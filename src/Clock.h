#pragma once

#include <Foo/Time.h>

namespace foo {


	class Clock {
	public:

		Clock()
			: start_time_{ Time::now() } {

		}

		Time elapsed() const {
			return Time::now() - start_time_;
		}

		Time restart() {
			auto now = Time::now();
			auto elapsed = now - start_time_;
			start_time_ = now;
			return elapsed;
		}

	private:

		Time start_time_;

	};

}