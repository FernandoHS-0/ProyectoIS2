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

void Inicio::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void Inicio::on_pushButton_5_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

int Inicio::getIdCliente(){
    QSqlQuery idCliente;
    int sigId;
    conexion.open();
    idCliente.exec("SELECT COUNT(IdUsuario) FROM Cliente;");
    while (idCliente.next()) {
        sigId = idCliente.value(0).toInt();
    }
    return sigId + 1;
}

void Inicio::on_btnRegistarr_clicked()
{
    QString nombre = ui->txtNombre->text(), apePat = ui->txtApPat->text(), apeMat = ui->txtApMat->text(), dir = ui->txtDir->text(), tel = ui->txtTel->text();
    QDate dob = ui->dteFechaNac->date();
    QSqlQuery registro;
    QSqlQuery registroC;
    int idCli = getIdCliente() + 1000;
    qDebug() << "Id a insertar: " << idCli;
    QString idCliente = QString::number(idCli);
    if(conexion.open()){
        qDebug() << "Conexion establecia";
        registroC.prepare("CALL REGISTRARCLIENTE("+idCliente+",'"+nombre+"','"+apePat+"','"+apeMat+"',:FNAC,"+tel+",'"+dir+"');");
        registroC.bindValue(":FNAC", dob);
        if(registroC.exec()){
            qDebug() << "Query ejecutado";
        }else {
            qDebug() << "Puro chile con el query";
        }
        conexion.close();
    }else{
        qDebug() << "Base de datos no abierta";
    }

}

void Inicio::on_pushButton_clicked()
{
    int noCliente = ui->txtNoCliente->text().toInt();
    QSqlQuery sesion;
    conexion.open();
    sesion.prepare("SELECT u.IdUsuario, u.Nombre, u.ApellidoP, u.apellidoM, u.FechaN, u.Telefono, u.Direccion, c.Mensual FROM Usuario AS u INNER JOIN Cliente AS c ON u.IdUsuario = c.IdUsuario WHERE u.IdUsuario = :noC;");
    sesion.bindValue(":noC", noCliente);
    sesion.exec();
    while (sesion.next()) {
        qDebug() << "Query ejecutado";
        qDebug() << "Id devuelto: " << sesion.value(0).toInt();
        if(sesion.value(0).toInt() == noCliente){
            QString name = sesion.value(1).toString(),
                    lastP = sesion.value(2).toString(),
                    lastM = sesion.value(3).toString(),
                    adress = sesion.value(6).toString();
            QDate dob = sesion.value(4).toDate();
            int phone = sesion.value(5).toInt(),
                    nCl = sesion.value(0).toInt(),
                    month = sesion.value(7).toInt();
            clienteContenido sesion(name, lastP, lastM, adress, dob, phone, nCl, month);
            Cliente pd(&sesion, this);
            pd.exec();
        }else{
            qDebug() << "Cliente no encontrado";
        }
    }

}
