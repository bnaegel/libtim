#include "guilibtim.h"
#include "ui_guilibtim.h"

GUILibTIM::GUILibTIM(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUILibTIM)
{
    ui->setupUi(this);

    graphicsScene_1 = new QGraphicsScene();
    ui->graphicsView_1->setScene(graphicsScene_1);
    selection = QPoint(0, 0);

    graphicsScene_2 = new QGraphicsScene();
    ui->graphicsView_2->setScene(graphicsScene_2);

    external_view = new QGraphicsView();

    series_criterion = new QLineSeries();
    // series_A->setColor(QColor(0.0, 0.0, 255.0));
    series_attribute = new QLineSeries();
    // series_B->setColor(QColor(0.0, 255.0, 0.0));
    series_nodes = new QScatterSeries();
    series_nodes->setMarkerSize(10.0);
    series_nodes->setColor(QColor(255.0, 0.0, 0.0));
    axis_X = new QValueAxis();
    axis_Y_criterion = new QValueAxis();
    axis_Y_attribute = new QValueAxis();
    axis_X->setRange(0, 255);
    ui->chartView->chart()->addAxis(axis_X, Qt::AlignBottom);
    ui->chartView->chart()->addAxis(axis_Y_criterion, Qt::AlignLeft);
    ui->chartView->chart()->addAxis(axis_Y_attribute, Qt::AlignRight);
    ui->chartView->chart()->addSeries(series_criterion);
    ui->chartView->chart()->addSeries(series_attribute);
    ui->chartView->chart()->addSeries(series_nodes);
    series_criterion->attachAxis(axis_X);
    series_attribute->attachAxis(axis_X);
    series_nodes->attachAxis(axis_X);
    series_criterion->attachAxis(axis_Y_criterion);
    series_attribute->attachAxis(axis_Y_attribute);

    connect(ui->graphicsView_1, SIGNAL(mousePressed(QPoint)),
            this, SLOT(update_selection(QPoint)));
    connect(ui->graphicsView_2, SIGNAL(mouseDoubleClicked()),
            this, SLOT(show_view_2_detached()));
}

GUILibTIM::~GUILibTIM()
{
    delete ui;
}

void GUILibTIM::on_actionImport_TIFF_triggered()
{
    // todo : replace with QFileDialog
    QString filename = "i3_2_slices.tif";
    qDebug() << filename;

    QImage image;
    if (image.load(filename))
    {
        this->libtim_image = ImageFromQImage(image);
        update_view_image(QImageFromImage(libtim_image));
        computeComponentTree(libtim_image);
    }

    // todo : 3D TIFF read implementation
    qDebug() << "NotImplemented";
}

void GUILibTIM::on_actionImport_PNG_triggered()
{
    // todo : replace with QFileDialog
    QString filename = "i3_1_slices.png";
    qDebug() << filename;

    QImage image;
    if (image.load(filename))
    {
        libtim_image = ImageFromQImage(image);
        update_view_image(QImageFromImage(libtim_image));
        computeComponentTree(libtim_image);
    }
}

void GUILibTIM::on_actionInvert_Image_triggered()
{
    for (TSize i = 0; i < libtim_image.getSizeX(); i++)
      for (TSize j = 0; j < libtim_image.getSizeY(); j++)
        for (TSize k = 0; k < libtim_image.getSizeZ(); k++) {
          libtim_image(i, j, k) = 255 - libtim_image(i, j, k);
        }

    update_view_image(QImageFromImage(libtim_image));
    computeComponentTree(libtim_image);
}

void GUILibTIM::on_comboBox_criterion_currentIndexChanged(int)
{
    update_view_chart();
}

void GUILibTIM::on_comboBox_attribute_currentIndexChanged(int)
{
    update_view_chart();
}

void GUILibTIM::on_doubleSpinBox_min_criterion_valueChanged(double)
{
    update_view_chart();
}

void GUILibTIM::on_doubleSpinBox_min_attribute_valueChanged(double)
{
    update_view_chart();
}

void GUILibTIM::on_doubleSpinBox_max_criterion_valueChanged(double)
{
    update_view_chart();
}

void GUILibTIM::on_doubleSpinBox_max_attribute_valueChanged(double)
{
    update_view_chart();
}

void GUILibTIM::on_pushButton_view_attribute_image_clicked()
{
    update_view_2_attribute_image();
}

void GUILibTIM::on_checkBox_view_node_stateChanged(int)
{
    update_view_2_node_pixels();
}

void GUILibTIM::on_spinBox_view_node_branch_valueChanged(int)
{
    update_view_chart();
    update_view_2_node_pixels();
    update_statusBar();
}

