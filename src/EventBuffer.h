#pragma once

#include "Event.h"
#include "event_traits.h"

#include <vector>

namespace foo {

	class EventBuffer {
	public:

		EventBuffer(size_t max_queued_events)
			: current_buffer_{ 0 }
			, max_queued_events_{ max_queued_events } {
			buffer_[0].reserve(max_queued_events_);
			buffer_[1].reserve(max_queued_events_);
		}

	public:

		template <Event::Type Type, class... Args>
		void push(Args&&... args) {
			if (size() < max_queued_events_) {
				if constexpr (FOO_INCLUDE_EVENT_CONSTRUCTORS) {
					buffer().emplace_back(Event::type_tag<Type>{}, std::forward<Args>(args)...);
				}
				else {
					buffer().emplace_back(make_event<Type>(std::forward<Args>(args)...));
				}
			}
		}

		decltype(auto) begin() const {
			return buffer().begin();
		}

		decltype(auto) end() const {
			return buffer().end();
		}

		decltype(auto) data() const {
			return buffer().data();
		}

		size_t size() const {
			return buffer().size();
		}

		void swap_buffers() {
			current_buffer_ ^= 1;
		}

		size_t max_queued_events() const {
			return max_queued_events_;
		}

	private:

		std::vector<Event>& buffer() {
			return buffer_[current_buffer_];
		}

		const std::vector<Event>& buffer() const {
			return buffer_[current_buffer_];
		}

	private:

		std::vector<Event> buffer_[2];
		size_t current_buffer_;
		size_t max_queued_events_;

	};

}