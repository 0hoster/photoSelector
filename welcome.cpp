#include "welcome.h"

Welcome::Welcome(QWidget *parent) : QDialog(parent) {
    imageLabel = new QLabel();
    welcomeLabel = new QLabel();
    initUI();
    initSlots();
    setWindowTitle("Photos Selector");
    setFixedSize(800, 500);
}

Welcome::~Welcome() {
    delete imageLabel;
    delete welcomeLabel;

}

void Welcome::initUI() {
    auto *mainFormLayout = new QFormLayout(this);


    imageLabel->setPixmap(QPixmap(":/images/image.png"));
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setMargin(30);
    mainFormLayout->addWidget(imageLabel);

    welcomeLabel->setText("Press Enter or click to continue.");
    welcomeLabel->setAlignment(Qt::AlignCenter);
    mainFormLayout->addWidget(welcomeLabel);

    setLayout(mainFormLayout);
}

void Welcome::initSlots() {

}

void Welcome::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Return) { //16777220
        qDebug("Start");
        event->accept();
    }

}

void Welcome::mouseDoubleClickEvent(QMouseEvent *event) {
    qDebug("Start m");
    event->accept();
}
