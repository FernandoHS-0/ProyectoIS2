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

    void on_pushButton_clicked();

    void on_Actualizar_clicked();

    void on_modificar_clicked();

    void on_Reservaciones_currentTextChanged(const QString &arg1);

    void on_pushButton_2_clicked();

    void on_estadoCuenta_clicked();

    void on_EstadoCuenta_2_clicked(bool checked);

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

private:
    Ui::Cliente *ui;
    QSqlDatabase conexion;
    clienteContenido *sesionCliente;
    QString IDUsuario;
    int MontoTotal=0;
    int TotalReservaciones=0;
    QString nomb;
    QString ApellidoP;
    QString ApellidoM;
};

#endif // CLIENTE_H
