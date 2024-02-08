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

    external_view = new QGraphicsViewClick();

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
    connect(ui->graphicsView_2, SIGNAL(mouseDoubleClicked(QPoint)),
            this, SLOT(show_view_2_detached()));
}

GUILibTIM::~GUILibTIM()
{
    delete ui;
}

void GUILibTIM::on_actionImport_Image_triggered()
{
    QString filename = QFileDialog::getOpenFileName(
                this, "Import Image", QDir::currentPath(), "Image (*.png *.tif *.tiff)");

    if(QFile::exists(filename))
    {
        QImage image;
        if (image.load(filename))
        {
            libtim_image = ImageFromQImage(image);
            update_view_image(QImageFromImage(libtim_image));
            computeComponentTree(libtim_image);
        }
        else
        {
            QMessageBox::critical(this, "Import Image", "cannot load image");
        }
    }
    else
    {
        QMessageBox::critical(this, "Import Image", "invalid image");
    }
}

void GUILibTIM::on_actionImport_ImageSequence_triggered()
{
    QString foldername = QFileDialog::getExistingDirectory(
                this, "Import Image Sequence", QDir::currentPath(), QFileDialog::ShowDirsOnly);

    QDir dir(foldername);
    if(dir.exists())
    {
        QList<QImage> images;
        QStringList filenames = dir.entryList(QStringList() << "*.tif" << "*.tiff" << "*.png", QDir::Files);
        foreach(QString filename, filenames)
        {
            QImage image;
            if (image.load(dir.filePath(filename)))
            {
                images.append(image);
            }
        }

        if(images.size() > 0)
        {
            ui->horizontalSlider_Z->setEnabled(true);
            ui->horizontalSlider_Z->setRange(0, images.size()-1);
            ui->horizontalSlider_Z->setValue(0);
            libtim_image = ImageFromQImageList(images);
            update_view_image(QImageFromImage(libtim_image));
            computeComponentTree(libtim_image);
        }
        else
        {
            QMessageBox::critical(this, "Import Image Sequence", "invalid image sequence");
        }
    }
    else
    {
        QMessageBox::critical(this, "Import Image Sequence", "invalid image sequence");
    }
}

void GUILibTIM::on_actionExport_Image_triggered()
{
    QString filename = QFileDialog::getSaveFileName(
                this, "Export Image", QDir::currentPath());

    if(filename.isEmpty() || filename.isNull())
        return;

    image_result.save(filename + ".png");
}

void GUILibTIM::on_actionInvert_Image_triggered()
{
    for (TSize i = 0; i < libtim_image.getSizeX(); i++)
      for (TSize j = 0; j < libtim_image.getSizeY(); j++)
        for (TSize k = 0; k < libtim_image.getSizeZ(); k++) {
          libtim_image(i, j, k) = 255 - libtim_image(i, j, k);
        }

    update_view_image(QImageFromImage(libtim_image, selection_z));
    computeComponentTree(libtim_image);
}

void GUILibTIM::on_actionFilterArea_triggered()
{
    if(componentTree == nullptr)
        return;

    FlatSE connexity;
    connexity.make3DN26();
    ComponentTree<U8> filter_tree = ComponentTree<U8>(libtim_image, connexity, 1);

    QString choice_selection_rule = ui->comboBox_filter_rule->currentText();
    int min = (int)ui->doubleSpinBox_filter_min->value();
    int max = (int)ui->doubleSpinBox_filter_max->value();

    ComponentTree<U8>::ConstructionDecision construct_rule =
            ConstructionDecisionFromQString(choice_selection_rule);

    if(max == 0)
    {
        filter_tree.areaFiltering(min);
    }
    else
    {
        filter_tree.areaFiltering(min, max);
    }

    Image<U8> res = filter_tree.constructImage(construct_rule);

    graphicsScene_2->clear();
    image_result = QImageFromImage(res, selection_z);
    QPixmap pixmap = QPixmap::fromImage(image_result);
    graphicsScene_2->addPixmap(pixmap);
}

