#pragma once

#include "pch.hpp"
#include <tlhelp32.h>

namespace ion
{
	class Process
	{
	private:
		std::unordered_map<int, int> processes_;

	public:
		static std::size_t getParentID(std::size_t pid)
		{
			HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			PROCESSENTRY32 pe = { 0 };
			pe.dwSize = sizeof(PROCESSENTRY32);

			if (Process32First(h, &pe))
			{
				do
				{
					if (pe.th32ProcessID == pid)
					{
						CloseHandle(h);
						return pe.th32ParentProcessID;
					}
				}
				while (Process32Next(h, &pe));
			}
			
			CloseHandle(h);
			return 0;
		}

		static void fromID(std::size_t pid)
		{
			
		}

		static Process& current()
		{
			static Process currentProcess;
			return currentProcess;
		}

		Process();

		const std::size_t pid;
		const std::size_t ppid;

	private:
	};
}
