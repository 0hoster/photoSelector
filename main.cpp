#include "mainwindow.h"

#include <QApplication>
#include <QString>
#include <QFont>
#include <QFontDatabase>
#include "welcome.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    // init font
    QFont font("LXGW WenKai");
    QApplication::setFont(font);

    Welcome welcomeDialog;
    if (welcomeDialog.exec() == QDialog::Accepted) {
        MainWindow m(nullptr, welcomeDialog.getRootDir());
        m.show();
        return QApplication::exec();
    }
    return 0;
}
