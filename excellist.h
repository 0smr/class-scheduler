#ifndef EXCELLIST_H
#define EXCELLIST_H

#include <QVariant>
#include <QObject>
#include <QVector>
#include <QDir>

#include <algorithm>

#include "xlsxdocument.h"
#include "course.h"
#include "professor.h"

class excelList : public QObject
{
	Q_OBJECT
public:
	//------------------ special member function ------------------
	explicit excelList(QObject *parent = nullptr);
	excelList(QDir , QObject *parent = nullptr);

	//------------------ common member function ------------------
	QVector<professor *> readProfessorInformation();
	QVector<QVector<course *> *> readConstraintSheet(QVector<course *>);
	bool writeData(QVector<professor *> );

	/*
	generic function for search it may slower than personilized.
	template <class type,typename pred> // predicated on
	static type find(typename QVector<type>::iterator begin,typename QVector<type>::iterator end,pred f)
	{	f(...,...);		}
	*/

	//------------------ getter member function ------------------

	//------------------ setter member function ------------------
	void setDirection(const QDir &);
	void setDirection(const QString &);

	//------------------ constexpr member function ------------------
	constexpr unsigned int getRowCount()const
	{
		return profInfoRowCount;
	}
signals:

public slots:

private:
	//------------------ common member ------------------
	QDir direction;

	unsigned int profInfoRowCount;
	//------------------ pointer member ------------------
	QXlsx::Document *book;
};

#endif // EXCELLIST_H
