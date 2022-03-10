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
        QPixmap pixmap = QPixmap::fromImage(image);
        graphicsScene->clear();
        rectClick = nullptr;
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

void GUILibTIM::update_views(QPoint p)
{
    selection = p;
    update_views();
}

void GUILibTIM::update_views()
{
    if(rectClick != nullptr)
        delete rectClick;

    rectClick = graphicsScene->addRect(selection.x()-4, selection.y()-4, 8, 8, QPen(QColor(255, 0, 0)), QBrush(QColor(255, 0, 0)));

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
