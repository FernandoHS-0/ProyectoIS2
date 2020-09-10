#ifndef CONTENEDOR_H
#define CONTENEDOR_H
#include <QString>
#include <QDate>


class contenedor
{
public:
    contenedor();
};

class clienteContenido{
    public:
        QString nombre, apPat, apMat, dir;
        int tel, noC;
        QDate dob;
        bool mensual;

        clienteContenido();
        clienteContenido(QString, QString, QString, QString, QDate, int, int, bool);
};

#endif // CONTENEDOR_H
