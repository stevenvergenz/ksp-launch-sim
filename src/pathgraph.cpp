#include "pathgraph.h"

PathGraph::PathGraph(QWidget *parent) : QWidget(parent), scale(QPoint(1,1))
{
	addVertex(QPointF(0.0, 0.0));
	addVertex(QPointF(1.0, 1.0));
	addVertex(QPointF(4.0, 2.0));
	addVertex(QPointF(9.0, 3.0));
	addVertex(QPointF(16.0, 4.0));
}

void PathGraph::paintEvent(QPaintEvent* event)
{
	QPainter p(this);
	p.setBackgroundMode(Qt::OpaqueMode);
	p.setBackground(QBrush(Qt::white));
	p.setRenderHint(QPainter::RenderHint::Antialiasing, true);

	p.translate(2, height()-2);
	p.scale(1,-1);

	QPointF lastPoint;
	for(auto i=vertices.begin(); i!=vertices.end(); ++i)
	{
		QPointF newPoint;
		newPoint.setX( pow(10, scale.x()/50.0)*(*i).x() + offset.x() );
		newPoint.setY( pow(10, scale.y()/50.0)*(*i).y() + offset.y() );

		p.setPen(QPen(QBrush(Qt::black), 3, Qt::SolidLine, Qt::RoundCap));
		p.drawPoint(newPoint);

		p.setPen(QPen(QBrush(Qt::black), 1, Qt::SolidLine, Qt::RoundCap));
		p.drawLine(lastPoint,newPoint);
		lastPoint = newPoint;
	}
}

void PathGraph::addVertex(QPointF point)
{
	vertices.push_back(point);
	update();
}

void PathGraph::clear()
{
	vertices.clear();
}

void PathGraph::setHorizontalScale(int scale)
{
	this->scale.setX(scale);
	update();
}

void PathGraph::setVerticalScale(int scale)
{
	this->scale.setY(scale);
	update();
}

void PathGraph::setHorizontalOffset(int offset)
{
	this->offset.setX(offset);
	update();
}

void PathGraph::setVerticalOffset(int offset)
{
	this->offset.setY(offset);
	update();
}
