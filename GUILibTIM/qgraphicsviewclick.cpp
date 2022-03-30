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
    if (event->button() == Qt::RightButton)
    {
        this->scale(2, 2);
        emit mouseRightClick(this->mapToScene(event->pos()).toPoint());
    }
    if (event->button() == Qt::MiddleButton)
    {
        this->resetTransform();
        emit mouseMiddleClick(this->mapToScene(event->pos()).toPoint());
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

void QGraphicsViewClick::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        emit mouseDoubleClicked(this->mapToScene(event->pos()).toPoint());
    }
}
