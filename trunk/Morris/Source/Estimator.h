#ifndef ESTIMATOR_H
#define ESTIMATOR_H

/************************************************************************/
/* Static Estimation                                                    */
/************************************************************************/

#include <QChar>

class Board;
class Estimator
{
public:
	Estimator() : startColor('W'), isOpening(true) {}
	int getEstimation(const Board& board);
	void setStartColor(QChar color)  { startColor = color; }
	void setOpening   (bool opening) { isOpening = opening; }
	void resetCounter() { counter = 0; }
	int  getCounter() const { return counter; }

protected:
	virtual int getOpeningEstimation(const Board& board) const = 0;
	virtual int getGameEstimation   (const Board& board) const = 0;

public:
	enum {MAX_ESTIMATION = 10000, MIN_ESTIMATION = -10000};

protected:
	QChar startColor;
	bool  isOpening;
	int   counter;
};

class BasicEstimator : public Estimator
{
public:
	virtual int getOpeningEstimation(const Board& board) const;
	virtual int getGameEstimation   (const Board& board) const;
};

class ImprovedEstimator : public Estimator
{
	virtual int getOpeningEstimation(const Board& board) const;
	virtual int getGameEstimation   (const Board& board) const;
};

#endif

