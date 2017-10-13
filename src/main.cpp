#include <QApplication>
#include <QHBoxLayout>
#include <QWidget>
#include <QMainWindow>
#include <QSurfaceFormat>

#include "raceview.h"

/* Matthew Conover
 *
 * Refrences:
 *  C++ refrence (cplusplus.com)
 *  OpenGL documentation
 *  Various stack overflow posts
 *  opengl-tutorial.org
 *  http://stackoverflow.com/questions/5281261/generating-a-normal-map-from-a-height-map
 *
 * Notes:
 * See the included Writeup.pdf file
 *
 * I can't belive the class is over. Wow.
 */
int main(int argc, char** argv) {
    QApplication app(argc, argv);

    // Create the main window and set title.
    QMainWindow mainWindow;
    mainWindow.setWindowTitle("Race v1");

    // Set the OpenGL version and buffer sizes
    QSurfaceFormat format;
    format.setVersion(4, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setAlphaBufferSize(8);
    QSurfaceFormat::setDefaultFormat(format);

    // Container for the OpenGL view
    QHBoxLayout* container = new QHBoxLayout;

    // Uncomment and replace with a class that is a subclass of GLViewQt
    RaceView* glWindow = new RaceView;
    container->addWidget(glWindow);

    // Create a widget, and add the container to the widget
    QWidget* w = new QWidget;
    w->setLayout(container);

    // Set the widget as the main widget for the QMainWindow
    mainWindow.setCentralWidget(w);

    // Make the main window visible
    mainWindow.show();
    return app.exec();
}
