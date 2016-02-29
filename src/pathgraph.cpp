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
	//printf("world: %lf %lf %lf %lf\n", world.left(), world.top(), world.width(), world.height());
	p.drawEllipse(world);

	p.setPen(Qt::black);
	p.drawLine(view.map(QPoint(-1000000,1000000)), view.map(QPoint(1000000,-1000000)));
	p.drawLine(view.map(QPoint(-1000000,0)), view.map(QPoint(1000000,0)));
	p.drawLine(view.map(QPoint(0,-1000000)), view.map(QPoint(0,1000000)));

	auto positions = *(this->positions);
	for(int i=1; i<positions.length(); ++i)
	{
		p.drawLine( view.map(QLineF(positions[i-1], positions[i])) );
	}

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

void PathGraph::setPositionList(const QList<QPointF> * const list){
	positions = list;
}
