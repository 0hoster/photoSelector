#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent, const QString &rootPath)
        : QMainWindow(parent) {
    statusLabel = new QLabel;
    root.setPath(rootPath);
    for (const auto &file: root.entryInfoList()) {
        QString fileName = file.fileName().toLower();
        if (fileName.endsWith(".png") || fileName.endsWith(".jpg") || fileName.endsWith(".jpeg") ||
            fileName.endsWith(".bmp")) {
            images.push_back(file);
        }
    }
    current = images.begin();
    setWindowTitle("Photos Selector - " + rootPath);
    initUI();
    initSlots();
    setSuitableScreenSize();

    connect(this,&MainWindow::currentImageChange,this,&MainWindow::updateInfo);
}

MainWindow::~MainWindow() {
    delete statusLabel;
}

void MainWindow::initUI() {
    auto status = this->statusBar();

    statusLabel->setText(current->fileName());
    status->addWidget(statusLabel);
}

void MainWindow::initSlots() {

}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.drawPixmap(0, 0, loadPixmap(current->absoluteFilePath()));
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
        ++current;
        if (current == images.end())current = images.begin();
    } else if (event->key() == Qt::Key_Left) {
        if (current == images.begin())current = images.end();
        --current;
    } else if (event->key() == Qt::Key_Up) {
        current = images.begin();
    } else if (event->key() == Qt::Key_Down) {
        current = images.end() - 1;
    }else if(event->key()==Qt::Key_Q){
        this->close();
    }else if(event->key()==Qt::Key_F){
        if(isFullScreen()){
            showNormal();
        }else{
            showFullScreen();
        }
    }else{
        return;
    }
    emit currentImageChange();
    update();
    event->accept();
}

void MainWindow::setSuitableScreenSize() {
    QImageReader reader;
    int h=0,w=0;
    for(const auto& file :images){
        reader.setFileName(file.absoluteFilePath());
        h=qMax(h,reader.size().height());
        w=qMax(w,reader.size().width());
    }
    if(h>=1080&&w>=1920){
        showFullScreen();
    }else{
        setBaseSize(h,w);
    }
}

void MainWindow::updateInfo() {
    statusLabel->setText(current->fileName());
}

