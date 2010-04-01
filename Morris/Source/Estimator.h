#ifndef ESTIMATOR_H
#define ESTIMATOR_H

/************************************************************************/
/* Static Estimation                                                    */
/************************************************************************/

#include <QChar>
#include <QHash>

class Board;

class EstimationDB
{
public:
	int search(const Board& board, bool opening) const;
	void save (const Board& board, int score, bool opening);

public:
	enum {NOT_FOUND = 123456789};

private:
	QHash<QString, int> dbOpen, dbGame;
};

class Estimator
{
public:
	Estimator() : startColor('W'), isOpening(true) {}
	void setStartColor(QChar color)  { startColor = color; }
	void setOpening   (bool opening) { isOpening = opening; }
	void resetCounter() { counter = 0; }
	int  getCounter() const { return counter; }

	virtual int getEstimation(const Board& board);

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
protected:
	virtual int getOpeningEstimation(const Board& board) const;
	virtual int getGameEstimation   (const Board& board) const;
};

class ImprovedEstimator : public Estimator
{
public:
	virtual int getEstimation(const Board& board);

protected:
	virtual int getOpeningEstimation(const Board& board) const;
	virtual int getGameEstimation   (const Board& board) const;

private:
	EstimationDB db;
};

#endif

