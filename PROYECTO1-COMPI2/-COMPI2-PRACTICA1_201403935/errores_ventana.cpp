#include "errores_ventana.h"
#include "ui_errores_ventana.h"

Errores_ventana::Errores_ventana(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Errores_ventana)
{
    ui->setupUi(this);
}

Errores_ventana::~Errores_ventana()
{
    delete ui;
}