void GUILibTIM::on_horizontalSlider_Z_valueChanged(int)
{
    // todo, no 3D image for now.
}

void GUILibTIM::update_view_image(QImage image)
{
    QPixmap pixmap = QPixmap::fromImage(image);
    graphicsScene_1->clear();
    rectClick = nullptr;
    graphicsScene_1->addPixmap(pixmap);
}

void GUILibTIM::update_selection(QPoint p)
{
    selection = p;
    update_view_1();
    update_view_2_node_pixels();
    update_view_chart();
    update_statusBar();
}

void GUILibTIM::update_view_1()
{
    if(rectClick != nullptr)
        delete rectClick;

    rectClick = graphicsScene_1->addRect
            (selection.x()-4, selection.y()-4, 8, 8,
             QPen(QColor(255, 0, 0)), QBrush(QColor(255, 0, 0)));
}

void GUILibTIM::update_view_2_node_pixels()
{
    if(!ui->checkBox_view_node->isChecked())
        return;
    if(componentTree == nullptr)
        return;

    QImage view_image;
    view_image = QImageFromImage(libtim_image);
    // view_image.convertTo(QImage::Format_RGB888); // Qt 5.1X
    view_image = view_image.convertToFormat(QImage::Format_RGB888);

    Node* n = componentTree->coordToNode(selection.x(), selection.y(), 0);

    for(int i = 0; i < ui->spinBox_view_node_branch->value(); ++i)
        n = n->father;

    std::vector<TOffset> pixels = componentTree->merge_pixels(n);
    /*
    std::function<std::vector<TOffset>(Node* n)> get_node_pixels;
    get_node_pixels = [&get_node_pixels](Node* n)->std::vector<TOffset>
    {
        std::vector<TOffset> all = n->pixels;

        Node::ContainerChilds::iterator jt;
        for (jt = n->childs.begin(); jt != n->childs.end(); ++jt) {
          std::vector<TOffset> tmp = get_node_pixels(*jt);

          all.insert(all.end(), tmp.begin(), tmp.end());
        }
        return all;
    };
    std::vector<TOffset> pixels = get_node_pixels(n);
    */

    for(std::vector<TOffset>::iterator it = std::begin(pixels); it != std::end(pixels); ++it) {
        Point<TCoord> p = libtim_image.getCoord((*it));

        view_image.setPixelColor(p.x, p.y, QColor(255.0, 0.0, 0.0));
    }

    graphicsScene_2->clear();
    QPixmap pixmap = QPixmap::fromImage(view_image);
    graphicsScene_2->addPixmap(pixmap);
}

void GUILibTIM::update_view_2_attribute_image()
{
    if(componentTree == nullptr)
        return;

    QString choice_attribute = ui->comboBox_attribute->currentText();
    QString choice_criterion = ui->comboBox_criterion->currentText();
    QString choice_selection_rule = ui->comboBox_selection_rule->currentText();

    int64_t limit = ui->doubleSpinBox_max_attribute->value();
    ComponentTree<U8>::Attribute attribute = AttributeFromQString(choice_attribute);
    ComponentTree<U8>::Attribute criterion = AttributeFromQString(choice_criterion);
    ComponentTree<U8>::ConstructionDecision rule = ConstructionDecisionFromQString(choice_selection_rule);

    QImage view_image;

    if(choice_attribute == "H") // int
    {
        Image<int64_t> res = componentTree->constructImageAttribute<int, long double>
                  (attribute, criterion, rule);
        view_image = QImageFromImage(res, limit);
    }
    else if (choice_attribute == "AREA")// int64_t
    {
        Image<int64_t> res = componentTree->constructImageAttribute<int64_t, long double>
                  (attribute, criterion, rule);
        view_image = QImageFromImage(res, limit);
    }
    else if (choice_attribute == "MSER")// long double
    {
        Image<long double> res = componentTree->constructImageAttribute<long double, long double>
                  (attribute, criterion, rule);
        view_image = QImageFromImage(res, limit);
    }
    else
    {
        qDebug() << "ERROR : update_view_2_attribute_image";
        exit(1);
    }

    graphicsScene_2->clear();
    QPixmap pixmap = QPixmap::fromImage(view_image);
    graphicsScene_2->addPixmap(pixmap);

}

