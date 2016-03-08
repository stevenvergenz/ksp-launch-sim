#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThreadPool>
#include <QDateTime>
#include <cmath>

#include "glm/glm.hpp"
#include "glm/vec2.hpp"

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
	void log(const SimFrame* frame = nullptr);
	void analyseResults(const SimFrame* bestResult);

private:
	Ui::MainWindow *ui;
	Simulator* sim;
	SimulationConfig* config;
};

#endif // MAINWINDOW_H
