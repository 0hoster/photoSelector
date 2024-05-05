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
#include <QRgb>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

typedef QString ImageName;
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
    QColor colorfulGrey = QColor(210, 122, 45);
    QColor blackGrey = QColor(51, 51, 51);
    QColor whiteGrey = QColor(217, 217, 217);
    QLabel *statusLabel;
    QWidget *mainWidget;
    QFormLayout *mainLayout;
    QFormLayout *categoryLayout;

    // Category
    QHash<ImageName , QSet<QString>> imageToCategory;
    QHash<QChar, Qt::Key> toKeys;
    QList<Cate> categories;
    const static int keyMax = 16;
    QList<Qt::Modifier> frontKeys = {Qt::MODIFIER_MASK,Qt::CTRL, Qt::ALT, Qt::SHIFT,  };
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

    void updateCategoryColorStatus();

    void updateCategoryColor();

    void updateCateShortcutDisplay();

    void finalize();

    // Pixmap

    QImage loadScaledImage(QSize size,const QString& filename = "");

protected:
    void paintEvent(QPaintEvent *event) override;

public:
    explicit MainWindow(QWidget *parent = nullptr, const ImageList &rootPath = ImageList {});

    ~MainWindow() override;

signals:

    void currentImageChange();

    void selectedCategoryChange();
};

#endif // MAINWINDOW_H
