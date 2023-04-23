#pragma once

#include <string_view>
#include <ostream>
#include <format>

namespace foo {

	/// @brief Basic logger class.
	class Logger {
	public:
		enum : int {
			None = -1,
			System,
			Error,
			Warn,
			Info,
			Verbose,
			All,
		};

	public:

		int log_level;
		std::reference_wrapper<std::ostream> out;
		std::reference_wrapper<std::ostream> err;

	public:

		/// @brief The actual logging function, skips checks and just outputs to out.
		void log(std::string_view tag, int level, std::string_view msg, std::true_type);

		/// @brief Log if level is in within set log_level.
		void log(std::string_view tag, int level, std::string_view msg, std::false_type);

	};

	/// @brief Sets the global logger.
	void set_logger(Logger& logger) noexcept;

	/// @brief Returns the global logger.
	Logger& get_logger() noexcept;

	/// @brief Output basic log.
	inline void log(std::string_view tag, int level, std::string_view msg) noexcept {
		try {
			auto logger = get_logger();
			logger.log(tag, level, msg, std::false_type{});
		}
		catch (...) {
			// is bad but just swallow errors for now..
		}
	}

	/// @brief Output formatted log.
	template <class... Args>
	inline void logf(std::string_view tag, int level, std::string_view fmt, Args&&... args) noexcept {
		auto logger = get_logger();
		try {
			if (level <= logger.log_level || logger.log_level == Logger::All) {
				logger.log(tag, level, std::vformat(fmt, std::make_format_args(args...)), std::true_type{});
			}
		}
		catch (const std::format_error& e) {
			logger.log("Logger", Logger::Info, std::format("format error: {}", e.what()), std::true_type{});
		}
		// TODO
	}

	/// @brief Logs a message callback.
	template <class F>
		requires requires (F f) { { f() } -> std::convertible_to<std::string_view>; }
	inline void logm(std::string_view tag, int level, F message) noexcept {
		try {
			auto logger = get_logger();
			if (level <= logger.log_level || logger.log_level == Logger::All) {
				logger.log(tag, level, message(), std::true_type{});
			}
		}
		catch (...) {
			// is bad but just swallow errors for now..
		}
	}

	/// @brief Set the global log level.
	inline void set_log_level(int level) noexcept {
		get_logger().log_level = level;
	}

#if NDEBUG
#define debug_log(tag, level, message) 
#define debug_logf(tag, level, message) 
#define debug_logm(tag, level, message) 
#else
#define debug_log(tag, level, msg) log(tag, level, msg)
#define debug_logf(tag, level, fmt, ...) logf(tag, level, fmt, __VA_ARGS__)
#define debug_logm(tag, level, message) logm(tag, level, message)
#endif
}