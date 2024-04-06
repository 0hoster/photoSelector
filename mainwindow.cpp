#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent, const QString &rootPath)
        : QMainWindow(parent) {
    statusLabel = new QLabel;
    mainWidget = new QWidget;
    mainLayout = new QFormLayout;
    categoryLayout = new QFormLayout;
    labelColor = QColor(Qt::black);
    categories.push_back(Cate{"MeiShan", 0, false});
    categories.push_back(Cate{"View", 0, false});
    categories.push_back(Cate{"Perfect", 0, false});

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
    switch (event->key()) {
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
        case Qt::Key_N:
            createNewCate();
            break;
        default:
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
    setLabelColor();
}

void MainWindow::updateCategory() {
    int canDisplay = this->height() / LABEL_HEIGHT;
    int itemIndex = 0;

    // Display atLeast
    int atLeast = qMin(qMax((canDisplay >> 1) - 1, 1), 5);
    for (int i = qMax(currentCate - atLeast, 0); i < currentCate; ++i) {
        auto *item = qobject_cast<QLabel *>(categoryLayout->itemAt(itemIndex++)->widget());
        assert(item != nullptr);
        item->setProperty("current",false);
        item->setText(categories[i].content);
    }
    // Display current
    auto *current = qobject_cast<QLabel *>(categoryLayout->itemAt(itemIndex++)->widget());
    assert(current != nullptr);
    current->setProperty("current",true);
    current->setText(categories[currentCate].content);
    // Display next
    for (int i = currentCate + 1; i < canDisplay && i < categories.size() - currentCate; ++i) {
        auto *item = qobject_cast<QLabel *>(categoryLayout->itemAt(itemIndex++)->widget());
        assert(item != nullptr);
        item->setProperty("current",false);
        item->setText(categories[i].content);
    }
}

void MainWindow::setLabelColor() {
    QImage image = loadPixmap(currentFile->absoluteFilePath()).toImage();
    int height = image.height()>>1;
    int width = image.width()>>1;
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
    if (all > 150) {
        style = style.arg("black");
    } else {
        style = style.arg("white");
    }
    style+="QLabel[current=true]{color:red;}";
    mainWidget->setStyleSheet(style);

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


