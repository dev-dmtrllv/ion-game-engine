#include "pch.hpp"

#include "Logger.hpp"
#include "Lazy.hpp"
#include "Console.hpp"
#include "Process.hpp"

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

void runLazy(ion::Logger& logger);

void run(ion::Logger& logger)
{
	logger.debug("Parent pid: ", ion::Process::getParentID(_getpid()));
	runLazy(logger);
}

void runLazy(ion::Logger& logger)
{
	using namespace ion;

	struct Vector
	{
		Vector(float xyz) :
			x(xyz),
			y(xyz),
			z(xyz)
		{}

		Vector(float x, float y, float z) :
			x(x),
			y(y),
			z(z)
		{}

		std::string log() const
		{
			return std::format("Vector {{ x: {}, y: {}, z: {} }}", x, y, z);
		}

		float x;
		float y;
		float z;
	};

	float xyz = 12345.678f;

	Lazy<Vector> lazy([&]() -> Vector { return xyz; });

	Lazy<Vector> lazy2(std::move(lazy));

	logger.debug(lazy2()); // only now will the lazy vector be calculated

	lazy2.reset(5.0f, 6.0f, 7.0f); // reset the lazy vector with arguments

	logger.debug(lazy2()); // lazy vector is already initialized with the arguments from above
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	try
	{
		using namespace ion;

		const Console::Scope scope(L"Ion Debug Console");

		const std::filesystem::path logPath = getLogPath();

		Logger::scoped(logPath, run);

		system("pause");

		return 0;
	}
	catch (const std::runtime_error& e)
	{
		MessageBoxA(NULL, e.what(), "Runtime Error", MB_OK);
		return 1;
	}
	catch(const std::exception& e)
	{
		MessageBoxA(NULL, e.what(), "Unknown Exception", MB_OK);
		return 1;
	}
}
