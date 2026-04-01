find_package(Qt6 REQUIRED COMPONENTS Core Test Gui Widgets)

add_executable(PixeonTests 
    tst_placeholder.cpp
    ../src/controller/ImageProcessor.cpp
    ../src/model/ImageModel.cpp
    ../src/model/ImageCollection.cpp
)

target_link_libraries(PixeonTests
    PRIVATE
        Qt6::Core
        Qt6::Test
        Qt6::Gui
        Qt6::Widgets
)

target_include_directories(PixeonTests PRIVATE ../src)

enable_testing()
add_test(NAME PixeonTests COMMAND PixeonTests)