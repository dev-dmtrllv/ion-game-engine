cmake_minimum_required (VERSION 3.8)

project (ion)

file(GLOB ion_srcs CONFIGURE_DEPENDS "include/*.h" "src/*.cpp" "include/js/*.h" "src/js/*.cpp")

add_executable (ion WIN32 ${ion_srcs})

target_precompile_headers(ion PUBLIC include/pch.hpp)

include_directories(ion include)

target_link_libraries(ion opengl32)

if(MSVC)
	add_definitions(-DUNICODE -D_UNICODE)
	add_compile_options(/W4 /WX)
else()
	add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

set_property(TARGET ion PROPERTY CXX_STANDARD 23)


if (WIN32)
	include_directories(ion SYSTEM ${PROJECT_SOURCE_DIR}/dependencies/includes/v8)

	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    	set(V8_TYPE "debug")
	else()
		set(V8_TYPE "release")
	endif()

	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
	add_definitions(-D_ITERATOR_DEBUG_LEVEL=0 -DV8_COMPRESS_POINTERS)
	STRING (REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	STRING (REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
	STRING (REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		target_link_libraries(ion ${PROJECT_SOURCE_DIR}/dependencies/win/v8/x64.${V8_TYPE}/obj/v8_monolith.lib)
		target_link_libraries(ion ${PROJECT_SOURCE_DIR}/dependencies/win/v8/x64.${V8_TYPE}/obj/v8_libbase.lib)
		target_link_libraries(ion ${PROJECT_SOURCE_DIR}/dependencies/win/v8/x64.${V8_TYPE}/obj/v8_libplatform.lib)
		target_link_libraries(ion ${PROJECT_SOURCE_DIR}/dependencies/win/v8/x64.${V8_TYPE}/obj/v8_snapshot.lib)
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		target_link_libraries(ion ${PROJECT_SOURCE_DIR}/dependencies/win/v8/x86.${V8_TYPE}/obj/v8_monolith.lib)
		target_link_libraries(ion ${PROJECT_SOURCE_DIR}/dependencies/win/v8/x86.${V8_TYPE}/obj/v8_libbase.lib)
		target_link_libraries(ion ${PROJECT_SOURCE_DIR}/dependencies/win/v8/x86.${V8_TYPE}/obj/v8_libplatform.lib)
		target_link_libraries(ion ${PROJECT_SOURCE_DIR}/dependencies/win/v8/x86.${V8_TYPE}/obj/v8_snapshot.lib)
	endif()
endif (WIN32)
