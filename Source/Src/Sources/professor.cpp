#include "professor.h"

//------------------ static variable initializing ------------------
unsigned int professor::counter{};

professor::professor(QObject *parent)
	:QObject (parent), timetable (nullptr), ID(counter ++)
{
	name = "no name";
	timetable = new QVector<unsigned short int>();
}

professor::professor(QString name, QObject *parent):professor(parent)
{
	this->name = name;
}

void professor::pushTime(unsigned short int freeHour)
{
	if(0 < freeHour && freeHour < 72)
		timetable ->push_back(freeHour);
}

void professor::addCourse(course *xVal)
{
	courseList.push_back(xVal);
}

void professor::setName(const QString &newName)
{
	name = newName;
}

void professor::setCourseList(const QVector<course *> &CL)
{
	courseList = CL;
}

const QVector<unsigned short> *professor::getTimetable() const
{
	return timetable ;
}

const QVector<course *> &professor::getCourseList() const
{
	return courseList;
}

const unsigned int * professor::getID() const
{
	return &ID;
}
