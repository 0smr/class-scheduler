#ifndef COURSEPLANING_H
#define COURSEPLANING_H

#include <QObject>
#include <QVector>
#include <QtDebug>

#include <numeric>			//maximum function
#include <random>
#include <chrono>			//chronograph

#include "course.h"			// course class
#include "professor.h"		// professor class

/*!
	\class coursePlaning
	\brief the course planing class is main class that contain some functions.
		   that function do some actions witch describe in below.
*/

class coursePlaning : public QObject
{
	Q_OBJECT
public:
	//------------------ special member function ------------------
	explicit coursePlaning(QObject * = nullptr);
	coursePlaning(int , const QVector<professor *> &, QObject * = nullptr);

	~coursePlaning();
	//------------------ common member function ------------------
	// add a constraint subset to constraintSet
	bool addConstraintSubSet(QVector<course *> *);

	// fill generation array with random value
	void randomFillFirstGeneration();

	// calculate and assign first generation fitness list (array)
	void evaluateFitness();

	// select parents with sample distribution
	QVector<QPair<unsigned int ,unsigned int > > parentSelection();

	// crossover parent and transform elite to new generation :(with given mutation size)_new_
	void crossover(unsigned int );

	// number of classes that changed randomly by single mutation operation
	void mutation(unsigned int);

	// process GA with given value
	void processGA (unsigned int , double );

	void extinction();

	//------------------ getter member function ------------------
	QVector<int> getElitesList(QVector <long long int>* = nullptr, unsigned int = 1) const;
	QVector<int> getInferiorList(QVector <long long int>* = nullptr, unsigned int = 1) const;
	const QVector<course *> &getCourseList()const;
	long long getMaxFitnessValue() const;

	//------------------ setter member function ------------------
	// set population size and allocate population (generations space)
	void setPopulationSize(const int &);
	// set professor list and then create course list and set max fitness value
	void setProfessorsList(const QVector<professor *> &);
	void setConstraintSet(const QVector <QVector<course *> *>&);

	// create course list and also return it
	const QVector<course *> &createCourseList();
	void initializeMaxFitValue();
	void setCompaction(long long int);
	void clearData();

	void setCrossOverProbability(double );
	void setSelectionPressure(double );

	void setExternalConflictCost(int );
	void setInternalConflictCost(int );


signals:
	/*
	 * signal will shot when new generation fitness evaluated
	 * inputs	( maximum generation fitness
	 *			, average generation fitness
	 *			, generation index )
	 * */
	void onNewGenerationFitEvaluated(long long
								   , long long
								   , unsigned int);
	void onProcessEnd(unsigned short int *);

	void log(QString );

	void onGenerationSizeChanged(int );
public slots:

private:
	//------------------ common member ------------------
	// keep size of population
	unsigned int populationSize;
	unsigned int generationSize;
	// number of elites that xchange with infriors
	unsigned int eliteSurvivors;
	unsigned int chance;

	bool evolution;
	bool resurrectionDay;//if true all generation will reset

	long long int maxFitnessValue;
	long long int bestFitness;
	long long int fitnessAverage;
	long long int compaction;//dosent use yet (it will determine compaction value of fitnesses)

	double selectionPressure;
	double crossoverProbability;
	//float mutationProbability;

	int InternalConflictCost;
	int ExternalConflictCost;
	//------------------ list member ------------------
	// set of constraint subSets
	QVector <QVector<course *> *> constraintSet;
	// list of all course
	QVector <course *> courseList;
	// list of professors
	QVector <professor *> professorsList;

	//------------------ pointer member ------------------
	QVector <long long int> *firstGenFitness;
	QVector <long long int> *secondGenFitness;

	unsigned short int **firstGeneration;
	unsigned short int **secondGeneration;

	//------------------ random device ------------------
	std::random_device rdSeed;
	std::mt19937 generator;
};
#endif // COURSEPLANING_H
