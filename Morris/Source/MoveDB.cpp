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
	MoveRecord* p = db.find(current.toString());
	if(p == 0 || (p != 0 && p->depth < current.getDepth()) )
	{
		MoveRecord::RecordType type = (score <= alpha) ? MoveRecord::UPPER_BOUND :
									  (score >= beta)  ? MoveRecord::LOWER_BOUND :
														 MoveRecord::EXACT_VALUE;
		db.insert(current.toString(), MoveRecord(next, score, current.getDepth(), type));
	}
}

//////////////////////////////////////////////////////////////////////////
int MoveDB::searchEstimation(const Board& board) const
{
	const int* p = estimationDB.find(board.toString());
	if(p != 0)
		return *p;
//	return MoveRecord::NOT_FOUND;

	MoveRecord record = searchMove(board);    // search move db for estimation
	return (record.type == MoveRecord::EXACT_VALUE) ? record.score : MoveRecord::NOT_FOUND;
}

void MoveDB::saveEstimation(const Board& board, int score) {
	estimationDB.insert(board.toString(), score);
}

void MoveDB::setSize(int size)
{
	dbWhite.setSize(size);
	dbBlack.setSize(size);
	estimationDB.setSize(size);
}

void HistoryHeuristc::save(const Board& from, const Board& to, int depth)
{
	if(depth > table.size())
		return;

//	table[depth-1][from][to] += depth * depth;
}

int HistoryHeuristc::search(const Board& from, const Board& to, int depth)
{
	if(depth > table.size())
		return 0;

	//return table[depth-1][from][to];
}

void HistoryHeuristc::deepen()
{
	table.push_back(HHTable(23, std::vector<int>(23, 0)));
}