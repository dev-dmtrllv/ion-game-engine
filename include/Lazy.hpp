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

		[[nodiscard]] static consteval std::size_t bufferSize() noexcept
		{
			constexpr std::size_t a = sizeof(T);
			constexpr std::size_t b = sizeof(Initializer);
			return (a > b ? a : b) / sizeof(char);
		}

		static void initDtor(Self& self) noexcept
		{
			self.buffer<Initializer>()->~Initializer();
		}

		static void evaluatedDtor(Self& self) noexcept
		{
			self.buffer<T>()->~T();
		}

		[[nodiscard]] static T& initGetter(Self& self)
		{
			Initializer* init = self.buffer<Initializer>();
			T* valuePtr = self.buffer<T>();
			new (valuePtr) T((*init)());
			self.getter_ = evaluatedGetter;
			self.dtor_ = evaluatedDtor;
			return *valuePtr;
		}

		[[nodiscard]] static T& evaluatedGetter(Self& self)
		{
			return *self.buffer<T>();
		}

	public:
		Lazy(const Lazy<T>& other)
		{
			memcpy(buffer_, other.buffer_, bufferSize());
			getter_ = other.getter_;
			dtor_ = other.dtor_;
		}

		Lazy(Lazy<T>&& other)
		{
			memcpy(buffer_, other.buffer_, bufferSize());

			getter_ = std::move(other.getter_);
			dtor_ = std::move(other.dtor_);
		}

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

		[[nodiscard]] T& operator()()
		{
			return getter_(*this);
		}

		[[nodiscard]] Lazy<T>& operator=(const Lazy<T>& other)
		{
			memcpy(buffer_, other.buffer_, bufferSize());

			getter_ = other.getter_;
			dtor_ = other.dtor_;

			return *this;
		}

		[[nodiscard]] Lazy<T>& operator=(Lazy<T>&& other)
		{
			memcpy(buffer_, other.buffer_, bufferSize());

			getter_ = std::move(other.getter_);
			dtor_ = std::move(other.dtor_);

			return *this;
		}

		[[nodiscard]] Lazy<T>& operator=(const T& value)
		{
			reset(value);
			return *this;
		}

		[[nodiscard]] Lazy<T>& operator=(T&& value)
		{
			reset(std::forward<T>(value));
			return *this;
		}

		[[nodiscard]] Lazy<T>& operator=(const Initializer& initializer)
		{
			reset(initializer);
			return *this;
		}

		[[nodiscard]] Lazy<T>& operator=(Initializer&& initializer)
		{
			reset(std::forward<Initializer>(initializer));
			return *this;
		}

		void reset(const T& value)
		{
			dtor_(*this);
			new (buffer<T>()) T(std::forward<T>(value));
			getter_ = evaluatedGetter;
		}

		void reset(T&& value)
		{
			dtor_(*this);
			new (buffer<T>()) T(std::forward<T>(value));
			getter_ = evaluatedGetter;
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
			dtor_(*this);

			memset(buffer_, 0, bufferSize());

			*(buffer<Initializer>()) = initializer;

			getter_ = initGetter;
		}

		template<typename... Args>
		void reset(Args&&... args)
		{
			dtor_(*this);
			new (buffer<T>()) T(std::forward<Args>(args)...);
			getter_ = evaluatedGetter;
		}

	private:
		template<typename ReturnType>
		ReturnType* buffer() noexcept { return static_cast<ReturnType*>(static_cast<void*>(buffer_)); }

		Getter getter_;
		Dtor dtor_;
		char buffer_[bufferSize()];
	};
}
