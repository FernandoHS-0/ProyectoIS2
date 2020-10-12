#include "inicio.h"
#include "ui_inicio.h"

#include <QMessageBox>

Inicio::Inicio(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Inicio)
{
    ui->setupUi(this);

    /* conexion base de datos */
    conexion = QSqlDatabase::addDatabase("QODBC");
    conexion.setPort(3306);
    conexion.setHostName("localhost");
    conexion.setUserName("root");
    conexion.setPassword("");
    conexion.setDatabaseName("ParkingALot");

    if(conexion.open())
        qDebug()<<"BASE DE DATOS CONECTADA EN INICIO";
    else
        qDebug()<<"ERROR CONEXION EN INICIO";
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
/* iniciar sesion */
void Inicio::on_pushButton_clicked()
{
    /* obtener numero de cliente */
    QString log_cliente;
    log_cliente=ui->txtNoCliente->text();

    if(log_cliente=="")
    {
        /* mensaje error a pantalla */
        qDebug()<<"Rellene el campo solicitado";
        QMessageBox::information(this,"Error","Inserte un numero de usuario","Aceptar");

        return;
    }

    /* buscar cliente */
    QSqlQuery login;
    login.prepare("SELECT `IdUsuario` FROM `cliente` WHERE `IdUsuario`= :num_cliente");
    login.bindValue(":num_cliente",log_cliente);
    login.exec();
    login.first();
    if(login.value(0).toString()==log_cliente)
    {
        qDebug()<<"Se encontro el usuario";
        Cliente pd(log_cliente,this);
        this->hide();
        pd.exec();
        this->show();

    }
    else
    {
        qDebug()<<"No se pudo encontrar el usuario";
         QMessageBox::information(this,"Error","Usuario no encontrado","Aceptar");
        /* mensaje error a pantalla de usuario no encontrado */
    }
}

void Inicio::on_BotonEntrarAlternativo_clicked()
{
    QString Matricula = ui->txtMatricula->text();
    if(Matricula=="")
    {
        QMessageBox::information(this,"Error","Ingrese una matricula valida.","Aceptar");
        return;
    }

    QSqlQuery login;
    login.prepare("SELECT u.IdUsuario, u.Nombre, u.ApellidoP, u.apellidoM, u.FechaN, u.Telefono, u.Direccion, u.Matricula, c.Mensual FROM Usuario AS u INNER JOIN Cliente AS c ON u.IdUsuario = c.IdUsuario WHERE u.Matricula = :noC;");
    login.bindValue(":noC", Matricula);
    login.exec();
    login.first();
    qDebug() << "Query ejecutado";
    QString Mat =  login.value(7).toString();
    qDebug() << "Matricula devuelta: " << login.value(7).toString();
    if(login.value(7).toString() != Matricula){
       QMessageBox::information(this,"Matricula no encontrada","Intentelo de nuevo","Aceptar");
       qDebug() << "Cliente no encontrado";
    }else{
            Cliente pd(login.value(0).toString(),this);
            this->hide();
            pd.exec();
            this->show();
    }
}

