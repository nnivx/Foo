#include "EventDispatcher.h"
#include "EventListener.h"

#include "Logger.h"
#include "Clock.h"

#include <map>
#include <set>

namespace {

	using namespace foo;


	struct listener_data {
		Clock timer;
		Time last_runtime;
		bool timeout;
		int errors;
	};

}

namespace foo {

	struct EventDispatcher::listener_info {
		std::vector<EventListener*> listeners[Event::num_types];
		std::map<EventListener*, listener_data> data;
		std::set<EventListener*> alive;
		std::set<EventListener*> dead;

		void update() {
			update_dead();
			update_alive();
		}

	private:

		void update_alive() {
			for (auto listener : alive) {
				const unsigned flags = listener->event_type();
				for (int etype = 0; etype < Event::num_types; ++etype) {
					if ((flags >> etype) & 1) {
						// O(n) find
						auto& lisv = listeners[etype];
						//for (size_t i = 0; i < lisv.size(); ++i) {
						//	if (lisv[i] == listener) return;
						//}
						lisv.emplace_back(listener);
						data.try_emplace(listener);
						debug_logf("EventDispatcher", Logger::Verbose, "Added {} to {}", (void*)&listener, etype);
					}
				}
			}
			alive.clear();
		}

		void update_dead() {
			for (auto listener : dead) {
				const unsigned flags = listener->event_type();
				for (int etype = 0; etype < Event::num_types; ++etype) {
					if ((flags >> etype) & 1) {
						// O(n) find
						auto& lisv = listeners[etype];
						for (size_t i = 0; i < lisv.size(); ++i) {
							if (lisv[i] == listener) {
								data.erase(lisv[i]);
								std::swap(lisv[i], lisv[lisv.size() - 1]);
								lisv.pop_back();
								debug_logf("EventDispatcher", Logger::Verbose, "Removed {} from {}", (void*)&listener, etype);
								//return;
							}
						}
					}
				}
			}
			dead.clear();
		}
	};

	EventDispatcher::EventDispatcher(size_t max_queued_events)
		: current_buffer_{ 0 }
		, max_queued_events_{ max_queued_events }
		, events_{ }
		, info_{ std::make_unique<listener_info>() } {
		events_[0].reserve(max_queued_events_);
		events_[1].reserve(max_queued_events_);
	}
	
	EventDispatcher::~EventDispatcher() = default;

	void EventDispatcher::update() {
		// update once before running
		info_->update();

		const auto update_buffer = current_buffer_;
		current_buffer_ ^= 1;

		for (const Event& e : events_[update_buffer]) {
			// we really are trashing around the cache..
			auto& lisv = info_->listeners[static_cast<int>(e.type)];
			for (size_t i = 0; i < lisv.size(); ++i) {
				//info_->update(); // update before invocation

				decltype(auto) listener = lisv[i];
				decltype(auto) data = info_->data[listener];

				// timed out?
				if (data.timeout) {
					if (data.timer.elapsed() < milliseconds(500))
						continue;
					else
						data.timeout = false;
				}

				// try invoke
				auto paddr = (const void*)listener;
				try {
					data.timer.restart();
					(*listener)(e);
				}
				catch (const EventException& ex) {
					++data.errors;
					logf("EventDispatcher", Logger::Error, "{}: {}", paddr, ex.what());
					if (data.errors > 3) {
						logf("EventDispatcher", Logger::Error, "{}: too many errors, removing", paddr);
						remove_listener(*listener);
						// dont fall through, we removed it and it's dangling
						continue;
					}
				}

				data.last_runtime = data.timer.restart();

				// simple timeout
				// TODO times out even if it's because we're paused or out of focus
				if (data.last_runtime > milliseconds(100)) {
					data.timeout = true;
					logf("EventDispatcher", Logger::Info, "{}: timed out", paddr);
				}
			}
		}

		events_[update_buffer].clear();
	}


	void EventDispatcher::add_listener(EventListener& listener) {
		info_->alive.insert(&listener);
#if 0
		const unsigned flags = listener.event_type();
		for (int etype = 0; etype < Event::num_types; ++etype) {
			if (flags & (1 << etype)) {
				// O(n) find
				auto& lisv = info_->listeners[etype];
				for (size_t i = 0; i < lisv.size(); ++i) {
					if (lisv[i] == &listener) return;
				}
				lisv.emplace_back(&listener);
				info_->data.try_emplace(&listener);
			}
		}
#endif 
	}

	void EventDispatcher::remove_listener(EventListener& listener) {
		debug_logf("EventDispatcher", Logger::Error, "Please remove {}", (void*)&listener);
		info_->dead.insert(&listener);
#if 0
		const unsigned flags = listener.event_type();
		for (int etype = 0; etype < Event::num_types; ++etype) {
			if (flags & (1 << etype)) {
				// O(n) find
				auto& lisv = info_->listeners[etype];
				for (size_t i = 0; i < lisv.size(); ++i) {
					if (lisv[i] == &listener) {
						//info_->data.erase(lisv[i]);
						//std::swap(lisv[i], lisv[lisv.size() - 1]);
						//lisv.pop_back();
						info_->dead.insert(&listener);
						return;
					}
				}
			}
		}
#endif
	}

	size_t EventDispatcher::max_queued_events() const {
		return max_queued_events_;
	}

}