void GUILibTIM::on_actionFilterContrast_triggered()
{
    if(componentTree == nullptr)
        return;

    FlatSE connexity;
    connexity.make3DN26();
    ComponentTree<U8> filter_tree = ComponentTree<U8>(libtim_image, connexity, 1);

    QString choice_selection_rule = ui->comboBox_filter_rule->currentText();
    int min = (int)ui->doubleSpinBox_filter_min->value();
    int max = (int)ui->doubleSpinBox_filter_max->value();

    ComponentTree<U8>::ConstructionDecision construct_rule =
            ConstructionDecisionFromQString(choice_selection_rule);

    if(max == 0)
    {
        filter_tree.contrastFiltering(min);
    }
    else
    {
        filter_tree.contrastFiltering(min, max);
    }

    Image<U8> res = filter_tree.constructImage(construct_rule);

    graphicsScene_2->clear();
    image_result = QImageFromImage(res, selection_z);
    QPixmap pixmap = QPixmap::fromImage(image_result);
    graphicsScene_2->addPixmap(pixmap);
}

void GUILibTIM::on_actionMorphological_Gradient_triggered()
{
    FlatSE connexity = getConnexity();
    Image<U8> res = morphologicalGradient(libtim_image, connexity);

    graphicsScene_2->clear();
    image_result = QImageFromImage(res, selection_z);
    QPixmap pixmap = QPixmap::fromImage(image_result);
    graphicsScene_2->addPixmap(pixmap);
}

void GUILibTIM::on_actionInternal_Morphological_Gradient_triggered()
{
    FlatSE connexity = getConnexity();
    Image<U8> res = internalMorphologicalGradient(libtim_image, connexity);

    graphicsScene_2->clear();
    image_result = QImageFromImage(res, selection_z);
    QPixmap pixmap = QPixmap::fromImage(image_result);
    graphicsScene_2->addPixmap(pixmap);
}

void GUILibTIM::on_actionExternal_Morphological_Gradient_triggered()
{
    FlatSE connexity = getConnexity();
    Image<U8> res = externalMorphologicalGradient(libtim_image, connexity);

    graphicsScene_2->clear();
    image_result = QImageFromImage(res, selection_z);
    QPixmap pixmap = QPixmap::fromImage(image_result);
    graphicsScene_2->addPixmap(pixmap);
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

void GUILibTIM::on_checkBox_view_node_border_stateChanged(int arg1)
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
    selection_z = ui->horizontalSlider_Z->value();
    update_view_image(QImageFromImage(libtim_image, selection_z));
    update_view_1();
    update_view_2_node_pixels();
    update_view_chart();
    update_statusBar();
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
    view_image = QImageFromImage(libtim_image, selection_z);
    // view_image.convertTo(QImage::Format_RGB888); // Qt 5.1X
    view_image = view_image.convertToFormat(QImage::Format_RGB888);

    Node* n = componentTree->coordToNode(selection.x(), selection.y(), selection_z);

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

    if(ui->checkBox_view_node_border->isChecked())
    {
        std::vector<TOffset> pixels_border = n->pixels_border;

        for(std::vector<TOffset>::iterator it = std::begin(pixels_border); it != std::end(pixels_border); ++it) {
            Point<TCoord> p = libtim_image.getCoord((*it));

            view_image.setPixelColor(p.x, p.y, QColor(0.0, 0.0, 255.0));
        }
    }

    graphicsScene_2->clear();
    image_result = view_image;
    QPixmap pixmap = QPixmap::fromImage(image_result);
    graphicsScene_2->addPixmap(pixmap);
}

