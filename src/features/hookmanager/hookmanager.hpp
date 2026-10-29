// Created by tevin on 29/10/2026

#ifndef JACI_HOOKMANAGER_HPP
#define JACI_HOOKMANAGER_HPP

class CHookManager
{
public:
	static void Initialize();
	static void* AddHook(void* hookedfn, const char* dll, const char* pattern, int offset);
	static void* AddSymbolHook(void* hookedfn, const char* dll, const char* name); // name exception because it looks better than AdddlsymHook or AddDLSYMHook
	static void Install(); // install all hooks
	static void Shutdown();
};

#endif