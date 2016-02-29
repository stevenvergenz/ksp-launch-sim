#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	sim(nullptr),
	config(new SimulationConfig())
{
	ui->setupUi(this);

	connect(ui->btnStart, SIGNAL(clicked(bool)), this, SLOT(startSim()));

	config->stages[0] = SimulationConfig::StageInfo(
		940.0+1000.0, // dry mass
		940.0+5000.0, // total mass
		300.0, // thrust
		1000.0, // Isp
		1.767, // cross-sectional area
		0.248  // drag coefficient
	);
	config->stageCount = 1;
	config->body = KerbolSystem::Kerbin;
	config->params.duration = 1000.0;
	config->params.timeResolution = 0.1;
	config->goal.desiredApoapsis = 100000;
	config->goal.desiredPeriapsis = 100000;
}

void MainWindow::startSim()
{
	ui->pathViewer->clear();
	sim = new Simulator(config);

	// connect slots
	connect(sim, SIGNAL(start(const SimFrame*)), this, SLOT(log(const SimFrame*)));
	connect(sim, SIGNAL(update(const SimFrame*)), this, SLOT(log(const SimFrame*)));
	connect(sim, SIGNAL(done()), this, SLOT(analyseResults()));

	QThreadPool::globalInstance()->start(sim);
}

void MainWindow::log(const SimFrame * frame)
{
	QString timestamp = QString("[%1] ").arg(QDateTime::currentDateTime().toString("hh:mm"));

	static double lastReportTime = 0.0;

	if(frame->time == 0.0 || frame->time > lastReportTime + 0.0)
	{
		lastReportTime += 1.0;
		double angle = acos( glm::dot(frame->orientation, glm::dvec2(frame->position.y, -frame->position.x))
			/ glm::length(frame->position) ) * 180/PI;

		QString str = QString("[%2] %3m | %4m/s | %5° | %6m/s ∆v")
			.arg(frame->time, 4, 'f', 2)
			.arg(glm::length(frame->position) - frame->config->body.radius, 4, 'f', 2)
			.arg(glm::length(frame->velocity), 4, 'f', 2)
			.arg(angle, 2, 'f', 0)
			.arg(frame->deltaV(), 2, 'f', 2);
		ui->textEdit->append(timestamp + str);

		ui->statusBar->showMessage(QString("%1 apoapsis, %2 periapsis")
			.arg(frame->orbit().x).arg(frame->orbit().y)
		);

		ui->pathViewer->addVertex(QPointF(frame->position.x, frame->position.y));
	}

}

void MainWindow::analyseResults()
{
	ui->textEdit->append(QString("[%1] Simulation complete")
		.arg(QDateTime::currentDateTime().toString("hh:mm"))
	);

	sim = nullptr;
}

MainWindow::~MainWindow()
{
	delete ui;
	delete config;
}
