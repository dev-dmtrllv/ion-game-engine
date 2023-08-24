cmake_minimum_required(VERSION 3.8)

project(ion)

file(GLOB ion_srcs CONFIGURE_DEPENDS "include/*.h" "src/*.cpp" "include/js/*.h" "src/js/*.cpp")

add_executable(ion WIN32 ${ion_srcs})

target_precompile_headers(ion PUBLIC include/pch.hpp)

include_directories(ion include)

target_link_libraries(ion opengl32)

include_directories(ion SYSTEM "dependencies/includes/v8")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(V8_BUILD_TYPE "debug")
else()
	set(V8_BUILD_TYPE "release")
endif()

if(MSVC)
	add_definitions(
		-DUNICODE
		-D_UNICODE
		-D_CRT_SECURE_NO_WARNINGS
		-D_ITERATOR_DEBUG_LEVEL=0
		-DV8_COMPRESS_POINTERS
	)

	add_compile_options(
		/W4
		/WX
		/analyze
	)

	string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
	string(REPLACE "/MDd" "/MTd" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

	target_link_libraries(ion ${PROJECT_SOURCE_DIR}/dependencies/win/v8/x64.${V8_BUILD_TYPE}/obj/v8_monolith.lib
		${PROJECT_SOURCE_DIR}/dependencies/win/v8/x64.${V8_BUILD_TYPE}/obj/v8_libbase.lib
		${PROJECT_SOURCE_DIR}/dependencies/win/v8/x64.${V8_BUILD_TYPE}/obj/v8_libplatform.lib
		${PROJECT_SOURCE_DIR}/dependencies/win/v8/x64.${V8_BUILD_TYPE}/obj/v8_snapshot.lib
	)
else()
	add_compile_options(-Wall -Wextra -Wpedantic -Werror -fsanitize=undefined -fsanitize=address -fsanitize-undefined-trap-on-error)
endif()

set_property(TARGET ion PROPERTY CXX_STANDARD 23)