void GUILibTIM::update_view_2_attribute_image()
{
    if(componentTree == nullptr)
        return;

    QString choice_attribute = ui->comboBox_attribute->currentText();
    QString choice_criterion = ui->comboBox_criterion->currentText();
    QString choice_limit_criterion = ui->comboBox_limit_criterion->currentText();
    QString choice_selection_rule = ui->comboBox_selection_rule->currentText();

    int64_t limit = ui->doubleSpinBox_max_attribute->value();
    ComponentTree<U8>::Attribute attribute = AttributeFromQString(choice_attribute);
    ComponentTree<U8>::Attribute criterion = AttributeFromQString(choice_criterion);
    ComponentTree<U8>::Attribute limit_criterion = AttributeFromQString(choice_limit_criterion);
    int64_t limit_criterion_min = ui->doubleSpinBox_min_limit_criterion->value();
    int64_t limit_criterion_max = ui->doubleSpinBox_max_limit_criterion->value();
    ComponentTree<U8>::ConstructionDecision rule = ConstructionDecisionFromQString(choice_selection_rule);

    QImage view_image;

    if(choice_attribute == "H"
            || choice_attribute == "CONTRAST"
            || choice_attribute == "VOLUME"
            || choice_attribute == "COMPLEXITY"
            || choice_attribute == "COMPACITY") // int
    {
        Image<int64_t> res;
        if(limit_criterion_min == 0 && limit_criterion_max == 0)
        {
            res = componentTree->constructImageAttribute<int, long double>
                      (attribute, criterion, rule);
        }
        else
        {
            res = componentTree->constructImageAttribute<int, long double, int64_t>
                      (attribute, criterion, rule, limit_criterion, limit_criterion_min, limit_criterion_max);
        }
        view_image = QImageFromImage(res, limit, selection_z);
    }
    else if (choice_attribute == "AREA")// int64_t
    {
        Image<int64_t> res;
        if(limit_criterion_min == 0 && limit_criterion_max == 0)
        {
            res = componentTree->constructImageAttribute<int64_t, long double>
                      (attribute, criterion, rule);
        }
        else
        {
            res = componentTree->constructImageAttribute<int64_t, long double, int64_t>
                      (attribute, criterion, rule, limit_criterion, limit_criterion_min, limit_criterion_max);
        }
        view_image = QImageFromImage(res, limit, selection_z);
    }
    else if (choice_attribute == "MSER"
             || choice_attribute == "MGB") // long double
    {
        Image<long double> res;
        if(limit_criterion_min == 0 && limit_criterion_max == 0)
        {
            res = componentTree->constructImageAttribute<long double, long double>
                      (attribute, criterion, rule);
        }
        else
        {
            res = componentTree->constructImageAttribute<long double, long double, int64_t>
                      (attribute, criterion, rule, limit_criterion, limit_criterion_min, limit_criterion_max);
        }
        view_image = QImageFromImage(res, limit, selection_z);
    }
    else
    {
        qDebug() << "ERROR : update_view_2_attribute_image";
        exit(1);
    }

    graphicsScene_2->clear();
    image_result = view_image;
    QPixmap pixmap = QPixmap::fromImage(image_result);
    graphicsScene_2->addPixmap(pixmap);

}

