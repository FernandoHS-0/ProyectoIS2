#include "cliente.h"
#include "ui_cliente.h"
#include <QStandardItemModel>
#include <QSqlError>
#include <QTime>

Cliente::Cliente(clienteContenido *ses, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Cliente)
{
    sesionCliente = ses;
    ui->setupUi(this);
    conexion = QSqlDatabase::addDatabase("QODBC");
    conexion.setUserName("root");
    conexion.setDatabaseName("ParkingALot");

    QStringList titulos;
    titulos << "ID Usuario" << "ID Reservacion" << "Num Espacio" << "Fecha " << "Hora Entrada " << "Hora Salida ";
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setHorizontalHeaderLabels(titulos);

     if(conexion.open()){
         QSqlQuery Prueba;
         IDUsuario = ses->getID();
         if(Prueba.exec("Select IDUSUARIO,IDRESERVACIONUNICA,NOESPACIO,FECHA,HORAENTRADA,HORASALIDA from reservacionunica WHERE IDUSUARIO ="+IDUsuario )){
             while(Prueba.next()){
                 QString idUsuario = Prueba.value(0).toString();
                 QString idReserva = Prueba.value(1).toString();
                 QString NumEspacio = Prueba.value(2).toString();
                 QString Fecha = Prueba.value(3).toString();
                 QString HoraEntrada = Prueba.value(4).toString();
                 QString HoraSalida = Prueba.value(5).toString();

                 ui->tableWidget->insertRow(ui->tableWidget->rowCount());
                 ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,new QTableWidgetItem(idUsuario));
                 ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,new QTableWidgetItem(idReserva));
                 ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,2,new QTableWidgetItem(NumEspacio));
                 ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,3,new QTableWidgetItem(Fecha));
                 ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,4,new QTableWidgetItem(HoraEntrada));
                 ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,5,new QTableWidgetItem(HoraSalida));
             }
         }
         Prueba.lastError();

         QStringList titulos2;
         ui->Registros->setColumnCount(4);
         titulos2 << "Fecha" << "Hora de entrada" << "Hora de salida" << "Numero Reservacion";
         ui->Registros->setHorizontalHeaderLabels(titulos2);
         QSqlQuery Registros;
         Registros.prepare("SELECT R.fecha, R.horaEntrada, R.horaSalida, R.IdReservacionUnica FROM usuario as U INNER JOIN cliente as C ON U.idUsuario=C.idUsuario INNER JOIN reservacionunica as R ON C.idUsuario=R.idUsuario WHERE U.idUsuario=:IdU;");
         Registros.bindValue(":IdU", sesionCliente->noC);
         if(Registros.exec()){
           while(Registros.next()){
                QString fec = Registros.value(0).toString();
                QString he = Registros.value(1).toString();
                QString hs = Registros.value(2).toString();
                QString idr = Registros.value(3).toString();

                 ui->Registros->insertRow(ui->Registros->rowCount());
                 ui->Registros->setItem(ui->Registros->rowCount()-1,0,new QTableWidgetItem(fec));
                 ui->Registros->setItem(ui->Registros->rowCount()-1,1,new QTableWidgetItem(he));
                 ui->Registros->setItem(ui->Registros->rowCount()-1,2,new QTableWidgetItem(hs));
                 ui->Registros->setItem(ui->Registros->rowCount()-1,3,new QTableWidgetItem(idr));

                 ui->Reservaciones->addItem(Registros.value(3).toString());
           }
       }
     }

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
        reservar.prepare("INSERT INTO ReservacionUnica SET idTarifa = (SELECT idTarifa FROM Tarifa WHERE idTarifa = 1), Pago_idPago = (SELECT idPago FROM Pago WHERE idPago = 1), NoEspacio = (SELECT NoEspacio FROM Espacio WHERE NoEspacio = 1), IdUsuario = (SELECT IdUsuario FROM Usuario WHERE IdUsuario = :idU), Fecha = :date, HoraEntrada = :llegada, HoraSalida = :salida;");
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
        reservar.prepare("INSERT INTO ReservacionMensual SET  idTarifa = (SELECT idTarifa FROM Tarifa WHERE idTarifa = 1), idPago = (SELECT idPago FROM Pago WHERE idPago = 1), NoEspacio = (SELECT NoEspacio FROM Espacio WHERE NoEspacio = 1), IdUsuario = (SELECT IdUsuario FROM Usuario WHERE IdUsuario = :idU), FechaInicio = :dateI, FechaFin = :dateF, HoraEntrada = :llegada, HoraSalida = :salida,;");
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


void Cliente::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}



void Cliente::on_Actualizar_clicked()
{

    //se obtiene el valor cambiado desde qtable gracias a la funcion item para la columna, data para el dato de la casilla
    QString HoraSalidaAct;
    QString HoraEntradaAct;
    HoraEntradaAct = ui->tableWidget->item(0,4)->data(0).toString();
    HoraSalidaAct = ui->tableWidget->item(0,5)->data(0).toString();
    qDebug() << HoraSalidaAct;

    /*comparar horas y minutos
    QTime t1 = QTime::fromString(HoraEntradaAct);
    QTime t2 = QTime::fromString(HoraSalidaAct);
    if(t2.hour() >= t1.hour() && (t1.minute() - t2.minute() != 0) ){
    */

    //
    //Mensaje de confirmacion para la actualizacion
    QMessageBox message, informacion;
    message.setText(tr("¿Confirmar actualizacion de horario?"));
    informacion.setText(tr("Su actualizacion fue realizada exitosamente."));
    QAbstractButton * confirmar = message.addButton(tr("Aceptar"), QMessageBox::AcceptRole);
    QAbstractButton * cancelar = message.addButton(tr("Aceptar"), QMessageBox::NoRole);
    QAbstractButton * aceptar = informacion.addButton(tr("Aceptar"), QMessageBox::AcceptRole);

    message.exec();
    if(message.clickedButton() == confirmar){

        if(conexion.open()){
        QSqlQuery ExtenderReservacion;
            if(ExtenderReservacion.exec("Update reservacionunica set HoraSalida = '"+HoraSalidaAct+"' where "+IDUsuario))
            qDebug() << "Si calo";
        }
        else{
           qDebug() << "No calo";
        }

    }

}

void Cliente::on_modificar_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}
