#pragma once

#include "sentinel.h"
#include "ethread.h"

namespace foo::lua {

	class pstack {
	public:

		void push_nil();

		void push_string();




	private:
		ethread L;
	};


}