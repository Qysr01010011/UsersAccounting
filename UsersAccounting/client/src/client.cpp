#include <QDebug>
#include <QApplication>
#include "../headers/UsersWindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    qDebug() << "Hello Client";

    UsersWindow* uw = new UsersWindow();
    uw->show();
    uw->resizeAndMove();

    return QApplication::exec();
}
