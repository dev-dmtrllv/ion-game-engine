#pragma once

#include "pch.hpp"

namespace ion::Benchmark
{
	template<typename T, typename Ratio = std::milli>
	inline T time(auto callback)
	{
		auto start = std::chrono::system_clock::now();
		callback();
		auto end = std::chrono::system_clock::now();
		return std::chrono::duration<T, Ratio>(end - start).count();
	}
}
