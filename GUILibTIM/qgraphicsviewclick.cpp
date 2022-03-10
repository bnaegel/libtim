#include "qgraphicsviewclick.h"

QGraphicsViewClick::QGraphicsViewClick(QWidget *parent)
{

}

void QGraphicsViewClick::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit mousePressed(this->mapToScene(event->pos()).toPoint());
    }
}
