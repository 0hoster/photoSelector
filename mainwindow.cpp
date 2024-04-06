#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent, const QString &rootPath)
        : QMainWindow(parent) {
    statusLabel = new QLabel;
    mainWidget = new QWidget;
    mainLayout = new QFormLayout;
    categoryLayout = new QFormLayout;
    labelColor = QColor(Qt::black);
    categories.push_back(Cate{"MeiShan", 0, false});

    root.setPath(rootPath);
    for (const auto &file: root.entryInfoList()) {
        QString fileName = file.fileName().toLower();
        if (fileName.endsWith(".png") || fileName.endsWith(".jpg") || fileName.endsWith(".jpeg") ||
            fileName.endsWith(".bmp")) {
            fileInfoList.push_back(file);
        }
    }
    currentFile = fileInfoList.begin();
    currentCate = categories.begin();
    setWindowTitle("Photos Selector - " + rootPath);
    initUI();
    initSlots();
    setSuitableScreenSize();
    updateCategory();

    connect(this, &MainWindow::currentImageChange, this, &MainWindow::updateInfo);
}

MainWindow::~MainWindow() {
    delete mainWidget->layout();
    delete mainWidget;
}

void MainWindow::initUI() {
    int screenHeight = QGuiApplication::primaryScreen()->size().height();
    categoryMax = screenHeight / LABEL_HEIGHT;

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
    if (event->key() == Qt::Key_Right) {
        ++currentFile;
        if (currentFile == fileInfoList.end())currentFile = fileInfoList.begin();
    } else if (event->key() == Qt::Key_Left) {
        if (currentFile == fileInfoList.begin())currentFile = fileInfoList.end();
        --currentFile;
    } else if (event->key() == Qt::Key_Up) {
        currentFile = fileInfoList.begin();
    } else if (event->key() == Qt::Key_Down) {
        currentFile = fileInfoList.end() - 1;
    } else if (event->key() == Qt::Key_Q) {
        this->close();
    } else if (event->key() == Qt::Key_F) {
        if (isFullScreen()) {
            showNormal();
        } else {
            showFullScreen();
        }
    } else {
        return;
    }
    emit currentImageChange();
    update();
    event->accept();
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
    updateCategory();
    setLabelColor();
}

void MainWindow::updateCategory() {
    int canDisplay = this->height() / LABEL_HEIGHT;
    auto needDisplay = categories.end() - currentCate;

    for (int i = 0; i < needDisplay && i < canDisplay && i < categoryMax; ++i) {
        auto *item = qobject_cast<QLabel *>(categoryLayout->itemAt(i)->widget());
        if (item == nullptr)break;
        item->setText((currentCate + i)->content);
    }
}

void MainWindow::setLabelColor() {
    QImage image = loadPixmap(currentFile->absoluteFilePath()).toImage();
    auto lastLayout = categoryLayout->itemAt(categoryLayout->count() - 1);
    auto label = qobject_cast<QLabel *>((lastLayout)->widget());
    auto labelPos = label->pos();
    int height = labelPos.y() + label->minimumHeight();
    int width = labelPos.x() + label->minimumWidth();
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
    QString style = "QLabel {color:%1;}";
    if (all > 180) {
        style = style.arg("black");
    } else {
        style = style.arg("white");
    }
    mainWidget->setStyleSheet(style);

}


