#include "MoveDB.h"
#include <QFile>
#include <QTextStream>

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

void MoveDB::load()
{
}

void MoveDB::save()
{
	QFile estimationBackup("estimation.txt");
	estimationBackup.open(QFile::WriteOnly | QFile::Truncate);
	QTextStream osEstimation(&estimationBackup);
	osEstimation << estimationDB;

	QFile whiteBackup("white.txt");
	whiteBackup.open(QFile::WriteOnly | QFile::Truncate);
	QTextStream osWhite(&whiteBackup);
	osWhite << dbWhite;

	QFile blackBackup("black.txt");
	blackBackup.open(QFile::WriteOnly | QFile::Truncate);
	QTextStream osBlack(&blackBackup);
	osBlack << dbBlack;
}

QTextStream& operator<<(QTextStream& os, const MoveRecord& record)
{
	os << record.nextMove.toString() << "\t" 
	   << record.score << "\t" << record.depth << "\t" << record.type;
	return os;
}

QTextStream& operator>>(QTextStream& is, MoveRecord& record)
{

//	is >> record.nextMove
	return is;
}