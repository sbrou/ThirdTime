#include "timer.h"

#include <QFile>
#include <QDate>

Timer::Timer() : QObject(), _breakBalance(0)
{
}

void Timer::StartTimer(bool start)
{
    if (start)
    {
        QTime startTime = QTime::currentTime();
        if (_sessionStart <= QTime(13, 00, 00) && startTime >= QTime(13, 00, 00))
            _breakBalance = 0;

        int breakDuration = BreakDuration();
        if (breakDuration > 0)
        {
            QTime theoricalEndOfBreak = _sessionEnd.addSecs(breakDuration * 60);
            _breakBalance += (startTime.secsTo(theoricalEndOfBreak) / 60);
        }

        _sessionStart = startTime;
        _sessionEnd = QTime();
    }
    else
    {
        _sessionEnd = QTime::currentTime();

        QFile logFile("ThirdTime.log");
        logFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream out(&logFile);
        out << _sessionStart.toString() << " - " << _sessionEnd.toString() << "\n";
        logFile.close();
    }
}

const QTime &Timer::SessionStartTime() const
{
    return _sessionStart;
}

const QTime &Timer::SessionStopTime() const
{
    return _sessionEnd;
}

int Timer::BreakBalance() const
{
    return _breakBalance;
}

bool Timer::InFocusMode() const
{
    return _sessionEnd.isNull();
}

int Timer::BreakDuration() const
{
    if (_sessionEnd.isNull())
        return -1;

    return (_sessionStart.secsTo(_sessionEnd) / 60) / 4;
}

QTime Timer::EndOfBreak() const
{
    int breakDuration = BreakDuration();
    return breakDuration > 0 ? _sessionEnd.addSecs(breakDuration * 60) : QTime();
}

void Timer::Save(QXmlStreamWriter &stream) const
{
    stream.writeStartElement("lastsession");
    if (_sessionStart.isValid())
        stream.writeTextElement("start", _sessionStart.toString());
    else
        stream.writeEmptyElement("start");

    if (_sessionEnd.isValid())
        stream.writeTextElement("stop", _sessionEnd.toString());
    else
        stream.writeEmptyElement("stop");

    stream.writeTextElement("breakbalance", QString::number(_breakBalance));
    stream.writeEndElement();
}

void Timer::Load(QXmlStreamReader &stream)
{
    while (stream.readNextStartElement())
    {
        if (stream.name().toString() == "start")
        {
            _sessionStart = QTime::fromString(stream.readElementText());
        }
        if (stream.name().toString() == "stop")
        {
            _sessionEnd = QTime::fromString(stream.readElementText());
        }
        if (stream.name().toString() == "breakbalance")
        {
            _breakBalance = stream.readElementText().toInt();
        }
    }
}
