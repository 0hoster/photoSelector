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
#include <QWidget>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGuiApplication>
#include <QList>
#include <QPalette>
#include <QColor>
#include <QImage>

class MainWindow : public QMainWindow {
Q_OBJECT

private:
    struct Cate {
        QString content;
        int count;
        bool isLocked;
    };
    // File System
    QDir root;
    QFileInfoList fileInfoList;
    QFileInfoList::iterator currentFile;
    // UI
    QColor labelColor;
    QLabel *statusLabel;
    QWidget *mainWidget;
    QFormLayout *mainLayout;
    QFormLayout *categoryLayout;
    // Category
    QList<Cate> categories;
    QList<Cate>::iterator currentCate;
    const static int LABEL_HEIGHT = 30;
    int categoryMax = 0;

    void initUI();

    void initSlots();

    void setSuitableScreenSize();

    void updateInfo();

    void updateCategory();

    void setLabelColor();

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
