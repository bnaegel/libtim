#ifndef GUILIBTIM_H
#define GUILIBTIM_H

#include <QMainWindow>
#include <QGraphicsView>

#include <QDebug>

namespace Ui {
class GUILibTIM;
}

class GUILibTIM : public QMainWindow
{
    Q_OBJECT

public:
    explicit GUILibTIM(QWidget *parent = 0);
    ~GUILibTIM();

private slots:
    void on_actionImport_TIFF_triggered();

    void on_actionImport_PNG_triggered();

private:
    Ui::GUILibTIM *ui;
    QGraphicsScene *graphicsScene;
};

#endif // GUILIBTIM_H
