#pragma once

#include "pch.hpp"

namespace ion::js
{
	std::string toString(v8::Isolate* isolate, v8::Local<v8::Value> value);

	v8::Local<v8::Value> getFromObject(v8::Isolate* isolate, v8::Local<v8::Object> obj, const std::string& str);

	template<typename T>
	v8::Local<T> unwrap(v8::MaybeLocal<T> value)
	{
		v8::Local<T> val;
		if (!value.ToLocal(&val))
		{
			std::string str = std::format("Could not unwrap {}!", typeid(T).name());
			throw std::runtime_error(str.c_str());
		}
		return val;
	}
}
