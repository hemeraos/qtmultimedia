/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "frequencymonitor.h"
#include <QtCore/QDebug>
#include <QtCore/QElapsedTimer>
#include <QtCore/QString>
#include <QtCore/QTime>
#include <QtCore/QTimer>

//#define VERBOSE_TRACE

inline QDebug qtTrace() { return qDebug() << "[frequencymonitor]"; }
#ifdef VERBOSE_TRACE
inline QDebug qtVerboseTrace() { return qtTrace(); }
#else
inline QNoDebug qtVerboseTrace() { return QNoDebug(); }
#endif

static const int DefaultSamplingInterval = 100;
static const int DefaultTraceInterval = 0;

class FrequencyMonitorPrivate : public QObject
{
    Q_OBJECT
public:
    FrequencyMonitorPrivate(FrequencyMonitor *parent);
    void calculateInstantaneousFrequency();

private slots:
    void calculateAverageFrequency();
    void stalled();

public:
    FrequencyMonitor *const q_ptr;
    QString m_label;
    bool m_active;
    qreal m_instantaneousFrequency;
    QElapsedTimer m_instantaneousElapsed;
    QTimer *m_averageTimer;
    QElapsedTimer m_averageElapsed;
    int m_count;
    qreal m_averageFrequency;
    QTimer *m_traceTimer;
    QTimer *m_stalledTimer;
};

FrequencyMonitorPrivate::FrequencyMonitorPrivate(FrequencyMonitor *parent)
:   QObject(parent)
,   q_ptr(parent)
,   m_active(false)
,   m_instantaneousFrequency(0)
,   m_averageTimer(new QTimer(this))
,   m_count(0)
,   m_averageFrequency(0)
,   m_traceTimer(new QTimer(this))
,   m_stalledTimer(new QTimer(this))
{
    m_instantaneousElapsed.start();
    connect(m_averageTimer, SIGNAL(timeout()),
            this, SLOT(calculateAverageFrequency()));
    if (DefaultSamplingInterval)
        m_averageTimer->start(DefaultSamplingInterval);
    m_averageElapsed.start();
    connect(m_traceTimer, SIGNAL(timeout()),
            q_ptr, SLOT(trace()));
    if (DefaultTraceInterval)
        m_traceTimer->start(DefaultTraceInterval);
    m_stalledTimer->setSingleShot(true);
    connect(m_stalledTimer, SIGNAL(timeout()),
            this, SLOT(stalled()));
}

void FrequencyMonitorPrivate::calculateInstantaneousFrequency()
{
    const qint64 ms = m_instantaneousElapsed.restart();
    m_instantaneousFrequency = ms ? qreal(1000) / ms : 0;
    m_stalledTimer->start(3 * ms);
    if (m_instantaneousFrequency)
        q_ptr->setActive(true);
    q_ptr->emit instantaneousFrequencyChanged(m_instantaneousFrequency);
    q_ptr->emit frequencyChanged();
}

void FrequencyMonitorPrivate::calculateAverageFrequency()
{
    const qint64 ms = m_averageElapsed.restart();
    m_averageFrequency = qreal(m_count * 1000) / ms;
    q_ptr->emit averageFrequencyChanged(m_averageFrequency);
    q_ptr->emit frequencyChanged();
    m_count = 0;
}

void FrequencyMonitorPrivate::stalled()
{
    if (m_instantaneousFrequency) {
        qtVerboseTrace() << "FrequencyMonitor::stalled";
        m_instantaneousFrequency = 0;
        q_ptr->emit instantaneousFrequencyChanged(m_instantaneousFrequency);
        q_ptr->emit frequencyChanged();
    }
}

FrequencyMonitor::FrequencyMonitor(QObject *parent)
:   QObject(parent)
,   d_ptr(0)
{
    d_ptr = new FrequencyMonitorPrivate(this);
    qtTrace() << "FrequencyMonitor::FrequencyMonitor";
}

FrequencyMonitor::~FrequencyMonitor()
{

}

const QString &FrequencyMonitor::label() const
{
    return d_func()->m_label;
}

bool FrequencyMonitor::active() const
{
    return d_func()->m_active;
}

int FrequencyMonitor::samplingInterval() const
{
    return d_ptr->m_averageTimer->isActive() ? d_ptr->m_averageTimer->interval() : 0;
}

int FrequencyMonitor::traceInterval() const
{
    return d_ptr->m_traceTimer->isActive() ? d_ptr->m_traceTimer->interval() : 0;
}

qreal FrequencyMonitor::instantaneousFrequency() const
{
    return d_func()->m_instantaneousFrequency;
}

qreal FrequencyMonitor::averageFrequency() const
{
    return d_func()->m_averageFrequency;
}

void FrequencyMonitor::notify()
{
    Q_D(FrequencyMonitor);
    ++(d->m_count);
    d->calculateInstantaneousFrequency();
}

void FrequencyMonitor::trace()
{
    Q_D(FrequencyMonitor);
    const QString value = QString::fromAscii("instant %1 average %2")
                            .arg(d->m_instantaneousFrequency, 0, 'f', 2)
                            .arg(d->m_averageFrequency, 0, 'f', 2);
    if (d->m_label.isEmpty())
        qtTrace() << "FrequencyMonitor::trace" << value;
    else
        qtTrace() << "FrequencyMonitor::trace" << "label" << d->m_label << value;
}

void FrequencyMonitor::setLabel(const QString &value)
{
    Q_D(FrequencyMonitor);
    if (d->m_label != value) {
        d->m_label = value;
        emit labelChanged(d->m_label);
    }
}

void FrequencyMonitor::setActive(bool value)
{
    Q_D(FrequencyMonitor);
    if (d->m_active != value) {
        d->m_active = value;
        emit activeChanged(d->m_active);
    }
}

void FrequencyMonitor::setSamplingInterval(int value)
{
    Q_D(FrequencyMonitor);
    if (samplingInterval() != value) {
        if (value) {
            d->m_averageTimer->setInterval(value);
            d->m_averageTimer->start();
        } else {
            d->m_averageTimer->stop();
        }
        emit samplingIntervalChanged(value);
    }
}

void FrequencyMonitor::setTraceInterval(int value)
{
    Q_D(FrequencyMonitor);
    if (traceInterval() != value) {
        if (value) {
            d->m_traceTimer->setInterval(value);
            d->m_traceTimer->start();
        } else {
            d->m_traceTimer->stop();
        }
        emit traceIntervalChanged(value);
    }
}

#include "frequencymonitor.moc"
