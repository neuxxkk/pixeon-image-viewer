set(SOURCES 
    main.cpp
    view/MainWindow.h
    view/MainWindow.cpp
)


qt_add_executable(PixeonImageViewer ${SOURCES})

target_link_libraries(PixeonImageViewer
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)