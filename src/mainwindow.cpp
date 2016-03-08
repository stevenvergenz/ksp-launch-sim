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

	SimulationConfig::StageInfo stage;
	stage.dryMass = 6000+4000;
	stage.totalMass = 6000+36000;
	stage.maxThrustAtmo = 1379.03;
	stage.maxThrustVac = 1500;
	stage.IspAtmo = 285;
	stage.IspVac = 310;
	stage.crossSectionalArea = 1.767;
	stage.dragCoefficient = 0.248;

	config->stages[0] = stage;
	config->stageCount = 1;
	config->body = KerbolSystem::Kerbin;

	config->params.duration = 2000;
	config->params.timeResolution = 5;
	config->params.searchDepth = 2;
	config->params.throttleStep = 0.2;
	config->params.radialStep = PI/8;

	config->goal.desiredApoapsis = 100000;
	config->goal.desiredPeriapsis = 100000;
}

void MainWindow::startSim()
{
	if(sim == nullptr)
	{
		ui->pathViewer->clear();
		sim = new Simulator(config);

		// connect slots
		connect(sim, SIGNAL(update(const SimFrame*)), this, SLOT(log(const SimFrame*)));
		connect(sim, SIGNAL(done(const SimFrame*)), this, SLOT(analyseResults(const SimFrame*)));

		QThreadPool::globalInstance()->start(sim);
		ui->btnStart->setText("Stop");
	}
	else
	{
		sim->abort = true;
		ui->btnStart->setDisabled(true);
	}
}

void MainWindow::log(const SimFrame * frame)
{
	/*QString timestamp = QString("[%1] ").arg(QDateTime::currentDateTime().toString("hh:mm"));

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
			.arg(frame->deltaVVac(), 2, 'f', 2);
		ui->textEdit->append(timestamp + str);

		ui->statusBar->showMessage(QString("%1 apoapsis, %2 periapsis")
			.arg(frame->orbit().x - frame->config->body.radius).arg(frame->orbit().y - frame->config->body.radius)
		);

		ui->pathViewer->addVertex(QPointF(frame->position.x, frame->position.y));
	}*/

	QPolygonF path;

	const SimFrame* temp = frame;
	while(temp != nullptr){
		path << QPointF(temp->position.x, temp->position.y);
		temp = temp->prev;
	}
	ui->pathViewer->setPath(path);

	QString timestamp = QString("[%1] ").arg(QDateTime::currentDateTime().toString("hh:mm"));
	glm::dvec2 orbit = frame->orbit();
	QString str = QString("[%1] %2m | %3m/s | %4m/s ∆v | %5km x %6km")
		.arg(frame->time, 4, 'f', 2)
		.arg(glm::length(frame->position) - frame->config->body.radius, 4, 'f', 2)
		.arg(glm::length(frame->velocity), 4, 'f', 2)
		.arg(frame->deltaVVac(), 2, 'f', 2)
		.arg((orbit.x - frame->config->body.radius)/1000, 3, 'f', 1)
		.arg((orbit.y - frame->config->body.radius)/1000, 3, 'f', 1);
	ui->textEdit->append(timestamp + str);

	delete frame;
}

void MainWindow::analyseResults(const SimFrame *bestResult)
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
