#pragma once

#include "pch.hpp"


namespace ion
{
	struct Console
	{
		Console(const std::wstring& title);

		Console(const Console&) = delete;
		Console(Console&&) = delete;

		~Console();
	};
}
