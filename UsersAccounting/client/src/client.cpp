#include <QDebug>
#include <QApplication>
#include <QLoggingCategory>
#include "../headers/UsersWindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    qDebug() << "Hello Client";

    QLoggingCategory::setFilterRules(
            "network.debug=true\n"
            "database.debug=false\n"
            "ws.debug=true\n"
            "*.info=true\n"
    );

    UsersWindow uw;
    uw.show();

    return QApplication::exec();
}
