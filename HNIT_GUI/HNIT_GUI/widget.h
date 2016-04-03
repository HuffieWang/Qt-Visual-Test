#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_pushButton_clicked();

    void updateNomal();

    void getImage();

    void disImage();

    void doYourAIGO();

    void goNomal();

    void on_pushButton_UP_clicked();

    void on_pushButton_DN_clicked();

    void on_pushButton_R_clicked();

    void on_pushButton_L_clicked();

//    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::Widget *ui;
    QTimer *timer;
};

#endif // WIDGET_H