void GUILibTIM::update_view_chart()
{
    if(componentTree == nullptr)
        return;

    series_criterion->clear();
    series_attribute->clear();
    series_nodes->clear();

    Node* n = componentTree->coordToNode(selection.x(), selection.y(), 0);
    series_nodes->append(n->h, 0);
    for(int i = 0; i < ui->spinBox_view_node_branch->value(); ++i)
        n = n->father;
    series_nodes->append(n->h, 0);

    Node* node = componentTree->coordToNode(selection.x(), selection.y(), 0);

    long double min_crit, min_attr, max_crit, max_attr, crit, attr,
            limit_min_crit, limit_min_attr, limit_max_crit, limit_max_attr;

    min_crit = std::numeric_limits<long double>::max();
    min_attr = std::numeric_limits<long double>::max();
    max_crit = 0;
    max_attr = 0;

    limit_min_crit = ui->doubleSpinBox_min_criterion->value();
    limit_min_attr = ui->doubleSpinBox_min_attribute->value();
    limit_max_crit = ui->doubleSpinBox_max_criterion->value();
    limit_max_attr = ui->doubleSpinBox_max_attribute->value();
    QString choice_attribute = ui->comboBox_attribute->currentText();
    QString choice_criterion = ui->comboBox_criterion->currentText();

    ComponentTree<U8>::Attribute attribute = AttributeFromQString(choice_attribute);
    ComponentTree<U8>::Attribute criterion = AttributeFromQString(choice_criterion);

    while(node != componentTree->m_root)
    {
        crit = componentTree->getAttribute<long double>(node, criterion);
        // A = (node->father->area - node->area) / (node->area);

        if(limit_max_crit > 0 && crit > limit_max_crit)
            crit = limit_max_crit;
        if(limit_min_crit < 0 && crit < limit_min_crit)
            crit = limit_min_crit;

        series_criterion->append(node->h, crit);
        max_crit = std::max(max_crit, crit);
        min_crit = std::min(min_crit, crit);

        attr = componentTree->getAttribute<long double>(node, attribute);
        if(limit_max_attr > 0 && attr > limit_max_attr)
            attr = limit_max_attr;
        if(limit_min_attr < 0 && attr < limit_min_attr)
            attr = limit_min_attr;

        series_attribute->append(node->h, attr);
        max_attr = std::max(max_attr, attr);
        min_attr = std::min(min_attr, attr);

        node = node->father;
    }

    if(limit_max_crit > 0)
        axis_Y_criterion->setMax(limit_max_crit);
    else
        axis_Y_criterion->setMax(max_crit);

    if(limit_min_crit < 0)
        axis_Y_criterion->setMin(limit_min_crit);
    else
        axis_Y_criterion->setMin(min_crit);

    if(limit_max_attr > 0)
        axis_Y_attribute->setMax(limit_max_attr);
    else
        axis_Y_attribute->setMax(max_attr);
    if(limit_min_attr < 0)
        axis_Y_attribute->setMin(limit_min_attr);
    else
        axis_Y_attribute->setMin(min_attr);
}

void GUILibTIM::update_statusBar()
{
    if(componentTree == nullptr)
        return;

    QString message;

    message += "x : ";
    message += QString::number(selection.x()).rightJustified(4, '0');
    message += ", ";

    message += "y : ";
    message += QString::number(selection.y()).rightJustified(4, '0');
    message += ", ";

    Node* n = componentTree->coordToNode(selection.x(), selection.y(), 0);
    message += "node h : ";
    message += QString::number(n->h).rightJustified(3, '0');
    message += ", ";
    message += "node area : ";
    message += QString::number(n->area).rightJustified(10, '0');
    message += ", ";

    for(int i = 0; i < ui->spinBox_view_node_branch->value(); ++i)
        n = n->father;
    message += "father h : ";
    message += QString::number(n->h).rightJustified(3, '0');
    message += ", ";
    message += "father area : ";
    message += QString::number(n->area).rightJustified(10, '0');
    message += ", ";

    ui->statusbar->showMessage(message);
}

void GUILibTIM::show_view_2_detached()
{
    external_view->setScene(graphicsScene_2);
    if(!external_view->isVisible())
        external_view->show();
}

