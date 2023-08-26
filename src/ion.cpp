#include "pch.hpp"

#include "Logger.hpp"
#include "Lazy.hpp"
#include "Console.hpp"
#include "Process.hpp"
#include "App.hpp"
#include "ScriptManager.hpp"
#include "Utils.hpp"


std::vector<std::string> getArgs()
{
	auto argvStr = std::string(GetCommandLineA());
	std::vector<std::string> args;
	std::string buffer;
	for (char c : argvStr)
	{
		if (c == ' ')
		{
			if (buffer.size() > 0)
			{
				args.emplace_back(buffer);
				buffer.clear();
			}
		}
		else
		{
			buffer += c;
		}
	}
	if (buffer.size() > 0)
		args.emplace_back(buffer);

	return args;
}

std::filesystem::path getAppPath(const std::vector<std::string>& args)
{
	std::filesystem::path p;
	
	if (args.size() > 1)
	{
		p = args.at(1);

		if (p.is_relative())
		{
			if (std::filesystem::exists(std::filesystem::current_path() / p))
				p = std::filesystem::current_path() / p;
			else
				p = std::filesystem::path(args.at(0)) / ".." / p;
		}
	}
	else
	{
		p = std::filesystem::path(args.at(0)) / "..";
	}

	p = p.lexically_normal();

	if(!std::filesystem::exists(p))
	{
		std::string err = std::format("App path \"{}\"does not exists!", p.string());
		throw std::runtime_error(err.c_str());
	}

	return p;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	try
	{
		using namespace ion;

		const Console console(L"Ion Debug Console");

		const std::vector<std::string> args = getArgs();
		const std::filesystem::path appPath = getAppPath(args);
		
		const std::filesystem::path logPath = appPath / "logs";

		Logger::scoped(logPath, [&](Logger& logger)
		{
			logger.info("Found app path: ", appPath);
			
			App::scoped(appPath.string(), [](const App& app)
			{
				
			});
		});

		system("pause");

		return 0;
	}
	catch (const std::runtime_error& e)
	{
		MessageBoxA(NULL, e.what(), "Runtime Error", MB_OK);
		return 1;
	}
	catch (const std::exception& e)
	{
		MessageBoxA(NULL, e.what(), "Unknown Exception", MB_OK);
		return 1;
	}
}
