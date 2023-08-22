#pragma once

#include "pch.hpp"

#undef ERROR

namespace ion
{
	template<class T>
	concept IsStringLike = std::is_convertible_v<T, std::string_view>;

	template<typename T>
	concept IsLoggable = requires(T loggable)
	{
		{ loggable.log() } -> IsStringLike;
	};

	template<typename T>
	concept IsException = std::is_base_of<std::exception, T>::value;

	template<IsLoggable T>
	std::ostream& operator<<(std::ostream& out, const T& loggable)
	{
		return out << loggable.log();
	}

	template<IsException T>
	std::ostream& operator<<(std::ostream& out, const T& e)
	{
		return out << e.what();
	}

	class Logger
	{
	public:
		enum class Level
		{
			INFO,
			DEBUG,
			WARNING,
			ERROR,
			FATAL
		};

	private:
		struct Message
		{
			Message(Level level, std::string&& msg);
			void log(std::fstream& file) const;

			Level level;
			std::string msg;
			std::chrono::system_clock::time_point time;
		};

		struct CtorTag
		{};

	public:
		/**
		 * @brief This will create a scoped logger which will be diposed at the end of scope
		 *
		 * @tparam ScopeCallback
		 * @param path The path where the logger should create its log files
		 * @param scope The scope in which the logger is available
		 */
		template<typename ScopeCallback>
		static void scoped(std::string_view path, ScopeCallback scope)
		{
			Logger& logger = init(path);

			try
			{
				scope(logger);
			}
			catch (const std::exception& e)
			{
				logger.fatal("Uncaught Exception: ", e);
				std::rethrow_exception(std::current_exception());
			}

			dispose();
		}

		/**
		 * @brief This will create a scoped logger which will be diposed at the end of scope
		 *
		 * @tparam ScopeCallback
		 * @param path The path where the logger should create its log files
		 * @param scope The scope in which the logger is available
		 */
		template<typename ScopeCallback>
		static void scoped(const std::filesystem::path& path, ScopeCallback scope)
		{
			const std::string p = path.string();
			scoped(std::string_view(p), scope);
		}

		static Logger& get();

	private:
		static std::optional<Logger> instance_;
		static std::array<const WORD, 5> colors_;
		static const WORD resetColor_;
		static std::array<const char*, 5> levels_;

		static Logger& init(std::string_view path);
		static void dispose();

	public:
		Logger(std::string_view path, CtorTag tag);
		~Logger();

		template<typename... Ts>
		void info(Ts&&... args)
		{
			std::string msg = stringer(std::forward<Ts>(args)...);
			send(Level::INFO, std::move(msg));
		}

		template<typename... Ts>
		void debug(Ts&&... args)
		{
#ifdef _DEBUG
			std::string msg = stringer(std::forward<Ts>(args)...);
			send(Level::DEBUG, std::move(msg));
#endif
		}

		template<typename... Ts>
		void warn(Ts&&... args)
		{
			std::string msg = stringer(std::forward<Ts>(args)...);
			send(Level::WARNING, std::move(msg));
		}

		template<typename... Ts>
		void fatal(Ts&&... args)
		{
			std::string msg = stringer(std::forward<Ts>(args)...);
			send(Level::FATAL, std::move(msg));
		}

		template<typename... Ts>
		void error(Ts&&... args)
		{
			std::string msg = stringer(std::forward<Ts>(args)...);
			send(Level::ERROR, std::move(msg));
		}

	private:
		template<typename... Ts>
		std::string stringer(Ts&&... args)
		{
			std::ostringstream stream;
			using List = int[];

			(void)List
			{
				0, ((void)(stream << args), 0)...
			};

			return stream.str();
		}

		void send(Level level, std::string&& msg)
		{
			std::unique_lock<std::mutex> lock(mutex_);
			queue_.emplace(level, std::forward<std::string>(msg));
			cv_.notify_one();
		}

		void threadHandler();

		std::atomic_bool running_;
		std::atomic_bool isReady_;
		std::fstream file_;
		std::queue<Message> queue_;
		std::condition_variable cv_;
		std::mutex mutex_;
		std::thread thread_;
	};
} // namespace ion
