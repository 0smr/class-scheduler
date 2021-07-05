#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	genNumber(300),popSize(30),
	selectionPressure(1),mutationProbability(0),crossoverProbability(1),
	excelFile("timetable.xlsx")//default excel file direction
{
	ui->setupUi(this);

	scheduler = new coursePlaning();//create course planning engine

	myGUISetup();
	setupEngine();
	createChart();

	//set population size
	scheduler->setPopulationSize(popSize);
	onLogCached("The application start correctly.");
}

MainWindow::~MainWindow()
{
	delete ui;

	delete scheduler;
}

void MainWindow::myGUISetup()
{
	//-------------------- set application icon --------------------
	setWindowIcon(QIcon(":/figure/ico/resource/classScheduler.ico"));
	setWindowTitle("smart course planning system");
	resize(550,550);

	//-------------------- create objects --------------------
	optionsLayout = new QGridLayout();
	mainGridLayout = new QGridLayout();
	plotLayout = new QGridLayout();
	processVLayout = new QVBoxLayout();
	resultsHLayout = new QHBoxLayout();

	chartView = new QtCharts::QChartView(this);
	chart = new QtCharts::QChart();

	mutationProbSlider = new QSlider(Qt::Horizontal,this);
	crossoverProbSlider = new QSlider(Qt::Horizontal,this);
	selectionPressureSlider = new QSlider(Qt::Horizontal,this);

	label1 = new QLabel("mutation\nsize",this);
	label2 = new QLabel("selection\nPressure",this);
	label3 = new QLabel("crossover\nProbability",this);

	generateBtn = new QPushButton(this);
	hideShowLogsBtn = new QPushButton(this);
	loadFileDialogBtn = new QPushButton(this);
	popSizeSpin = new QSpinBox(this);
	generationSize = new QSpinBox(this);

	courseTableView = new QTreeWidget(this);
	logView = new QTextEdit(this);

	//-------------------- create group boxes --------------------
	plotGroup = new QGroupBox(this);
	processGroup = new QGroupBox(this);
	optionsGroup = new QGroupBox(this);
	resultsGroup = new QGroupBox(this);

	//-------------------- main grid layout --------------------
	QScrollArea *scrollArea;

	scrollArea = new QScrollArea(this);
	scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setWidget(resultsGroup);
	scrollArea->setWidgetResizable(true);
	scrollArea->setGeometry (0,0,500,50);


	mainGridLayout->addWidget(plotGroup,0,0,2,2);
	mainGridLayout->addWidget(processGroup,0,2);
	mainGridLayout->addWidget(optionsGroup,1,2);
	//mainGridLayout->addWidget(resultsGroup,2,0,1,3);

	//-------------------- plot set layout --------------------
	plotGroup->setLayout(plotLayout);
	processGroup->setLayout(processVLayout);
	optionsGroup->setLayout(optionsLayout);
	resultsGroup->setLayout(resultsHLayout);

	//-------------------- set main layout --------------------
	ui->centralWidget->setLayout(mainGridLayout);

	//-------------------- set other layouts --------------------


	plotLayout->addWidget(chartView);

	optionsLayout->addWidget(mutationProbSlider,0,1,1,2);
	optionsLayout->addWidget(selectionPressureSlider,1,1,1,2);
	optionsLayout->addWidget(crossoverProbSlider,2,1,1,2);
	optionsLayout->addWidget(label1,0,0);
	optionsLayout->addWidget(label2,1,0);
	optionsLayout->addWidget(label3,2,0);

	processVLayout->addWidget(loadFileDialogBtn);
	processVLayout->addWidget(generateBtn);
	processVLayout->addWidget(hideShowLogsBtn);
	processVLayout->addWidget(popSizeSpin);
	processVLayout->addWidget(generationSize);
	processVLayout->addSpacing(200);

	resultsHLayout->addWidget(courseTableView,2);
	resultsHLayout->addWidget(logView,1);

	//-------------------- objects design --------------------
	setMinimumSize(550,450);
	plotGroup->setTitle("  plot  ");

	resultsGroup->setTitle("  results  ");
	resultsGroup->setMaximumHeight(200);
	//resultsGroup->setMaximumWidth(700);

	processGroup->setMaximumWidth(200);
	processGroup->setTitle("  process  ");

	optionsGroup->setTitle("  options  ");
	optionsGroup->setMaximumWidth(200);

	mutationProbSlider->setStyleSheet("QSlider::groove {height:5px;border: 1px solid #000;left: 4px; right: 4px;}"
									 "QSlider::handle {height: 10px;background: #0AC6FF;margin: -5px -4px;}"
									 "QSlider::handle:pressed {background: #4262FF;}"
									 "QSlider::add-page {background: white;}"
									 "QSlider::sub-page {background: #7AE9FF;}");
	selectionPressureSlider->setStyleSheet(mutationProbSlider->styleSheet());
	crossoverProbSlider->setStyleSheet(mutationProbSlider->styleSheet());

	loadFileDialogBtn->setText("load excel file");
	generateBtn->setText("start GA process");
	hideShowLogsBtn->setText("hide/show logs");

	generateBtn->setDisabled(true);
	hideShowLogsBtn->setCheckable(true);

	popSizeSpin->setRange(2,INT_MAX);
	popSizeSpin->setValue(popSize);
	popSizeSpin->setToolTip("population size spin box.");

	generationSize->setRange(30,INT_MAX);
	generationSize->setValue(static_cast<int>(genNumber));
	generationSize->setToolTip("generation size spin box.");

	chart->setTitle("generation fitness");
	chartView->setChart(chart);
	chartView->setRenderHints(QPainter::Antialiasing);

	logView->setStyleSheet("QTextEdit {background-color:#FCFFEB; border-radius:5px;}"
						   "QTextEdit:hover {border:1px solid #ADEBFF}");
	logView->setReadOnly(true);//make log view read only

	//-------------------- slider configuration --------------------
	mutationProbSlider->setRange(0,100);
	crossoverProbSlider->setRange(0,100);
	selectionPressureSlider->setRange(50,400);

	mutationProbSlider->setValue(50);
	crossoverProbSlider->setValue(50);
	selectionPressureSlider->setValue(100);

	//-------------------- setup connections --------------------
	connect(scheduler , &coursePlaning::onNewGenerationFitEvaluated
			,this, &MainWindow::addDataToChart);
	connect(scheduler , &coursePlaning::onProcessEnd
			,this, &MainWindow::showResult);

	connect(loadFileDialogBtn , &QPushButton::clicked
			,this, &MainWindow::loadExceFile);
	connect(generateBtn , &QPushButton::clicked
			,this, &MainWindow::startGaAlgorithm);
	connect(hideShowLogsBtn , QOverload<bool>::of(&QPushButton::clicked)
			,this, &MainWindow::toggleResultView);

	connect(popSizeSpin,QOverload<int>::of(&QSpinBox::valueChanged)
			,this,&MainWindow::populationSizeValueChanged);
	connect(generationSize,QOverload<int>::of(&QSpinBox::valueChanged)
			,this,&MainWindow::generationSizeValueChanged);

	connect(mutationProbSlider,&QSlider::valueChanged
			,this, &MainWindow::onMutationProbabilityValueChanged);
	connect(crossoverProbSlider,&QSlider::valueChanged
			,this, &MainWindow::crossoverProbabilityValueChanged);
	connect(selectionPressureSlider,&QSlider::valueChanged
			,this, &MainWindow::selectionPressureValueChanged);

	connect(scheduler,&coursePlaning::log
			,this,&MainWindow::onLogCached);
	connect(scheduler,&coursePlaning::onGenerationSizeChanged
			,this,&MainWindow::generationSizeValueChanged);
}

