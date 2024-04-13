#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent, const QString &rootPath)
        : QMainWindow(parent) {
    statusLabel = new QLabel;
    mainWidget = new QWidget;
    mainLayout = new QFormLayout;
    categoryLayout = new QFormLayout;
    labelColor = QColor(Qt::black);

    for (int i = 0; i < 100; ++i) {
        categories.push_back(Cate{QString::asprintf("%2d", i), 0, false});
    }
    // Test data

    setWindowTitle("Photos Selector - " + rootPath);
    initValues(rootPath);
    initFont();
    initUI();
    initSlots();
    setSuitableScreenSize();
    updateCateShortcut();
    updateCategory();
//    setLabelColor();
//    updateLabelColor();
}

MainWindow::~MainWindow() {
    delete mainWidget->layout();
    delete mainWidget;
}

void MainWindow::initValues(const QString &rootPath) {
    root.setPath(rootPath);
    for (const auto &file: root.entryInfoList()) {
        QString fileName = file.fileName().toLower();
        if (fileName.endsWith(".png") || fileName.endsWith(".jpg") || fileName.endsWith(".jpeg") ||
            fileName.endsWith(".bmp")) {
            fileInfoList.push_back(file);
        }
    }
    currentFile = fileInfoList.begin();

    assert(strlen(frontKeys) <= keyMax && strlen(backKeys) <= keyMax);

    categoryMax = (int) std::strlen(frontKeys) * std::strlen(backKeys);

    categoryEnd = (int) categories.size() / categoryMax;
    if (categories.size() % categoryMax == 0) {
        categoryEnd = qMax(categoryEnd - 1, 0);
    }
}

void MainWindow::initUI() {
    categoryLayout->setVerticalSpacing(5);
    categoryLayout->setHorizontalSpacing(5);
    for (int i = 0; i < categoryMax; ++i) {
        auto *itemLabel = new QLabel();
        auto *item = new QLabel();
        itemLabel->setFont(labelFont);
        item->setFont(labelFont);
        categoryLayout->addRow(itemLabel, item);
    }
    statusLabel->setText(currentFile->fileName());

    mainLayout->addWidget(statusLabel);
    mainLayout->addItem(categoryLayout);

    mainWidget->setLayout(mainLayout);
    this->setCentralWidget(mainWidget);
}

void MainWindow::initFont() {
    labelFont = QFont("JetBrains Mono");
}

void MainWindow::initSlots() {
    auto *imageNext = new QShortcut(QKeySequence(Qt::Key_Right), this);
    connect(imageNext, &QShortcut::activated, this, [&]() {
        // Next image
        ++currentFile;
        if (currentFile == fileInfoList.end())currentFile = fileInfoList.begin();
        emit currentImageChange();
    });

    auto *imagePrevious = new QShortcut(QKeySequence(Qt::Key_Left), this);
    connect(imagePrevious, &QShortcut::activated, this, [&]() {
        // Previous image
        if (currentFile == fileInfoList.begin())currentFile = fileInfoList.end();
        --currentFile;
        emit currentImageChange();
    });
    auto *imageTop = new QShortcut(QKeySequence(Qt::Key_Up), this);
    connect(imageTop, &QShortcut::activated, this, [&]() {
        // First image
        currentFile = fileInfoList.begin();
        emit currentImageChange();
    });

    auto *imageBottom = new QShortcut(QKeySequence(Qt::Key_Down), this);
    connect(imageBottom, &QShortcut::activated, this, [&]() {
        // Last image
        currentFile = fileInfoList.end() - 1;
        emit currentImageChange();
    });


    auto *catePreviousPage = new QShortcut(QKeySequence(Qt::Key_K), this);
    connect(catePreviousPage, &QShortcut::activated, this, [&]() {
        --currentCatePage;
        if (currentCatePage <= -1)currentCatePage = categoryEnd;
        updateCategory();
    });

    auto *cateNextPage = new QShortcut(QKeySequence(Qt::Key_J), this);
    connect(cateNextPage, &QShortcut::activated, this, [&]() {
        ++currentCatePage;
        if (currentCatePage > categoryEnd)currentCatePage = 0;
        updateCategory();
    });

    auto *cateTopPage = new QShortcut(QKeySequence(Qt::Key_K | Qt::SHIFT), this);
    connect(cateTopPage, &QShortcut::activated, this, [&]() {
        currentCatePage = 0;
        updateCategory();
    });

    auto *cateBottomPage = new QShortcut(QKeySequence(Qt::Key_J | Qt::SHIFT), this);
    connect(cateBottomPage, &QShortcut::activated, this, [&]() {
        currentCatePage = categoryEnd;
        updateCategory();
    });

    auto *cateNew = new QShortcut(QKeySequence(Qt::Key_N), this);
    connect(cateNew, &QShortcut::activated, this, [&]() {
        createNewCate();
        updateCategory();
        updateLabelColor();
    });

    auto *windowClose = new QShortcut(QKeySequence(Qt::Key_Q), this);
    connect(windowClose, &QShortcut::activated, this, [&]() {
        this->close();
    });

    auto *windowToggleFullscreen = new QShortcut(QKeySequence(Qt::Key_F), this);
    connect(windowToggleFullscreen, &QShortcut::activated, this, [&]() {
        // Toggle Fullscreen
        if (isFullScreen()) {
            showNormal();
        } else {
            showFullScreen();
        }
    });

    connect(this, &MainWindow::currentImageChange, this, &MainWindow::updateInfo);
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.drawPixmap(0, 0, loadPixmap(currentFile->absoluteFilePath()));
    event->accept();
}

