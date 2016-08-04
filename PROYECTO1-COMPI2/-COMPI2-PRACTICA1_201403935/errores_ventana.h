#ifndef ERRORES_VENTANA_H
#define ERRORES_VENTANA_H

#include <QMainWindow>

namespace Ui {
class Errores_ventana;
}

class Errores_ventana : public QMainWindow
{
    Q_OBJECT

public:
    explicit Errores_ventana(QWidget *parent = 0);
    ~Errores_ventana();

private:
    Ui::Errores_ventana *ui;
};

#endif // ERRORES_VENTANA_H
