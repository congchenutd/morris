#ifndef ESTIMATOR_H
#define ESTIMATOR_H

/************************************************************************/
/* Static Estimation                                                    */
/************************************************************************/

#include <QChar>
#include "MoveDB.h"

class Board;

class Estimator
{
public:
	Estimator() : startColor('W'), isOpening(true), db(0) {}
	virtual ~Estimator() {}

	void setStartColor(QChar color)  { startColor = color;  }
	void setOpening   (bool opening) { isOpening = opening; }
	void setDB        (MoveDB* db)   { this->db = db;       }
	void resetCounter() { counter = 0; }
	int  getCounter() const { return counter; }
	
	virtual void clearDB() {}
	virtual int getEstimation(const Board& board);

protected:
	virtual int getOpeningEstimation(const Board& board) const = 0;
	virtual int getGameEstimation   (const Board& board) const = 0;

public:
	enum {MAX_ESTIMATION = 10000, MIN_ESTIMATION = -10000};

protected:
	QChar   startColor;
	bool    isOpening;
	MoveDB* db;
	int     counter;
};

class BasicEstimator : public Estimator
{
protected:
	virtual int getOpeningEstimation(const Board& board) const;
	virtual int getGameEstimation   (const Board& board) const;
};

class ImprovedEstimator : public BasicEstimator
{
public:
	virtual int getEstimation(const Board& board);
};

#endif

