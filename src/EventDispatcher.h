#pragma once

#include "Event.h"
#if !FOO_INCLUDE_EVENT_CONSTRUCTORS
#include "event_traits.h"
#endif

#include <vector>
#include <memory>

namespace foo {

	class EventListener;

	class EventDispatcher {
	public:

		explicit EventDispatcher(size_t max_queued_events);

		~EventDispatcher();

		template <Event::Type Type, class... Args>
		void push(Args&&... args) {
			if (events_[current_buffer_].size() < max_queued_events_) {
#if FOO_INCLUDE_EVENT_CONSTRUCTORS
				events_[current_buffer_].emplace_back(Event::type_tag<Type>{}, std::forward<Args>(args)...);
#else
				events_[current_buffer_].emplace_back(make_event<Type>(std::forward<Args>(args)...));
#endif
			}
		}

		void update();

		void add_listener(EventListener& listener);

		void remove_listener(EventListener& listener);

		size_t max_queued_events() const;

	private:

		size_t max_queued_events_;
		size_t current_buffer_;
		std::vector<Event> events_[2];

		struct listener_info;

		std::unique_ptr<listener_info> info_;

	};

}