#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThreadPool>
#include <QDateTime>

#include "configs.h"
#include "simulator.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

public slots:
	void startSim();
	void log(const QString& msg);

private:
	Ui::MainWindow *ui;
	Simulator* sim;
};

#endif // MAINWINDOW_H
