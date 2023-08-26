#pragma once

#include "pch.hpp"
#include "Logger.hpp"

namespace ion
{
	class App
	{
	private:
		static std::optional<App> app_;

		struct ConstructToken
		{
			explicit ConstructToken() = default;
		};

	public:
		struct Version
		{
			static Version fromString(std::string_view str);

			Version(std::size_t major, std::size_t minor, std::size_t patch) :
				major(major),
				minor(minor),
				patch(patch),
				string(std::format("{}.{}.{}", major, minor, patch))
			{}

			const std::size_t major;
			const std::size_t minor;
			const std::size_t patch;
			const std::string string;
		};

		struct Config
		{
			static std::optional<Config> fromPath(std::string_view path);

			Config(std::string&& rootPath, std::string&& name, Version&& version, std::string&& entry) :
				rootPath(rootPath),
				name(name),
				version(version),
				entry(entry)
			{}

			const std::string rootPath;
			const std::string name;
			const Version version;
			const std::string entry;
		};

		template<typename Scope>
		static void scoped(std::string_view appPath, std::optional<Config>&& config, Scope scope)
		{
			if (!app_.has_value())
			{
				if (!config.has_value())
					config.emplace(std::string(appPath), "App", App::Version(0, 0, 0), "index.js");

				app_.emplace(std::move(config.value()), ConstructToken());

				scope(app_.value());
				app_.reset();
			}
			else
			{
				scope(app_.value());
			}
		}

		template<typename Scope>
		static void scoped(std::string_view appPath, Scope scope)
		{
			scoped(appPath, Config::fromPath(appPath), scope);
		}

		static [[nodiscard]] App& getInstance();

		App(Config&& config, ConstructToken&& token);
		App(const App&) = delete;
		App(App&&) = delete;
		~App();

		const Config config;

	};
}
