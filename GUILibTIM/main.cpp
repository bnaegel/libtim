#include "guilibtim.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GUILibTIM w;
    w.show();

    return a.exec();
}
