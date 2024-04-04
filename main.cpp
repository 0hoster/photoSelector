#include "mainwindow.h"

#include <QApplication>
#include <QString>
#include "welcome.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Welcome welcomeDialog;
    if (welcomeDialog.exec() == QDialog::Accepted) {
        MainWindow m(nullptr, welcomeDialog.getRootDir());
        m.show();
        return QApplication::exec();
    }
    return 0;
}
