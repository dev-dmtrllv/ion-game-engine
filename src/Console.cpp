#include "Console.hpp"

namespace ion
{
	Console::Console(const std::wstring& title)
	{
#ifdef _DEBUG
		FILE* conin = stdin;
		FILE* conout = stdout;
		FILE* conerr = stderr;

		if (!AttachConsole(ATTACH_PARENT_PROCESS))
			AllocConsole();

		freopen_s(&conin, "CONIN$", "r", stdin);
		freopen_s(&conout, "CONOUT$", "w", stdout);
		freopen_s(&conerr, "CONOUT$", "w", stderr);

		SetConsoleTitle(title.c_str());
#endif
	}

	Console::~Console()
	{
#ifdef _DEBUG
		FreeConsole();
#endif
	}
}
