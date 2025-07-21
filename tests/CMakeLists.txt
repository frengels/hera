set(
  TESTS
  accumulate
  all_view
  any_of_all_of
  collect_into
  common_reference
  drop_view
  element_type
  enumerate_view
  filter_view
  find_if
  for_each
  get
  head_view
  integer_sequence
  iota_view
  make_from_range
  move_view
  nth_element
  optional
  pair
  ref_view
  reorder_view
  size
  sort
  string
  transform_view
  tuple
  tuple_array
  type_
  type_in_pack
  type_list
  unpack
  unreorder_view
  zip_view
  )

set (DESTRUCTURE_TEST OFF)

if (${DESTRUCTURE_TEST})
  list(APPEND ${TESTS} destructure)
endif ()

find_package(Catch2 REQUIRED)

add_library(catch STATIC catch.cpp)
target_link_libraries(catch PUBLIC Catch2::Catch2WithMain)

function(make_test target)
  string(CONCAT target_src ${target} ".cpp")
  add_executable(${target} ${target_src})
  target_link_libraries(${target} PRIVATE ${PROJECT_NAME} catch)
  add_test(NAME ${target} COMMAND ${target})
endfunction()

foreach(t ${TESTS})
  make_test(${t})
endforeach()
