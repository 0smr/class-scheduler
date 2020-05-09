#include "course.h"

//static variable initializing
unsigned long long int course::counter{};

course::course(QObject *parent) : QObject(parent)
  , unit(1), ID(counter++)
{
	profID = nullptr;
	generationSL = nullptr;
	name = "no name";
}

course::course(QString name, const unsigned int *pID, QObject *parent):course(parent)
{
	this->name = name;
	this->profID = pID;
}

course::course(QString name, unsigned int *pID,const  QVector<unsigned short> * tSL
			   , unsigned short **, QObject *parent)
	:course(name ,pID ,parent)
{
	this->timetableSL = tSL;
}

void course::addConstraintSubSet(QVector<course *> *limiterList)
{
	constraintSetSL.push_back(limiterList);
}

course * const *course::find(const QVector<course *> &container, const QString &val)
{
	for(auto x = container.begin() ; x!=container.end() ; ++x)
		if((*x)->getName() == val)
			return x;
	return nullptr;
}

void course::setTimetableSL(const QVector<unsigned short> * tSL)
{
	this->timetableSL = tSL;
}

void course::setGenerationSL(unsigned short *** gSL)
{
	generationSL = gSL;
}

void course::setUnit(const unsigned short & un)
{
	unit = un;
}

void course::setColumn(unsigned int col)
{
	column = col;
}

const QString &course::getName() const
{
	return name;
}

long long course::getCurrentFitness(unsigned int gen,int EConflict, int IConflict) const
{
	//equal to long long
	long long int fitness{};

	for(int i = 0; i < constraintSetSL.size() ; ++i)
		for(int j = 0; j < constraintSetSL[i]->size() ;++j)
			if(constraintSetSL[i]->at(j)->ID != ID &&
					(*generationSL)[gen][constraintSetSL[i]->at(j)->column] == (*generationSL)[gen][column])
				fitness += EConflict;

	for(int i = 0; i < professorCoursesLS->size() ; ++i)
		if((*professorCoursesLS)[i]->ID != ID &&
					(*generationSL)[gen][(*professorCoursesLS)[i]->column] == (*generationSL)[gen][column])
			fitness += IConflict;

	return fitness;
}

const QVector<unsigned short> *course::getTimetableSL() const
{
	return timetableSL;
}

void course::setProfessorCoursesLS(const QVector<course *> *value)
{
	professorCoursesLS = value;
}

