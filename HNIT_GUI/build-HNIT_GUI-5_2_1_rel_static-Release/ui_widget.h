/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QPushButton *pushButton;
    QLabel *label;
    QPushButton *pushButton_UP;
    QPushButton *pushButton_DN;
    QPushButton *pushButton_L;
    QPushButton *pushButton_R;
    QLineEdit *vEdit;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *wEdit;
    QComboBox *comboBox;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QStringLiteral("Widget"));
        Widget->resize(250, 354);
        pushButton = new QPushButton(Widget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(10, 320, 71, 25));
        label = new QLabel(Widget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(5, 5, 240, 240));
        pushButton_UP = new QPushButton(Widget);
        pushButton_UP->setObjectName(QStringLiteral("pushButton_UP"));
        pushButton_UP->setGeometry(QRect(150, 290, 41, 25));
        pushButton_DN = new QPushButton(Widget);
        pushButton_DN->setObjectName(QStringLiteral("pushButton_DN"));
        pushButton_DN->setGeometry(QRect(150, 320, 41, 25));
        pushButton_L = new QPushButton(Widget);
        pushButton_L->setObjectName(QStringLiteral("pushButton_L"));
        pushButton_L->setGeometry(QRect(200, 320, 41, 25));
        pushButton_R = new QPushButton(Widget);
        pushButton_R->setObjectName(QStringLiteral("pushButton_R"));
        pushButton_R->setGeometry(QRect(100, 320, 41, 25));
        vEdit = new QLineEdit(Widget);
        vEdit->setObjectName(QStringLiteral("vEdit"));
        vEdit->setGeometry(QRect(50, 260, 41, 20));
        label_2 = new QLabel(Widget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 260, 31, 16));
        label_3 = new QLabel(Widget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(110, 260, 41, 16));
        wEdit = new QLineEdit(Widget);
        wEdit->setObjectName(QStringLiteral("wEdit"));
        wEdit->setGeometry(QRect(170, 260, 41, 20));
        comboBox = new QComboBox(Widget);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setGeometry(QRect(10, 290, 71, 22));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QApplication::translate("Widget", "Widget", 0));
        pushButton->setText(QApplication::translate("Widget", "\345\220\257\345\212\250", 0));
        label->setText(QString());
        pushButton_UP->setText(QApplication::translate("Widget", "\345\212\240", 0));
        pushButton_DN->setText(QApplication::translate("Widget", "\345\207\217", 0));
        pushButton_L->setText(QApplication::translate("Widget", "\345\217\263", 0));
        pushButton_R->setText(QApplication::translate("Widget", "\345\267\246", 0));
        vEdit->setText(QApplication::translate("Widget", "0", 0));
        label_2->setText(QApplication::translate("Widget", "\351\200\237\345\272\246\357\274\232", 0));
        label_3->setText(QApplication::translate("Widget", "\350\247\222\351\200\237\345\272\246\357\274\232", 0));
        wEdit->setText(QApplication::translate("Widget", "18", 0));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
