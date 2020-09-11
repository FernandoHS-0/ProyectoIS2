#include "cliente.h"
#include "ui_cliente.h"

Cliente::Cliente(clienteContenido *ses, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Cliente)
{
    sesionCliente = ses;
    ui->setupUi(this);
    conexion = QSqlDatabase::addDatabase("QODBC");
    conexion.setUserName("root");
    conexion.setDatabaseName("ParkingALot");
}

Cliente::~Cliente()
{
    delete ui;
}

void Cliente::on_reservaciones_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void Cliente::on_comboBox_currentIndexChanged(int index)
{
    ui->stackedWidget_2->setCurrentIndex(index);
}

void Cliente::on_agendarEst_clicked()
{

    QMessageBox mensaje, info;
    mensaje.setText(tr("¿Confirmar reservación?"));
    info.setText(tr("Su reservación fue realizada"));
    QAbstractButton * confirmar = mensaje.addButton(tr("Aceptar"), QMessageBox::AcceptRole);
    QAbstractButton * cancelar = mensaje.addButton(tr("Aceptar"), QMessageBox::NoRole);
    QAbstractButton * aceptar = info.addButton(tr("Aceptar"), QMessageBox::AcceptRole);

    mensaje.exec();
    if(mensaje.clickedButton() == confirmar){
        conexion.open();
        QDate fechaR = ui->calendarioEst->selectedDate();
        QTime llegada = ui->estLlegada->time(),
                salida = ui->estSalida->time();
        QSqlQuery reservar;
        reservar.prepare("INSERT INTO ReservacionUnica SET Fecha = :date, HoraEntrada = :llegada, HoraSalida = :salida, idTarifa = (SELECT idTarifa FROM Tarifa WHERE idTarifa = 1), Pago_idPago = (SELECT idPago FROM Pago WHERE idPago = 1), NoEspacio = (SELECT NoEspacio FROM Espacio WHERE NoEspacio = 1), IdUsuario = (SELECT IdUsuario FROM Usuario WHERE IdUsuario = :idU);");
        reservar.bindValue(":date", fechaR);
        reservar.bindValue(":llegada", llegada);
        reservar.bindValue(":salida", salida);
        reservar.bindValue(":idU", sesionCliente->noC);
        if(reservar.exec()){
            info.exec();
            qDebug() << "Reservacion realizada";
            conexion.close();
        }else{
            qDebug() << "Puro chile con el query";
        }
    }


}

void Cliente::on_mensualInicio_clicked(const QDate &date)
{
   /* QDate nueva(date.day(), date.month()+1, date.year());
    ui->mensualFin->setSelectedDate(nueva);*/
}

void Cliente::on_mensualInicio_selectionChanged()
{
    /*QDate date = ui->mensualInicio->selectedDate();
    int mes = date.month() + 1;
    qDebug() << "Sig mes: " << mes;
    ui->mensualFin->setCurrentPage(mes, date.year());*/
}

void Cliente::on_agendarMensual_clicked()
{
    conexion.open();
    QDate date = ui->mensualInicio->selectedDate();
    QDate nueva(date.year(), date.month()+1, date.day());
    QDate fechaR = ui->calendarioEst->selectedDate();
    QMessageBox mensaje, info;
    mensaje.setText(tr("¿Confirmar reservación?"));
    info.setText(tr("Su reservación fue realizada"));
    QAbstractButton * confirmar = mensaje.addButton(tr("Aceptar"), QMessageBox::AcceptRole);
    QAbstractButton * cancelar = mensaje.addButton(tr("Aceptar"), QMessageBox::NoRole);
    QAbstractButton * aceptar = info.addButton(tr("Aceptar"), QMessageBox::AcceptRole);

    mensaje.exec();
    if(mensaje.clickedButton() == confirmar){
        QTime llegada(7,00,00),
                salida(15,00,00);
        QSqlQuery reservar;
        reservar.prepare("INSERT INTO ReservacionMensual SET NoReservacion = 1, FechaInicio = :dateI, FechaFin = :dateF, HoraEntrada = :llegada, HoraSalida = :salida, idTarifa = (SELECT idTarifa FROM Tarifa WHERE idTarifa = 1), idPago = (SELECT idPago FROM Pago WHERE idPago = 1), NoEspacio = (SELECT NoEspacio FROM Espacio WHERE NoEspacio = 1), IdUsuario = (SELECT IdUsuario FROM Usuario WHERE IdUsuario = :idU);");
        reservar.bindValue(":date", fechaR);
        reservar.bindValue(":llegada", llegada);
        reservar.bindValue(":salida", salida);
        reservar.bindValue(":idU", sesionCliente->noC);
        reservar.bindValue(":dateI", date);
        reservar.bindValue(":dateF", nueva);
        if(reservar.exec()){
            info.exec();
            qDebug() << "Reservacion realizada";
            conexion.close();
        }else{
            qDebug() << "Fallo la reservacion";
        }
    }
}
