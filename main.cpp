#include "inicio.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("fusion");
    Inicio w;
    w.show();

    return a.exec();
}
