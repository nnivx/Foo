#include "Logger.h"

#include <chrono>

namespace {
	
	foo::Logger* g_logger;

	const char* levelstr[]{
			"Sys",
			"Err",
			"Wrn",
			"Inf",
			"Ver",
	};

	static_assert(sizeof(levelstr) / sizeof(const char*) >= (foo::Logger::All - 1), "Bro, please.");

	// TODO workaround
	auto since_start() {
		static const auto start = std::chrono::high_resolution_clock::now();
		return std::chrono::high_resolution_clock::now() - start;
	}

}

namespace foo {

	void Logger::log(std::string_view tag, int level, std::string_view msg, std::true_type) {
		const auto tm = std::chrono::hh_mm_ss(since_start()); 
		(level == Error? err: out).get()
			// timestamp
			<< "  " << tm << "\b\b\b\b\b\b" // TODO
			// type
			<< "  " << '[' << levelstr[std::min(Logger::All - 1, level)] << ']'
			// tag
			<< ' ' << tag
			// message
			<< '\t' << ' ' << msg << '\n';
	}

	void Logger::log(std::string_view tag, int level, std::string_view msg, std::false_type) {
		if (level < log_level || log_level == All)
			log(tag, level, msg, std::true_type{});
	}

	void set_logger(Logger& logger) noexcept {
		g_logger = &logger;
	}

	Logger& get_logger() noexcept {
		return *g_logger;
	}

}