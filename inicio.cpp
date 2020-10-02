#include "inicio.h"
#include "ui_inicio.h"

Inicio::Inicio(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Inicio)
{
    ui->setupUi(this);
    conexion = QSqlDatabase::addDatabase("QODBC");
    conexion.setUserName("root");
    conexion.setDatabaseName("ParkingALot");

}

Inicio::~Inicio()
{
    delete ui;
}

void Inicio::on_clickaqui_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void Inicio::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void Inicio::on_pushButton_5_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void Inicio::on_IniciaSesionAlt_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void Inicio::on_RegistrarseAlt_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


int Inicio::getIdCliente(){
    QSqlQuery idCliente;
    int sigId = 0;
    conexion.open();
    idCliente.exec("SELECT COUNT(IdUsuario) FROM Cliente;");
    while (idCliente.next()) {
        sigId = idCliente.value(0).toInt();
    }
    return sigId + 1;
}

void Inicio::on_btnRegistarr_clicked()
{
    int idCli = getIdCliente() + 1000;
    QMessageBox mensaje, info;
    mensaje.setText(tr("¿Sus datos son correctos?"));
    info.setText("Su número de cliente es: "+ QString::number(idCli) +"\nUselo para iniciar sesion");
    QAbstractButton * confirmar = mensaje.addButton(tr("Aceptar"), QMessageBox::AcceptRole);
    QAbstractButton * cancelar = mensaje.addButton(tr("Aceptar"), QMessageBox::NoRole);
    QAbstractButton * aceptar = info.addButton(tr("Aceptar"), QMessageBox::AcceptRole);
    mensaje.exec();
    if(mensaje.clickedButton() == confirmar){
        QString nombre = ui->txtNombre->text(), apePat = ui->txtApPat->text(), apeMat = ui->txtApMat->text(), dir = ui->txtDir->text(), tel = ui->txtTel->text();
        QDate dob = ui->dteFechaNac->date();
        QSqlQuery registro;
        QSqlQuery registroC;

        qDebug() << "Id a insertar: " << idCli;
        QString idCliente = QString::number(idCli);
        if(conexion.open()){
            qDebug() << "Conexion establecia";
            registroC.prepare("CALL REGISTRARCLIENTE("+idCliente+",'"+nombre+"','"+apePat+"','"+apeMat+"',:FNAC,"+tel+",'"+dir+"');");
            registroC.bindValue(":FNAC", dob);
            if(registroC.exec()){
                conexion.close();
                info.exec();
                qDebug() << "Query ejecutado";
            }else {
                qDebug() << "Puro chile con el query";
            }
        }else{
            qDebug() << "Base de datos no abierta";
        }
    }


}

void Inicio::on_pushButton_clicked()
{
    QMessageBox info;
    info.setText("Número de usuario no encontrado");
    QAbstractButton * acept = info.addButton(tr("Aceptar"), QMessageBox::AcceptRole);
    int noCliente = ui->txtNoCliente->text().toInt();
    QSqlQuery sesion;
    conexion.open();
    sesion.prepare("SELECT u.IdUsuario, u.Nombre, u.ApellidoP, u.apellidoM, u.FechaN, u.Telefono, u.Direccion, u.Matricula, c.Mensual FROM Usuario AS u INNER JOIN Cliente AS c ON u.IdUsuario = c.IdUsuario WHERE u.IdUsuario = :noC;");
    sesion.bindValue(":noC", noCliente);
    sesion.exec();
    while (sesion.next()) {
        qDebug() << "Query ejecutado";
        QString idUsuario =  sesion.value(0).toString();
        qDebug() << "Id devuelto: " << sesion.value(0).toInt();
        if(sesion.value(0).toInt() != noCliente){
            info.exec();
            qDebug() << "Cliente no encontrado";
        }else{
            QString name = sesion.value(1).toString(),
                    lastP = sesion.value(2).toString(),
                    lastM = sesion.value(3).toString(),
                    adress = sesion.value(6).toString(),
                    mat = sesion.value(7).toString();
            QDate dob = sesion.value(4).toDate();
            int phone = sesion.value(5).toInt(),
                    nCl = sesion.value(0).toInt(),
                    month = sesion.value(7).toInt();
            clienteContenido sesion(name, lastP, lastM, adress, dob, phone, nCl, month,idUsuario);
            Cliente pd(&sesion, this);
            pd.exec();
        }
    }

}

void Inicio::on_BotonEntrarAlternativo_clicked()
{

    QMessageBox information;
    information.setText("Matricula no encontrada.");
    QAbstractButton * acept = information.addButton(tr("Aceptar"), QMessageBox::AcceptRole);
    QString Matricula = ui->txtMatricula->text();
    QSqlQuery login;
    conexion.open();
    login.prepare("SELECT u.IdUsuario, u.Nombre, u.ApellidoP, u.apellidoM, u.FechaN, u.Telefono, u.Direccion, u.Matricula, c.Mensual FROM Usuario AS u INNER JOIN Cliente AS c ON u.IdUsuario = c.IdUsuario WHERE u.Matricula = :noC;");
    login.bindValue(":noC", Matricula);
    login.exec();
    while (login.next()) {
        qDebug() << "Query ejecutado";
        QString Mat =  login.value(7).toString();
        qDebug() << "Matricula devuelta: " << login.value(7).toString();
        if(login.value(7).toString() != Matricula){
            information.exec();
            qDebug() << "Cliente no encontrado";
        }else{
            QString name = login.value(1).toString(),
                    lastP = login.value(2).toString(),
                    lastM = login.value(3).toString(),
                    adress = login.value(6).toString(),
                    mat = login.value(7).toString();
            QDate dob = login.value(4).toDate();
            int phone = login.value(5).toInt(),
                    nCl = login.value(0).toInt(),
                    month = login.value(7).toInt();
            clienteContenido login(name, lastP, lastM, adress, dob, phone, nCl, month,Mat);
            Cliente pd(&login, this);
            pd.exec();
        }
    }

}

