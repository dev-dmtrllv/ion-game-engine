#include "pch.hpp"

#include "Logger.hpp"
#include "Lazy.hpp"

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

		const std::size_t lambdaSize = sizeof(std::function<int()>);

		ion::Logger::scoped(logPath, [](ion::Logger& logger)
		{
			struct Test
			{
				Test(int x) : x(x) {};

				const std::string log() const { return std::format("Test {{ x: {} }}", x); }

				int x;
			};

			ion::Lazy<Test> lazy([&] { return Test(123); });

			logger.info(lazy());

			lazy = 4;
			logger.info(lazy());

			lazy = [&] { return Test(456); };
			logger.info(lazy());
		});

		return 0;
	}
	catch (const std::runtime_error& e)
	{
		MessageBoxA(NULL, e.what(), "Runtime Error", MB_OK);
		return 1;
	}
}
