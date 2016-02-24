#ifndef PATHGRAPH_H
#define PATHGRAPH_H


#include <QWidget>
#include <QList>
#include <QPointF>
#include <QPainter>
#include <cmath>

class PathGraph : public QWidget
{
	Q_OBJECT
public:
	explicit PathGraph(QWidget *parent = 0);
	void addVertex(QPointF point);
	void clear();

signals:

public slots:
	void setHorizontalScale(int scale);
	void setVerticalScale(int scale);
	void setHorizontalOffset(int offset);
	void setVerticalOffset(int offset);

protected:
	QList<QPointF> vertices;
	QPoint scale;
	QPoint offset;
	void paintEvent(QPaintEvent* event);
};

#endif // PATHGRAPH_H
