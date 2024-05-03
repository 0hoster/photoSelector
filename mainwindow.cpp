#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent, const QStringList &imageNames)
        : QMainWindow(parent) {
    statusLabel = new QLabel;
    mainWidget = new QWidget;
    mainLayout = new QFormLayout;
    categoryLayout = new QFormLayout;
    labelColor = QColor(Qt::black);
    // Test data
    assert(imageNames.size()>0);

    setWindowTitle("Photos Selector - " + imageNames[0]);
    initValues(imageNames);
    initUI();
    initSlots();
    setSuitableScreenSize();
    updateCateShortcutDisplay();
    updateCategory();
//    setLabelColor();
//    updateLabelColor();
}

MainWindow::~MainWindow() {
    delete mainWidget->layout();
    delete mainWidget;
}

void MainWindow::initValues(const QStringList &imageNames) {
    toKeys['a'] = Qt::Key_A, toKeys['b'] = Qt::Key_B;
    toKeys['c'] = Qt::Key_C, toKeys['d'] = Qt::Key_D;
    toKeys['e'] = Qt::Key_E, toKeys['f'] = Qt::Key_F;
    toKeys['g'] = Qt::Key_G, toKeys['h'] = Qt::Key_H;
    toKeys['i'] = Qt::Key_I, toKeys['j'] = Qt::Key_J;
    toKeys['k'] = Qt::Key_K, toKeys['l'] = Qt::Key_L;
    toKeys['m'] = Qt::Key_M, toKeys['n'] = Qt::Key_N;
    toKeys['o'] = Qt::Key_O, toKeys['p'] = Qt::Key_P;
    toKeys['q'] = Qt::Key_Q, toKeys['r'] = Qt::Key_R;
    toKeys['s'] = Qt::Key_S, toKeys['t'] = Qt::Key_T;
    toKeys['u'] = Qt::Key_U, toKeys['v'] = Qt::Key_V;
    toKeys['w'] = Qt::Key_W, toKeys['x'] = Qt::Key_X;
    toKeys['y'] = Qt::Key_Y, toKeys['z'] = Qt::Key_Z;
    toKeys['1'] = Qt::Key_1, toKeys['2'] = Qt::Key_2;
    toKeys['3'] = Qt::Key_3, toKeys['4'] = Qt::Key_4;
    toKeys['5'] = Qt::Key_5, toKeys['6'] = Qt::Key_6;
    toKeys['7'] = Qt::Key_7, toKeys['8'] = Qt::Key_8;
    toKeys['9'] = Qt::Key_9, toKeys['0'] = Qt::Key_0;

    for(const auto& i:imageNames){
        fileInfoList.push_back(QFileInfo(i));
    }
    currentFile = fileInfoList.begin();

    assert(frontKeys.size() <= keyMax && strlen(backKeys) <= keyMax);

    categoryPerPage = int(frontKeys.size()) * std::strlen(backKeys);
    // 不可避免
}

void MainWindow::initUI() {
    categoryLayout->setVerticalSpacing(5);
    categoryLayout->setHorizontalSpacing(5);

    for (int i = 0; i < categoryPerPage; ++i) {
        auto *itemLabel = new QLabel();
        auto *item = new QLabel();
        categoryLayout->addRow(itemLabel, item);
    }
    statusLabel->setText(currentFile->fileName());

    mainLayout->addWidget(statusLabel);
    mainLayout->addItem(categoryLayout);

    mainWidget->setLayout(mainLayout);
    this->setCentralWidget(mainWidget);
}

void MainWindow::keySelect(int x, int y) {
    int index = itemIndex() + x * (int) strlen(backKeys) + y;
    if (index >= categories.size())return;
    qDebug() << categories[index].content;
}

