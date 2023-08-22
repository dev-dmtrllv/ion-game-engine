#include "pch.hpp"

#include "Logger.hpp"
#include <strsafe.h>

std::filesystem::path getLogPath()
{
	std::filesystem::path cwd = std::filesystem::current_path();
#ifdef _DEBUG
	cwd = (cwd / "../../../").lexically_normal();
#endif
	return cwd / "logs";
}

void createConsole(const std::wstring& title = L"Ion Console")
{
	FILE* conin = stdin;
	FILE* conout = stdout;
	FILE* conerr = stderr;

	if (!AttachConsole(ATTACH_PARENT_PROCESS))
		AllocConsole();

	freopen_s(&conin, "CONIN$", "r", stdin);
	freopen_s(&conout, "CONOUT$", "w", stdout);
	freopen_s(&conerr, "CONOUT$", "w", stderr);

	SetConsoleTitle(title.c_str());
}

void destroyConsole()
{
	FreeConsole();
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	createConsole();

	const std::filesystem::path logPath = getLogPath();

	ion::Logger::scoped(logPath, [](ion::Logger& logger)
	{
		logger.debug("Hello, World! :)");
		logger.info("INFO TEST");
		logger.fatal("FATAL TEST");
		logger.warn("Warning test");
		logger.error("Error test");
	});

	destroyConsole();
	
	return 0;
}
