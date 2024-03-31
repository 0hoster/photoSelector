#include "mainwindow.h"

#include <QApplication>
#include "welcome.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Welcome w;
    w.show();
    return QApplication::exec();
}
