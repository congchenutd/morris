#include "HashTable.h"
#include <QtGui>

ulong djb2::getKey(const QString& str) const
{
	unsigned long hash = 5381;
	
	for(int i=0; i<str.length(); ++i)
		hash = ((hash << 5) + hash) + str.at(i).toAscii();  // hash * 33 + c

	return hash;
}

ulong djb2::updateKey(const QString& str, int, QChar) const {
	return getKey(str);
}