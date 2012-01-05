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

#ifndef FREQUENCYMONITOR_H
#define FREQUENCYMONITOR_H

#include <QtCore/QObject>
#include <QtCore/QTimer>

class FrequencyMonitorPrivate;

/**
 * Class for measuring frequency of events
 *
 * Occurrence of the event is notified by the client via the notify() slot.
 * On a regular interval, both an instantaneous and a rolling average
 * event frequency are calculated.
 */
class FrequencyMonitor : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FrequencyMonitor)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(int samplingInterval READ samplingInterval WRITE setSamplingInterval NOTIFY samplingIntervalChanged)
    Q_PROPERTY(int traceInterval READ traceInterval WRITE setTraceInterval NOTIFY traceIntervalChanged)
    Q_PROPERTY(qreal instantaneousFrequency READ instantaneousFrequency NOTIFY instantaneousFrequencyChanged)
    Q_PROPERTY(qreal averageFrequency READ averageFrequency NOTIFY averageFrequencyChanged)
public:
    FrequencyMonitor(QObject *parent = 0);
    ~FrequencyMonitor();

    static void qmlRegisterType();

    const QString &label() const;
    bool active() const;
    int samplingInterval() const;
    int traceInterval() const;
    qreal instantaneousFrequency() const;
    qreal averageFrequency() const;

signals:
    void labelChanged(const QString &value);
    void activeChanged(bool);
    void samplingIntervalChanged(int value);
    void traceIntervalChanged(int value);
    void frequencyChanged();
    void instantaneousFrequencyChanged(qreal value);
    void averageFrequencyChanged(qreal value);

public slots:
    Q_INVOKABLE void notify();
    Q_INVOKABLE void trace();
    void setActive(bool value);
    void setLabel(const QString &value);
    void setSamplingInterval(int value);
    void setTraceInterval(int value);

private:
    FrequencyMonitorPrivate *d_ptr;
};

#endif // FREQUENCYMONITOR_H
