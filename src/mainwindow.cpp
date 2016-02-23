#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	connect(ui->btnStart, SIGNAL(clicked(bool)), this, SLOT(startSim()));
}

void MainWindow::startSim()
{
	//SimulationConfig::ShipInfo ship = {100};
	//SimulationConfig::GoalInfo goal = {1000,1000};
	SimulationConfig config;
	//config.ship = ship;
	config.body = KerbolSystem::Kerbin;
	//config.goal = goal;
	//config.duration = 10;

	sim = new Simulator(config);

	// connect slots
	connect(sim, SIGNAL(start(const QString&)), this, SLOT(log(const QString&)));
	connect(sim, SIGNAL(update(const QString&)), this, SLOT(log(const QString&)));
	connect(sim, SIGNAL(done(const QString&)), this, SLOT(log(const QString&)));

	QThreadPool::globalInstance()->start(sim);
}

void MainWindow::log(const QString &msg)
{
	QString str = QString("[%1] %2").arg(QDateTime::currentDateTime().toString("hh:mm")).arg(msg);
	ui->textEdit->append(str);
}

MainWindow::~MainWindow()
{
	delete ui;
}
