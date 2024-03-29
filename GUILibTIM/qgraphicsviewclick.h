#ifndef QGRAPHICSVIEWCLICK_H
#define QGRAPHICSVIEWCLICK_H

#include <QDebug>

#include <QGuiApplication>
#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>

class QGraphicsViewClick : public QGraphicsView
{
    Q_OBJECT

public:
    QGraphicsViewClick(QWidget *parent = nullptr);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

signals:
    void mousePressed(QPoint p);
    void mouseDoubleClicked(QPoint p);
    void mouseRightClick(QPoint p);
    void mouseMiddleClick(QPoint p);

private:
    bool leftClickPressed = false;
};

#endif // QGRAPHICSVIEWCLICK_H
