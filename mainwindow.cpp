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

    root.setPath(rootPath);
    for (const auto &file: root.entryInfoList()) {
        QString fileName = file.fileName().toLower();
        if (fileName.endsWith(".png") || fileName.endsWith(".jpg") || fileName.endsWith(".jpeg") ||
            fileName.endsWith(".bmp")) {
            fileInfoList.push_back(file);
        }
    }
    currentFile = fileInfoList.begin();
    setWindowTitle("Photos Selector - " + rootPath);
    initUI();
    initSlots();
    setSuitableScreenSize();
    updateCategory();
    setLabelColor();
    updateLabelColor();

    connect(this, &MainWindow::currentImageChange, this, &MainWindow::updateInfo);
}

MainWindow::~MainWindow() {
    delete mainWidget->layout();
    delete mainWidget;
}

void MainWindow::initUI() {

    for (int i = 0; i < categoryMax; ++i) {
        auto *item = new QLabel();
        categoryLayout->addWidget(item);
    }
    statusLabel->setText(currentFile->fileName());

    mainLayout->addWidget(statusLabel);
    mainLayout->addItem(categoryLayout);

    mainWidget->setLayout(mainLayout);
    this->setCentralWidget(mainWidget);
}

void MainWindow::initSlots() {

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

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (processToggleImage(event->key())) {
        emit currentImageChange();
        update();
        event->accept();
    } else if (processToggleCate(event->key())) {
        emit currentCateChange();
        updateCategory();
        updateLabelColor();
    } else {
        processToggleWindow(event->key());
    }
}

bool MainWindow::processToggleImage(int key) {
    switch (key) {
        case Qt::Key_Right:
            // Next image
            ++currentFile;
            if (currentFile == fileInfoList.end())currentFile = fileInfoList.begin();
            break;
        case Qt::Key_Left:
            // Present image
            if (currentFile == fileInfoList.begin())currentFile = fileInfoList.end();
            --currentFile;
            break;
        case Qt::Key_Up:
            // First image
            currentFile = fileInfoList.begin();
            break;
        case Qt::Key_Down:
            // Last image
            currentFile = fileInfoList.end() - 1;
            break;
        default:
            return false;
    }
    return true;
}

bool MainWindow::processToggleWindow(int key) {
    switch (key) {
        case Qt::Key_Q:
            // Quit
            this->close();
            break;
        case Qt::Key_F:
            // Toggle Fullscreen
            if (isFullScreen()) {
                showNormal();
            } else {
                showFullScreen();
            }
            break;
        default:
            return false;
    }
    return true;
}

bool MainWindow::processToggleCate(int key) {
    switch (key) {
        case Qt::Key_N:
            // Add category
            createNewCate();
            break;
        case Qt::Key_K:
            --currentCate;
            if (currentCate == -1)currentCate = categories.size() - 1;
            break;
        case Qt::Key_J:
            ++currentCate;
            if (currentCate == categories.size())currentCate = 0;
            break;
        default:
            return false;
    }
    return true;
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
    setLabelColor();
}

void MainWindow::updateCategory() {
    int itemIndex = 0;

    // Display atLeast
    for (auto i = qMax(currentCate - 4, 0); i < currentCate; ++i) {
        auto *item = qobject_cast<QLabel *>(categoryLayout->itemAt(itemIndex++)->widget());
        assert(item != nullptr);
        item->setProperty("current", false);
        item->setText(categories[i].content);
    }
    // Display current
    auto *current = qobject_cast<QLabel *>(categoryLayout->itemAt(itemIndex++)->widget());
    assert(current != nullptr);
    current->setProperty("current", true);
    current->setText(categories[currentCate].content);
    // Display next
    for (auto i = currentCate + 1; itemIndex < categoryMax && i < categories.size(); ++i) {
        auto *item = qobject_cast<QLabel *>(categoryLayout->itemAt(itemIndex++)->widget());
        assert(item != nullptr);
        item->setProperty("current", false);
        item->setText(categories[i].content);
    }
    // Cleaning
    while (itemIndex < categoryMax) {
        auto *item = qobject_cast<QLabel *>(categoryLayout->itemAt(itemIndex++)->widget());
        assert(item != nullptr);
        item->setProperty("current", false);
        item->setText("");
    }
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


