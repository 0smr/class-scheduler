#ifndef COURSE_H
#define COURSE_H

#include <QObject>
#include <QVector>
#include <QDebug>

class course : public QObject
{
	Q_OBJECT
public:
	//------------------ special member function ------------------
	explicit course() = default;
	explicit course(QObject *parent = nullptr);

	course(QString name  ,const unsigned int* pID = nullptr, QObject *parent = nullptr);
	course(QString name  , unsigned int* pID , const QVector<unsigned short> *
			 , unsigned short int ** =nullptr , QObject *parent = nullptr);

	//------------------ common member function ------------------
	// add constraint subset to CSSLList
	void addConstraintSubSet(QVector<course *> *);
	static course * const *find(const QVector<course *> &container, const QString &val);

	//------------------ setter member function ------------------
	void setTimetableSL(const QVector<unsigned short int> *);
	void setProfessorCoursesLS(const QVector<course *> *);
	void setGenerationSL(unsigned short int *** );
	void setUnit(const unsigned short &);
	void setColumn(unsigned int );

	//------------------ getter member function ------------------
	const QString &getName()const;
	//(generation indexNumber, External Conflict Value, Internal Conflict Value)
	long long getCurrentFitness(unsigned int , int = 9 , int = 12)const;
	const QVector<unsigned short int> * getTimetableSL() const;

	//------------------ constexpr member function ------------------
	// return size of timetableSL
	constexpr int getTimetableSLSize() const
	{
		return timetableSL->size();
	}

	// return size of constraintSetSL
	constexpr int getCSSLSize() const
	{
		return constraintSetSL.size();
	}


signals:

public slots:

private:
	//------------------ list member ------------------
	QString name;

	//QString prof.Name

	// constraint set symbolic link
	QVector<QVector<course *> *> constraintSetSL;
	// timetable symbolic link (number of days that a master can present)
	const QVector<unsigned short int> *timetableSL;
	const QVector<course *> *professorCoursesLS;

	//------------------ common member ------------------
	/*
	 * column index in generation table.
	 * can equal to ID.
	 */
	unsigned int column;

	// hour in week, a number between 1 - 72
	short int hour;
	unsigned short int unit;

	//------------------ pointer member ------------------
	const unsigned int * profID;
	unsigned short int *** generationSL;
	//------------------ const member ------------------
	const unsigned long long int ID;

	//------------------ static member ------------------
	static unsigned long long int counter;
};
#endif // COURSE_H
