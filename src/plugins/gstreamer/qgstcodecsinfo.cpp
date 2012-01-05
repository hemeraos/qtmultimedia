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

#include "qgstcodecsinfo.h"

#include <QtCore/qset.h>

#ifdef QMEDIA_GSTREAMER_CAMERABIN
#include <gst/pbutils/pbutils.h>
#include <gst/pbutils/encoding-profile.h>
#endif


QGstCodecsInfo::QGstCodecsInfo(QGstCodecsInfo::ElementType elementType)
{

#if GST_CHECK_VERSION(0,10,31)

    GstElementFactoryListType gstElementType = 0;
    switch (elementType) {
    case AudioEncoder:
        gstElementType = GST_ELEMENT_FACTORY_TYPE_AUDIO_ENCODER;
        break;
    case VideoEncoder:
        gstElementType = GST_ELEMENT_FACTORY_TYPE_VIDEO_ENCODER;
        break;
    case Muxer:
        gstElementType = GST_ELEMENT_FACTORY_TYPE_MUXER;
        break;
    }

    GstCaps *allCaps = supportedElementCaps(gstElementType);
    GstCaps *caps = gst_caps_new_empty();

    uint codecsCount = gst_caps_get_size(allCaps);
    for (uint i=0; i<codecsCount; i++) {
        gst_caps_append_structure(caps, gst_caps_steal_structure(allCaps, 0));
        gchar * capsString = gst_caps_to_string(caps);

        QString codec = QLatin1String(capsString);
        m_codecs.append(codec);

#ifdef QMEDIA_GSTREAMER_CAMERABIN
        gchar *description = gst_pb_utils_get_codec_description(caps);
        m_codecDescriptions.insert(codec, QString::fromUtf8(description));

        if (description)
            g_free(description);
#else
        m_codecDescriptions.insert(codec, codec);
#endif

        if (capsString)
            g_free(capsString);

        gst_caps_remove_structure(caps, 0);
    }
#else
    Q_UNUSED(elementType);
#endif // GST_CHECK_VERSION(0,10,31)
}

QStringList QGstCodecsInfo::supportedCodecs() const
{
    return m_codecs;
}

QString QGstCodecsInfo::codecDescription(const QString &codec) const
{
    return m_codecDescriptions.value(codec);
}

#if GST_CHECK_VERSION(0,10,31)

/*!
  List all supported caps for all installed elements of type \a elementType.

  Caps are simplified to mime type and a few field necessary to distinguish
  different codecs like mpegversion or layer.
 */
GstCaps* QGstCodecsInfo::supportedElementCaps(GstElementFactoryListType elementType,
                                         GstRank minimumRank,
                                         GstPadDirection padDirection)
{
    GList *elements = gst_element_factory_list_get_elements(elementType, minimumRank);
    GstCaps *res = gst_caps_new_empty();

    QSet<QByteArray> fakeEncoderMimeTypes;
    fakeEncoderMimeTypes << "unknown/unknown"
                  << "audio/x-raw-int" << "audio/x-raw-float"
                  << "video/x-raw-yuv" << "video/x-raw-rgb";

    QSet<QByteArray> fieldsToAdd;
    fieldsToAdd << "mpegversion" << "layer" << "layout" << "raversion"
                << "wmaversion" << "wmvversion" << "variant";

    GList *element = elements;
    while (element) {
        GstElementFactory *factory = (GstElementFactory *)element->data;
        element = element->next;

        const GList *padTemplates = gst_element_factory_get_static_pad_templates(factory);
        while (padTemplates) {
            GstStaticPadTemplate *padTemplate = (GstStaticPadTemplate *)padTemplates->data;
            padTemplates = padTemplates->next;

            if (padTemplate->direction == padDirection) {
                const GstCaps *caps = gst_static_caps_get(&padTemplate->static_caps);
                for (uint i=0; i<gst_caps_get_size(caps); i++) {
                    const GstStructure *structure = gst_caps_get_structure(caps, i);

                    //skip "fake" encoders
                    if (fakeEncoderMimeTypes.contains(gst_structure_get_name(structure)))
                        continue;

                    GstStructure *newStructure = gst_structure_new(gst_structure_get_name(structure), NULL);

                    //add structure fields to distinguish between formats with similar mime types,
                    //like audio/mpeg
                    for (int j=0; j<gst_structure_n_fields(structure); j++) {
                        const gchar* fieldName = gst_structure_nth_field_name(structure, j);
                        if (fieldsToAdd.contains(fieldName)) {
                            const GValue *value = gst_structure_get_value(structure, fieldName);
                            GType valueType = G_VALUE_TYPE(value);

                            //don't add values of range type,
                            //gst_pb_utils_get_codec_description complains about not fixed caps

                            if (valueType != GST_TYPE_INT_RANGE && valueType != GST_TYPE_DOUBLE_RANGE &&
                                valueType != GST_TYPE_FRACTION_RANGE && valueType != GST_TYPE_LIST &&
                                valueType != GST_TYPE_ARRAY)
                                gst_structure_set_value(newStructure, fieldName, value);
                        }
                    }

                    gst_caps_merge_structure(res, newStructure);
                }
            }
        }
    }
    gst_plugin_feature_list_free(elements);

    return res;
}
#endif //GST_CHECK_VERSION(0,10,31)
