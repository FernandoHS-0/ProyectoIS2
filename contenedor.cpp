#include "contenedor.h"

contenedor::contenedor()
{

}


clienteContenido::clienteContenido(){

}

clienteContenido::clienteContenido(QString nmb, QString aP, QString aM, QString adr, QDate fecha, int pne, int nCl, bool mens,QString id){
    nombre = nmb;
    apPat = aP;
    apMat = aM;
    dir = adr;
    dob = fecha;
    tel = pne;
    noC = nCl;
    mensual = mens;
    idCliente = id;
}

QString clienteContenido::getID()
{
    return idCliente;
}