void GUILibTIM::update_view_chart()
{
    if(componentTree == nullptr)
        return;

    series_criterion->clear();
    series_attribute->clear();
    series_nodes->clear();

    Node* n = componentTree->coordToNode(selection.x(), selection.y(), selection_z);
    series_nodes->append(n->h, 0);
    for(int i = 0; i < ui->spinBox_view_node_branch->value(); ++i)
        n = n->father;
    series_nodes->append(n->h, 0);

    Node* node = componentTree->coordToNode(selection.x(), selection.y(), selection_z);

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

    message += "z : ";
    message += QString::number(selection_z).rightJustified(4, '0');
    message += ", ";

    Node* n = componentTree->coordToNode(selection.x(), selection.y(), selection_z);
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
    else if(choice == "MEAN")
    {
        a = ComponentTree<U8>::MEAN;
    }
    else if(choice == "VARIANCE")
    {
        a = ComponentTree<U8>::VARIANCE;
    }
    else if(choice == "MEAN_NGHB")
    {
        a = ComponentTree<U8>::MEAN_NGHB;
    }
    else if(choice == "VARIANCE_NGHB")
    {
        a = ComponentTree<U8>::VARIANCE_NGHB;
    }
    else if(choice == "OTSU")
    {
        a = ComponentTree<U8>::OTSU;
    }
    else if(choice == "CONTRAST")
    {
        a = ComponentTree<U8>::CONTRAST;
    }
    else if(choice == "VOLUME")
    {
        a = ComponentTree<U8>::VOLUME;
    }
    else if(choice == "MGB")
    {
        a = ComponentTree<U8>::MGB;
    }
    else if(choice == "COMPLEXITY")
    {
        a = ComponentTree<U8>::COMPLEXITY;
    }
    else if(choice == "COMPACITY")
    {
        a = ComponentTree<U8>::COMPACITY;
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

Image<U8> GUILibTIM::ImageFromQImageList(QList<QImage> &qimages)
{
    qDebug() << qimages.size();
    Image<U8> image(qimages[0].width(), qimages[0].height(), qimages.size());

    for(int z = 0; z < qimages.size(); ++z)
        for(int y = 0; y < qimages[z].height(); ++y)
            for(int x = 0; x < qimages[z].width(); ++x)
                image(x, y, z) = qimages[z].pixel(x, y);

    return image;
}

QImage GUILibTIM::QImageFromImage(Image<U8> &image, unsigned int z)
{
    // return QImage(image.getData(), image.getSizeX(), image.getSizeY(), QImage::Format_Grayscale8);

    QImage qimage(image.getSizeX(), image.getSizeY(), QImage::Format_Grayscale8);

    for(int x = 0; x < qimage.width(); ++x)
        for(int y = 0; y < qimage.height(); ++y)
        {
            QColor color(image(x, y, z), image(x, y, z), image(x, y, z));
            qimage.setPixelColor(x, y, color);
        }

    return qimage;
}

QImage GUILibTIM::QImageFromImage(Image<int> &image, int limit, unsigned int z)
{
    QImage qimage(image.getSizeX(), image.getSizeY(), QImage::Format_Grayscale8);

    if(limit == 0)
        limit = std::max(image.getMax(), 1);

    for(int x = 0; x < qimage.width(); ++x)
    {
        for(int y = 0; y < qimage.height(); ++y)
        {
            double normalized_val = ((double)std::min(image(x, y, z), limit) / (double)limit);
            int c = ((double)normalized_val*255.0);
            QColor color(c, c, c);
            qimage.setPixelColor(x, y, color);
        }
    }

    return qimage;
}

QImage GUILibTIM::QImageFromImage(Image<int64_t> &image, int64_t limit, unsigned int z)
{
    QImage qimage(image.getSizeX(), image.getSizeY(), QImage::Format_Grayscale8);

    if(limit == 0)
        limit = std::max(image.getMax(), (int64_t)1);

    for(int x = 0; x < qimage.width(); ++x)
    {
        for(int y = 0; y < qimage.height(); ++y)
        {
            double normalized_val = ((double)std::min(image(x, y, z), limit) / (double)limit);
            int c = ((double)normalized_val*255.0);
            QColor color(c, c, c);
            qimage.setPixelColor(x, y, color);
        }
    }

    return qimage;
}

QImage GUILibTIM::QImageFromImage(Image<long double> &image, long double limit, unsigned int z)
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
            normalized_val = ((long double)std::min(image(x, y, z), limit) / (long double)limit);
            c = ((long double)normalized_val*255.0);
            QColor color(c, c, c);
            qimage.setPixelColor(x, y, color);
        }
    }

    return qimage;
}

FlatSE GUILibTIM::getConnexity()
{
    FlatSE connexity;
    if(libtim_image.getSizeZ() > 1)
    {
        connexity.make3DN27();
    }
    else
    {
        connexity.make2DN9();
    }
    return connexity;
}

void GUILibTIM::computeComponentTree(Image<U8> &image)
{
    if(componentTree != nullptr)
        delete componentTree;

    unsigned int delta = 2;

    FlatSE connexity;

    ComputedAttributes ca = ComputedAttributes::AREA;
    ca = (ComputedAttributes)(ca | ComputedAttributes::AREA_DERIVATIVES);
    ca = (ComputedAttributes)(ca | ComputedAttributes::CONTRAST);
    ca = (ComputedAttributes)(ca | ComputedAttributes::VOLUME);


    if(image.getSizeZ() > 1)
    {
        connexity.make3DN27();
    }
    else
    {
        connexity.make2DN9();
        if(ui->checkBox_compute_border->isChecked())
        {
            ca = (ComputedAttributes)(ca | ComputedAttributes::BORDER_GRADIENT);
            ca = (ComputedAttributes)(ca | ComputedAttributes::COMP_LEXITY_ACITY);
        }
        if(ui->checkBox_compute_neighborhood->isChecked())
        {
            ca = (ComputedAttributes)(ca | ComputedAttributes::OTSU);
        }
    }

    QTime t_start = QTime().currentTime();
    qDebug() << t_start.toString();

    componentTree = new ComponentTree<U8>(image, connexity, ca, delta);

    QTime t_end = QTime().currentTime();
    qint64 t_diff = t_start.msecsTo(t_end);
    qDebug() << t_end.toString();
    qDebug() << t_diff << "ms";
}
