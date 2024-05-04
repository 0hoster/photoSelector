#include "welcome.h"

Welcome::Welcome(QWidget *parent) : QDialog(parent) {
    imageLabel = new QLabel();
    welcomeLabel = new QLabel();
    tipLabel = new QLabel();
    containedFile.clear();
    imagesResult.clear();
    initUI();
    initSlots();
    setWindowTitle("Photos Selector");
    setFixedSize(800, 500);
}

Welcome::~Welcome() {
    delete imageLabel;
    delete welcomeLabel;
    delete tipLabel;
}

void Welcome::initUI() {
    auto *mainFormLayout = new QFormLayout(this);

    imageLabel->setPixmap(QPixmap(":/fileInfoList/image.png"));
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setMargin(30);
    mainFormLayout->addWidget(imageLabel);

    welcomeLabel->setText("Press Enter or click to continue.\nRight"
                          " click to quit.");
    welcomeLabel->setAlignment(Qt::AlignCenter);
    mainFormLayout->addWidget(welcomeLabel);

    tipLabel->setAlignment(Qt::AlignCenter);
    QFont font = tipLabel->font();
    font.setPixelSize(15);
    font.setItalic(true);
    tipLabel->setFont(font);

    QPalette palette;
    palette.setColor(QPalette::WindowText, Qt::red);
    tipLabel->setPalette(palette);
    mainFormLayout->addWidget(tipLabel);

    setLayout(mainFormLayout);
}

void Welcome::initSlots() {

}

void Welcome::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Return) { //16777220
        chooseRootFolder();
        event->accept();
    }
}

void Welcome::chooseRootFolder() {
    tipLabel->setText("");
    if (rootFolder.isEmpty()) {
        rootFolder = QDir::homePath() + "/Pictures";
    }
    QString title = "Choose a root folder 🙂";
    QString rootString = QFileDialog::getExistingDirectory(this, title, rootFolder);
    if (rootString.isEmpty())return;

    // Test if correct

    QDir root(rootString);
    bool haveImage = false;
    for (auto i: root.entryList()) {
        i = i.toLower();
        if (i.endsWith(".png") || i.endsWith(".jpg") || i.endsWith(".jpeg") || i.endsWith(".bmp")) {
            haveImage = true;
            break;
        }
    }
    if (!haveImage) {
        QString tip = "Cannot find any picture(PNG,JPG,JPEG,BMP) in folder. 😣";
        tipLabel->setText(tip);
        return;
    }
    if (repeatCheck(root))
        this->accept();
}

bool Welcome::repeatCheck(QDir files) {
    files.setSorting(QDir::Size);
    imagesResult.clear();
    containedFile.clear();
    QStringList repeatedFile;

    for (const auto &fileInfo: files.entryInfoList()) {
        QString fileName = fileInfo.absoluteFilePath();

        QString fileNameLowered = fileName.toLower();
        bool isImage = fileNameLowered.endsWith(".png") || fileNameLowered.endsWith(".jpg")
                       || fileNameLowered.endsWith(".jpeg")
                       || fileNameLowered.endsWith(".bmp");
        if (!isImage)continue; // Skip non-image file

        QFile file(fileName);
        if (file.open(QFile::ReadOnly)) {
            QByteArray byte = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5);
            if (containedFile.contains(byte)) {
                repeatedFile.push_back(fileName);
            } else {
                imagesResult.push_back(Image{fileInfo, byte});
                containedFile.insert(byte);
            }
        }
    }
    if (!repeatedFile.empty()) {
        QString notice = "Notice that these photos are totally copies of ones before.\n";
        for (const auto &i: repeatedFile) {
            notice.push_back("- " + i + "\n");
        }
        auto result = QMessageBox::information(this, "Repeated files detected", notice,
                                               QMessageBox::Ok | QMessageBox::Cancel);
        if (result == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void Welcome::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        chooseRootFolder();
        event->accept();
    } else if (event->button() == Qt::RightButton) {
        event->accept();
        this->reject();
    }
}

Welcome::ImageList Welcome::getRootDir() {
    return imagesResult;
}
