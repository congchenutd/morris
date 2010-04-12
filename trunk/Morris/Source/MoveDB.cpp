#include "MoveDB.h"

MoveRecord MoveDB::searchMove(const Board& board) const
{
	const HashTable<MoveRecord>& db = board.getSelfColor() == 'W' ? dbWhite : dbBlack;
	const MoveRecord* p = db.find(board.toString());
	if(p == 0)
		return MoveRecord();
	return *p;
}

void MoveDB::saveMove(const Board& current, const Board& next, int score, int alpha, int beta)
{
	HashTable<MoveRecord>& db = current.getSelfColor() == 'W' ? dbWhite : dbBlack;
	MoveRecord::RecordType type = (score <= alpha) ? MoveRecord::UPPER_BOUND :
								  (score >= beta)  ? MoveRecord::LOWER_BOUND :
													 MoveRecord::EXACT_VALUE;
	db.insert(current.toString(), MoveRecord(next, score, current.getDepth(), type));
}

//////////////////////////////////////////////////////////////////////////
int MoveDB::searchEstimation(const Board& board) const
{
	const int* p = estimationDB.find(board.toString());
	if(p != 0)
		return *p;
	return MoveRecord::NOT_FOUND;
//	return searchMove(board).score;    // search move db for estimation
}

void MoveDB::saveEstimation(const Board& board, int score) {
	estimationDB.insert(board.toString(), score);
}
