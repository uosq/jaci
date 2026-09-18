#pragma once

#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include <iostream>
#include <string_view>
#include <source_location>

//#define LOG(fmt, ...) std::fprintf(stderr, "'%s' at line '%i' " fmt "\n", __extension__ __PRETTY_FUNCTION__, __builtin_LINE(), ## __VA_ARGS__)

struct log_location {
	std::string_view fmt;
	std::source_location loc;

	template <typename T>
	requires std::convertible_to<T, std::string_view>
	log_location(const T& f, const std::source_location l = std::source_location::current()) : fmt(f), loc(l) {}
};

template<typename... Args>
void LOG(const log_location target, Args&&... args)
{
	const std::string message = fmt::vformat(target.fmt, fmt::make_format_args(args...));

	std::cout << "[" << target.loc.function_name() << "]: "
		  << target.loc.line() << ": "
		  << message << std::endl;
}