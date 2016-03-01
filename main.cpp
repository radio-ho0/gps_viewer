#include "inputtest.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    InputTest w;
    w.show();


    return a.exec();
}
