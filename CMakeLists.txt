cmake_minimum_required(VERSION 3.14)

project(embit
        VERSION 4.0.0
        DESCRIPTION
        "A fast C++11/14/17/20 header only library for lazy evaluation and function tools"
        HOMEPAGE_URL "https://github.com/MarcDirven/embit"
        LANGUAGES CXX)

# ---- Warning guard ----
# Protect dependents from this project's warnings if the guard isn't disabled
set(embit_warning_guard "SYSTEM")
if(embit_INCLUDE_WITHOUT_SYSTEM)
    set(embit_warning_guard "")
endif()

# ---- Declare library ----
add_library(embit INTERFACE)
add_library(embit::embit ALIAS embit)


target_compile_features(embit INTERFACE cxx_std_11)

target_include_directories(embit
        ${embit_warning_guard}
        INTERFACE
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>")

# ---- Install ----
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(embit_directory "embit-${PROJECT_VERSION}")
set(embit_include_directory "${CMAKE_INSTALL_INCLUDEDIR}/${embit_directory}")

install(TARGETS embit
        EXPORT embitTargets
        INCLUDES
        DESTINATION "${embit_include_directory}")

write_basic_package_version_file(
        embitConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
        ARCH_INDEPENDENT)

set(embit_install_cmakedir "${CMAKE_INSTALL_LIBDIR}/cmake/${embit_directory}")

install(EXPORT embitTargets
        NAMESPACE embit::
        DESTINATION "${embit_install_cmakedir}")

install(FILES
        "${PROJECT_SOURCE_DIR}/cmake/embitConfig.cmake"
        "${PROJECT_BINARY_DIR}/embitConfigVersion.cmake"
        DESTINATION "${embit_install_cmakedir}")

install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
        DESTINATION "${embit_include_directory}")

set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.md")

include(CPack)
