#pragma once

#include "pch.hpp"


namespace ion::Console
{
	struct Scope
	{
		Scope(const std::wstring& title);

		Scope(const Scope&) = delete;
		Scope(Scope&&) = delete;

		~Scope();
	};
}
