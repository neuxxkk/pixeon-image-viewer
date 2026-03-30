find_package(Qt6 REQUIRED COMPONENTS Core Test)

add_executable(PixeonTests tst_placeholder.cpp)

target_link_libraries(PixeonTests
    PRIVATE
        Qt6::Core
        Qt6::Test
)

enable_testing()
add_test(NAME PixeonTests COMMAND PixeonTests)