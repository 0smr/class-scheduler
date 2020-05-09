#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QPropertyAnimation>
#include <QLogValueAxis>
#include <QSplineSeries>
#include <QGridLayout>
#include <QPushButton>
#include <QTreeWidget>
#include <QFileDialog>
#include <QChartView>
#include <QValueAxis>
#include <QBoxLayout>
#include <QTextEdit>
#include <QGroupBox>
#include <QSpinBox>
#include <QSlider>
#include <QString>
#include <QChart>
#include <QLabel>
#include <QDebug>

#include "xlsxdocument.h"
#include "excellist.h"
#include "courseplaning.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	void myGUISetup();
	void createChart();

	void setupEngine();

private slots:
	void addDataToChart(long long , long long , unsigned int );
	void loadExceFile();
	void startGaAlgorithm();
	void toggleResultView(bool = true);

	void onMutationProbabilityValueChanged();
	void crossoverProbabilityValueChanged();
	void selectionPressureValueChanged();
	void populationSizeValueChanged(int);
	void generationSizeValueChanged(int);

	void showResult(unsigned short *);

	void onLogCached(QString log);

private:
	Ui::MainWindow *ui;

	//------------------ common member ------------------
	coursePlaning *scheduler;
	unsigned int genNumber;

	int popSize;

	double selectionPressure;
	double mutationProbability;
	double crossoverProbability;

	QFileInfo excelFile;
	//------------------ GUI objects ------------------
	QGridLayout *mainGridLayout ,*plotLayout ,*optionsLayout;
	QVBoxLayout *processVLayout ;
	QHBoxLayout *resultsHLayout;
	QGroupBox *plotGroup, *processGroup,
	*optionsGroup, *resultsGroup;

	QtCharts::QSplineSeries *bestFitLS, *averageLS;
	QtCharts::QChartView *chartView;
	QtCharts::QChart *chart;

	QLabel *label1, *label2, *label3;//labels
	QSlider *mutationProbSlider, *selectionPressureSlider ,*crossoverProbSlider;//sliders
	QPushButton *loadFileDialogBtn ,* generateBtn ,*hideShowLogsBtn;// buttons
	QSpinBox *popSizeSpin,*generationSize;

	QTreeWidget *courseTableView;
	QTextEdit *logView;
};

#endif // MAINWINDOW_H
