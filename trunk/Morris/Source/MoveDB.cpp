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

void MoveDB::saveMove(const Board& current, const Board& next, int score)
{
	HashTable<MoveRecord>& db = current.getSelfColor() == 'W' ? dbWhite : dbBlack;
	MoveRecord* p = db.find(current.toString());
	if(p == 0 || (p != 0 && p->depth <= current.getDepth()) )
		db.insert(current.toString(), MoveRecord(next, score, current.getDepth()));
}

//////////////////////////////////////////////////////////////////////////
int MoveDB::searchEstimation(const Board& board) const
{
	const int* p = estimationDB.find(board.toString());
	if(p != 0)
		return *p;
//	return MoveRecord::NOT_FOUND;

	return searchMove(board).score;    // search move db for estimation
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
	QFile estimationBackup("estimation.txt");
	estimationBackup.open(QFile::ReadOnly);
	QTextStream isEstimation(&estimationBackup);
	isEstimation >> estimationDB;

	QFile whiteBackup("white.txt");
	whiteBackup.open(QFile::ReadOnly);
	QTextStream osWhite(&whiteBackup);
	osWhite >> dbWhite;

	QFile blackBackup("black.txt");
	blackBackup.open(QFile::ReadOnly);
	QTextStream osBlack(&blackBackup);
	osBlack >> dbBlack;
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


//////////////////////////////////////////////////////////////////////////
QTextStream& operator<<(QTextStream& os, const MoveRecord& record)
{
	os << record.nextMove.toString() << "\t" 
	   << record.score << "\t" << record.depth << "\t";
	return os;
}

QTextStream& operator>>(QTextStream& is, MoveRecord& record)
{
	QString nextMove;
	is >> nextMove;
	record.nextMove.setString(nextMove);

	is >> record.score >> record.depth;
	return is;
}