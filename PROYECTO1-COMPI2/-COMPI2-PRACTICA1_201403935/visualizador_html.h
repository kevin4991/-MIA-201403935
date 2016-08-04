#ifndef VISUALIZADOR_HTML_H
#define VISUALIZADOR_HTML_H

#include <QMainWindow>

namespace Ui {
class visualizador_html;
}

class visualizador_html : public QMainWindow
{
    Q_OBJECT

public:
    explicit visualizador_html(QWidget *parent = 0);
    ~visualizador_html();

private:
    Ui::visualizador_html *ui;
};

#endif // VISUALIZADOR_HTML_H
