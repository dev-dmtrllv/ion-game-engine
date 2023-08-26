#include "js/Utils.hpp"

namespace ion::js
{
	std::string toString(v8::Isolate* isolate, v8::Local<v8::Value> value)
	{
		v8::String::Utf8Value utf8(isolate, value->ToString(isolate->GetCurrentContext()).ToLocalChecked());
		return std::string(*utf8);
	}

	v8::Local<v8::Value> getFromObject(v8::Isolate* isolate, v8::Local<v8::Object> obj, const std::string& str)
	{
		return obj->Get(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, str.c_str()).ToLocalChecked()).ToLocalChecked();
	}
}