void MainWindow::createChart()
{
	bestFitLS = new QtCharts::QSplineSeries(this);
	averageLS = new QtCharts::QSplineSeries(this);

	bestFitLS->setPen(QPen(QBrush(QColor(255,200,15)),1));
	bestFitLS->setName("best fitness");

	averageLS->setPen(QPen(QBrush(Qt::cyan),1));
	averageLS->setName("fitness average");

	//chart->legend()->setAlignment(Qt::AlignLeft);
	chart->setAnimationOptions(QtCharts::QChart::AllAnimations);
	chart->setAnimationDuration(500);
	chart->setAnimationEasingCurve(QEasingCurve::InOutQuad);

	chart->addSeries(bestFitLS);
	chart->addSeries(averageLS);

	chart->createDefaultAxes();

	chart->axes(Qt::Horizontal).first()->setRange(0,genNumber);
	chart->axes(Qt::Vertical).first()->setRange(0,scheduler->getMaxFitnessValue());

	//QtCharts::QLineSeries::
}

void MainWindow::setupEngine()
{
	if(excelFile.isFile())
	{
		excelList book(excelFile.filePath());

		//read profssor information and set it to professor list
		scheduler->setProfessorsList(book.readProfessorInformation());//now we can set internal conflict cost too
		//set constraint list
		scheduler->setConstraintSet(book.readConstraintSheet(scheduler->getCourseList()));

		scheduler->setCrossOverProbability(crossoverProbability);
		scheduler->setSelectionPressure(selectionPressure);
		mutationProbability = 0.5;

		generateBtn->setEnabled(true);
	}
}

