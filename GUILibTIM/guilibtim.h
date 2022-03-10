#ifndef GUILIBTIM_H
#define GUILIBTIM_H

#include <QDebug>

#include <QMainWindow>
#include <QGraphicsView>
#include <QtCharts>

using namespace QtCharts;

#include "Algorithms/ComponentTree.h"
#include "Algorithms/ConnectedComponents.h"
#include "Common/FlatSE.h"
#include "Common/Image.h"

using namespace LibTIM;

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

    void update_views(QPoint p);
    void update_views();

private:
    Ui::GUILibTIM *ui;
    // graphic view (image)
    QGraphicsScene *graphicsScene;
    QGraphicsRectItem *rectClick = nullptr;
    QPoint selection;
    // chart view (values)
    QLineSeries* series_A;
    QLineSeries* series_B;
    QValueAxis* axis_X;
    QValueAxis* axis_YA;
    QValueAxis* axis_YB;

    ComponentTree<U8> *componentTree = nullptr;

    void computeComponentTree(QImage image);
};

#endif // GUILIBTIM_H
