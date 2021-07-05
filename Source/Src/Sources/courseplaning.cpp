#include "courseplaning.h"

coursePlaning::coursePlaning(QObject *parent) : QObject(parent),
		eliteSurvivors(1),chance(0),bestFitness(0),fitnessAverage(0),
		selectionPressure(1.0),crossoverProbability(0.5),

		InternalConflictCost(3),ExternalConflictCost(2)
{
	generator.seed(rdSeed());
}

coursePlaning::coursePlaning(int pSize,const QVector<professor *> & profList, QObject *parent)
	:coursePlaning(parent)
{
	setPopulationSize(pSize);
	setProfessorsList(profList);
}

coursePlaning::~coursePlaning()
{
	// deallocate memory of generations.
	for(unsigned int iter = 0;iter < populationSize; ++iter)
	{
		delete[] firstGeneration [iter];
		delete[] secondGeneration [iter];
	}

	delete[] firstGeneration ;
	delete[] secondGeneration ;

	delete[] firstGenFitness ;  // deallocate first generation fitness
	delete[] secondGenFitness ; // deallocate first generation fitness
}

//  add a constraint sub set to constraint set
bool coursePlaning::addConstraintSubSet(QVector<course *> * limiterList)
{
	constraintSet.push_back(limiterList);

	for(QVector<course *>::iterator i = limiterList->begin() ; i!=limiterList->end() ; ++i)
		(*i)->addConstraintSubSet(limiterList);

	return true;
}

//  random fill first generation
void coursePlaning::randomFillFirstGeneration()
{
	std::uniform_int_distribution<unsigned short int> dist;

	for(std::size_t i = 0; i < populationSize ; ++i)
		for(std::size_t j = 0; j < static_cast<unsigned>(courseList.size()) ; ++j)
			firstGeneration[i][j] = courseList[static_cast<int>(j)]->getTimetableSL()->
					at(dist(generator) % courseList[static_cast<int>(j)]->getTimetableSLSize());
}

//  evaluate first generation fitness list
void coursePlaning::evaluateFitness()
{
	fitnessAverage = 0;
	long long genFitness {0};
	evolution = true;

	for(auto i = 0u ; i < populationSize ; ++i)
	{
		genFitness = 0;
		for(auto j = 0 ; j < courseList.size() ; ++j )
		{
			genFitness += courseList[j]->getCurrentFitness(i,ExternalConflictCost,InternalConflictCost);
		}
		//genFitness += maxFitnessValue; in new way we do not shift fitnesses

		genFitness = maxFitnessValue / static_cast<long long>(pow(genFitness+1,selectionPressure));

		fitnessAverage += genFitness;

		if(genFitness > bestFitness)//set Best fitness
		{
			bestFitness = genFitness;
			chance = 0; //reset chances
		}
		else
		{
			evolution = false;
		}

		(*firstGenFitness)[static_cast<int>(i)] = genFitness;
	}

	fitnessAverage /= populationSize;
}

//  random select parent with sampling distribution
QVector<QPair<unsigned int, unsigned int> > coursePlaning::parentSelection()
{
	QVector <QPair<unsigned int, unsigned int> > parents{};

	std::discrete_distribution <> dist(firstGenFitness->begin(),firstGenFitness->end());
	auto parentIter = populationSize/2+1;

	while(--parentIter)
		parents.push_back(QPair<unsigned , unsigned >
						  (static_cast<unsigned>(dist(generator)),
						   static_cast<unsigned>(dist(generator))));
	return  parents;
}

//  crossover parent to create offsprings
void coursePlaning::crossover(unsigned int mutationSize)
{
	// random seed device
	std::uniform_int_distribution <int> dist{};
	std::uniform_real_distribution<> realDist(0.0,1.0);

	//select parent (sample selection)
	QVector < QPair<unsigned, unsigned> >  parents = parentSelection();
	unsigned int firstParent{} , secondParent{};
	bool crossoverOccurrence{};

	// swap generation and it's fitnesses
	qSwap(secondGenFitness,firstGenFitness);
	qSwap(secondGeneration,firstGeneration);

	for(std::size_t iter = 0; iter<populationSize ; ++++iter)
	{
		crossoverOccurrence = true;
		//cross over probability
		if(realDist(generator) < crossoverProbability)
			crossoverOccurrence = false;

		// pick parents
		firstParent = parents.back().first;
		secondParent = parents.back().second;

		parents.pop_back();

		// cross of parents
		for(std::size_t gIter = 0 ; static_cast<int>(gIter) < courseList.size() ; ++gIter)
			if(dist(generator) & 0x1 || crossoverOccurrence)
			{
				firstGeneration[iter][gIter] = secondGeneration[firstParent][gIter];
				firstGeneration[iter+1][gIter] = secondGeneration[secondParent][gIter];
			}
			else
			{
				firstGeneration[iter][gIter] = secondGeneration[secondParent][gIter];
				firstGeneration[iter+1][gIter] = secondGeneration[firstParent][gIter];
			}
	}
	// mutate with given mutation size
	mutation(mutationSize);

	evaluateFitness();

	int elite = getElitesList(secondGenFitness).first();
	int inferior = getInferiorList(firstGenFitness).first();

	std::swap(firstGeneration[inferior],secondGeneration[elite]);

	evaluateFitness();
}

