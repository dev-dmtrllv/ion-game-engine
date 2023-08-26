#pragma once

#include "pch.hpp"

namespace ion
{
	class Process;

	class Process
	{
	public:
#ifdef _WIN32
		using Handle = HANDLE;
#endif

	private:
		static std::size_t getParentID(std::size_t pid);
		static std::string getExecPath(Handle handle);

	public:
		static Process current();

		Process(std::size_t pid);
		~Process();

		const std::size_t pid;
		const std::size_t ppid;

	private:
#ifdef _WIN32
		const HANDLE handle_;
#endif

	public:
		const std::string execPath;
	};
}
