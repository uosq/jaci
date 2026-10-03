//
// Created by tevin on 27/09/2026.
//

#include <charconv>
#include <vector>

#include "../classes/convar.hpp"

#include "../features/hook_manager/hookmanager.hpp"
#include "../interfaces/interfaces.hpp"

#if 0
typedef enum
{
	eCmdExecutionMarker_Enable_FCVAR_SERVER_CAN_EXECUTE='a',
	eCmdExecutionMarker_Disable_FCVAR_SERVER_CAN_EXECUTE='b',
	
	eCmdExecutionMarker_Enable_FCVAR_CLIENTCMD_CAN_EXECUTE='c',
	eCmdExecutionMarker_Disable_FCVAR_CLIENTCMD_CAN_EXECUTE='d'
}       ECmdExecutionMarker;
#endif

enum
{
	NUMBER_INVALID = -471239523,
	NUMBER_INT = 0,
	NUMBER_FLOAT,
};

static std::vector<std::string> split(std::string text, const std::string& separator)
{
	std::vector<std::string> tokens;

	size_t pos = 0;
	std::string token;

	while ((pos = text.find(separator)) != std::string::npos)
	{
		token = text.substr(0, pos);
		tokens.push_back(token);
		text.erase(0, pos + separator.length());
	}

	// last token
	tokens.push_back(text);

	return tokens;
}

template<typename T>
static bool convert_to_number(const std::string& number, T& value)
{
	auto [p, errorcode] = std::from_chars(number.data(), number.data() + number.size(), value);
	return errorcode == std::errc{};
}

static int find_number_type(const std::string& text)
{
	if (text.empty())
		return NUMBER_INVALID;

	// 1.5; 1.6; 1000.0 = float
	return text.find(".") ? NUMBER_FLOAT : NUMBER_INT;
}

// look for: Cbuf_AddText buffer overflow
INIT_HOOK_OFFSET(AddText, void, (const char* text), "engine.so", "48 89 4D C0 0F 29 45 B0 C7 45 C8 F2 00 00 00", 0x3C)
{
	//this doesn't have support for strings, shit
	if (std::string_view full_text{text}; full_text.starts_with("set_convar "))
	{
		make_log(std::format("Ran set_convar, full text -> {}", text));

		auto tokens = split(text, " ");

		make_log(std::format("set_convar tokens -> {}", tokens.size()));

		// ["set_convar", "sv_cheats", "1"]
		if (tokens.size() < 3)
			return;

		const std::string& cvar_text = tokens[1];

		make_log(std::format("Cvar: {} -> {}", cvar_text, tokens[2]));

		ConVar* cvar = g_enginecvar->FindVar(cvar_text.c_str());

		if (!cvar)
		{
			make_log(std::format("Couldn't find cvar '{}'", cvar_text));
			return;
		}

		const std::string& cvar_value = tokens[2];

		int value_type = find_number_type(cvar_value);

		if (value_type == NUMBER_INVALID)
		{
			make_log("Invalid number!");
			return;
		}

		if (value_type == NUMBER_INT)
		{
			int value = 0;
			
			if (!convert_to_number<int>(cvar_value, value))
			{
				make_log("Couldn't convert to cvar value integer!");
				return;
			}

			cvar->SetValue(value);
		}
		else
		{
			float value = 0.0f;
			
			if (!convert_to_number<float>(cvar_value, value))
			{
				make_log("Couldn't convert to cvar value integer!");
				return;
			}

			cvar->SetValue(value);
		}

		return;
	}

	original_AddText(text);
}

#if 0
INIT_HOOK_OFFSET(AddTextWithMarkers, void, (ECmdExecutionMarker left, const char* text, ECmdExecutionMarker right), "engine.so", "89 95 98 FB FF FF 0F 8F", 0x2B)
{
	original_AddTextWithMarkers(left, text, right);

	LOG("Cbuf_AddTextWithMarkers called, text: {}", text);
}
#endif