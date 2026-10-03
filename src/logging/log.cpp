#include "log.hpp"

#include <fstream>

#include "../../thirdparty/csv/csv2.hpp"

#include "../features/config/config.hpp"

static uint32_t id_counter = 0;
static std::vector<log_info> logs {};

log_info::log_info(const std::string& text) : id(id_counter++), message(text), time(std::chrono::system_clock::now()) {}

std::string log_info::get_formatted_timestamp() const
{
        return std::format("{:%d/%m/%Y - %H:%M}", time);
}

std::string log_info::get_formatted_text() const
{
        return std::format("{} - {}", get_formatted_timestamp(), message);
}

const std::string& log_info::get_text() const
{
        return message;
}

void make_log(const std::string& text)
{
        if (!f_config.logs)
                return;

        logs.emplace_back(log_info{text});
}

const std::vector<log_info>& get_logs()
{
        return logs;
}

void export_logs()
{
        std::ofstream stream("logs.csv");
        csv2::Writer<csv2::delimiter<','>> writer(stream);

        std::vector<std::vector<std::string>> rows
        {
                {"id", "timestamp", "message"},
        };

        rows.reserve(logs.size() + 1);

        for (const auto& log : logs)
                rows.push_back({std::to_string(log.id), log.get_formatted_timestamp(), log.get_text()});

        writer.write_rows(rows);
        stream.close();
}

void clear_logs()
{
        logs.clear();
}