#ifndef TIMER_H
#define TIMER_H

#include <QObject>
#include <QTime>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

class Timer : public QObject
{
    Q_OBJECT
public:
    Timer();

    int breakDurationMin() const;
    QTime endOfBreak() const;

    void Save(QXmlStreamWriter & stream) const;
    void Load(QXmlStreamReader & stream);

    int breakBalance() const;

public slots:
    QTime StartTimer(bool start);

private:
    QTime _sessionStart;
    int _breakDurationMin;
    int _breakBalance;
};

#endif // TIMER_H
