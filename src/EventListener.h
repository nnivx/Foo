#pragma once

#include "Event.h"

#include <stdexcept>

namespace foo {

	class EventException : public std::runtime_error {
	public:

		explicit EventException(const std::string& message)
			: std::runtime_error(message.c_str()) {

		}

		explicit EventException(const char* message)
			: std::runtime_error(message) {

		}

	};

	class EventListener {
	public:

		virtual void operator()(const Event& e) = 0;

		virtual unsigned event_type() const noexcept = 0;

	protected:

		virtual ~EventListener() {

		}

	};

}