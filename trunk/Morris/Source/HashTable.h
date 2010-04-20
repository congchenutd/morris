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
	virtual ulong getKey(const QString& str) const = 0;
};

class djb2 : public KeyGenerator
{
public:
	virtual ulong getKey(const QString& str) const;
};

class Zobrist : public KeyGenerator
{
public:
	virtual ulong getKey(const QString& str) const;

private:
	static unsigned long randomNumbers[23][3];
};


template <class T>
class HashTable
{
public:
	typedef std::vector< std::pair<QString, T> > Buckets;

	HashTable();
	~HashTable();
	void insert(const QString& key, const T& record);
	T*       find(const QString& key);
	const T* find(const QString& key) const;
	void clear();
	void setSize(int size);
	mutable int hit;
	mutable int collision;
	mutable int visit;
	mutable int length;

	template <class T>
	friend QTextStream& operator << (QTextStream& os, const HashTable<T>& ht);
	template <class T>
	friend QTextStream& operator >> (QTextStream& os, HashTable<T>& ht);

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
	hit = length = collision = visit = 0;
}

template <class T>
HashTable<T>::~HashTable() {
	delete keyGenerator;
}

template <class T>
void HashTable<T>::insert(const QString& key, const T& record)
{
	int pos = keyGenerator->getKey(key) % bucketSize;
	buckets[pos].first = key;
	buckets[pos].second = record;
	length ++;
}

template <class T>
T* HashTable<T>::find(const QString& key) {
	return const_cast<T*>(const_cast<const HashTable*>(this)->find(key));
}

template <class T>
const T* HashTable<T>::find(const QString& key) const
{
	visit ++;
	int pos = keyGenerator->getKey(key) % bucketSize;
	if(buckets[pos].first != key)
	{
		collision ++;
		return 0;
	}
	hit ++;
	return &(buckets[pos].second);
}

template <class T>
void HashTable<T>::clear() {
	fill(buckets.begin(), buckets.end(), std::make_pair(QString(), T()));
}

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