set(SOURCES 
    main.cpp
    model/ImageModel.h
    model/ImageModel.cpp
    model/ImageCollection.h
    model/ImageCollection.cpp
    view/MainWindow.h
    view/MainWindow.cpp
    view/ImageViewWidget.h
    view/ImageViewWidget.cpp
    controller/ImageProcessor.h
    controller/ImageProcessor.cpp
)


qt_add_executable(PixeonImageViewer
    ${SOURCES}
    ../resources/resources.qrc
)

if(WIN32)
    target_sources(PixeonImageViewer PRIVATE ../resources/app.rc)
endif()

target_link_libraries(PixeonImageViewer
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)