enable_testing()

add_executable(test_rubric tst_rubric.cpp)
add_test(NAME test_rubric COMMAND test_rubric)
target_link_libraries(test_rubric PRIVATE Repertorization Qt6::Test)


add_executable(test_repmodel tst_repmodel.cpp)
add_test(NAME test_repmodel COMMAND test_repmodel)
target_link_libraries(test_repmodel PRIVATE Repertorization Qt6::Test)
