find_program(LIT_COMMAND lit)

message(STATUS "lit command: ${LIT_COMMAND}")

add_custom_target(test-sea-cut
  COMMAND ${LIT_COMMAND} -v ${CMAKE_CURRENT_SOURCE_DIR} --path=${CMAKE_BINARY_DIR}
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  DEPENDS sea-cut
  )

