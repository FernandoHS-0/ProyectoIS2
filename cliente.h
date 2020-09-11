#ifndef CLIENTE_H
#define CLIENTE_H

#include <QDialog>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QMessageBox>
#include "contenedor.h"

namespace Ui {
class Cliente;
}

class Cliente : public QDialog
{
    Q_OBJECT

public:
    explicit Cliente(clienteContenido *, QWidget *parent = nullptr);
    ~Cliente();

private slots:
    void on_reservaciones_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_agendarEst_clicked();

    void on_mensualInicio_clicked(const QDate &date);

    void on_mensualInicio_selectionChanged();

    void on_agendarMensual_clicked();

private:
    Ui::Cliente *ui;
    QSqlDatabase conexion;
    clienteContenido *sesionCliente;
};

#endif // CLIENTE_H
