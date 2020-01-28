set(
  TESTS
  size
  get
  common_reference
  tuple_array
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
  make_from_range
  type_identity
  any_of_all_of
  zip_view
  accumulate
  integer_sequence
  reorder_view
  sort
  ref_view
  find_if
  optional
  drop_view
  enumerate_view
  unreorder_view
  element_type
  )

find_package(Catch2 REQUIRED)

add_library(catch STATIC catch.cpp)
target_link_libraries(catch PUBLIC Catch2::Catch2)

function(make_test target)
  string(CONCAT target_src ${target} ".cpp")
  add_executable(${target} ${target_src})
  target_link_libraries(${target} PRIVATE ${PROJECT_NAME} catch)
  add_test(NAME ${target} COMMAND ${target})
endfunction()

foreach(t ${TESTS})
  make_test(${t})
endforeach()
