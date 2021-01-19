/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdarwindevicemanager_p.h"
#include "qmediadevicemanager.h"
#include "qcamerainfo_p.h"
#include "qaudiodeviceinfo_p.h"
#include "private/qcoreaudiodeviceinfo_p.h"
#include "private/qcoreaudioinput_p.h"
#include "private/qcoreaudiooutput_p.h"

#import <AVFoundation/AVFoundation.h>

#if defined(Q_OS_IOS) || defined(Q_OS_TVOS)
# include "qcoreaudiosessionmanager_p.h"
#endif

QT_BEGIN_NAMESPACE

#if defined(Q_OS_MACOS)
AudioDeviceID defaultAudioDevice(QAudio::Mode mode)
{
    AudioDeviceID audioDevice;
    UInt32 size = sizeof(audioDevice);
    const AudioObjectPropertySelector selector = (mode == QAudio::AudioOutput) ? kAudioHardwarePropertyDefaultOutputDevice
                                                                               : kAudioHardwarePropertyDefaultInputDevice;
    AudioObjectPropertyAddress defaultDevicePropertyAddress = { selector,
                                                                kAudioObjectPropertyScopeGlobal,
                                                                kAudioObjectPropertyElementMaster };

    if (AudioObjectGetPropertyData(kAudioObjectSystemObject,
                                   &defaultDevicePropertyAddress,
                                   0, NULL, &size, &audioDevice) != noErr) {
        qWarning("QAudioDeviceInfo: Unable to find default %s device",  (mode == QAudio::AudioOutput) ? "output" : "input");
        return 0;
    }

    return audioDevice;
}

static QByteArray uniqueId(AudioDeviceID device, QAudio::Mode mode)
{
    CFStringRef name;
    UInt32 size = sizeof(CFStringRef);

    AudioObjectPropertyScope audioPropertyScope = mode == QAudio::AudioInput ? kAudioDevicePropertyScopeInput : kAudioDevicePropertyScopeOutput;

    AudioObjectPropertyAddress audioDeviceNamePropertyAddress = { kAudioDevicePropertyDeviceUID,
                                                                  audioPropertyScope,
                                                                  kAudioObjectPropertyElementMaster };

    if (AudioObjectGetPropertyData(device, &audioDeviceNamePropertyAddress, 0, NULL, &size, &name) != noErr) {
        qWarning() << "QAudioDeviceInfo: Unable to get device UID";
        return QByteArray();
    }

    QString s = QString::fromCFString(name);
    CFRelease(name);
    return s.toUtf8();
}

QList<QAudioDeviceInfo> availableAudioDevices(QAudio::Mode mode)
{

    QList<QAudioDeviceInfo> devices;

    AudioDeviceID defaultDevice = defaultAudioDevice(mode);
    devices << QAudioDeviceInfo(new QCoreAudioDeviceInfo(defaultDevice, uniqueId(defaultDevice, mode), mode));

    UInt32 propSize = 0;
    AudioObjectPropertyAddress audioDevicesPropertyAddress = { kAudioHardwarePropertyDevices,
                                                               kAudioObjectPropertyScopeGlobal,
                                                               kAudioObjectPropertyElementMaster };

    if (AudioObjectGetPropertyDataSize(kAudioObjectSystemObject,
                                       &audioDevicesPropertyAddress,
                                       0, NULL, &propSize) == noErr) {

        const int dc = propSize / sizeof(AudioDeviceID);

        if (dc > 0) {
            AudioDeviceID*  audioDevices = new AudioDeviceID[dc];

            if (AudioObjectGetPropertyData(kAudioObjectSystemObject, &audioDevicesPropertyAddress, 0, NULL, &propSize, audioDevices) == noErr) {
                for (int i = 0; i < dc; ++i) {
                    if (audioDevices[i] == defaultDevice)
                        continue;

                    AudioStreamBasicDescription sf;
                    UInt32 size = sizeof(AudioStreamBasicDescription);
                    AudioObjectPropertyAddress audioDeviceStreamFormatPropertyAddress = { kAudioDevicePropertyStreamFormat,
                                                                                    (mode == QAudio::AudioInput ? kAudioDevicePropertyScopeInput : kAudioDevicePropertyScopeOutput),
                                                                                    kAudioObjectPropertyElementMaster };

                    if (AudioObjectGetPropertyData(audioDevices[i], &audioDeviceStreamFormatPropertyAddress, 0, NULL, &size, &sf) == noErr)
                        devices << QAudioDeviceInfo(new QCoreAudioDeviceInfo(audioDevices[i], uniqueId(audioDevices[i], mode), mode));
                }
            }

            delete[] audioDevices;
        }
    }

    return devices;
}
#endif

QDarwinDeviceManager::QDarwinDeviceManager()
    : QMediaPlatformDeviceManager()
{
}

QDarwinDeviceManager::~QDarwinDeviceManager()
{
}

QList<QAudioDeviceInfo> QDarwinDeviceManager::audioInputs() const
{
#ifdef Q_OS_IOS
    QList<QAudioDeviceInfo> devices;
    devices.append(QAudioDeviceInfo(new QCoreAudioDeviceInfo(0, "default", QAudio::AudioInput)));
    return devices;
#else
    return availableAudioDevices(QAudio::AudioInput);
#endif
}

QList<QAudioDeviceInfo> QDarwinDeviceManager::audioOutputs() const
{
#ifdef Q_OS_IOS
    QList<QAudioDeviceInfo> devices;
    devices.append(QAudioDeviceInfo(new QCoreAudioDeviceInfo(0, "default", QAudio::AudioOutput)));
    return devices;
#else
    return availableAudioDevices(QAudio::AudioOutput);
#endif
}

QList<QCameraInfo> QDarwinDeviceManager::videoInputs() const
{
    updateCameraDevices();
    return m_cameraDevices;
}

void QDarwinDeviceManager::updateCameraDevices() const
{
#ifdef Q_OS_IOS
    // Cameras can't change dynamically on iOS. Update only once.
    if (!m_cameraDevices.isEmpty())
        return;
#else
    // On OS X, cameras can be added or removed. Update the list every time, but not more than
    // once every 500 ms
    if (deviceCheckTimer.isValid() && deviceCheckTimer.elapsed() < 500) // ms
        return;
#endif

    QList<QCameraInfoPrivate> cameras;

    AVCaptureDevice *defaultDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    NSArray *videoDevices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
    for (AVCaptureDevice *device in videoDevices) {

        QCameraInfoPrivate *info = new QCameraInfoPrivate;
        if (defaultDevice && [defaultDevice.uniqueID isEqualToString:device.uniqueID])
            info->isDefault = true;
        info->id = QByteArray([[device uniqueID] UTF8String]);
        info->description = QString::fromNSString([device localizedName]);


        m_cameraDevices.append(QCameraInfo(info));
    }

#ifndef Q_OS_IOS
    deviceCheckTimer.restart();
#endif
}


void QDarwinDeviceManager::updateAudioDevices()
{
    // Headsets can be added or removed. Update the list every time, but not more than
    // once every 500 ms
    if (deviceCheckTimer.isValid() && deviceCheckTimer.elapsed() < 500) // ms
        return;

    deviceCheckTimer.restart();
}

QAbstractAudioInput *QDarwinDeviceManager::createAudioInputDevice(const QAudioDeviceInfo &info)
{
    return new CoreAudioInput(info);
}

QAbstractAudioOutput *QDarwinDeviceManager::createAudioOutputDevice(const QAudioDeviceInfo &info)
{
    return new QCoreAudioOutput(info);
}


QT_END_NAMESPACE