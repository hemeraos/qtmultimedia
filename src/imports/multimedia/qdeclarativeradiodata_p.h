/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Toolkit.
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

#ifndef QDECLARATIVERADIODATA_P_H
#define QDECLARATIVERADIODATA_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtDeclarative/qdeclarative.h>
#include <qradiodata.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QDeclarativeRadioData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString stationId READ stationId NOTIFY stationIdChanged)
    Q_PROPERTY(QDeclarativeRadioData::ProgramType programType READ programType NOTIFY programTypeChanged)
    Q_PROPERTY(QString programTypeName READ programTypeName NOTIFY programTypeNameChanged)
    Q_PROPERTY(QString stationName READ stationName NOTIFY stationNameChanged)
    Q_PROPERTY(QString radioText READ radioText NOTIFY radioTextChanged)
    Q_PROPERTY(bool alternativeFrequenciesEnabled READ alternativeFrequenciesEnabled
               WRITE setAlternativeFrequenciesEnabled NOTIFY alternativeFrequenciesEnabledChanged)
    Q_ENUMS(Error)
    Q_ENUMS(ProgramType)

public:

    enum Error {
        NoError = QRadioData::NoError,
        ResourceError = QRadioData::ResourceError,
        OpenError = QRadioData::OpenError,
        OutOfRangeError = QRadioData::OutOfRangeError
    };

    enum ProgramType {
        Undefined = QRadioData::Undefined,
        News = QRadioData::News,
        CurrentAffairs = QRadioData::CurrentAffairs,
        Information = QRadioData::Information,
        Sport = QRadioData::Sport,
        Education = QRadioData::Education,
        Drama = QRadioData::Drama,
        Culture = QRadioData::Culture,
        Science = QRadioData::Science,
        Varied = QRadioData::Varied,
        PopMusic = QRadioData::PopMusic,
        RockMusic = QRadioData::RockMusic,
        EasyListening = QRadioData::EasyListening,
        LightClassical = QRadioData::LightClassical,
        SeriousClassical = QRadioData::SeriousClassical,
        OtherMusic = QRadioData::OtherMusic,
        Weather = QRadioData::Weather,
        Finance = QRadioData::Finance,
        ChildrensProgrammes = QRadioData::ChildrensProgrammes,
        SocialAffairs = QRadioData::SocialAffairs,
        Religion = QRadioData::Religion,
        PhoneIn = QRadioData::PhoneIn,
        Travel = QRadioData::Travel,
        Leisure = QRadioData::Leisure,
        JazzMusic = QRadioData::JazzMusic,
        CountryMusic = QRadioData::CountryMusic,
        NationalMusic = QRadioData::NationalMusic,
        OldiesMusic = QRadioData::OldiesMusic,
        FolkMusic = QRadioData::FolkMusic,
        Documentary = QRadioData::Documentary,
        AlarmTest = QRadioData::AlarmTest,
        Alarm = QRadioData::Alarm,
        Talk = QRadioData::Talk,
        ClassicRock = QRadioData::ClassicRock,
        AdultHits = QRadioData::AdultHits,
        SoftRock = QRadioData::SoftRock,
        Top40 = QRadioData::Top40,
        Soft = QRadioData::Soft,
        Nostalgia = QRadioData::Nostalgia,
        Classical = QRadioData::Classical,
        RhythmAndBlues = QRadioData::RhythmAndBlues,
        SoftRhythmAndBlues = QRadioData::SoftRhythmAndBlues,
        Language = QRadioData::Language,
        ReligiousMusic = QRadioData::ReligiousMusic,
        ReligiousTalk = QRadioData::ReligiousTalk,
        Personality = QRadioData::Personality,
        Public = QRadioData::Public,
        College = QRadioData::College,
    };

    QDeclarativeRadioData(QObject *parent = 0);
    ~QDeclarativeRadioData();

    Q_INVOKABLE bool isAvailable() const;

    QString stationId() const;
    QDeclarativeRadioData::ProgramType programType() const;
    QString programTypeName() const;
    QString stationName() const;
    QString radioText() const;
    bool alternativeFrequenciesEnabled() const;

public Q_SLOTS:
    void setAlternativeFrequenciesEnabled(bool enabled);

Q_SIGNALS:
    void stationIdChanged(QString stationId);
    void programTypeChanged(QDeclarativeRadioData::ProgramType programType);
    void programTypeNameChanged(QString programTypeName);
    void stationNameChanged(QString stationName);
    void radioTextChanged(QString radioText);
    void alternativeFrequenciesEnabledChanged(bool enabled);

    void errorChanged();
    void error(QDeclarativeRadioData::Error errorCode);

private Q_SLOTS:
    void _q_programTypeChanged(QRadioData::ProgramType programType);
    void _q_error(QRadioData::Error errorCode);

private:
    Q_DISABLE_COPY(QDeclarativeRadioData)

    QRadioData *m_radioData;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeRadioData))

QT_END_HEADER

#endif // QDECLARATIVERADIODATA_P_H
