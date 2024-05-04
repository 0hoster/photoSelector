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
#include <QSet>
#include <QCryptographicHash>
#include <QMessageBox>

class Welcome : public QDialog {
Q_OBJECT

private:
    QString rootFolder;
    QStringList imagesResult;
    QLabel *imageLabel;
    QLabel *welcomeLabel;
    QLabel *tipLabel;
    QSet<QByteArray> containedFile;

    void initUI();

    void initSlots();

    void chooseRootFolder();

    bool repeatCheck(QDir files);

protected:
    void keyPressEvent(QKeyEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

public:
    explicit Welcome(QWidget *parent = nullptr);

    ~Welcome() override;

    QStringList getRootDir();
};

#endif // WELCOME_H
