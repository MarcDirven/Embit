cmake_minimum_required(VERSION 3.14)

project(EmbitTests LANGUAGES CXX)

set(EMBIT_CATCH_VERSION "2.13.6" CACHE STRING "Version of Catch2 to use for testing")
Include(FetchContent)
FetchContent_Declare(
		Catch2
		GIT_REPOSITORY https://github.com/catchorg/Catch2.git
		GIT_TAG v${EMBIT_CATCH_VERSION})
FetchContent_MakeAvailable(Catch2)

# ---- Import root project ----
option(TEST_INSTALLED_VERSION "Import the library using find_package" OFF)
if (TEST_INSTALLED_VERSION)
	find_package(embit REQUIRED CONFIG)
else ()
	# Enable warnings from includes
	set(embit_INCLUDE_WITHOUT_SYSTEM ON CACHE INTERNAL "")

	include(FetchContent)
	FetchContent_Declare(embit SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
	FetchContent_MakeAvailable(embit)
endif ()

# ---- Tests ----
add_executable(EmbitTests
		Map.cpp
		Main.cpp
		)

target_compile_features(EmbitTests PRIVATE cxx_std_11)

target_compile_options(EmbitTests
		PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wpedantic -Wextra -Wall -Wno-unused-function>)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
	include(CheckCXXCompilerFlag)
	check_cxx_compiler_flag(-Wa,-mbig-obj "HAS_MBIG_OBJ")
	if (${HAS_MBIG_OBJ})
		add_definitions("-Wa,-mbig-obj")
	endif ()
endif ()

target_link_libraries(EmbitTests PRIVATE embit::embit Catch2::Catch2)

enable_testing()

add_test(NAME EmbitTests COMMAND EmbitTests)
