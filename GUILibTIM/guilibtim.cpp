#include "guilibtim.h"
#include "ui_guilibtim.h"

GUILibTIM::GUILibTIM(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUILibTIM)
{
    ui->setupUi(this);

    graphicsScene = new QGraphicsScene();
    ui->graphicsView->setScene(graphicsScene);
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
        QPixmap pixmap = QPixmap::fromImage(image);
        graphicsScene->clear();
        graphicsScene->addPixmap(pixmap);

        computeComponentTree(image);
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
        QPixmap pixmap = QPixmap::fromImage(image);
        graphicsScene->clear();
        rectClick = nullptr;
        graphicsScene->addPixmap(pixmap);

        computeComponentTree(image);
    }
}

void GUILibTIM::on_graphicsView_mousePressed(QPoint p)
{
    qDebug() << p;

    if(rectClick != nullptr)
        delete rectClick;

    rectClick = graphicsScene->addRect(p.x()-4, p.y()-4, 8, 8, QPen(QColor(255, 0, 0)), QBrush(QColor(255, 0, 0)));

    if(componentTree == nullptr)
        return;

    QLineSeries* series = new QLineSeries();

    Node* node = componentTree->coordToNode(p.x(), p.y(), 0);

    while(node != componentTree->m_root)
    {
        // series->append(node->h, node->area);
        long double x = node->mser;
        long double limit = 4096;
        if(x > limit)
            x = limit;
        series->append(node->h, x);
        node = node->father;
    }

    ui->chartView->chart()->removeAllSeries();
    ui->chartView->chart()->addSeries(series);
    ui->chartView->chart()->createDefaultAxes();

    ui->chartView->chart()->axes(Qt::Horizontal)[0]->setRange(0, 255);
}

void GUILibTIM::computeComponentTree(QImage image)
{
    if(componentTree != nullptr)
        delete componentTree;

    Image<U8> im(image.width(), image.height(), 1);
    for(int x = 0; x < image.width(); ++x)
        for(int y = 0; y < image.height(); ++y)
            im(x, y, 0) = image.pixel(x, y);

    unsigned int delta = 5;
    FlatSE connexity;
    connexity.make3DN26();
    componentTree = new ComponentTree<U8>(im, connexity, delta);
}
