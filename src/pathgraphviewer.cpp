#include "pathgraphviewer.h"
#include "ui_pathgraphviewer.h"

PathGraphViewer::PathGraphViewer(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PathGraphViewer),
	min(1e7, 1e7),
	max(-1e7, -1e7)

{
	ui->setupUi(this);
	ui->widget->setPathList(&paths);
	ui->widget->setViewWindow(QRectF(-6e5, -6e5, 1.2e6, 1.2e6));

	connect(ui->widget, SIGNAL(viewUpdated(QRect,int)), this, SLOT(updateScaleScroll(QRect,int)));
}

PathGraphViewer::~PathGraphViewer()
{
	delete ui;
}

void PathGraphViewer::addBranch(QPointF from, QPointF to)
{
	paths.push_back( QLineF(from, to) );

	bool update = true;
	if(to.x() > max.x())
		max.setX(to.x());
	if(to.x() < min.x())
		min.setX(to.x());
	if(to.y() > max.y())
		max.setY(to.y());
	if(to.y() < min.y())
		min.setY(to.y());
	if(to.x() < max.x() && to.x() > min.x() && to.y() < max.y() && to.y() > min.y())
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
	paths.clear();
	ui->widget->setViewWindow(QRectF(-6e5, -6e5, 1.2e6, 1.2e6));

}
