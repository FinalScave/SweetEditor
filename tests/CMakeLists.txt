set(TEST_PRODUCT_NAME test)
add_definitions(-DCATCH_AMALGAMATED_CUSTOM_MAIN=ON)
add_executable(${TEST_PRODUCT_NAME}
        ${3DPARTY_DIR}/include/catch2/catch_amalgamated.cpp
        tests_main.cpp
)

target_include_directories(${TEST_PRODUCT_NAME} PRIVATE
        ${3DPARTY_DIR}/include
        ${SRC_DIR}/include
)

target_link_libraries(${TEST_PRODUCT_NAME} PRIVATE
        sweeteditor
)

enable_testing()
add_test(NAME UnitTests COMMAND ${TEST_PRODUCT_NAME})