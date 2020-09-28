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
        QString idCliente;
        QString Matricula;

        clienteContenido();
        clienteContenido(QString, QString, QString, QString, QDate, int, int, bool,QString);
        QString getID();
};

#endif // CONTENEDOR_H
