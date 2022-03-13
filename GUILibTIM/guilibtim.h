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

    void on_actionInvert_Image_triggered();

    void on_spinBox_view_node_branch_valueChanged(int);

    void on_pushButton_view_attribute_clicked();

    void update_views(QPoint p);
    void update_views();
    void update_view_node();

private:
    // UI
    Ui::GUILibTIM *ui;
    // graphic view (image)
    QGraphicsScene *graphicsScene;
    QGraphicsRectItem *rectClick = nullptr;
    QPoint selection;
    void update_image_view(QImage image);
    // chart view (values)
    QLineSeries* series_A;
    QLineSeries* series_B;
    QValueAxis* axis_X;
    QValueAxis* axis_YA;
    QValueAxis* axis_YB;
    // grahic view (secondary)
    QGraphicsScene *graphicsScene_2;

    // DATA
    // image
    Image<U8> libtim_image;
    // component tree
    ComponentTree<U8> *componentTree = nullptr;
    static Image<U8> ImageFromQImage(QImage &qimage);
    static QImage    QImageFromImage(Image<U8> &image);
    static QImage    QImageFromImage(Image<int64_t> &image, int64_t limit);
    static QImage    QImageFromImage(Image<long double> &image, long double limit);
    void computeComponentTree(Image<U8> &image);
};

#endif // GUILIBTIM_H
