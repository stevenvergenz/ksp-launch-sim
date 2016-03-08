#include "pathgraph.h"

PathGraph::PathGraph(QWidget *parent) : QWidget(parent)
{

}

void PathGraph::paintEvent(QPaintEvent* event)
{
	QPainter p(this);
	p.setRenderHint(QPainter::RenderHint::Antialiasing, true);

	// draw background
	p.setBrush(Qt::white);
	p.drawRect(0, 0, width(), height());

	// draw world
	p.setPen(Qt::darkGreen);
	p.setBrush(Qt::transparent);
	QRectF world(-6e5, -6e5, 1.2e6, 1.2e6);
	world = view.mapRect(world);
	p.drawEllipse(world);

	// draw atmo
	world = view.mapRect(QRectF(-670000, -670000, 1.34e6, 1.34e6));
	p.setPen(Qt::lightGray);
	p.drawEllipse(world);

	// draw center of gravity
	p.setPen(Qt::lightGray);
	p.drawLine(view.map(QPoint(-100000,0)), view.map(QPoint(100000,0)));
	p.drawLine(view.map(QPoint(0,-100000)), view.map(QPoint(0,100000)));

	// draw launch path
	p.setPen(Qt::black);
	p.drawPolyline(view.map(*path));

	// draw background
	p.setPen(Qt::gray);
	p.drawRect(0, 0, width(), height());
}

void PathGraph::setViewWindow(QRectF viewBox, bool update)
{
	this->viewBox = viewBox;

	// clear old transform
	view.reset();

	// calculate new scale based on target ratio
	int scaleX = round(viewBox.width()*1.2 / (width()-2));
	int scaleY = round(viewBox.height()*1.2 / (height()-2));
	int scale = scaleX > scaleY ? scaleX : scaleY;
	if(scale < 1) scale = 1;

	view = view.scale(1.0/scale, -1.0/scale);
	view.translate(scale*width()/2 - viewBox.center().x(), -scale*height()/2 - viewBox.center().y());

	emit viewUpdated(viewBox.toAlignedRect(), scale);

	if(update)
		this->update();
}

void PathGraph::resizeEvent(QResizeEvent *event){
	setViewWindow(viewBox, false);
}

void PathGraph::setPathPtr(const QPolygonF* const path){
	this->path = path;
}
