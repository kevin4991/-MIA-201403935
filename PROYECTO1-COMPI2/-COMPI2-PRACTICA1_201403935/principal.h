#ifndef PRINCIPAL_H
#define PRINCIPAL_H

#include <QMainWindow>

namespace Ui {
class PRINCIPAL;
}

class PRINCIPAL : public QMainWindow
{
    Q_OBJECT

public:
    explicit PRINCIPAL(QWidget *parent = 0);
    ~PRINCIPAL();

private:
    Ui::PRINCIPAL *ui;
};

#endif // PRINCIPAL_H
