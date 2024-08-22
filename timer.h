#ifndef TIMER_H
#define TIMER_H

#include <QObject>
#include <QTime>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

struct Parameters
{
    int numerator;
    int denominator;
    QTime afternoonStart;
};

class Timer : public QObject
{
    Q_OBJECT
public:
    Timer();

    int BreakDuration() const;
    QTime EndOfBreak() const;
    int BreakBalance() const;
    bool InFocusMode() const;
    const QTime &SessionStartTime() const;
    const QTime &SessionStopTime() const;

    void Save(QXmlStreamWriter &stream) const;
    void Load(QXmlStreamReader &stream);

public slots:
    void StartTimer(bool start);

private:
    QTime _sessionStart;
    QTime _sessionEnd;
    int _breakBalance;

    static Parameters _params;
};

#endif // TIMER_H
