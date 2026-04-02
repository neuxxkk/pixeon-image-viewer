#include <QApplication>
#include "view/MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Pixeon Image Viewer");
    app.setApplicationVersion("1.0");

    app.setWindowIcon(QIcon(":/app_icon.png"));

    MainWindow window;
    window.show();

    return app.exec();
}