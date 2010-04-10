#include "MoveDB.h"

MoveRecord MoveDB::searchMove(const Board& board) const
{
	const QHash<QString, MoveRecord>& db = board.getSelfColor() == 'W' ? dbWhite : dbBlack;
	QHash<QString, MoveRecord>::const_iterator it = db.find(board.toString());
	if(it == db.end())
		return MoveRecord();
	if(it.value().depth > board.getDepth())
		return MoveRecord();
	return it.value();
}

void MoveDB::saveMove(const Board& current, const MoveRecord& next)
{
	QHash<QString, MoveRecord>& db = current.getSelfColor() == 'W' ? dbWhite : dbBlack;
	db.insert(current.toString(), next);
}

//////////////////////////////////////////////////////////////////////////
int MoveDB::searchEstimation(const Board& board) const
{
	QHash<QString, int>::const_iterator it = estimationDB.find(board.toString());
	if(it != estimationDB.end())
		return it.value();
	return searchMove(board).score;    // search move db for estimation
}

void MoveDB::saveEstimation(const Board& board, int score) {
	estimationDB.insert(board.toString(), score);
}