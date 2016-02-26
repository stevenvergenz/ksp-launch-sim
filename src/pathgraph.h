#ifndef PATHGRAPH_H
#define PATHGRAPH_H


#include <QWidget>
#include <QList>
#include <QPointF>
#include <QPainter>
#include <QPaintEvent>
#include <QTransform>

#include <cmath>
#include <cstdio>

class PathGraph : public QWidget
{
	Q_OBJECT
public:
	explicit PathGraph(QWidget *parent = 0);
	void setPositionList(const QList<QPointF>* const list);

signals:
	void viewUpdated(QRect scrollArea, int scale);

public slots:
	void setViewWindow(QRectF view, bool update = true);

protected:
	const QList<QPointF>* positions;
	QRectF viewBox;
	QTransform view;
	void paintEvent(QPaintEvent* event);
	void resizeEvent(QResizeEvent* event);
};

#endif // PATHGRAPH_H
