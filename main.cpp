#include "mainwindow.h"

#include <QApplication>
#include <QString>
#include <QFont>
#include <QFontDatabase>
#include "welcome.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    // init font
    int id = QFontDatabase::addApplicationFont(":/font/LXGWWenKai-Regular.ttf");
    if (id == -1) {
        qDebug("Cannot open custom font ?!");
    } else {
        QString family = QFontDatabase::applicationFontFamilies(id).at(0);
        QFont font(family);
        QApplication::setFont(font);
    }

    Welcome welcomeDialog;
    if (welcomeDialog.exec() == QDialog::Accepted) {
        MainWindow m(nullptr, welcomeDialog.getRootDir());
        m.show();
        return QApplication::exec();
    }
    return 0;
}
