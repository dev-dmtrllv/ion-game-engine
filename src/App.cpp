#include "App.hpp"
#include "ScriptManager.hpp"
#include "js/Utils.hpp"
#include "Logger.hpp"

namespace ion
{
	App::Version App::Version::fromString(std::string_view str)
	{
		std::size_t versions[3] = { 0, 0, 0 };
		std::size_t versionIndex = 0;
		std::string buffer;

		for (const char c : str)
		{
			if (c == '.')
			{
				versions[versionIndex++] = std::atoi(buffer.c_str());
				buffer.clear();
			}
			else
			{
				buffer += c;
			}
		}

		if (buffer.size() > 0)
			versions[versionIndex++] = std::atoi(buffer.c_str());

		return App::Version(versions[0], versions[1], versions[2]);
	}

	std::optional<App> App::app_;

	std::optional<App::Config> App::Config::fromPath(std::string_view path)
	{
		return ScriptManager::runStandAlone<App::Config>([&](v8::Isolate* isolate, v8::Local<v8::Context>& context)
		{
			std::filesystem::path packagePath = std::filesystem::path(path) / "package.json";

			Logger::get().debug("Loading ", packagePath.string(), "...");

			if (!std::filesystem::exists(packagePath))
				throw std::runtime_error("Could not find package.json!");

			std::ifstream file(packagePath, std::ios::binary);
			std::vector<char> fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			fileContents.emplace_back('\0');

			auto data = js::unwrap(v8::String::NewFromUtf8(isolate, fileContents.data()));
			auto value = js::unwrap(v8::JSON::Parse(context, data));

			if (!value->IsObject())
				throw std::runtime_error("Config is not an object!");

			auto json = value.As<v8::Object>();

			auto nameValue = js::getFromObject(isolate, json, "name");
			auto versionValue = js::getFromObject(isolate, json, "version");
			auto entryValue = js::getFromObject(isolate, json, "main");

			if (nameValue->IsUndefined())
				throw std::runtime_error("package.name is undefined!");
			else if (!nameValue->IsString())
				throw std::runtime_error("package.name is not a string!");

			if(entryValue->IsUndefined())
				throw std::runtime_error("packge.main is undefined!");

			std::string name =  js::toString(isolate, nameValue);
			std::string version = js::toString(isolate, versionValue);
			std::string main = js::toString(isolate, entryValue);

			return App::Config(std::string(path), std::move(name), App::Version::fromString(version), std::move(main));
		});
	}

	App& App::getInstance()
	{
		if (!app_.has_value()) [[unlikely]]
			throw std::runtime_error("Cannot call App::getInstance outside an App::scope()!");

			return app_.value();
	}

	App::App(Config&& config, ConstructToken&& _) :
		config(config)
	{
		auto& logger = Logger::get();

		if(config.version.major == 0 && config.version.minor == 0 && config.version.patch == 0)
			logger.warn("Invalid package version!");
		
		std::filesystem::path root = config.rootPath;

		if(!std::filesystem::exists(root / config.entry))
			throw std::runtime_error("Cannot find entry script!");
		
		Logger::get().debug("App ", config.name, " ", config.version.string, " loaded!");
	}

	App::~App()
	{
		Logger::get().debug("Unloading ", config.name, "...");
	}
}
