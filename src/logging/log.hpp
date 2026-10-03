#ifndef JACI_LOG_HPP
#define JACI_LOG_HPP

#include <chrono>
#include <vector>

//#define LOG(fmt, ...) std::fprintf(stderr, "'%s' at line '%i' " fmt "\n", __extension__ __PRETTY_FUNCTION__, __builtin_LINE(), ## __VA_ARGS__)

struct log_info
{
	log_info(const std::string& text);

	uint32_t id;
	std::string message;
	std::chrono::system_clock::time_point time;

	std::string get_formatted_timestamp() const;
	std::string get_formatted_text() const;
	const std::string& get_text() const;
};

void make_log(const std::string& text);
const std::vector<log_info>& get_logs();
void export_logs();
void clear_logs();

#endif