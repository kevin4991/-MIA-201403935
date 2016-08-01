#include "principal.h"
#include "ui_principal.h"

PRINCIPAL::PRINCIPAL(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PRINCIPAL)
{
    ui->setupUi(this);
}

PRINCIPAL::~PRINCIPAL()
{
    delete ui;
}
