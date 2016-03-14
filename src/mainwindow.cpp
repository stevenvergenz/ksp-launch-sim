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

	config->params.duration = 20;
	config->params.timeResolution = 1;
	config->params.searchDepth = 2;
	config->params.throttleStep = 0.5;
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
	QString str = QString("[%1] %2km x %3km | %4m/s est. ∆v total")
		.arg(frame->time, 4, 'f', 2)
		.arg((frame->orbit().x - frame->config->body.radius)/1000, 5, 'f', 1)
		.arg((frame->orbit().y - frame->config->body.radius)/1000, 5, 'f', 1)
		.arg(frame->score, 2, 'f', 2);
	ui->textEdit->append(timestamp + str);

	delete frame;
}

void MainWindow::analyseResults(const SimFrame *bestResult)
{
	ui->textEdit->append(QString("[%1] Simulation complete, final orbit %2km x %3km")
		.arg(QDateTime::currentDateTime().toString("hh:mm"))
		.arg((bestResult->orbit().x - bestResult->config->body.radius)/1000, 5, 'f', 1)
		.arg((bestResult->orbit().y - bestResult->config->body.radius)/1000, 5, 'f', 1)
	);

	// write flight data to file
	std::ofstream output;
	output.open("flightdata.csv");
	output << "time,altitudeDelta,velocityDelta,longitude,throttle,orientation,deltaV,apogee,perigee" << std::endl;

	while(bestResult != nullptr)
	{
		double angle = acos( glm::dot(bestResult->orientation, glm::dvec2(bestResult->position.y, -bestResult->position.x))
			/ glm::length(bestResult->position) ) * 180/PI;

		double mu = G * config->body.mass;
		double energy = pow(glm::length(bestResult->velocity),2) / 2 - mu/glm::length(bestResult->position);

		output << bestResult->time << ",";
		output << glm::length(bestResult->position) - bestResult->config->body.radius << ",";
		output << glm::length(bestResult->velocity) << ",";
		output << atan2(bestResult->position.x, bestResult->position.y)*180/PI << ",";
		output << bestResult->throttle << ",";
		output << angle << ",";
		output << bestResult->deltaVSpent << ",";
		output << bestResult->orbit().x - bestResult->config->body.radius << ",";
		output << bestResult->orbit().y - bestResult->config->body.radius << std::endl;
		bestResult = bestResult->prev;
	}
	output.close();

	sim = nullptr;
}

MainWindow::~MainWindow()
{
	delete ui;
	delete config;
}
