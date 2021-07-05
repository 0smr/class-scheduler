#ifndef PROFESSOR_H
#define PROFESSOR_H

#include <QObject>
#include <QString>
#include <QVector>

#include "course.h"

class professor : public QObject
{
	Q_OBJECT
public:
	//------------------ common member ------------------
	explicit professor(QObject *parent = nullptr);
	professor(QString ,QObject *parent = nullptr);

	//------------------ member function ------------------
	void pushTime(unsigned short int );

	//------------------ setter member function ------------------
	void addCourse(course *);
	void setName(const QString &);
	void setCourseList(const QVector <course *> &);
	void setTimetable(QVector <unsigned short int> *);

	//------------------ getter member function ------------------
	const QVector<unsigned short> *getTimetable()const;
	const QVector<course *> &getCourseList()const;
	const unsigned int *getID()const;

	//------------------ constexpr member function ------------------
	constexpr unsigned short int getTimetable(int index)const
	{
		return timetable ->at(index);
	}

signals:

public slots:

private:
	//------------------ common member ------------------
	QVector <course *> courseList;

	//time domain list ,number between 1 - 72
	QVector<unsigned short int> *timetable;
	QString name;
	//QString family,suffix,prefix,etcetera.

	//------------------ const members ------------------
	const unsigned int ID;

	//------------------ static members ------------------
	static unsigned int counter;
};

#endif // PROFESSOR_H
