#ifndef CLIENTE_H
#define CLIENTE_H
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QDialog>

namespace Ui {
class Cliente;
}

class Cliente : public QDialog
{
    Q_OBJECT

public:
    explicit Cliente(QWidget *parent = nullptr);
    ~Cliente();

private slots:

    void on_BotonAgregar_clicked();


private:
    Ui::Cliente *ui;
    QSqlDatabase conexion;
};

#endif // CLIENTE_H
