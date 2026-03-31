set(SOURCES 
    main.cpp
    view/MainWindow.h
    view/MainWindow.cpp
    view/ImageViewWidget.h
    view/ImageViewWidget.cpp
    model/ImageModel.h
    model/ImageModel.cpp
)


qt_add_executable(PixeonImageViewer ${SOURCES})

target_link_libraries(PixeonImageViewer
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)