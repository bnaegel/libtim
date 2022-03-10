#include "qgraphicsviewclick.h"

QGraphicsViewClick::QGraphicsViewClick(QWidget *parent)
{

}

void QGraphicsViewClick::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        leftClickPressed = true;
        emit mousePressed(this->mapToScene(event->pos()).toPoint());
    }
}

void QGraphicsViewClick::mouseMoveEvent(QMouseEvent *event)
{
    if(leftClickPressed)
        emit mousePressed(this->mapToScene(event->pos()).toPoint());
}

void QGraphicsViewClick::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        leftClickPressed = false;
    }
}
