#ifndef GUILIBTIM_H
#define GUILIBTIM_H

#include <QDebug>

#include <QMainWindow>
#include <QGraphicsView>
#include <QtCharts>

using namespace QtCharts;

#include "qgraphicsviewclick.h"

#include "Algorithms/ComponentTree.h"
#include "Algorithms/ConnectedComponents.h"
#include "Algorithms/Morphology.h"
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
    void on_actionImport_Image_triggered();
    void on_actionImport_ImageSequence_triggered();
    void on_actionInvert_Image_triggered();
    void on_actionFilterArea_triggered();
    void on_actionFilterContrast_triggered();
    void on_actionMorphological_Gradient_triggered();

    void on_comboBox_criterion_currentIndexChanged(int);
    void on_comboBox_attribute_currentIndexChanged(int);
    void on_doubleSpinBox_min_criterion_valueChanged(double);
    void on_doubleSpinBox_min_attribute_valueChanged(double);
    void on_doubleSpinBox_max_criterion_valueChanged(double);
    void on_doubleSpinBox_max_attribute_valueChanged(double);
    void on_pushButton_view_attribute_image_clicked();
    void on_checkBox_view_node_stateChanged(int);
    void on_spinBox_view_node_branch_valueChanged(int);
    void on_horizontalSlider_Z_valueChanged(int);

    void update_view_image(QImage image);
    void update_selection(QPoint p);
    void update_view_1();
    void update_view_2_node_pixels();
    void update_view_2_attribute_image();
    void update_view_chart();
    void update_statusBar();
    void show_view_2_detached();

private:
    // libtim <-> QString
    ComponentTree<U8>::Attribute AttributeFromQString(QString choice);
    ComponentTree<U8>::ConstructionDecision ConstructionDecisionFromQString(QString choice);
    // UI
    Ui::GUILibTIM *ui;
    // graphic view (image)
    QGraphicsScene *graphicsScene_1;
    QGraphicsRectItem *rectClick = nullptr;
    QPoint selection;
    unsigned int selection_z = 0;
    // chart view (values)
    QLineSeries* series_criterion;
    QLineSeries* series_attribute;
    QScatterSeries* series_nodes;
    QValueAxis* axis_X;
    QValueAxis* axis_Y_criterion;
    QValueAxis* axis_Y_attribute;
    // grahic view (nodes pixel or attribute image)
    QGraphicsScene *graphicsScene_2;
    // grahic view (external)
    QGraphicsViewClick *external_view;

    // DATA
    // image
    Image<U8> libtim_image;
    // component tree
    ComponentTree<U8> *componentTree = nullptr;
    static Image<U8> ImageFromQImage(QImage &qimage);
    static Image<U8> ImageFromQImageList(QList<QImage> &qimages);
    static QImage    QImageFromImage(Image<U8> &image, unsigned int z=0);
    static QImage    QImageFromImage(Image<int> &image, int limit, unsigned int z=0);
    static QImage    QImageFromImage(Image<int64_t> &image, int64_t limit, unsigned int z=0);
    static QImage    QImageFromImage(Image<long double> &image, long double limit, unsigned int z=0);
    void computeComponentTree(Image<U8> &image);
};

#endif // GUILIBTIM_H
