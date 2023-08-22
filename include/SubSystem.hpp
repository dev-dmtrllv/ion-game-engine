#pragma once

#include "pch.hpp"

namespace ion
{
	class ISubSystem
	{

	};

	template<typename T>
	concept IsSubSystem = std::is_base_of<ISubSystem, T>;

	template<IsSubSystem T>
	class SubSystem : public ISubSystem
	{
	public:
		void initialize() { static_cast<T*>(this)->onInitialize(); }
		void terminate() { static_cast<T*>(this)->onTerminate(); }

	protected:
		virtual void onInitialize() {  };
		virtual void onTerminate() {  };
	};
}
