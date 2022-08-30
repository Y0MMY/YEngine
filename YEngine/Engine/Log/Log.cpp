#include "log.h"

FILE* Log::m_file;
Log* Log::m_instance;
Log Log::_log;

Log::Log()
{
	if (!m_instance)
	{
		m_file = nullptr;
		m_instance = this;
		m_init();
	}
	else
	{
		Err( "Log alreay created" );
	}
}

Log::~Log()
{
	m_close();
	m_instance = nullptr;
}

void Log::m_init()
{
	setlocale(LC_ALL, "rus");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	if (fopen_s(&m_file, LOGNAME, "w") == 0)
	{
		char timer[9];
		_strtime_s(timer, 9);
		char date[9];
		_strdate_s(date, 9);
		fprintf(m_file, "Лог создан: %s %s.\n", date, timer);
		fprintf(m_file, "---------------------------------------\n\n");
	}
	else
	{
		printf("Ошибка при создании файла лога...\n");
		m_file = nullptr;
	}
#ifdef IS_DEBUG

	FILE* conin = stdin;
	FILE* conout = stdout;
	FILE* conerr = stderr;
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen_s(&conin, "CONIN$", "r", stdin);
	freopen_s(&conout, "CONOUT$", "w", stdout);
	freopen_s(&conerr, "CONOUT$", "w", stderr);
	SetConsoleTitle("YOMMY Engine");
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)
	{
		GetLastError();
	}

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode))
	{
		GetLastError();
	}

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(hOut, dwMode))
	{
		GetLastError();
	}
#endif // DEBUG

}

void Log::m_close()
{
	if (!m_file)
		return;

	char timer[9];
	_strtime_s(timer, 9);
	char date[9];
	_strdate_s(date, 9);
	fprintf(m_file, "\n---------------------------------------\n");
	fprintf(m_file, "Конец лога: %s %s.", date, timer);
	fclose(m_file);
}

void Log::Print(const char* message, ...)
{
	va_list args;
	va_start(args, message);
	int len = _vscprintf(message, args) + 1;
	char* buffer = static_cast<char*>(malloc(len * sizeof(char)));
	vsprintf_s(buffer, len, message, args);
	m_print("", buffer);
	va_end(args);
	free(buffer);
}

void Log::Debug(const char* message, ...)
{
#ifdef _DEBUG
	va_list args;
	va_start(args, message);
	int len = _vscprintf(message, args) + 1;
	char* buffer = static_cast<char*>(malloc(len * sizeof(char)));
	vsprintf_s(buffer, len, message, args);
	m_print("\u001b[34m[DEBUG]: ", buffer);
	va_end(args);
	free(buffer);
#endif
}

void Log::Info(const char* message, ...)
{
#ifdef _DEBUG
	va_list args;
	va_start(args, message);
	int len = _vscprintf(message, args) + 1;
	char* buffer = static_cast<char*>(malloc(len * sizeof(char)));
	vsprintf_s(buffer, len, message, args);
	m_print("\u001b[32m[INFO]: ", buffer);
	va_end(args);
	free(buffer);
#endif
}

void Log::Err(const char* message, ...)
{
	va_list args;
	va_start(args, message);
	int len = _vscprintf(message, args) + 1;
	char* buffer = static_cast<char*>(malloc(len * sizeof(char)));
	vsprintf_s(buffer, len, message, args);
	m_print("\u001B[31m[ERROR]: ", buffer);
	va_end(args);
	free(buffer);
}

void Log::m_print(const char* levtext, const char* text)
{
	char timer[9];
	_strtime_s(timer, 9);
	clock_t cl = clock();

#ifdef IS_DEBUG

	printf("%s%s\n", levtext, text);
#endif // IS_DEBUG

	if (m_file)
	{
		fprintf(m_file, "%s::%d: %s%s\n", timer, cl, levtext, text);
		fflush(m_file);
	}
}
