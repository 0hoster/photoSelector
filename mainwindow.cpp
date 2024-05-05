#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent, const ImageList &imageNames)
        : QMainWindow(parent) {
    statusLabel = new QLabel;
    mainWidget = new QWidget;
    mainLayout = new QFormLayout;
    categoryLayout = new QFormLayout;
    fileInfoList = imageNames;
    currentFile = fileInfoList.begin();
    // Test data
    qDebug() << imageNames.size();
    assert(!imageNames.empty());
    setSuitableScreenSize();
    update();
    setWindowTitle("Photos Selector - " + imageNames[0].fileName());
    initValues();
    initUI();
    initSlots();
    updateCateShortcutDisplay();
    updateCategory();
    emit currentImageChange();
}

MainWindow::~MainWindow() {
    delete mainWidget->layout();
    delete mainWidget;
}

void MainWindow::initValues() {
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

    assert(frontKeys.size() <= keyMax && backKeys.size() <= keyMax);

    categoryPerPage = int(frontKeys.size()) * int(backKeys.size());
}

void MainWindow::initUI() {
    categoryLayout->setVerticalSpacing(5);
    categoryLayout->setHorizontalSpacing(5);

    QFont font;
    font.setPixelSize(20);
    font.setBold(true);
    for (int i = 0; i < categoryPerPage; ++i) {
        auto *itemLabel = new QLabel();
        auto *item = new QLabel();
        item->setFont(font);
        itemLabel->setFont(font);
        categoryLayout->addRow(itemLabel, item);
    }
    statusLabel->setFont(font);
    statusLabel->setText(currentFile->fileName());

    mainLayout->addWidget(statusLabel);
    mainLayout->addItem(categoryLayout);

    mainWidget->setLayout(mainLayout);
    this->setCentralWidget(mainWidget);
}

void MainWindow::keySelect(int x, int y) {
    int offset = x * (int) backKeys.size() + y;
    int index = itemIndex() + offset;
    if (index >= categories.size())return;
    auto *selectedItem = qobject_cast<QLabel *>(categoryLayout->itemAt(offset * 2 + 1)->widget());
    if (selectedItem->property("selected") != true) {
        qDebug() << "New selected" << categories[index].content;

        imageToCategory[currentFile->absoluteFilePath()].insert(categories[index].content);
        selectedItem->setProperty("selected", true);
        ++categories[index].count;
    } else {
        qDebug() << "cancel selected" << categories[index].content;

        imageToCategory[currentFile->absoluteFilePath()].remove(categories[index].content);
        selectedItem->setProperty("selected", false);
        --categories[index].count;
        assert(categories[index].count >= 0);
    }
    updateCategoryColor();
}

