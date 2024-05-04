#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "welcome.h"
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
#include <QMessageBox>
#include <QMultiHash>
#include <QRgb>

typedef Welcome::Image Image;
typedef Welcome::ImageList ImageList;

class MainWindow : public QMainWindow {
Q_OBJECT

private:
    struct Cate {
        QString content;
        int count;
        bool isLocked;
    };
    // File System
    ImageList fileInfoList;
    ImageList::iterator currentFile;

    // UI
    QColor selectedColor = Qt::red;
    QLabel *statusLabel;
    QWidget *mainWidget;
    QFormLayout *mainLayout;
    QFormLayout *categoryLayout;

    // Category
    QMultiHash<QByteArray, QString> imageToCategory;
    QHash<QChar, Qt::Key> toKeys;
    QList<Cate> categories;
    const static int keyMax = 16;
    QList<Qt::Modifier> frontKeys = {Qt::CTRL, Qt::SHIFT, Qt::ALT, Qt::MODIFIER_MASK};
    QString backKeys = "asdf";

    int categoryEnd = 0;
    int currentCatePage = 0;
    int categoryPerPage = 0;

    void createNewCate();

    void addCate(const QString &newCate, bool isLook = false);

    [[nodiscard]] int itemIndex() const { return categoryPerPage * currentCatePage; }

    [[nodiscard]] int totalCateToDisplay() const {
        int total = qMin(categoryPerPage, int(categories.size()) - currentCatePage * categoryPerPage);
        assert(total >= 0);
        return total;
    }

    void keySelect(int x, int y);

    void initValues();

    void initUI();

    void initSlots();

    void setSuitableScreenSize();

    void setCategoryLabelAt(int index, const QString &label);

    void setCategoryAt(int index, const QString &content, const QString &label = nullptr);

    void updateInfo();

    void updateCategory();

    void updateCategoryColor();

    void updateCateShortcutDisplay();

    // Pixmap
    QImage currentImage;

    QPixmap loadPixmap(const QString &filename);

protected:
    void paintEvent(QPaintEvent *event) override;

public:
    explicit MainWindow(QWidget *parent = nullptr, const ImageList &rootPath = QList<Welcome::Image>{});

    ~MainWindow() override;

signals:

    void currentImageChange();

    void selectedCategoryChange();
};

#endif // MAINWINDOW_H
