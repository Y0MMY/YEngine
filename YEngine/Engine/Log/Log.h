#pragma once
#include <Windows.h>
#include <clocale>
#include <stdio.h>
#include <sstream>


#define LOGNAME "log.txt"

class Log
{
public:
	Log();
	~Log();

	static Log* Get() { return m_instance; }

	static void Print(const char* message, ...);
	static void Info(const char* message, ...);
	static void Debug(const char* message, ...);
	static void Err(const char* message, ...);
	static Log& log()
	{
		return _log;
	}

private:
	static Log* m_instance;

	void m_init();
	void m_close();
	static void m_print(const char* levtext, const char* text);

	static FILE* m_file;
	static Log _log; 
};
