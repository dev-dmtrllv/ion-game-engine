#pragma once

#include "pch.hpp"

namespace ion
{
	class ScriptManager
	{
	public:
		template<typename R, typename Callback>
		static R runStandAlone(Callback callback)
		{
			std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
			v8::V8::InitializePlatform(platform.get());
			v8::V8::Initialize();

			v8::Isolate::CreateParams create_params;
			create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
			v8::Isolate* isolate = v8::Isolate::New(create_params);

			auto innerScope = [&]
			{
				v8::Isolate::Scope isolate_scope(isolate);
				v8::HandleScope handle_scope(isolate);
				v8::Local<v8::Context> context = v8::Context::New(isolate);
				v8::Context::Scope context_scope(context);

				return callback(isolate, context);
			};

			R result = innerScope();

			isolate->Dispose();
			v8::V8::Dispose();
			v8::V8::DisposePlatform();
			delete create_params.array_buffer_allocator;

			return result;
		}

		ScriptManager();
		~ScriptManager();
	};
}
