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

	config->stages[0] = SimulationConfig::StageInfo(940.0+450.0, 940.0+1500.0, 162.91, 140.0);
	config->stageCount = 1;
	config->body = KerbolSystem::Kerbin;
	config->params.duration = 1000.0;
	config->params.timeResolution = 0.1;
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

	if(frame->prev != nullptr && frame->currentStage > frame->prev->currentStage)
	{
		auto orbit = frame->orbit();
		QString str = QString("Stage depleted, apoapsis = %1, periapsis = %2")
			.arg(orbit.x - frame->config->body.radius)
			.arg(orbit.y - frame->config->body.radius);

		ui->textEdit->append(timestamp + str);
	}

	else if(frame->prev != nullptr && frame->currentMass == frame->config->stages[frame->currentStage].dryMass
		&& frame->currentMass < frame->prev->currentMass)
	{
		auto orbit = frame->orbit();
		QString str = QString("Fuel depleted, apoapsis = %1, periapsis = %2")
			.arg(orbit.x - frame->config->body.radius)
			.arg(orbit.y - frame->config->body.radius);
		ui->textEdit->append(timestamp + str);
	}

	static double lastReportTime = 0.0;

	if(frame->time == 0.0 || frame->time > lastReportTime + 1.0)
	{
		lastReportTime += 1.0;
		QString str = QString("[%2] %3m altitude at %4m/s")
			.arg(frame->time, 4, 'f', 2)
			.arg(glm::length(frame->position) - frame->config->body.radius, 4, 'f', 2)
			.arg(glm::length(frame->velocity), 4, 'f', 2);
		ui->textEdit->append(timestamp + str);
	}



	ui->pathViewer->addVertex(QPointF(frame->position.x, frame->position.y));
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
