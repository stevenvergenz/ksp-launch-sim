#include "pathgraphviewer.h"
#include "ui_pathgraphviewer.h"

PathGraphViewer::PathGraphViewer(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PathGraphViewer),
	min(1e7, 1e7),
	max(-1e7, -1e7)

{
	ui->setupUi(this);
	ui->widget->setPathPtr(&path);
	ui->widget->setViewWindow(QRectF(-6e5, -6e5, 1.2e6, 1.2e6));

	connect(ui->widget, SIGNAL(viewUpdated(QRect,int)), this, SLOT(updateScaleScroll(QRect,int)));
}

PathGraphViewer::~PathGraphViewer()
{
	delete ui;
}

void PathGraphViewer::setPath(QPolygonF path)
{
	this->path = path;
	ui->widget->setViewWindow( path.boundingRect() );
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
	ui->widget->setViewWindow(QRectF(-6e5, -6e5, 1.2e6, 1.2e6));

}
