#include "HashTable.h"
#include <QtGui>
#include <time.h>

quint64 djb2::getKey(const QString& str) const
{
	quint64 hash = 5381;
	
	for(int i=0; i<str.length(); ++i)
		hash = ((hash << 5) + hash) + str.at(i).toAscii();  // hash * 33 + c

	return hash;
}

Zobrist::Zobrist()
{
	srand(time(0));
	for(int i=0; i<23; ++i)
		for(int j=0; j<3; ++j)
			randomNumbers[i][j] = rand64();
}

int Zobrist::menToIndex(QChar color) const {
	return color == 'x' ? 0 :
		   color == 'W' ? 1 : 2;
}

quint64 Zobrist::getKey(const QString& str) const
{
	quint64 result = 0;
	for(int i=0; i<23; ++i)
		result ^= randomNumbers[i][menToIndex(str.at(i))];
	return result;
}

quint64 Zobrist::rand64() const 
{
	return rand() ^ ((quint64)rand() << 15) ^ ((quint64) rand() << 30 ) ^ 
		   ((quint64)rand() << 45) ^ ((quint64) rand() << 60);
}
