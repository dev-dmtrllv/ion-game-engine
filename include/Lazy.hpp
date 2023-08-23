#pragma once

#include "pch.hpp"

namespace ion
{
	template<typename T>
	class Lazy
	{
	private:
		using Initializer = std::function<T()>;
		using Self = Lazy<T>;
		using Getter = T & (*)(Self&);
		using Dtor = void(*)(Self&);

		static consteval std::size_t bufferSize()
		{
			constexpr std::size_t a = sizeof(T);
			constexpr std::size_t b = sizeof(Initializer);
			return (a > b ? a : b) / sizeof(char);
		}

		static void initDtor(Self& self)
		{
			Initializer* init = self.buffer<Initializer>();
			init->~Initializer();
		}

		static void evaluatedDtor(Self& self)
		{
			T* init = self.buffer<T>();
			init->~T();
		}

		static T& initGetter(Self& self)
		{
			Initializer* init = self.buffer<Initializer>();
			T* valuePtr = self.buffer<T>();
			new (valuePtr) T((*init)());
			self.getter_ = evaluatedGetter;
			self.dtor_ = evaluatedDtor;
			return *valuePtr;
		}

		static T& evaluatedGetter(Self& self)
		{
			return *self.buffer<T>();
		}

	public:
		Lazy(const Lazy<T>& other) = delete;
		Lazy(Lazy<T>&&) = delete;

		Lazy(const Initializer& initializer)
		{
			memset(buffer_, 0, bufferSize());
			
			*buffer<Initializer>() = initializer;

			dtor_ = initDtor;
			getter_ = initGetter;
		}
		
		Lazy(Initializer&& initializer)
		{
			memset(buffer_, 0, bufferSize());
			
			*buffer<Initializer>() = std::forward<Initializer>(initializer);

			dtor_ = initDtor;
			getter_ = initGetter;
		}

		~Lazy()
		{
			dtor_(*this);
		}

		T& operator()()
		{
			return getter_(*this);
		}

		Lazy<T>& operator=(const T& value)
		{
			reset(value);
			return *this;
		}

		Lazy<T>& operator=(T&& value)
		{
			reset(std::forward<T>(value));
			return *this;
		}

		Lazy<T>& operator=(const Initializer& initializer)
		{
			reset(initializer);
			return *this;
		}

		Lazy<T>& operator=(Initializer&& initializer)
		{
			reset(std::forward<Initializer>(initializer));
			return *this;
		}

		void reset(const T& value)
		{
			dtor_(*this);
			// memset(buffer_, 0, bufferSize());
			new (buffer<T>()) T(std::forward<T>(value));
		}

		void reset(T&& value)
		{
			dtor_(*this);
			// memset(buffer_, 0, bufferSize());
			new (buffer<T>()) T(std::forward<T>(value));
		}

		void reset(Initializer&& initializer)
		{
			dtor_(*this);

			memset(buffer_, 0, bufferSize());

			*(buffer<Initializer>()) = std::forward<Initializer>(initializer);

			dtor_ = initDtor;
			getter_ = initGetter;
		}

		void reset(const Initializer& initializer)
		{
			memset(buffer_, 0, bufferSize());

			*(buffer<Initializer>()) = initializer;

			getter_ = initGetter;
		}

	private:
		template<typename T>
		T* buffer() { return static_cast<T*>(static_cast<void*>(buffer_)); }

		Getter getter_;
		Dtor dtor_;
		char buffer_[bufferSize()];
	};
}