void coursePlaning::mutation(unsigned int mutationSize)
{
	std::random_device rdSeed;
	std::mt19937 generator(rdSeed());
	std::uniform_int_distribution<unsigned short int> dist;


	unsigned int column{} , gen{};

	while(mutationSize--)
	{
		gen = dist(generator) % populationSize;
		column = dist(generator) % courseList.size();

		firstGeneration[gen][column]
				= courseList[static_cast<int>(column)]->getTimetableSL()->
				at(dist(generator) % courseList[static_cast<int>(column)]->getTimetableSLSize());
	}
}

void coursePlaning::processGA(unsigned int genSize , double mutationSize)
{
	unsigned int MC = static_cast<unsigned>(mutationSize * populationSize);
	auto generation = 0u;
	generationSize = genSize;

	emit log("the process has been started.");

	randomFillFirstGeneration();
	evaluateFitness();

	emit onNewGenerationFitEvaluated(bestFitness,fitnessAverage,generation);

	while(++generation <= generationSize && bestFitness != maxFitnessValue)//
	{
		crossover(MC);

		evaluateFitness();

		if(evolution == false)
		{
			resurrectionDay = ++chance > generationSize/4 ? true : false;
		}

		emit onNewGenerationFitEvaluated(bestFitness,fitnessAverage,generation);

		if(resurrectionDay == true)
		{
			bestFitness = 0;
			resurrectionDay = false;

			generationSize += generationSize/8;

			emit onGenerationSizeChanged(static_cast<int>(generationSize));

			extinction();
			evaluateFitness();
			emit onNewGenerationFitEvaluated(bestFitness,fitnessAverage,generation);
		}
	}

	int elite = getElitesList(firstGenFitness).first();
	emit onProcessEnd(firstGeneration[elite]);

	if(bestFitness == maxFitnessValue)
	{
		emit log("the process has been ended with answer in " + QString::number(generation) + "th generation.");
	}
	else
	{
		emit log("the process has been ended with best fitness: " + QString::number(bestFitness) + ".");
	}
}

inline void coursePlaning::extinction()//it can be more complicated
{
	emit log(QString("an extinction happend.\n")
			 + "warnning: generation size changed in " + QString::number(generationSize/7) + " number");

	randomFillFirstGeneration();
}

QVector<int> coursePlaning::getElitesList(QVector <long long int>* fit,unsigned int count) const
{
	QVector <int> champions{};

	long long *maxIndex{};

	while(count--)
	{
		maxIndex = std::max_element(fit->begin(),fit->end());

		champions.push_back(static_cast<int>(maxIndex - &fit->first()));
		(*fit)[champions.last()] = LONG_LONG_MIN;
	}

	return champions;
}

QVector<int> coursePlaning::getInferiorList(QVector<long long> *fit, unsigned int count) const
{
	QVector <int> champions{};

	long long *minIndex{};

	while(count--)
	{
		minIndex = std::min_element(fit->begin(),fit->end());

		champions.push_back(static_cast<int>(minIndex - &fit->first()));
		(*fit)[champions.last()] = LONG_LONG_MAX;
	}

	return champions;
}

const QVector<course *> &coursePlaning::getCourseList() const
{
	return courseList;
}

const QVector<course *> &coursePlaning::createCourseList()
{
	for(auto prof = professorsList.begin() ; prof != professorsList.end() ; ++prof)
		for(auto cour = (*prof)->getCourseList().begin() ;
			cour != (*prof)->getCourseList().end() ; ++cour)
		{
			(*cour)->setColumn(static_cast<unsigned>(courseList.size()));
			(*cour)->setGenerationSL(&firstGeneration);
			(*cour)->setProfessorCoursesLS(&(*prof)->getCourseList());

			courseList.push_back(*cour);
		}

	emit log("course list created.");

	return courseList;
}

void coursePlaning::initializeMaxFitValue()
{
	maxFitnessValue = courseList.size() * courseList.size() * InternalConflictCost ;

	emit log("max fitness initialized with value of: " + QString::number(maxFitnessValue) + ".");
}

void coursePlaning::clearData()
{
	bestFitness = 0;
	fitnessAverage = 0;
}

void coursePlaning::setCrossOverProbability(double value)
{
	crossoverProbability = value;
}

void coursePlaning::setSelectionPressure(double value)
{
	selectionPressure = value;
}

void coursePlaning::setExternalConflictCost(int value)
{
	ExternalConflictCost = value;
}

void coursePlaning::setInternalConflictCost(int value)
{
	InternalConflictCost = value;
}

void coursePlaning::setCompaction(long long value)
{
	compaction = value;
}

long long coursePlaning::getMaxFitnessValue() const
{
	return maxFitnessValue;
}

void coursePlaning::setProfessorsList(const QVector<professor *> &profList)
{
	professorsList = profList;
	createCourseList();
	initializeMaxFitValue();
}

void coursePlaning::setConstraintSet(const QVector<QVector<course *> *> &CS)
{
	constraintSet = CS;
}

void coursePlaning::setPopulationSize(const int &pSize)
{
	populationSize = static_cast<uint>(pSize);

	// allocate parent generation
	firstGeneration = new unsigned short int *[populationSize];

	// allocate offspring generation
	secondGeneration = new unsigned short int *[populationSize];

	for(unsigned int iter = 0;iter < populationSize; ++iter)
	{
		firstGeneration [iter] = new unsigned short int [courseList.size()]{}; //number courses
		secondGeneration [iter] = new unsigned short int [courseList.size()]{};
	}

	// allocate generations fitness
	firstGenFitness = new QVector<long long int>(static_cast<int>(populationSize));
	secondGenFitness = new QVector<long long int>(static_cast<int>(populationSize));
}
