#include "excellist.h"

excelList::excelList(QObject *parent)
	: QObject(parent),profInfoRowCount(0),book(nullptr)
{

}

excelList::excelList(QDir dir, QObject *parent):excelList(parent)
{
	direction = dir;
	book = new QXlsx::Document(direction.path());
}

QVector<professor *> excelList::readProfessorInformation()
{
	QVector <professor *> excelContent{};
	professor *prof = nullptr;
	course *edu = nullptr;

	int columnIter{}, rowIter{};
	QVariant cell{}, unitCell{};

	for(rowIter = 1 ; !book->read(rowIter,1).isNull() ; ++rowIter)
	{
		cell = book->read(rowIter,1);
		columnIter = 2;

		// professor information start with info in first column
		if(cell.toString() == "info")
		{
			prof = new professor();
			prof->setName(book->read(rowIter,2).toString()+" "+book->read(rowIter,3).toString());
		}

		else if(cell.toString() == "schedule")
		{
			cell = book->read(rowIter,columnIter);

			while(!cell.isNull())
			{
				++columnIter;
				prof->pushTime(static_cast<unsigned short>(cell.toInt()));
				cell = book->read(rowIter,columnIter);
			}
		}

		else if(cell.toString() == "courses")
		{
			cell = book->read(rowIter,columnIter);

			while(!cell.isNull())
			{
				++columnIter;
				edu = new course(cell.toString(),prof->getID());
				edu->setTimetableSL(prof->getTimetable());

				unitCell = book->read(rowIter+1,columnIter);
				if(!unitCell.isNull())
					edu->setUnit(static_cast<unsigned short>(unitCell.toInt()));

				//add edu(new course) to professor courseList
				prof->addCourse(edu);

				cell = book->read(rowIter,columnIter);
			}

			if(book->read(rowIter+1,1).toString() == "unit")
				++rowIter;

			//push back professor in list
			excelContent.push_back(prof);
		}
	}
	profInfoRowCount = static_cast<unsigned>(rowIter);

	return excelContent;
}

QVector<QVector<course *> *> excelList::readConstraintSheet(QVector <course *> CL)//not work.......
{
	QVector<QVector<course *> *> constraintSet{};
	QVector<course *> *subSet{};

	course * const* tVal{};

	QVariant cell{};
	int rowIter{1} , colIter{};

	if(book->selectSheet("constraint"))
	{
		while(!book->read(rowIter,1).isNull())
		{
			colIter = 2;
			subSet = new QVector<course *>();
			cell = book->read(rowIter , colIter);

			while(!cell.isNull())
			{
				tVal = course::find(CL , cell.toString());
				if(tVal)
				{
					subSet->push_back(*tVal);
					(*tVal)->addConstraintSubSet(subSet);
				}

				++colIter;
				cell = book->read(rowIter , colIter);
			}

			if(!subSet->empty())
				constraintSet.push_back(subSet);

			++rowIter;
		}
	}

	return constraintSet;
}

bool excelList::writeData(QVector<professor *> data)
{
	for(auto iter = data.begin() ; iter != data.end() ; ++iter)
	{

	}
	return true;
}
