#include "cliente.h"
#include "ui_cliente.h"
#include <QStandardItemModel>
#include <QSqlError>
#include <QTime>
#include <QPrinter>
#include <QPdfWriter>
#include <QTextDocument>
#include <QDesktopServices>


Cliente::Cliente(QString idUsuario,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Cliente)
{
    /* conexion base de datos */
    ui->setupUi(this);

    conexion = QSqlDatabase::addDatabase("QODBC");
    conexion.setUserName("root");
    conexion.setDatabaseName("ParkingALot");
    if(conexion.open())
            qDebug()<<"BASE DE DATOS CONECTADA EN CLIENTE";
    else
            qDebug()<<"ERROR CONEXION EN CLIENTE";

    /* pasa el usuario en sesion */
    IDUsuario=idUsuario;
    /* -- PRECARGA DE DATOS --*/
    QStringList titulos;
    titulos << "ID Usuario" << "ID Reservacion" << "Num Espacio" << "Fecha " << "Hora Entrada " << "Hora Salida ";
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setHorizontalHeaderLabels(titulos);

    /* OBTIENE LAS RESERVAS UNICAS DEL USUARIO */
    QSqlQuery Prueba;
    if(Prueba.exec("Select IDUSUARIO,IDRESERVACIONUNICA,NOESPACIO,FECHA,HORAENTRADA,HORASALIDA "
                   "FROM reservacionunica "
                   "WHERE IDUSUARIO ="+IDUsuario)){
        while(Prueba.next())
        {
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
    /* obtiene los registros */
     QStringList titulos2;
     ui->Registros->setColumnCount(4);
     titulos2 << "Fecha" << "Hora de entrada" << "Hora de salida" << "Numero Reservacion";
     ui->Registros->setHorizontalHeaderLabels(titulos2);
     QSqlQuery Registros;
     Registros.prepare("SELECT R.fecha, R.horaEntrada, R.horaSalida, R.IdReservacionUnica "
                       "FROM usuario as U INNER JOIN cliente as C ON U.idUsuario=C.idUsuario INNER JOIN reservacionunica as R ON C.idUsuario=R.idUsuario "
                       "WHERE U.idUsuario=:IdU;");
     Registros.bindValue(":IdU", IDUsuario);
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

    /* CARGAR DATOS PERSONALES DEL USUARIO*/
    QSqlQuery Nombre;
    QString Telefono;
    QString direccion;
    ui->EstadoCuentaWidget->setVisible(false);
    ui->InformacionAdicional->setVisible(false);
    Nombre.prepare("Select * from usuario where idUsuario="+IDUsuario);
    qDebug()<<IDUsuario;
    Nombre.exec();
    Nombre.first();
    nomb=Nombre.value(1).toString();
    ApellidoP=Nombre.value(2).toString();
    ApellidoM=Nombre.value(3).toString();
    Telefono=Nombre.value(5).toString();
    direccion= Nombre.value(6).toString();
    ui->LabelNombre->setText(nomb);
    ui->ApellidoP->setText(ApellidoP);
    ui->ApellidoM->setText(ApellidoM);
    ui->Telefono->setText(Telefono);
    ui->Direccion->setText(direccion);

    /* OVERBOOK */
    bool overbook=1;
    QList<QPushButton*> Espacios = {ui->espacio16, ui->espacio17, ui->espacio18, ui->espacio19, ui->espacio20, ui->espacio21, ui->espacio22, ui->espacio23, ui->espacio24, ui->espacio25, ui->espacio26, ui->espacio27, ui->espacio28, ui->espacio29, ui->espacio30, ui->espacio31, ui->espacio32, ui->espacio33, ui->espacio34, ui->espacio35, ui->espacio36, ui->espacio37, ui->espacio38, ui->espacio39, ui->espacio40, ui->espacio41, ui->espacio42, ui->espacio43, ui->espacio44, ui->espacio45, ui->espacio46, ui->espacio47, ui->espacio48, ui->espacio49, ui->espacio50, ui->espacio51, ui->espacio52, ui->espacio53, ui->espacio54, ui->espacio55, ui->espacio56, ui->espacio57, ui->espacio58, ui->espacio59, ui->espacio60};
    QSqlQuery Lugares;
    int iterador=0;
    Lugares.prepare("SELECT Estado FROM Espacio WHERE NoEspacio > 15;");
    Lugares.exec();
    while(Lugares.next())
    {
        if(Lugares.value(0).toString() != "Libre"){
            Espacios[iterador]->setStyleSheet("QPushButton{border: 1px solid #ff2747; border-bottom: none; background-color: white;}");
            Espacios[iterador]->setDisabled(1);
        }else{
            overbook = 0;
        }

        QSqlQuery extension;
        extension.prepare("SELECT Fecha, HoraEntradaReal, HoraSalida FROM reservacionunica WHERE idUsuario = :idU;");
        extension.bindValue(":idU", sesionCliente->noC);
        if(extension.exec()){
            while(extension.next()){
                if(extension.value(0).toDate() == QDate::currentDate() && !extension.value(1).toString().isNull()){
                    ui->ExtenderTiempo->setEnabled(1);
                    }
                else{
                    ui->ExtenderTiempo->setDisabled(1);
                }
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
    /*  MENSAJE HACER OVERBOOK*/
    QMessageBox overbook;
    overbook.setText("Actualmente no se encuentran lugares disponibles. ¿Desea reservar en overbooking?\nEl reservar en overbooking no le asegura un lugar, su acceso esta sujeto a que otro usuario no se presente a su reservación, del cual se le cederá su lugar\n¿Esta de acuerdo con esto?");
    QAbstractButton * btnAceptar = overbook.addButton("Aceptar", QMessageBox::AcceptRole);
    QAbstractButton * btnCancelar = overbook.addButton("Cancelar", QMessageBox::NoRole);
    overbook.setIcon(QMessageBox::Information);

    /* MENSAJE DE CONFIRMACION REALIZADA */
    QMessageBox confirmacion;
    confirmacion.setIcon(QMessageBox::Information);
    confirmacion.setText("Su reservación ha sido procesada correctamente.");
    QAbstractButton * oki = confirmacion.addButton("Aceptar", QMessageBox::YesRole);

    overbook.exec();

    if(overbook.clickedButton() == btnAceptar){
        //conexion.open();
        QDate fecha = ui->calendarioEst->date();
        QTime hLlegada = ui->estLlegada->time(),
              hSalida = ui->estSalida->time();
        QSqlQuery rOver;
        rOver.prepare("INSERT INTO ReservacionUnica SET idTarifa = (SELECT idTarifa FROM Tarifa WHERE idTarifa = 1), idPago = null, NoEspacio = null, IdUsuario = (SELECT IdUsuario FROM Usuario WHERE IdUsuario = :idU), Fecha = :date, HoraEntrada = :llegada, HoraSalida = :salida, HoraEntradaReal = null, HoraSalidaReal = null, Overbooking = 1;");
        rOver.bindValue(":date", fecha);
        rOver.bindValue(":llegada", hLlegada);
        rOver.bindValue(":salida", hSalida);
        rOver.bindValue(":idU", sesionCliente->noC);
        if(rOver.exec()){
            confirmacion.exec();
            conexion.close();
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
    /* parte pago */
    /* finaliza el pago y la reserva */
    double costoM;
    //obtener tarifa estandar mensual
    QSqlQuery tarifaM;
    tarifaM.prepare("SELECT  `Monto` FROM `tarifa` WHERE `idTarifa`=2");
    tarifaM.exec();
    tarifaM.first();
    costoM=tarifaM.value(0).toDouble();
    float montoTotal=costoM*240;
    ui->lb_montoTotal->setText("$"+QString::number(montoTotal));


    //conexion.open(); no necesario
    QDate date = ui->mensualInicio->selectedDate();
    QDate nueva(date.year(), date.month()+1, date.day());
    //QDate fechaR = ui->calendarioEst->selectedDate();
    /* MENSAJE CONFIRMACION RESERVACION MENSUAL */
    int mensaje=QMessageBox::question(this,"Confirmación reserva mensual","¿Desea continuar con la reserva mensual?","Aceptar","Cancelar");

    if(mensaje == 0)
    {
        tip_res=2;
        ui->stackedWidget->setCurrentIndex(4);
        return;
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

void Cliente::on_Reservaciones_currentTextChanged(const QString &arg1)
{
    int idreservacion=arg1.toInt();
    QSqlQuery Reservaciones;
    Reservaciones.prepare("Select fecha, horaEntrada, horaSalida from reservacionunica where idReservacionUnica=:Idru ;");
    Reservaciones.bindValue(":Idru", idreservacion);
    if(Reservaciones.exec()){
        while(Reservaciones.next()){
            QDate Fecha=Reservaciones.value(0).toDate();
            QTime Llegada =Reservaciones.value(1).toTime();
            QTime Salida =Reservaciones.value(2).toTime();
            ui->NuevaFecha->setDate(Fecha);
            ui->NuevaLlegada->setTime(Llegada);
            ui->NuevaSalida->setTime(Salida);

        }
    }
}

void Cliente::on_pushButton_2_clicked()
{
    QMessageBox mensaje;
        QMessageBox info;

        mensaje.setText("Confirmar modificación de reservación");
        info.setText("Su reservacion ha sido modificada");

        mensaje.setIcon(QMessageBox::Warning);
        QAbstractButton * btnSi = mensaje.addButton(tr("Confirmar"), QMessageBox::YesRole);
        QAbstractButton * btnNo = mensaje.addButton(tr("Cancelar"), QMessageBox::NoRole);

        info.setIcon(QMessageBox::Information);
        QAbstractButton * btnAceptar = info.addButton(tr("Aceptar"), QMessageBox::AcceptRole);

        mensaje.exec();
        if(mensaje.clickedButton() == btnSi){
            QSqlQuery modificacion;
            QDate modf=ui->NuevaFecha->date();
            QTime modl=ui->NuevaLlegada->time(), mods=ui->NuevaSalida->time();
            int NoReservacion=ui->Reservaciones->currentText().toInt();
            modificacion.prepare("UPDATE reservacionunica SET fecha=:modf, horaEntrada=:modl, horaSalida=:mods WHERE idReservacionUnica=:Idru");
            modificacion.bindValue(":modf", modf);
            modificacion.bindValue(":modl", modl);
            modificacion.bindValue(":mods", mods);
            modificacion.bindValue(":Idru", NoReservacion);
            if(modificacion.exec()){
                info.exec();
            }

        }
}

void Cliente::on_estadoCuenta_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}


void Cliente::on_EstadoCuenta_2_clicked(bool checked)
{
    qDebug()<<checked;
    if(checked==1){
        ui->EstadoCuentaWidget->setVisible(false);
        ui->InformacionAdicional->setVisible(false);
        checked=0;
}
    else{
    ui->totalReservaciones->setNum(TotalReservaciones);
    ui->EstadoCuentaWidget->setVisible(true);
    ui->InformacionAdicional->setVisible(true);

    QStringList titulos;
    titulos << "ID Usuario" << "ID Reservacion" << "Num Espacio" << "Fecha " << "Hora Entrada " << "Hora Salida ";
    ui->Reservas->setColumnCount(6);
    ui->Reservas->setHorizontalHeaderLabels(titulos);

    QString html =
    "<img class='imageLeft' src='C:/Users/luisd/Desktop/universidad/QT/Estacionamiento/Estacionamiento/Estacionamiento/img/park_b.png' alt='Ed' width='100' height='100'/><strong>"
    "<h1 style='text-align: center';>Estado de cuenta</h1>"
    "<hr />"
    "<h4 style='text-align: left';>A la orden de: "+ApellidoP+" "+ApellidoM+" "+nomb+"</h4>"
    "<p></p>"
    "<hr />";

    QSqlQuery Prueba;
    if(Prueba.exec("Select IDUSUARIO,IDRESERVACIONUNICA,NOESPACIO,FECHA,HORAENTRADA,HORASALIDA from reservacionunica WHERE IDUSUARIO ="+IDUsuario )){
        while(Prueba.next()){
            QString idUsuario = Prueba.value(0).toString();
            QString idReserva = Prueba.value(1).toString();
            QString NumEspacio = Prueba.value(2).toString();
            QString Fecha = Prueba.value(3).toString();
            QString HoraEntrada = Prueba.value(4).toString();
            QString HoraSalida = Prueba.value(5).toString();

            ui->Reservas->insertRow(ui->Reservas->rowCount());
            ui->Reservas->setItem(ui->Reservas->rowCount()-1,0,new QTableWidgetItem(idUsuario));
            ui->Reservas->setItem(ui->Reservas->rowCount()-1,1,new QTableWidgetItem(idReserva));
            ui->Reservas->setItem(ui->Reservas->rowCount()-1,2,new QTableWidgetItem(NumEspacio));
            ui->Reservas->setItem(ui->Reservas->rowCount()-1,3,new QTableWidgetItem(Fecha));
            ui->Reservas->setItem(ui->Reservas->rowCount()-1,4,new QTableWidgetItem(HoraEntrada));
            ui->Reservas->setItem(ui->Reservas->rowCount()-1,5,new QTableWidgetItem(HoraSalida));
            TotalReservaciones=TotalReservaciones+1;
            html += "<p>ID&nbsp; &nbsp; No.Espacio&nbsp; &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Fecha&nbsp; &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Hora Entrada&nbsp; &nbsp; Hora Salida</p>"
             "<p>"+idReserva+"&nbsp; &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"+NumEspacio+"&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"+Fecha+"&nbsp; &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"+HoraEntrada+"&nbsp; &nbsp;&nbsp;"+HoraSalida+"</p>"
             "<hr />"
             "<p></p>"
             "<p></p>";

        }
       }

    int montototal=0;
    QSqlQuery monto;

    monto.prepare("select  r.idReservacionUnica, t.monto  from reservacionunica as r inner join tarifa as t on t.idTarifa=r.idtarifa  where r.idUsuario='"+IDUsuario+"';");
   monto.exec();
   while(monto.next()){
    montototal=montototal + monto.value(1).toInt();
   }

QString total= QString::number(montototal);
QString reserv= QString::number(TotalReservaciones);
 ui->totalReservaciones->setNum(TotalReservaciones);
 ui->MontoTotal->setNum(montototal);






 html +="<blockquote>"
 "<p>PUEBLA,"+QDate::currentDate().toString() +"</p>"
 "<p>Total de reservaciones: "+reserv+"</p>"
  "<p>Monto total: "+total+"</p>"
 "</blockquote>"

 "<p style='text-align: right';>&nbsp; &nbsp; &nbsp;<img class='imageLeft' src='C:/Users/luisd/Desktop/universidad/QT/Estacionamiento/Estacionamiento/Estacionamiento/img/firma.png' alt='Ed' width='50' height='50'/>&nbsp; &nbsp; &nbsp;</p>"
"<p style='text-align: right';>_________________</p>"
"<p style='text-align: right';>PARK -A- LOT&nbsp; &nbsp; &nbsp; &nbsp;</p>"
"<hr />"
"<p style='text-align: left';></p>";

    QTextDocument document;
    document.setHtml(html);

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName("/tmp/vale.pdf");
    printer.setPageMargins(QMarginsF(15, 15, 15, 15));

    document.print(&printer);
    QDesktopServices::openUrl(QUrl::fromLocalFile("/tmp/vale.pdf"));

    }
}

void Cliente::on_pushButton_3_clicked()
{
    ui->Pisos->setCurrentIndex(0);
}

void Cliente::on_pushButton_5_clicked()
{
    ui->Pisos->setCurrentIndex(1);
}

void Cliente::on_pushButton_6_clicked()
{
    ui->Pisos->setCurrentIndex(2);
}

void Cliente::on_ExtenderTiempo_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void Cliente::on_espacio16_clicked()
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
        QDate fechaR = ui->calendarioEst->date();
        QTime llegada = ui->estLlegada->time(),
                salida = ui->estSalida->time();
        QSqlQuery reservar;
        reservar.prepare("INSERT INTO ReservacionUnica SET idTarifa = (SELECT idTarifa FROM Tarifa WHERE idTarifa = 1), Pago_idPago = (SELECT idPago FROM Pago WHERE idPago = 1), NoEspacio = (SELECT NoEspacio FROM Espacio WHERE NoEspacio = 16), IdUsuario = (SELECT IdUsuario FROM Usuario WHERE IdUsuario = :idU), Fecha = :date, HoraEntrada = :llegada, HoraSalida = :salida;");
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

void Cliente::on_espacio17_clicked()
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
        QDate fechaR = ui->calendarioEst->date();
        QTime llegada = ui->estLlegada->time(),
                salida = ui->estSalida->time();
        QSqlQuery reservar;
        reservar.prepare("INSERT INTO ReservacionUnica SET idTarifa = (SELECT idTarifa FROM Tarifa WHERE idTarifa = 1), Pago_idPago = (SELECT idPago FROM Pago WHERE idPago = 1), NoEspacio = (SELECT NoEspacio FROM Espacio WHERE NoEspacio = 17), IdUsuario = (SELECT IdUsuario FROM Usuario WHERE IdUsuario = :idU), Fecha = :date, HoraEntrada = :llegada, HoraSalida = :salida;");
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

void Cliente::on_espacio18_clicked()
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
        QDate fechaR = ui->calendarioEst->date();
        QTime llegada = ui->estLlegada->time(),
                salida = ui->estSalida->time();
        QSqlQuery reservar;
        reservar.prepare("INSERT INTO ReservacionUnica SET idTarifa = (SELECT idTarifa FROM Tarifa WHERE idTarifa = 1), Pago_idPago = (SELECT idPago FROM Pago WHERE idPago = 1), NoEspacio = (SELECT NoEspacio FROM Espacio WHERE NoEspacio = 18), IdUsuario = (SELECT IdUsuario FROM Usuario WHERE IdUsuario = :idU), Fecha = :date, HoraEntrada = :llegada, HoraSalida = :salida;");
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

void Cliente::on_espacio19_clicked()
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
        QDate fechaR = ui->calendarioEst->date();
        QTime llegada = ui->estLlegada->time(),
                salida = ui->estSalida->time();
        QSqlQuery reservar;
        reservar.prepare("INSERT INTO ReservacionUnica SET idTarifa = (SELECT idTarifa FROM Tarifa WHERE idTarifa = 1), Pago_idPago = (SELECT idPago FROM Pago WHERE idPago = 1), NoEspacio = (SELECT NoEspacio FROM Espacio WHERE NoEspacio = 19), IdUsuario = (SELECT IdUsuario FROM Usuario WHERE IdUsuario = :idU), Fecha = :date, HoraEntrada = :llegada, HoraSalida = :salida;");
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

void Cliente::on_espacio20_clicked()
{
    /*
    QMessageBox mensaje, info;
    mensaje.setText(tr("¿Confirmar reservación?"));
    info.setText(tr("Su reservación fue realizada"));
    QAbstractButton * confirmar = mensaje.addButton(tr("Aceptar"), QMessageBox::AcceptRole);
    QAbstractButton * cancelar = mensaje.addButton(tr("Aceptar"), QMessageBox::NoRole);
    QAbstractButton * aceptar = info.addButton(tr("Aceptar"), QMessageBox::AcceptRole);

    mensaje.exec();
    if(mensaje.clickedButton() == confirmar){
        conexion.open();
        QDate fechaR = ui->calendarioEst->date();
        QTime llegada = ui->estLlegada->time(),
                salida = ui->estSalida->time();
        QSqlQuery reservar;
        reservar.prepare("INSERT INTO ReservacionUnica SET idTarifa = (SELECT idTarifa FROM Tarifa WHERE idTarifa = 1), Pago_idPago = (SELECT idPago FROM Pago WHERE idPago = 1), NoEspacio = (SELECT NoEspacio FROM Espacio WHERE NoEspacio = 20), IdUsuario = (SELECT IdUsuario FROM Usuario WHERE IdUsuario = :idU), Fecha = :date, HoraEntrada = :llegada, HoraSalida = :salida;");
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
    }*/
}

void Cliente::on_espacio21_clicked()
{

}

void Cliente::on_espacio22_clicked()
{
    /* MENSAJE CONFIRMACION RESERVACION */
    int res=QMessageBox::question(this,"Confirmar reservacion","¿Está seguro de realizar esta reservacion?","Aceptar","Cancelar");

    /* HACER RESERVA */
    if(res==0) //si se confirma la reserva...
    {
        /* SECCION DE PAGO */
        double costo;
        //obtener tarifa estandar mensual
        QSqlQuery tarifa;
        tarifa.prepare("SELECT  `Monto` FROM `tarifa` WHERE `idTarifa`=1");
        tarifa.exec();
        tarifa.first();
        costo=tarifa.value(0).toDouble();
        //se capturan las horas y fecha de la reserva
        QDate fechaR = ui->calendarioEst->date();
        QTime llegada = ui->estLlegada->time(),
               salida = ui->estSalida->time();
        //obtener horas ocupadas
        float horas=salida.hour()-llegada.hour(); //horas
        float min=salida.minute()-llegada.minute(); //minutos
        float hora=min/60+horas;  //horas con decimales del tiempo de estadía.

        //calculo de monto total
        float montoTotal=costo*hora;
        ui->lb_montoTotal->setText("$"+QString::number(montoTotal));
        tip_res=0;
        ui->stackedWidget->setCurrentIndex(4);
    }
}

void Cliente::on_espacio26_clicked()
{

}

void Cliente::on_pb_finalizasReserva_clicked()
{
    if(tip_res==0)
    {
        qDebug()<<"Reserva estandar";
        /* finaliza el pago y la reserva */
        double costo;
        //obtener tarifa estandar mensual
        QSqlQuery tarifa;
        tarifa.prepare("SELECT  `Monto` FROM `tarifa` WHERE `idTarifa`=1");
        tarifa.exec();
        tarifa.first();
        costo=tarifa.value(0).toDouble();
        //se capturan las horas y fecha de la reserva
        QDate fechaR = ui->calendarioEst->date();
        QTime llegada = ui->estLlegada->time(),
               salida = ui->estSalida->time();
        //obtener horas ocupadas
        float horas=salida.hour()-llegada.hour(); //horas
        float min=salida.minute()-llegada.minute(); //minutos
        float hora=min/60+horas;  //horas con decimales del tiempo de estadía.

        //calculo de monto total
        float montoTotal=costo*hora;
        ui->lb_montoTotal->setText("$"+QString::number(montoTotal));
        //captura de informacion de pago
        QString numTar=ui->le_numTarjeta->text();
        QString nombre=ui->le_nombreTitular->text();
        QString cvv=ui->le_cvv->text();
        QDate fecha=ui->de_fechaExp->date();

        //query insertar forma de pago
        QSqlQuery metodo;
        metodo.prepare("INSERT INTO `metododepago`(`IdUsuario`, `NoTarjeta`, `NombreTarjeta`, `FechaVencimiento`, `CodigoSeguridad`) "
                       "VALUES (:numCliente,:numTarjeta,:titular,:fecha_exp,:cvv)");
        metodo.bindValue(":numCliente",IDUsuario);
        metodo.bindValue(":numTarjeta",numTar);
        metodo.bindValue(":titular",nombre);
        metodo.bindValue(":cvv",cvv);
        metodo.bindValue(":fecha_exp",fecha);
        metodo.exec();
        //se recupera el id del metodopago
        QSqlQuery pago;
        pago.prepare("SELECT MAX(`idMetodoDePago`) FROM `metododepago`");
        pago.exec();
        pago.first();
        QString idPago=pago.value(0).toString();
        //se sube la informacion del pago restante (tabla pago)
        QSqlQuery pago2;
        pago2.prepare("INSERT INTO `pago`(`idPago`, `Fecha`, `Monto`, `IdUsuario`) "
                      "VALUES (:idPago,:fecha,:monto,:idUsuario)");
        pago2.bindValue(":idPago",idPago);
        pago2.bindValue(":fecha",QDate::currentDate());
        pago2.bindValue(":monto",QString::number(montoTotal));
        pago2.bindValue(":idUsuario",IDUsuario);
        pago2.exec();
        /* SE SUBE LA INFORMACION Y REALIZA LA RESERVA ESTANDAR */
        QSqlQuery reservar;
        reservar.prepare("INSERT INTO ReservacionUnica SET idTarifa = (SELECT idTarifa FROM Tarifa WHERE idTarifa = 1), "
                         "idPago = (SELECT idPago FROM Pago WHERE idPago = :idPago), "
                         "NoEspacio = (SELECT NoEspacio FROM Espacio WHERE NoEspacio = 22), "
                         "IdUsuario = (SELECT IdUsuario FROM Usuario WHERE IdUsuario = :idU), "
                         "Fecha = :date, HoraEntrada = :llegada, HoraSalida = :salida;");
        reservar.bindValue(":idPago",idPago);
        reservar.bindValue(":date", fechaR);
        reservar.bindValue(":llegada", llegada);
        reservar.bindValue(":salida", salida);
        reservar.bindValue(":idU", IDUsuario);
        if(reservar.exec()){
            QMessageBox::information(this,"Reservacion realizada.","Gracias por su reserva!","Aceptar");
            qDebug() << "RESERVACION REALIZADA";
            //conexion.close();
        }else{
            QMessageBox::information(this,"Reservacion rechazada.","Ocurrio un error, intentelo de nuevo.","Aceptar");
            qDebug() << "RESERVACION FALLIDA";
        }
    }
    else
    {   qDebug()<<"Reserva mensual";
        /* finaliza el pago y la reserva */
        double costoM;
        //obtener tarifa estandar mensual
        QSqlQuery tarifaM;
        tarifaM.prepare("SELECT  `Monto` FROM `tarifa` WHERE `idTarifa`=2");
        tarifaM.exec();
        tarifaM.first();
        costoM=tarifaM.value(0).toDouble();
        float montoTotal=costoM*240;
        ui->lb_montoTotal->setText("$"+QString::number(montoTotal));
        //captura de informacion de pago
        QString numTar=ui->le_numTarjeta->text();
        QString nombre=ui->le_nombreTitular->text();
        QString cvv=ui->le_cvv->text();
        QString fecha=ui->de_fechaExp->text();

        //query insertar forma de pago
        QSqlQuery metodo;
        metodo.prepare("INSERT INTO `metododepago`(`IdUsuario`, `NoTarjeta`, `NombreTarjeta`, `FechaVencimiento`, `CodigoSeguridad`) "
                       "VALUES (:numCliente,:numTarjeta,:titular,:fecha_exp,:cvv)");
        metodo.bindValue(":numCliente",IDUsuario);
        metodo.bindValue(":numTarjeta",numTar);
        metodo.bindValue(":titular",nombre);
        metodo.bindValue(":cvv",cvv);
        metodo.bindValue(":fecha_exp",fecha);
        metodo.exec();
        //se recupera el id del metodopago
        QSqlQuery pago;
        pago.prepare("SELECT MAX(`idMetodoDePago`) FROM `metododepago`");
        pago.exec();
        pago.first();
        QString idPago=pago.value(0).toString();
        //se sube la informacion del pago restante (tabla pago)
        QSqlQuery pago2;
        pago2.prepare("INSERT INTO `pago`(`idPago`, `Fecha`, `Monto`, `IdUsuario`) "
                      "VALUES (:idPago,:fecha,:monto,:idUsuario)");
        pago2.bindValue(":idPago",idPago);
        pago2.bindValue(":fecha",QDate::currentDate());
        pago2.bindValue(":monto",QString::number(montoTotal));
        pago2.bindValue(":idUsuario",IDUsuario);
        pago2.exec();

        //se completa la reserva
        QTime llegada(7,00,00),
              salida(15,00,00);
        //reserva mensual
        QDate date = ui->mensualInicio->selectedDate();
        QDate nueva(date.year(), date.month()+1, date.day());
        QSqlQuery reservar;
        reservar.prepare("INSERT INTO `reservacionmensual`(`idTarifa`, `idPago`, `NoEspacio`, `IdUsuario`, `FechaInicio`, `FechaFin`, `HoraEntrada`, `HoraSalida`) "
                         "VALUES (2,:idPago,1,:idU,:dateI,:dateF,:llegada,:salida)");
        //reservar.bindValue(":date", fechaR);
        reservar.bindValue(":idPago",idPago);
        reservar.bindValue(":llegada", llegada);
        reservar.bindValue(":salida", salida);
        reservar.bindValue(":idU", IDUsuario);
        reservar.bindValue(":dateI", date);
        reservar.bindValue(":dateF", nueva);
        if(reservar.exec()){
            // mensaje reserva
            qDebug() << "Reservacion realizada";
            conexion.close();
        }else{
            qDebug() << "Fallo la reservacion";
        }
    }
    ui->stackedWidget->setCurrentIndex(0);
}

void Cliente::on_EstadoCuenta_2_clicked()
{

}
