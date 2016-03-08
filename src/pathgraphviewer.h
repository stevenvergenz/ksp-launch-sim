#ifndef PATHGRAPHVIEWER_H
#define PATHGRAPHVIEWER_H

#include <QWidget>
#include <QList>

#include "configs.h"

namespace Ui {
class PathGraphViewer;
}

class PathGraphViewer : public QWidget
{
	Q_OBJECT

public:
	explicit PathGraphViewer(QWidget *parent = 0);
	~PathGraphViewer();
	void setPath(QPolygonF path);
	void clear();

public slots:
	void updateScaleScroll(QRect scrollArea, int scale);

private:
	Ui::PathGraphViewer *ui;

	QPointF min, max;
	QPolygonF path;
};

#endif // PATHGRAPHVIEWER_H
