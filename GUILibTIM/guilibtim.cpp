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
        graphicsScene->addPixmap(pixmap);
    }
}
