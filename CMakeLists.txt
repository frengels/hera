cmake_minimum_required(VERSION 3.8)

project(hera VERSION "0.0.1" LANGUAGES CXX)

set(IS_MASTER_PROJECT OFF)

if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
  set(IS_MASTER_PROJECT ON)
endif()

# only enable these targets if we're the master project
option(HERA_TESTS "Build hera's unit tests" ${IS_MASTER_PROJECT})
option(HERA_EXAMPLES "Build hera's examples" ${IS_MASTER_PROJECT})

if (HERA_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if (HERA_EXAMPLES)
  add_subdirectory(examples)
endif()

include(CMakePackageConfigHelpers)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  target_compile_options(${PROJECT_NAME} INTERFACE -Xclang -fconcepts-ts)
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  target_compile_options(${PROJECT_NAME} INTERFACE -fconcepts)
endif()

target_include_directories(
  ${PROJECT_NAME}
  INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

write_basic_package_version_file(
  ${PROJECT_NAME}ConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}Config)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION lib/cmake/${PROJECT_NAME})

install(
  EXPORT ${PROJECT_NAME}Config
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION lib/cmake/${PROJECT_NAME})

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
  DESTINATION .)