void MainWindow::initSlots() {
    auto *imageNext = new QShortcut(QKeySequence(Qt::Key_Right), this);
    auto *imageNextB = new QShortcut(QKeySequence(Qt::SHIFT | Qt::Key_N), this);
    auto doImageNext = [&]() {
        ++currentFile;
        if (currentFile == fileInfoList.end())currentFile = fileInfoList.begin();
        emit currentImageChange();
    };
    connect(imageNext, &QShortcut::activated, this, doImageNext);
    connect(imageNextB, &QShortcut::activated, this, doImageNext);

    auto *imagePrevious = new QShortcut(QKeySequence(Qt::Key_Left), this);
    connect(imagePrevious, &QShortcut::activated, this, [&]() {
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
        if (currentCatePage <= -1)currentCatePage = categoryEnd - 1;
        updateCategory();
    });

    auto *cateNextPage = new QShortcut(QKeySequence(Qt::Key_J), this);
    connect(cateNextPage, &QShortcut::activated, this, [&]() {
        ++currentCatePage;
        if (currentCatePage >= categoryEnd)currentCatePage = 0;
        updateCategory();
    });

    auto *cateTopPage = new QShortcut(QKeySequence(Qt::Key_K | Qt::SHIFT), this);
    connect(cateTopPage, &QShortcut::activated, this, [&]() {
        currentCatePage = 0;
        updateCategory();
    });

    auto *cateBottomPage = new QShortcut(QKeySequence(Qt::Key_J | Qt::SHIFT), this);
    connect(cateBottomPage, &QShortcut::activated, this, [&]() {
        currentCatePage = categoryEnd - 1;
        updateCategory();
    });

    auto *cateNew = new QShortcut(QKeySequence(Qt::Key_N), this);
    connect(cateNew, &QShortcut::activated, this, [&]() {
        createNewCate();
        updateCategory();
        updateLabelColor();
    });

    for (int i = 0; i < frontKeys.size(); ++i) {
        for (int j = 0; j < strlen(backKeys); ++j) {
            QShortcut *select;
            if (frontKeys[i] == Qt::MODIFIER_MASK) {
                select = new QShortcut(QKeySequence(toKeys[backKeys[j]]), this);
            } else {
                select = new QShortcut(QKeySequence(frontKeys[i] | toKeys[backKeys[j]]), this);
            }
            connect(select, &QShortcut::activated, this, [i, j, this]() { keySelect(i, j); });
        }
    }

    auto *windowClose = new QShortcut(QKeySequence(Qt::Key_Q), this);
    connect(windowClose, &QShortcut::activated, this, [&]() {
        this->close();
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

void MainWindow::updateCateShortcutDisplay() {
    int index = 0;
    for (auto &frontKey: frontKeys) {
        for (int j = 0; j < strlen(backKeys); ++j) {
            QString cut;
            switch (frontKey) {
                case Qt::CTRL:
                    cut.push_back("C-");
                    break;
                case Qt::SHIFT:
                    cut.push_back("S-");
                    break;
                case Qt::ALT:
                    cut.push_back("A-");
                    break;
                case Qt::META:
                    cut.push_back("M-");
                    break;
                case Qt::MODIFIER_MASK:
                    break;
            }
            setCategoryLabelAt(index, cut + backKeys[j]);
            ++index;
        }
    }
}

void MainWindow::updateCategory() {
    long long total = qMin(categoryPerPage, categories.size() - currentCatePage * categoryPerPage - 1);
    assert(total >= 0);

    for (int i = 0; i < total; ++i) {
        setCategoryAt(i, categories[i + itemIndex()].content);
    }
    for (int i = (int) total; i < categoryPerPage; ++i) {
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
        addCate(newCate,false);
        updateCategory();
    }
}

void MainWindow::addCate(const QString &newCate,bool isLock){
    categories.push_back(Cate{newCate,0,isLock});
    while(categoryEnd*categoryPerPage<categories.size())++categoryEnd;
}

void MainWindow::updateLabelColor() {
    QString selectAll = "QLabel {color:rgb(%1,%2,%3);}";
    QString selectCur = "QLabel[current=true] {color:rgb(%1,%2,%3);}";
    QString style = selectAll.arg(labelColor.red()).arg(labelColor.green()).arg(labelColor.blue()) + \
                    selectCur.arg(currentColor.red()).arg(currentColor.green()).arg(currentColor.blue());
    mainWidget->setStyleSheet(style);
}


