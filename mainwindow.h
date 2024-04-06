#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QLabel>
#include <QPushButton>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDir>
#include <QPaintEvent>
#include <QStatusBar>
#include <QPainter>
#include <QPixmap>
#include <QImageReader>
#include <QSize>
#include <QKeyEvent>

class MainWindow : public QMainWindow {
Q_OBJECT

private:
    QLabel *statusLabel;
    QDir root;
    QFileInfoList images;
    QFileInfoList::iterator current;

    void initUI();

    void initSlots();

    void setSuitableScreenSize();

    void updateInfo();

    QPixmap loadPixmap(const QString &filename);

protected:
    void paintEvent(QPaintEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

public:
    explicit MainWindow(QWidget *parent = nullptr, const QString &rootPath = "");

    ~MainWindow() override;

signals:

    void currentImageChange();
};

#endif // MAINWINDOW_H
