foreach(TEST trianglemesh_test trianglestrip_test trianglestripifier_test)
  add_executable(${TEST} ${TEST}.cpp)
  target_link_libraries (${TEST} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} tristrip)
  add_test(${TEST} ${TEST})
endforeach()

