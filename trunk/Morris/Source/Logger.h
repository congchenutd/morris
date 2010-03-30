#ifndef Logger_h__
#define Logger_h__

#include <QObject>
#include <QApplication>

class Logger : public QObject
{
	Q_OBJECT

public:
	Logger() : counterOn(true), counter(0) {}

	Logger& operator << (const QString& log)
	{
		QString msg = counterOn ? tr("%1: %2").arg(counter++).arg(log) : log;
		emit logAvailable(msg);
		qApp->processEvents();
		return *this;
	}

	bool isCounterOn() const { return counterOn; }
	void setCounterOn(bool on) { counterOn = on; }
	void resetCounter() {
		counter = 0;
		*this << "Log reset";
	}

signals:
	void logAvailable(const QString& log);

private:
	bool counterOn;
	unsigned long counter;
};

#endif // Logger_h__
