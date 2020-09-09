#include "cliente.h"
#include "ui_cliente.h"
#include "QDebug"

Cliente::Cliente(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Cliente)
{
    ui->setupUi(this);
    conexion = QSqlDatabase::addDatabase("QODBC");
    conexion.setUserName("root");
    conexion.setDatabaseName("ParkingALot");
}

Cliente::~Cliente()
{
    delete ui;
}


void Cliente::on_BotonAgregar_clicked()
{
    QString tipo= ui->TipoReservacion->currentText();
    qDebug()<< tipo;
    QDate FechaInicio = ui->CalendarioInicio->selectedDate();
    QDate FechaFinaliza = ui->CalendarioFinaliza->selectedDate();
    QTime HoraLlegada=ui->HoraLlegada->time();
    QTime HoraSalida=ui->HoraSalida->time();

    if(tipo=="Estandar"){
        QSqlQuery reservacion;
        conexion.open();
        reservacion.exec("INSERT INTO ReservacionUnica(Fecha,HorarioEntrada,HoraSalida,)");
    }
    else{
        QSqlQuery reservacion;
        conexion.open();
        reservacion.exec("INSERT INTO        ReservacionMensual(FechaInicio,FechaFin,HorarioEntrada,HoraSalida,)");
    }


}

