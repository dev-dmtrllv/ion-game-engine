#include "pch.hpp"
#include "Logger.hpp"

namespace ion
{
	Logger::Message::Message(Logger::Level level, std::string&& msg) :
		level(level),
		msg(std::forward<std::string>(msg)),
		time(std::chrono::system_clock::now())
	{}

	void Logger::Message::log(std::fstream& file) const
	{
		auto t = std::chrono::system_clock::to_time_t(time);
		auto s = std::localtime(&t);
		char buf[29] = { '\0' };
		std::strftime(buf, 21, "%Y-%m-%dT %H:%M:%S", s);

		const std::size_t index = static_cast<std::size_t>(level);
		auto levelString = levels_[index];
		
#ifdef _DEBUG
		auto color = colors_[index];
		
		printf("%s ", buf);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
		printf("[%s]", levelString);

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), resetColor_);
		printf(" %s\n", msg.data());
#endif

		file << std::format("{} [{}] {}\n", buf, levelString, msg.data());
	}

	std::optional<Logger> Logger::instance_;

	std::array<const WORD, 5> Logger::colors_ = {
		3, // INFO
		1, // DEBUG
		6, // WARN
		4, // ERROR
		12, // FATAL
	};

	std::array<const char*, 5> Logger::levels_ = {
		"INFO",
		"DEBUG",
		"WARN",
		"ERROR",
		"FATAL",
	};

	const WORD Logger::resetColor_ = 0xF;

	// std::array<const char*, 5> Logger::colors_ = {
	// 	"\033[34m",
	// 	"\033[30m",
	// 	"\033[33m",
	// 	"\033[31m",
	// 	"\033[31m",
	// };

	// const char* Logger::resetColor_ = "\033[39m\033[49m";

	Logger& Logger::init(std::string_view path)
	{
		namespace Fs = std::filesystem;

		if (instance_.has_value())
			throw std::runtime_error("Logger is already initialized");

		auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		auto s = std::localtime(&t);
		char buf[29] = { '\0' };

		std::strftime(buf, 21, "%Y-%m-%d", s);

		Fs::path fsPath = Fs::path(path);

		if (Fs::is_regular_file(fsPath))
		{
			std::string msg = std::format("Cannot use a file as a log directory! Tried to use {}!", fsPath.string());
			throw std::runtime_error(msg.c_str());
		}

		if (!Fs::exists(fsPath))
			Fs::create_directory(fsPath);

		Fs::path dir = fsPath / buf;

		if (Fs::is_directory(fsPath))
		{
			std::size_t version = 0;

			Fs::path check = dir.string() + ".log";

			while (Fs::exists(check))
			{
				version++;
				check = dir.string() + "_" + std::to_string(version) + ".log";
			}

			std::string p = check.string();
			return instance_.emplace(p, CtorTag{});
		}

		return instance_.emplace(path, CtorTag{});
	}

	Logger& Logger::get()
	{
		assert(instance_.has_value());
		return instance_.value();
	}

	void Logger::dispose()
	{
		assert(instance_.has_value());
		instance_.reset();
	}

	Logger::Logger(std::string_view path, [[maybe_unused]] CtorTag tag) :
		running_(true),
		file_(path.data(), std::ios_base::ate | std::ios_base::out),
		queue_(),
		cv_(),
		mutex_(),
		thread_([&] { threadHandler(); })
	{
		while (!isReady_.load(std::memory_order::acquire)) continue;
	}

	Logger::~Logger()
	{
		running_.store(false, std::memory_order::release);

		cv_.notify_one();
		thread_.join();

		while (queue_.size() > 0)
		{
			auto msg = queue_.front();
			msg.log(file_);
			queue_.pop();
		}
	}

	void Logger::threadHandler()
	{
		isReady_.store(true, std::memory_order::release);

		while (running_.load(std::memory_order::acquire))
		{
			if (queue_.size() == 0)
			{
				std::unique_lock lock(mutex_);
				cv_.wait(lock, [&] { return !running_.load(std::memory_order::acquire) || queue_.size() > 0; });
			}

			while (queue_.size() > 0)
			{
				std::unique_lock lock(mutex_);
				queue_.front().log(file_);
				queue_.pop();
			}
		}
	}
} // namespace ion
