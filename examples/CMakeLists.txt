set(EXAMPLES
  filter_ints
  unpack_apply)

function(make_example target)
  string(CONCAT target_src ${target} ".cpp")
  add_executable(${target} ${target_src})
  target_link_libraries(${target} PRIVATE ${PROJECT_NAME} catch)
endfunction()


foreach(e ${EXAMPLES})
  make_example(${e})
endforeach()