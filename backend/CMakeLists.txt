cmake_minimum_required(VERSION 4.4)
project(nymm_backend)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif()
# Drogon Build
set(BUILD_ORM ON)
set(BUILD_SQLITE ON)
set(BUILD_POSTGRESQL OFF)
set(BUILD_MYSQL OFF)
set(BUILD_REDIS OFF)

add_subdirectory(third_party/drogon)

# TagLib
find_package(PkgConfig REQUIRED)
pkg_check_modules(TAGLIB REQUIRED taglib)

# PROJECT [LIBRARY] MODULE
add_library(library
   src/library/files.cpp
   src/library/library.cpp
   src/library/sql.cpp
)

target_include_directories(library
   PUBLIC
      ${CMAKE_SOURCE_DIR}/src/include   # library.h
   PRIVATE
      ${CMAKE_SOURCE_DIR}/src/library   # library_internal.h
      ${TAGLIB_INCLUDE_DIRS}
)

target_link_libraries(library
   PRIVATE
      ${TAGLIB_LIBRARIES}
      Drogon::Drogon
)

# MAIN
add_executable(backend src/main.cpp)
target_link_libraries(backend 
   PRIVATE 
      Drogon::Drogon 
      library
)
