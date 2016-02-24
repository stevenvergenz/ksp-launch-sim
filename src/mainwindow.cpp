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
	connect(ui->horizontalScale, SIGNAL(valueChanged(int)), ui->graph, SLOT(setHorizontalScale(int)));
	connect(ui->verticalScale, SIGNAL(valueChanged(int)), ui->graph, SLOT(setVerticalScale(int)));
}

void MainWindow::startSim()
{
	ui->graph->clear();
	config->body = KerbolSystem::Kerbin;

	sim = new Simulator(config);

	// connect slots
	connect(sim, SIGNAL(start(const SimFrame*)), this, SLOT(log(const SimFrame*)));
	connect(sim, SIGNAL(update(const SimFrame*)), this, SLOT(log(const SimFrame*)));
	connect(sim, SIGNAL(done()), this, SLOT(analyseResults()));

	QThreadPool::globalInstance()->start(sim);
}

void MainWindow::log(const SimFrame * frame)
{
	QString str = QString("[%1] [%2] %3m altitude at %4m/s ")
		.arg(QDateTime::currentDateTime().toString("hh:mm"))
		.arg(frame->time, 4, 'f', 2)
		.arg(frame->position.mag() - frame->config->body.radius, 4, 'f', 2)
		.arg(frame->velocity.mag(), 4, 'f', 2);
	ui->textEdit->append(str);

	double downrangeDist = (PI/2 - atan(frame->position.y()/frame->position.x())) * frame->config->body.radius;
	ui->graph->addVertex(QPointF(downrangeDist, frame->position.mag()-frame->config->body.radius));
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
