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

typedef QFileInfo Image;
typedef QList<QFileInfo> ImageList;
typedef QSet<QByteArray> ImageSet;

class Welcome : public QDialog {
Q_OBJECT

private:
    QString rootFolder;
    ImageList imagesResult;
    QLabel *imageLabel;
    QLabel *welcomeLabel;
    QLabel *tipLabel;
    ImageSet containedFile;

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

    ImageList getRootDir() { return imagesResult; };

    ImageSet getImageSet() { return containedFile; };
};

#endif // WELCOME_H
