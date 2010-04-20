#ifndef HashTable_h__
#define HashTable_h__

#include <QChar>
#include <QVector>
#include <QPair>
#include <vector>

class QString;

class KeyGenerator
{
public:
	virtual ulong getKey(const QString& str) const = 0;
	virtual ulong updateKey(const QString& str, int pos, QChar ch) const = 0;
};

class djb2 : public KeyGenerator
{
public:
	virtual ulong getKey(const QString& str) const;
	virtual ulong updateKey(const QString& str, int pos, QChar ch) const;
};

class Zobrist : public KeyGenerator
{
public:
	virtual ulong getKey(const QString& str) const;
	virtual ulong updateKey(const QString& str, int pos, QChar ch) const;

private:
	static unsigned long randomNumbers[23][3];
};

template <class T>
class HashTable
{
public:
	HashTable();
	~HashTable();
	void insert(const QString& key, const T& record);
	T*       find(const QString& key);
	const T* find(const QString& key) const;
	void clear();
	void setSize(int size);

private:
	std::vector< std::pair<QString, T> > buckets;
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
}

template <class T>
T* HashTable<T>::find(const QString& key)
{
	int pos = keyGenerator->getKey(key) % bucketSize;
	if(buckets[pos].first != key)
		return 0;
	return &(buckets[pos].second);
}

template <class T>
const T* HashTable<T>::find(const QString& key) const
{
	ulong pos = keyGenerator->getKey(key) % bucketSize;
	if(buckets[pos].first != key)
		return 0;
	return &(buckets[pos].second);
}

template <class T>
void HashTable<T>::clear() {
	fill(buckets.begin(), buckets.end(), std::make_pair(QString(), T()));
}

#endif // HashTable_h__