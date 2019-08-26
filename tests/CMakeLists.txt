set(
  TESTS
  traits
  size
  at
  get
  begin_end
  iter_move
  common_reference
  tuple_array
  ranges
  subrange
  iota_view
  all_view
  filter_view
  for_each
  unpack
  move_view
  head_view
  pair
  type_list
  tuple
  transform_view
  )

find_package(Catch2 REQUIRED)

add_library(catch STATIC catch.cpp)
target_link_libraries(catch PUBLIC Catch2::Catch2)

function(make_test target)
  string(CONCAT target_src ${target} ".cpp")
  add_executable(${target} ${target_src})
  target_link_libraries(${target} PRIVATE ${CMAKE_PROJECT_NAME} catch)
  add_test(NAME ${target} COMMAND ${target})
endfunction()

foreach(t ${TESTS})
  make_test(${t})
endforeach()
