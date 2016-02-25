#include "pathgraph.h"

PathGraph::PathGraph(QWidget *parent) : QWidget(parent), scale(1)
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
	p.setBackground(QBrush(Qt::white));
	p.setBackgroundMode(Qt::OpaqueMode);

	p.setRenderHint(QPainter::RenderHint::Antialiasing, true);

	p.translate(0, height());
	p.scale(1,-1);

	p.setPen(QPen(QBrush(Qt::black), 1, Qt::SolidLine, Qt::RoundCap));

	for(int i=1; i<points.length(); ++i)
	{
		p.drawLine(points[i-1], points[i]);
	}
}

void PathGraph::addVertex(QPointF point)
{
	vertices.push_back(point);

	QPointF newPoint;
	newPoint.setX( pow(10, scale/50.0) * point.x() );
	newPoint.setY( pow(10, scale/50.0) * point.y() );
	points.push_back(newPoint);

	if(newPoint.x() > max.x())
		max.setX(newPoint.x());
	if(newPoint.y() > max.y())
		max.setY(newPoint.y());

	resize((int)ceil(max.x()), (int)ceil(max.y()));
	update();
}

void PathGraph::clear()
{
	vertices.clear();
	points.clear();
	update();
}

void PathGraph::setScale(int scale)
{
	this->scale = scale;

	points.clear();
	max = QPointF();

	for(int i=0; i<vertices.length(); i++)
	{
		QPointF newPoint;
		newPoint.setX( pow(10, scale/50.0) * vertices[i].x() );
		newPoint.setY( pow(10, scale/50.0) * vertices[i].y() );
		points.push_back(newPoint);

		if(newPoint.x() > max.x())
			max.setX(newPoint.x());
		if(newPoint.y() > max.y())
			max.setY(newPoint.y());
	}

	resize((int)ceil(max.x()), (int)ceil(max.y()));

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
