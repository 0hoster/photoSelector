#ifndef WELCOME_H
#define WELCOME_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QPixmap>
#include <QString>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QFileDialog>
#include <QDir>
#include <QFont>
#include <QPalette>

class Welcome : public QDialog {
Q_OBJECT

private:
    QString rootFolder;
    QLabel *imageLabel;
    QLabel *welcomeLabel;
    QLabel *tipLabel;

    void initUI();

    void initSlots();

    void chooseRootFolder();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event)override;

public:
    explicit Welcome(QWidget *parent = nullptr);
    ~Welcome() override;
    QString getRootDir();
};

#endif // WELCOME_H
