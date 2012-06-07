/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the plugins of the Qt Toolkit.
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSOUNDINSTANCE_P_H
#define QSOUNDINSTANCE_P_H

#include <QVector3D>
#include <QObject>
#include "qsoundsource_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QDeclarativeSound;
class QDeclarativeAudioEngine;

class QSoundInstance : public QObject
{
    Q_OBJECT
public:
    explicit QSoundInstance(QObject *parent);
    ~QSoundInstance();

    void play();

    enum State
    {
        StoppedState = QSoundSource::StoppedState,
        PlayingState = QSoundSource::PlayingState,
        PausedState = QSoundSource::PausedState
    };
    State state() const;

    void setPosition(const QVector3D& position);
    void setDirection(const QVector3D& direction);
    void setVelocity(const QVector3D& velocity);

    //this gain and pitch is used for dynamic user control during execution
    void setGain(qreal gain);
    void setPitch(qreal pitch);
    void setCone(qreal innerAngle, qreal outerAngle, qreal outerGain);

    //this varPitch and varGain is calculated from config in PlayVariation
    void updateVariationParameters(qreal varPitch, qreal varGain, bool looping);

    void bindSoundDescription(QDeclarativeSound *sound);

    void update3DVolume(const QVector3D& listenerPosition);

    bool attenuationEnabled() const;

Q_SIGNALS:
    void stateChanged(QSoundInstance::State state);

public Q_SLOTS:
    void pause();
    void stop();

private Q_SLOTS:
    void resume();
    void bufferReady();
    void categoryVolumeChanged();
    void handleSourceStateChanged(QSoundSource::State);

private:
    void setState(State state);
    void prepareNewVariation();
    void detach();
    qreal categoryVolume() const;
    void updatePitch();
    void updateGain();
    void updateConeOuterGain();

    void sourcePlay();
    void sourcePause();
    void sourceStop();

    QSoundSource *m_soundSource;
    QSoundBuffer *m_bindBuffer;

    QDeclarativeSound *m_sound;
    int m_variationIndex;

    bool                 m_isReady; //true if the sound source is already bound to some sound buffer
    qreal                m_gain;
    qreal                m_attenuationGain;
    qreal                m_varGain;
    qreal                m_pitch;
    qreal                m_varPitch;
    State                m_state;
    qreal                m_coneOuterGain;

    QDeclarativeAudioEngine *m_engine;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSOUNDINSTANCE_P_H
