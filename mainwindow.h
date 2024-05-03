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
#include <QInputDialog>
#include <QShortcut>
#include <cstring>
#include <QMessageBox>

class MainWindow : public QMainWindow {
Q_OBJECT

private:
    struct Cate {
        QString content;
        int count;
        bool isLocked;
    };
    // File System
    QFileInfoList fileInfoList;
    QFileInfoList::iterator currentFile;
    // UI
    QColor labelColor;
    QColor currentColor = Qt::red;
    QLabel *statusLabel;
    QWidget *mainWidget;
    QFormLayout *mainLayout;
    QFormLayout *categoryLayout;
    // Category
    void createNewCate();
    void addCate(const QString &newCate,bool isLook = false);
    const static int keyMax = 16;
    QList<Qt::Modifier> frontKeys = {Qt::CTRL, Qt::SHIFT, Qt::ALT,Qt::MODIFIER_MASK};
    char backKeys[keyMax + 1] = "asdf";
    QList<Cate> categories;
    int categoryEnd = 0;
    int currentCatePage = 0;
    int categoryPerPage = 0;
    const static int LABEL_HEIGHT = 30;
    // Maps
    [[nodiscard]] int itemIndex() const{return categoryPerPage * currentCatePage;}
    QHash<char, Qt::Key> toKeys;

    void keySelect(int x, int y);

    void initValues(const QStringList &rootPath);

    void initUI();

    void initFont();

    void initSlots();

    void setSuitableScreenSize();

    void updateInfo();

    void updateCateShortcutDisplay();

    void setCategoryLabelAt(int index, const QString &label);

    void setCategoryAt(int index, const QString &content, const QString &label = nullptr);

    void updateCategory();

    QPixmap loadPixmap(const QString &filename);

protected:
    void paintEvent(QPaintEvent *event) override;

public:
    explicit MainWindow(QWidget *parent = nullptr, const QStringList &rootPath = QStringList());

    ~MainWindow() override;

signals:

    void currentImageChange();

    void categoryEndChange();
};

#endif // MAINWINDOW_H