QPixmap MainWindow::loadPixmap(const QString &filename) {
    QImageReader reader(filename);
    reader.setAutoTransform(true);
    QSize imageSize = reader.size();

    QSize scaledSize = imageSize.scaled(this->width(), this->height(), Qt::KeepAspectRatio);
    reader.setScaledSize(scaledSize);
    return QPixmap::fromImageReader(&reader);
}

void MainWindow::setSuitableScreenSize() {
    QImageReader reader;
    int h = 0, w = 0;
    for (const auto &file: fileInfoList) {
        reader.setFileName(file.absoluteFilePath());
        h = qMax(h, reader.size().height());
        w = qMax(w, reader.size().width());
    }
    if (h >= 1080 && w >= 1920) {
        showFullScreen();
    } else {
        setBaseSize(h, w);
    }
}

void MainWindow::updateInfo() {
    statusLabel->setText(currentFile->fileName());
    updateLabelColor();
    setLabelColor();
    update();
}

void MainWindow::updateCateShortcut() {
    int index = 0;
    for (int i = 0; i < strlen(frontKeys); ++i) {
        for (int j = 0; j < strlen(backKeys); ++j) {
            setCategoryLabelAt(index, QString::asprintf("%c%c", frontKeys[i], backKeys[j]));
            ++index;
        }
    }
}

void MainWindow::updateCategory() {
    long long total = qMin(categoryMax, categories.size() - currentCatePage * categoryMax - 1);
    assert(total > 0);
    int itemIndex = (int) categoryMax * currentCatePage;

    for (int i = 0; i < total; ++i) {
        setCategoryAt(i, categories[i + itemIndex].content);
    }
    for (int i = (int) total; i < categoryMax; ++i) {
        setCategoryAt(i, "");
    }
}

void MainWindow::setCategoryAt(int index, const QString &content, const QString &label) {
    // Cate at 0 -> label[0*2], contentItem[0*2+1]
    auto *contentItem = qobject_cast<QLabel *>(categoryLayout->itemAt(index * 2 + 1)->widget());
    assert(contentItem != nullptr);
    contentItem->setText(content);
    if (label != nullptr) {
        setCategoryLabelAt(index, label);
    }
}

void MainWindow::setCategoryLabelAt(int index, const QString &label) {
    auto *labelItem = qobject_cast<QLabel *>(categoryLayout->itemAt(index * 2)->widget());
    assert(labelItem != nullptr);
    labelItem->setText(label);
}

void MainWindow::setLabelColor() {
    QImage image = loadPixmap(currentFile->absoluteFilePath()).toImage();
    int height = image.height() >> 1;
    int width = image.width() >> 1;
    long long all = 0;
    int count = 0;
    for (int i = 0; i < width && i < this->width(); i += qMax(width >> 4, 1)) {
        for (int j = 0; j < height && j < this->height(); j += qMax(height >> 4, 1)) {
            all += image.pixelColor(i, j).red();
            all += image.pixelColor(i, j).green();
            all += image.pixelColor(i, j).blue();
            ++count;
        }
    }
    all /= count * 3;
    if (all > 150) {
        labelColor = Qt::black;
    } else {
        labelColor = Qt::white;
    }
}

void MainWindow::createNewCate() {
    bool ok = false;
    QString newCate = QInputDialog::getText(this, "New Category", "What 's the new category? 😝", \
    QLineEdit::Normal, "", &ok, Qt::FramelessWindowHint);
    if (ok) {
        categories.push_back(Cate{newCate, 0, false});
        updateCategory();
    }
}

void MainWindow::updateLabelColor() {
    QString selectAll = "QLabel {color:rgb(%1,%2,%3);}";
    QString selectCur = "QLabel[current=true] {color:rgb(%1,%2,%3);}";
    QString style = selectAll.arg(labelColor.red()).arg(labelColor.green()).arg(labelColor.blue()) + \
                    selectCur.arg(currentColor.red()).arg(currentColor.green()).arg(currentColor.blue());
    mainWidget->setStyleSheet(style);
}