void MainWindow::showResult(unsigned short int * lastResult)
{
	QTreeWidgetItem * header = new QTreeWidgetItem();
	QVector<course *> tempCList = scheduler->getCourseList();

	for(int i = 0 ; i< tempCList.size() ; ++i)
		header->setText(i,tempCList[i]->getName());

	courseTableView->clear();
	QTreeWidgetItem *root = new QTreeWidgetItem(courseTableView);

	for(int i=0 ; i< tempCList.size() ; i++)
		root->setText(i,QString::number(lastResult[i]));

	courseTableView->setHeaderItem(header);
}

void MainWindow::addDataToChart(long long bestFitness
								,long long fitnessAverage
								, unsigned int genIndex)
{
	qDebug() << genIndex << bestFitness << fitnessAverage;

	bestFitLS->append(genIndex,bestFitness);
	averageLS->append(genIndex,fitnessAverage);

	chartView->update();
}

void MainWindow::loadExceFile()
{
	excelFile = QFileDialog::getOpenFileName(this,"symbol table:","/timeTable.xlsx",tr("Excel File (*.xlsx)"));

	setupEngine();
	chart->axes(Qt::Vertical).first()->setRange(0,scheduler->getMaxFitnessValue());//fix chart V side
}

void MainWindow::startGaAlgorithm()
{
	bestFitLS->clear();
	averageLS->clear();
	if(genNumber != static_cast<unsigned>(generationSize->value()))
	{
		logView->clear();
	}

	scheduler->clearData();

	generationSizeValueChanged(generationSize->value()+1);

	scheduler->setPopulationSize(popSize);
	scheduler->processGA(genNumber,mutationProbability);
}

void MainWindow::toggleResultView(bool check)
{
	if(check == true)
	{
		resultsGroup->hide();
	}
	else
	{
		resultsGroup->show();
	}
}

void MainWindow::onMutationProbabilityValueChanged()
{
	mutationProbability = mutationProbSlider->value()/100.0;
	mutationProbSlider->setToolTip(QString::number(mutationProbability)
								   + '/' +QString::number(mutationProbSlider->maximum()/100));
	mutationProbSlider->setToolTipDuration(5000);
}

void MainWindow::crossoverProbabilityValueChanged()
{
	crossoverProbability = crossoverProbSlider->value()/100.0;
	scheduler->setCrossOverProbability(crossoverProbability);

	crossoverProbSlider->setToolTip(QString::number(crossoverProbability)
										+ '/' + QString::number(crossoverProbSlider->maximum()/100));
	crossoverProbSlider->setToolTipDuration(5000);
}

void MainWindow::selectionPressureValueChanged()
{
	selectionPressure = selectionPressureSlider->value()/100.0;
	scheduler->setSelectionPressure(selectionPressure);

	selectionPressureSlider->setToolTip(QString::number(selectionPressure)
										+ '/' +QString::number(selectionPressureSlider->maximum()/100.0));
	selectionPressureSlider->setToolTipDuration(5000);
}

void MainWindow::populationSizeValueChanged(int value)
{
	if((value & 0x1) == 0x1)
	{
		++value;
		popSizeSpin->setValue(value);

		onLogCached("warning: population size must be an even number.");
	}

	popSize = value;
}

void MainWindow::generationSizeValueChanged(int value)
{
	onLogCached("notice:chart X axis changed.");

	genNumber = static_cast<unsigned>(value);
	chart->axes(Qt::Horizontal).first()->setRange(0,genNumber);
}

void MainWindow::onLogCached(QString log)
{
	logView->append(log);
}
