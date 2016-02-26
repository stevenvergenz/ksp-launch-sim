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
	connect(ui->verticalScale, SIGNAL(valueChanged(int)), ui->graph, SLOT(setScale(int)));

	config->stages[0] = SimulationConfig::StageInfo(1950.0, 3000.0, 192.0, 165.0);
	config->stages[1] = SimulationConfig::StageInfo(450.0, 1500.0, 192.0, 165.0);
	config->stageCount = 2;
	config->body = KerbolSystem::Kerbin;
	config->params.duration = 1000.0;
	config->params.timeResolution = 0.1;
}

void MainWindow::startSim()
{
	ui->graph->clear();
	sim = new Simulator(config);

	// connect slots
	connect(sim, SIGNAL(start(const SimFrame*)), this, SLOT(log(const SimFrame*)));
	connect(sim, SIGNAL(update(const SimFrame*)), this, SLOT(log(const SimFrame*)));
	connect(sim, SIGNAL(done()), this, SLOT(analyseResults()));

	QThreadPool::globalInstance()->start(sim);
}

void MainWindow::log(const SimFrame * frame)
{
	static double lastReportTime = 0.0;
	if(frame->time > lastReportTime)
	{
		lastReportTime = frame->time;
		QString str = QString("[%1] [%2] %3m altitude at %4m/s")
			.arg(QDateTime::currentDateTime().toString("hh:mm"))
			.arg(frame->time, 4, 'f', 2)
			.arg(glm::length(frame->position) - frame->config->body.radius, 4, 'f', 2)
			.arg(glm::length(frame->velocity), 4, 'f', 2);
		ui->textEdit->append(str);
	}

	if(frame->prev != nullptr && frame->currentStage > frame->prev->currentStage)
		ui->textEdit->append("Stage depleted");

	else if(frame->prev != nullptr && frame->currentMass == frame->config->stages[frame->currentStage].dryMass
		&& frame->currentMass < frame->prev->currentMass)
		ui->textEdit->append("Fuel depleted");


	ui->graph->addVertex(QPointF(frame->time, glm::length(frame->position)-frame->config->body.radius));
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
