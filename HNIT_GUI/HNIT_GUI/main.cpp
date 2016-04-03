#include "widget.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("湖南工学院地面站");
    w.setWindowIcon(QIcon(QPixmap(":/hnit.ico")));
    w.setFixedSize(250, 354);
    w.show();

    return a.exec();
}
