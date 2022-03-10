#ifndef QGRAPHICSVIEWCLICK_H
#define QGRAPHICSVIEWCLICK_H

#include <QDebug>

#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>

class QGraphicsViewClick : public QGraphicsView
{
    Q_OBJECT

public:
    QGraphicsViewClick(QWidget *parent = nullptr);

    void mousePressEvent(QMouseEvent *event);

signals:
    void mousePressed(QPoint p);
};

#endif // QGRAPHICSVIEWCLICK_H
