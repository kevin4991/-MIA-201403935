#include "visualizador_html.h"
#include "ui_visualizador_html.h"

visualizador_html::visualizador_html(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::visualizador_html)
{
    ui->setupUi(this);
}

visualizador_html::~visualizador_html()
{
    delete ui;
}
