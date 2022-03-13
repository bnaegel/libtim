#include "guilibtim.h"
#include "ui_guilibtim.h"

GUILibTIM::GUILibTIM(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUILibTIM)
{
    ui->setupUi(this);

    graphicsScene = new QGraphicsScene();
    ui->graphicsView->setScene(graphicsScene);
    selection = QPoint(0, 0);

    external_scene = new QGraphicsScene();
    external_view = new QGraphicsView(external_scene);

    series_A = new QLineSeries();
    series_B = new QLineSeries();
    axis_X = new QValueAxis();
    axis_YA = new QValueAxis();
    axis_YB = new QValueAxis();
    axis_X->setRange(0, 255);
    ui->chartView->chart()->addAxis(axis_X, Qt::AlignBottom);
    ui->chartView->chart()->addAxis(axis_YA, Qt::AlignLeft);
    ui->chartView->chart()->addAxis(axis_YB, Qt::AlignRight);
    ui->chartView->chart()->addSeries(series_A);
    ui->chartView->chart()->addSeries(series_B);
    series_A->attachAxis(axis_X);
    series_B->attachAxis(axis_X);
    series_A->attachAxis(axis_YA);
    series_B->attachAxis(axis_YB);

    connect(ui->graphicsView, SIGNAL(mousePressed(QPoint)), this, SLOT(update_views(QPoint)));
    connect(ui->doubleSpinBox_YA, SIGNAL(valueChanged(double)), this, SLOT(update_views()));
    connect(ui->doubleSpinBox_YB, SIGNAL(valueChanged(double)), this, SLOT(update_views()));
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
        update_image_view(QImageFromImage(libtim_image));
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
        update_image_view(QImageFromImage(libtim_image));
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

    update_image_view(QImageFromImage(libtim_image));
    computeComponentTree(libtim_image);
}

void GUILibTIM::on_pushButton_view_attribute_clicked()
{
    if(componentTree == nullptr)
        return;

    QString choice = ui->comboBox_attribute->currentText();
    int64_t limit = ui->doubleSpinBox_value_limit->value();

    QImage view_image;
    if(choice == "value AREA")
    {
        Image<int64_t> res = componentTree->constructImageAttribute<int64_t, long double>
                  (ComponentTree<U8>::AREA, ComponentTree<U8>::AREA, ComponentTree<U8>::DIRECT);
        view_image = QImageFromImage(res, limit);
    }
    else if(choice == "value AREA min MSER")
    {
        Image<int64_t> res = componentTree->constructImageAttribute<int64_t, long double>
                  (ComponentTree<U8>::AREA, ComponentTree<U8>::MSER, ComponentTree<U8>::MIN);
        view_image = QImageFromImage(res, limit);
    }
    else if(choice == "value MSER min MSER")
    {
        Image<long double> res = componentTree->constructImageAttribute<long double, long double>
                  (ComponentTree<U8>::MSER, ComponentTree<U8>::MSER, ComponentTree<U8>::MIN);
        view_image = QImageFromImage(res, limit);
    }
    else
    {
        return;
    }

    external_scene->clear();
    QPixmap pixmap = QPixmap::fromImage(view_image);
    external_scene->addPixmap(pixmap);
    if(!external_view->isVisible())
        external_view->show();
}

void GUILibTIM::on_pushButton_view_node_clicked()
{
    if(componentTree == nullptr)
        return;

    QImage view_image;
    view_image = QImageFromImage(libtim_image);
    view_image.convertTo(QImage::Format_RGB888);

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

    external_scene->clear();
    QPixmap pixmap = QPixmap::fromImage(view_image);
    external_scene->addPixmap(pixmap);
    if(!external_view->isVisible())
        external_view->show();
}

void GUILibTIM::update_views(QPoint p)
{
    selection = p;

    if(rectClick != nullptr)
        delete rectClick;

    rectClick = graphicsScene->addRect(selection.x()-4, selection.y()-4, 8, 8, QPen(QColor(255, 0, 0)), QBrush(QColor(255, 0, 0)));

    if(componentTree == nullptr)
        return;

    update_views();
}

void GUILibTIM::update_views()
{
    if(componentTree == nullptr)
        return;

    series_A->clear();
    series_B->clear();

    Node* node = componentTree->coordToNode(selection.x(), selection.y(), 0);

    long double max_A, max_B, A, B, limit_A, limit_B;
    max_A = 0;
    max_B = 0;
    limit_A = ui->doubleSpinBox_YA->value();
    limit_B = ui->doubleSpinBox_YB->value();

    while(node != componentTree->m_root)
    {
        A = node->mser;

        if(limit_A > 0 && A > limit_A)
            A = limit_A;
        series_A->append(node->h, A);
        max_A = std::max(max_A, A);

        B = node->area;

        if(limit_B > 0 && B > limit_B)
            B = limit_B;
        series_B->append(node->h, B);
        max_B = std::max(max_B, B);

        node = node->father;
    }
    axis_YA->setRange(0, max_A);
    axis_YB->setRange(0, max_B);
}

void GUILibTIM::update_image_view(QImage image)
{
    QPixmap pixmap = QPixmap::fromImage(image);
    graphicsScene->clear();
    rectClick = nullptr;
    graphicsScene->addPixmap(pixmap);
}

Image<U8> GUILibTIM::ImageFromQImage(QImage &qimage)
{
    Image<U8> image(qimage.width(), qimage.height(), 1);

    for(int x = 1; x < qimage.width(); ++x)
        for(int y = 1; y < qimage.height(); ++y)
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

QImage GUILibTIM::QImageFromImage(Image<int64_t> &image, int64_t limit)
{
    QImage qimage(image.getSizeX(), image.getSizeY(), QImage::Format_Grayscale8);

    if(limit == 0)
        limit = std::max(image.getMax(), 1LL);

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
