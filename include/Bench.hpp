#pragma once

#include "pch.hpp"

namespace ion::Bench
{
	inline double time(auto callback)
	{
		auto start = std::chrono::system_clock::now();
		callback();
		auto end = std::chrono::system_clock::now();
		return std::chrono::duration<double, std::milli>(end - start).count();
	}
}