ComponentTree<U8>::Attribute GUILibTIM::AttributeFromQString(QString choice)
{
    ComponentTree<U8>::Attribute a;

    if(choice == "H")
    {
        a = ComponentTree<U8>::H;
    }
    else if(choice == "AREA")
    {
        a = ComponentTree<U8>::AREA;
    }
    else if(choice == "AREA_D_AREAN_H")
    {
        a = ComponentTree<U8>::AREA_D_AREAN_H;
    }
    else if(choice == "AREA_D_AREAN_H_D")
    {
        a = ComponentTree<U8>::AREA_D_AREAN_H_D;
    }
    else if(choice == "AREA_D_H")
    {
        a = ComponentTree<U8>::AREA_D_H;
    }
    else if(choice == "AREA_D_AREAN")
    {
        a = ComponentTree<U8>::AREA_D_AREAN;
    }
    else if(choice == "MSER")
    {
        a = ComponentTree<U8>::MSER;
    }
    else if(choice == "AREA_D_DELTA_H")
    {
        a = ComponentTree<U8>::AREA_D_DELTA_H;
    }
    else if(choice == "AREA_D_DELTA_AREAF")
    {
        a = ComponentTree<U8>::AREA_D_DELTA_AREAF;
    }
    else
    {
        qDebug() << "ERROR : AttributeFromQString";
        exit(1);
    }
    return a;
}

ComponentTree<U8>::ConstructionDecision GUILibTIM::ConstructionDecisionFromQString(QString choice)
{
    ComponentTree<U8>::ConstructionDecision rule;
    if(choice == "MIN")
    {
        rule = ComponentTree<U8>::MIN;
    }
    else if(choice == "MAX")
    {
        rule= ComponentTree<U8>::MAX;
    }
    else if(choice == "DIRECT")
    {
        rule = ComponentTree<U8>::DIRECT;
    }
    else
    {
        qDebug() << "ERROR : ConstructionDecisionFromQString";
        exit(1);
    }
    return rule;
}

Image<U8> GUILibTIM::ImageFromQImage(QImage &qimage)
{
    Image<U8> image(qimage.width(), qimage.height(), 1);

    for(int x = 0; x < qimage.width(); ++x)
        for(int y = 0; y < qimage.height(); ++y)
            image(x, y, 0) = qimage.pixel(x, y);

    return image;
}

QImage GUILibTIM::QImageFromImage(Image<U8> &image)
{
    // return QImage(image.getData(), image.getSizeX(), image.getSizeY(), QImage::Format_Grayscale8);

    QImage qimage(image.getSizeX(), image.getSizeY(), QImage::Format_Grayscale8);

    for(int x = 0; x < qimage.width(); ++x)
        for(int y = 0; y < qimage.height(); ++y)
        {
            QColor color(image(x, y, 0), image(x, y, 0), image(x, y, 0));
            qimage.setPixelColor(x, y, color);
        }

    return qimage;
}

QImage GUILibTIM::QImageFromImage(Image<int> &image, int limit)
{
    QImage qimage(image.getSizeX(), image.getSizeY(), QImage::Format_Grayscale8);

    if(limit == 0)
        limit = std::max(image.getMax(), 1);

    for(int x = 0; x < qimage.width(); ++x)
    {
        for(int y = 0; y < qimage.height(); ++y)
        {
            double normalized_val = ((double)std::min(image(x, y, 0), limit) / (double)limit);
            int c = ((double)normalized_val*255.0);
            QColor color(c, c, c);
            qimage.setPixelColor(x, y, color);
        }
    }

    return qimage;
}

QImage GUILibTIM::QImageFromImage(Image<int64_t> &image, int64_t limit)
{
    QImage qimage(image.getSizeX(), image.getSizeY(), QImage::Format_Grayscale8);

    if(limit == 0)
        limit = std::max(image.getMax(), (int64_t)1);

    for(int x = 0; x < qimage.width(); ++x)
    {
        for(int y = 0; y < qimage.height(); ++y)
        {
            double normalized_val = ((double)std::min(image(x, y, 0), limit) / (double)limit);
            int c = ((double)normalized_val*255.0);
            QColor color(c, c, c);
            qimage.setPixelColor(x, y, color);
        }
    }

    return qimage;
}

QImage GUILibTIM::QImageFromImage(Image<long double> &image, long double limit)
{
    QImage qimage(image.getSizeX(), image.getSizeY(), QImage::Format_Grayscale8);

    if(limit == 0)
        limit = std::max((long double)image.getMax(), (long double)1.0);

    long double normalized_val;
    int c;

    for(int x = 0; x < qimage.width(); ++x)
    {
        for(int y = 0; y < qimage.height(); ++y)
        {
            normalized_val = ((long double)std::min(image(x, y, 0), limit) / (long double)limit);
            c = ((long double)normalized_val*255.0);
            QColor color(c, c, c);
            qimage.setPixelColor(x, y, color);
        }
    }

    return qimage;
}

void GUILibTIM::computeComponentTree(Image<U8> &image)
{
    if(componentTree != nullptr)
        delete componentTree;

    unsigned int delta = 5;
    FlatSE connexity;
    connexity.make3DN26();
    componentTree = new ComponentTree<U8>(image, connexity, delta);
}
