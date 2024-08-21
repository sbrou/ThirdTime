#include "timer.h"

#include <QFile>
#include <QDate>

Timer::Timer() : QObject()
               , _breakDurationMin(-1)
               , _breakBalance(0)
{
}

QTime Timer::StartTimer(bool start)
{
    if (start)
    {
        QTime startTime = QTime::currentTime();
        if (startTime >= QTime(13, 00, 00))
            _breakBalance = 0;

        if (_breakDurationMin > 0)
        {
            QTime theoricalEndOfBreak = _sessionStart.addSecs(_breakDurationMin * 60);
            _breakBalance += (theoricalEndOfBreak.secsTo(startTime) / 60);
        }

        _sessionStart = startTime;
        _breakDurationMin = -1;

        return startTime;
    }
    else
    {
        QTime endTime = QTime::currentTime();
        int breakDurationSecs = -endTime.secsTo(_sessionStart) / 4;
        _breakDurationMin = breakDurationSecs; // / 60;

        QFile logFile("ThirdTime.log");
        logFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream out(&logFile);
        out << _sessionStart.toString() << " - " << endTime.toString() << "\n";
        logFile.close();

        return endTime;
    }
}

int Timer::breakBalance() const
{
    return _breakBalance;
}

int Timer::breakDurationMin() const
{
    return _breakDurationMin;
}

QTime Timer::endOfBreak() const
{
    return _breakDurationMin > 0 ? _sessionStart.addSecs(_breakDurationMin * 60) : QTime();
}

void Timer::Save(QXmlStreamWriter &stream) const
{
    stream.writeStartElement("lastsession");
    stream.writeAttribute("state", _breakDurationMin > 0 ? "break" : "focus");
    if (_sessionStart.isValid())
        stream.writeTextElement("start", _sessionStart.toString());
    else
        stream.writeEmptyElement("start");

    stream.writeTextElement("breakDurationMin", QString::number(_breakDurationMin));
    stream.writeTextElement("breakBalance", QString::number(_breakBalance));
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
        if (stream.name().toString() == "breakDurationMin")
        {
            _breakDurationMin = stream.readElementText().toInt();
        }
        if (stream.name().toString() == "breakBalance")
        {
            _breakBalance = stream.readElementText().toInt();
        }
    }
}
