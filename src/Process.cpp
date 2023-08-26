#include "Process.hpp"

namespace ion
{
	std::size_t Process::getParentID(std::size_t pid)
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

	std::string Process::getExecPath(Handle handle)
	{
#ifdef _WIN32
		CHAR filename[MAX_PATH];

		if (GetModuleFileNameExA(handle, NULL, filename, MAX_PATH) == 0)
			return std::string();

		return std::string(filename);
#endif
	}

	Process Process::current()
	{
		return Process(_getpid());
	}

	Process::Process(std::size_t pid) :
		pid(pid),
		ppid(getParentID(pid)),
#ifdef _WIN32
		handle_(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, static_cast<DWORD>(pid))),
#endif
		execPath(getExecPath(handle_))
	{
		if (handle_ == nullptr)
			throw std::runtime_error("Could not get process handle!");
	}

	Process::~Process()
	{
#ifdef _WIN32
		CloseHandle(handle_);
#endif
	}
}
