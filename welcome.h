#ifndef WELCOME_H
#define WELCOME_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QPixmap>
#include <QString>
#include <QKeyEvent>
#include <QMouseEvent>

class Welcome : public QDialog {
Q_OBJECT

private:
    QString rootFolder;
    QLabel *imageLabel;
    QLabel *welcomeLabel;

    void initUI();

    void initSlots();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event)override;

public:
    explicit Welcome(QWidget *parent = nullptr);

    ~Welcome() override;
};

#endif // WELCOME_H
