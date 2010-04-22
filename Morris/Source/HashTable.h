#ifndef HashTable_h__
#define HashTable_h__

#include <QChar>
#include <QVector>
#include <QPair>
#include <vector>
#include <QTextStream>

class QString;

class KeyGenerator
{
public:
	virtual quint64 getKey(const QString& str) const = 0;
};

class djb2 : public KeyGenerator
{
public:
	virtual quint64 getKey(const QString& str) const;
};

class Zobrist : public KeyGenerator
{
public:
	Zobrist();
	virtual quint64 getKey(const QString& str) const;

private:
	int menToIndex(QChar color) const;
	quint64 rand64() const;

private:
	quint64 randomNumbers[23][3];
};


template <class T>
class HashTable
{
public:
	typedef std::vector< std::pair<QString, T> > Buckets;

	HashTable();
	~HashTable();
	void insert(const QString& key, const T& record);
	T*       find(const QString& key, bool returnSynonym = false);
	const T* find(const QString& key, bool returnSynonym = false) const;
	void clear();
	void setSize(int size);
	mutable int hit;
	mutable int visit;
	mutable int insertion;
	int collision;
	int length;

	template <class T>
	friend QTextStream& operator << (QTextStream& os, const HashTable<T>& ht);
	template <class T>
	friend QTextStream& operator >> (QTextStream& os, HashTable<T>& ht);

private:
	int getPosition(const QString& key) const;

private:
	Buckets buckets;
	int bucketSize;
	KeyGenerator* keyGenerator;
};


template <class T>
void HashTable<T>::setSize(int size)
{
	if(size != bucketSize)
	{
		bucketSize = size;
		buckets.resize(bucketSize);
		clear();
	}
}

template <class T>
HashTable<T>::HashTable()
{
	keyGenerator = new djb2;
	setSize(1000000);   // 1 million
	hit = insertion = visit = length = 0;
	collision = 0;
}

template <class T>
HashTable<T>::~HashTable() {
	delete keyGenerator;
}

template <class T>
void HashTable<T>::insert(const QString& key, const T& record)
{
	int pos = getPosition(key);
	if(buckets[pos].first.isEmpty())
		length ++;
	buckets[pos].first = key;
	buckets[pos].second = record;
}

template <class T>
T* HashTable<T>::find(const QString& key, bool returnSynonym) {
	return const_cast<T*>(const_cast<const HashTable*>(this)->find(key, returnSynonym));
}

template <class T>
const T* HashTable<T>::find(const QString& key, bool returnSynonym) const
{
	int pos = getPosition(key);
	if(buckets[pos].first.isEmpty())
		return 0;
	if(returnSynonym)
		return &(buckets[pos].second);
	else
		return buckets[pos].first != key ? 0 : &(buckets[pos].second);
}

template <class T>
void HashTable<T>::clear() {
	fill(buckets.begin(), buckets.end(), std::make_pair(QString(), T()));
}

template <class T>
int HashTable<T>::getPosition(const QString& key) const {
	return keyGenerator->getKey(key) % bucketSize;
}


//////////////////////////////////////////////////////////////////////////
template <class T>
QTextStream& operator << (QTextStream& os, const HashTable<T>& ht)
{
	int i = 0;
	for(HashTable<T>::Buckets::const_iterator it = ht.buckets.begin(); it != ht.buckets.end(); ++it, ++i)
		if(!it->first.isEmpty())
			os << i << "\t" << it->first << "\t" << it->second << "\r\n";
	return os;
}

template <class T>
QTextStream& operator >> (QTextStream& is, HashTable<T>& ht)
{
	while(!is.atEnd())
	{
		int index;
		is >> index;
		is >> ht.buckets[index].first >> ht.buckets[index].second;
	}
	return is;
}

#endif // HashTable_h__