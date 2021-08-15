cmake_minimum_required(VERSION 3.5)

project(lan64)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/externals/cmake-modules/")

set(CMAKE_INSTALL_MESSAGE LAZY)

# Generate revision information
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REF_SPEC GIT_REV)
git_describe(GIT_DESC --always --long --dirty)
git_branch_name(GIT_BRANCH)
git_get_exact_tag(GIT_TAG)

set(GIT_REVISION_FILE "${CMAKE_CURRENT_BINARY_DIR}/src/build_info.cpp")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/build_info.cpp.in" "${GIT_REVISION_FILE}" @ONLY)


add_library(lan64
        ${GIT_REVISION_FILE}
        src/build_info.hpp
        src/resource_handle.hpp)

target_include_directories(lan64 PRIVATE src)
target_include_directories(lan64 PUBLIC include)
