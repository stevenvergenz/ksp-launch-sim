#include "pathgraphviewer.h"
#include "ui_pathgraphviewer.h"

PathGraphViewer::PathGraphViewer(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PathGraphViewer),
	min(1e7, 1e7),
	max(-1e7, -1e7)

{
	ui->setupUi(this);
	ui->widget->setPositionList(&positions);
	ui->widget->setViewWindow(QRectF(-6e5, -6e5, 1.2e6, 1.2e6));

	connect(ui->widget, SIGNAL(viewUpdated(QRect,int)), this, SLOT(updateScaleScroll(QRect,int)));
}

PathGraphViewer::~PathGraphViewer()
{
	delete ui;
}

void PathGraphViewer::addVertex(QPointF point)
{
	positions.push_back(point);

	bool update = true;
	if(point.x() > max.x())
		max.setX(point.x());
	if(point.x() < min.x())
		min.setX(point.x());
	if(point.y() > max.y())
		max.setY(point.y());
	if(point.y() < min.y())
		min.setY(point.y());
	if(point.x() < max.x() && point.x() > min.x() && point.y() < max.y() && point.y() > min.y())
		update = false;

	if(update){
		//printf("viewWindow: %lf %lf %lf %lf\n", min.x(), max.x(),min.y(), max.y());
		ui->widget->setViewWindow(QRectF(min.x(), min.y(), max.x()-min.x(), max.y()-min.y()));
	}
	else
		ui->widget->update();
}

void PathGraphViewer::updateScaleScroll(QRect scrollArea, int scale)
{
	ui->horizontalScrollBar->setMinimum(scrollArea.left());
	ui->horizontalScrollBar->setMaximum(scrollArea.right());
	ui->verticalScrollBar->setMinimum(scrollArea.bottom());
	ui->verticalScrollBar->setMaximum(scrollArea.top());
}

void PathGraphViewer::clear()
{
	positions.clear();
	ui->widget->setViewWindow(QRectF(-6e5, -6e5, 1.2e6, 1.2e6));

}
