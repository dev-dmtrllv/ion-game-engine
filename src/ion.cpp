#include "pch.hpp"

#include "Logger.hpp"
#include <strsafe.h>

std::filesystem::path getLogPath()
{
	WCHAR path[MAX_PATH];
	GetModuleFileNameW(NULL, path, MAX_PATH);

	std::filesystem::path logPath(path);
#ifdef _DEBUG
	logPath = (logPath / "../../..").lexically_normal();
#endif
	return logPath / "../logs";
}

namespace ion::Console
{
	struct Scope
	{
		Scope(const std::wstring& title)
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

		Scope(const Scope&) = delete;
		Scope(Scope&&) = delete;

		~Scope()
		{
#ifdef _DEBUG
			FreeConsole();
#endif
		}
	};
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	try
	{
		const ion::Console::Scope scope(L"Ion Debug Console");

		const std::filesystem::path logPath = getLogPath();

		ion::Logger::scoped(logPath, [](ion::Logger& logger)
		{
			throw std::runtime_error("Exception test :D");
		});

		return 0;
	}
	catch(const std::runtime_error& e)
	{
		MessageBoxA(NULL, e.what(), "Runtime Error", MB_OK);
		return 1;
	}
}