void MainWindow::initSlots() {
    auto *imageNext = new QShortcut(QKeySequence(Qt::Key_Right), this);
    auto *imageNextB = new QShortcut(QKeySequence(Qt::Key_Return), this);
    auto doImageNext = [this]() {
        ++currentFile;
        if (currentFile == fileInfoList.end())currentFile = fileInfoList.begin();
        emit currentImageChange();
    };
    connect(imageNext, &QShortcut::activated, this, doImageNext);
    connect(imageNextB, &QShortcut::activated, this, doImageNext);

    auto *imagePrevious = new QShortcut(QKeySequence(Qt::Key_Left), this);
    connect(imagePrevious, &QShortcut::activated, this, [this]() {
        if (currentFile == fileInfoList.begin())currentFile = fileInfoList.end();
        --currentFile;
        emit currentImageChange();
    });
    auto *imageTop = new QShortcut(QKeySequence(Qt::Key_Up), this);
    connect(imageTop, &QShortcut::activated, this, [this]() {
        // First image
        currentFile = fileInfoList.begin();
        emit currentImageChange();
    });

    auto *imageBottom = new QShortcut(QKeySequence(Qt::Key_Down), this);
    connect(imageBottom, &QShortcut::activated, this, [this]() {
        // Last image
        currentFile = fileInfoList.end() - 1;
        emit currentImageChange();
    });


    auto *catePreviousPage = new QShortcut(QKeySequence(Qt::Key_K), this);
    connect(catePreviousPage, &QShortcut::activated, this, [this]() {
        --currentCatePage;
        if (currentCatePage <= -1)currentCatePage = categoryEnd - 1;
        updateCategory();
    });

    auto *cateNextPage = new QShortcut(QKeySequence(Qt::Key_J), this);
    connect(cateNextPage, &QShortcut::activated, this, [this]() {
        ++currentCatePage;
        if (currentCatePage >= categoryEnd)currentCatePage = 0;
        updateCategory();
    });

    auto *cateTopPage = new QShortcut(QKeySequence(Qt::Key_K | Qt::SHIFT), this);
    connect(cateTopPage, &QShortcut::activated, this, [this]() {
        currentCatePage = 0;
        updateCategory();
    });

    auto *cateBottomPage = new QShortcut(QKeySequence(Qt::Key_J | Qt::SHIFT), this);
    connect(cateBottomPage, &QShortcut::activated, this, [this]() {
        currentCatePage = categoryEnd - 1;
        updateCategory();
    });

    auto *cateNew = new QShortcut(QKeySequence(Qt::Key_N), this);
    connect(cateNew, &QShortcut::activated, this, [this]() {
        createNewCate();
        updateCategory();
    });

    for (int i = 0; i < frontKeys.size(); ++i) {
        for (int j = 0; j < backKeys.size(); ++j) {
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
    connect(windowClose, &QShortcut::activated, this, [this]() {
        this->close();
    });

    auto *doFinalize = new QShortcut(QKeySequence(Qt::ALT | Qt::Key_Return), this);
    connect(doFinalize, &QShortcut::activated, this, [this]() {
        finalize();
    });

    connect(this, &MainWindow::currentImageChange, this, &MainWindow::updateInfo);
    connect(this, &MainWindow::selectedCategoryChange, this, &MainWindow::updateCategoryColor);
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.drawImage(0, 0, loadScaledImage(size()));
    event->accept();
}

QImage MainWindow::loadScaledImage(QSize size, const QString &filename) {
    QImageReader ImageReader(filename == "" ? currentFile->absoluteFilePath() : filename);
    ImageReader.setAutoTransform(true);
    QSize imageSize = ImageReader.size();
    QSize scaledSize = imageSize.scaled(size, Qt::KeepAspectRatio);
    ImageReader.setScaledSize(scaledSize);
    return ImageReader.read();
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
    updateCategoryColorStatus();
    update();
}

void MainWindow::updateCateShortcutDisplay() {
    int total = totalCateToDisplay();
    int index = 0;
    for (auto &frontKey: frontKeys) {
        for (auto backKey: backKeys) {
            if (index >= total) {
                setCategoryLabelAt(index, "");
                ++index;
                continue;
            }
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
            setCategoryLabelAt(index, cut + backKey);
            ++index;
        }
    }
}

void MainWindow::updateCategory() {
    int total = totalCateToDisplay();

    for (int i = 0; i < total; ++i) {
        setCategoryAt(i, categories[i + itemIndex()].content);
    }
    for (int i = (int) total; i < categoryPerPage; ++i) {
        setCategoryAt(i, "");
    }
    updateCateShortcutDisplay();
}

void MainWindow::updateCategoryColorStatus() {
    int total = totalCateToDisplay();
    for (int i = 0; i < total; ++i) {
        auto *labelItem = qobject_cast<QLabel *>(categoryLayout->itemAt(2 * i + 1)->widget());
        bool isExist = imageToCategory[currentFile->absoluteFilePath()].contains(categories[i + itemIndex()].content);
        labelItem->setProperty("selected", isExist);
    }
    updateCategoryColor();
}

void MainWindow::updateCategoryColor() {
    int black = 0, white = 0, counter = 0;
    int red, blue, green;
    QImage currentImage = loadScaledImage(size().scaled(1920 / 4, 1080 / 4, Qt::KeepAspectRatio));
    for (int i = 0; i < currentImage.height() >> 1; i += 5) {
        for (int j = 0; j < currentImage.width() >> 2; j += 5) {
            ++counter;
            QRgb *pixel = (QRgb *) currentImage.constBits();
            red = qRed(pixel[i * currentImage.width() + j]);
            green = qGreen(pixel[i * currentImage.width() + j]);
            blue = qBlue(pixel[i * currentImage.width() + j]);
            if ((red + green + blue) > 225 * 3)++black;
            else if ((red + green + blue) < 70 * 3)++white;
        }
    }
    QColor labelFrontColor;
    if (black * 3 > counter && white * 3 > counter) {
        labelFrontColor = colorfulGrey;
    } else if (white * 3 > counter) {
        labelFrontColor = whiteGrey;
    } else {
        labelFrontColor = blackGrey;
    }
    QPalette palette = statusLabel->palette();
    palette.setBrush(QPalette::WindowText, labelFrontColor);
    statusLabel->setPalette(palette);
    for (int index = 1; index < categoryLayout->count(); index += 2) {
        auto *labelItem = qobject_cast<QLabel *>(categoryLayout->itemAt(index - 1)->widget());
        auto *item = qobject_cast<QLabel *>(categoryLayout->itemAt(index)->widget());

        if (item->property("selected") == true) {
            palette.setBrush(QPalette::WindowText, selectedColor);
        } else {
            palette.setBrush(QPalette::WindowText, labelFrontColor);
        }
        labelItem->setPalette(palette);
        item->setPalette(palette);
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

void MainWindow::createNewCate() {
    bool ok = false;
    QString newCate = QInputDialog::getText(this, "New Category", "What 's the new category? 😝", \
                                            QLineEdit::Normal, "", &ok,
                                            isFullScreen() ? Qt::FramelessWindowHint : Qt::Dialog);
    if (ok) {
        if (newCate.isEmpty()) {
            QMessageBox::about(this, "Hey man", "You enter an EMPTY string, are you serious?");
            return;
        }
        auto isRepeated = [this, newCate]() {
            if (std::any_of(categories.begin(), categories.end(),
                            [newCate](const Cate &i) { return i.content == newCate; })) {
                return true;
            }
            return false;
        };
        if (newCate.isEmpty() || isRepeated()) {
            QMessageBox::about(this, "Notice here",
                               QString("You enter the category `%1`, which is already in queue.").arg(newCate));
            return;
        }
        addCate(newCate, false);
        updateCategory();
    }
}

void MainWindow::addCate(const QString &newCate, bool isLock) {
    categories.push_back(Cate{newCate, 0, isLock});
    while (categoryEnd * categoryPerPage < categories.size())++categoryEnd;
}

void MainWindow::finalize() {
    QJsonObject mainData;
    QJsonObject imageData;
    for (auto image = imageToCategory.constBegin(); image != imageToCategory.constEnd(); ++image) {
        QJsonArray categoriesData;
        for (const auto &category: image.value()) {
            categoriesData.push_back(category);
        }
        imageData[image.key()] = categoriesData;
    }
    mainData["images"] = imageData;
    QJsonDocument document(mainData);
    QFile output("output.json");
    if(output.open(QIODeviceBase::WriteOnly)){
        output.write(document.toJson(QJsonDocument::Indented));
    }
    output.close();
    qDebug() << document.toJson(QJsonDocument::Compact);
}



