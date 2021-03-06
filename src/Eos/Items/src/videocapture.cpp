// Copyright (c) 2016-2018 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#include <QQuickWindow>

#include <stdio.h>
#include <QSGSimpleTextureNode>
#include <QSGSimpleRectNode>
#include <QMutexLocker>
#include <QTimer>

#include "videocapture.h"

QMutex VideoCapture::mutex;

VideoCapture::VideoCapture(QQuickItem *parent) : QQuickItem(parent),
    m_frameCount(0),
    m_initialized(false),
    m_isVTAvailable(false),
    window_id(-1),
    resource_id(0),
    context_id(0),
    currentTextureId(0),
    nextTextureId(0),
    currentTexture(0)
{
    qWarning() << "[QML Video Capture Plugin]" << " " << "Created";
    setFlag(QQuickItem::ItemHasContents, true);
    QObject::connect(this, SIGNAL(enabledChanged()), this, SLOT(enabledChanged()), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(videoFrameChanged()), this, SLOT(acquireVideoTexture()), Qt::QueuedConnection);
}

VideoCapture::~VideoCapture()
{
    terminate();

    qWarning() << "[QML Video Capture Plugin]" << " " << "Destroyed";
}

bool VideoCapture::initialize()
{
    QMutexLocker locker(&mutex);

    if (m_initialized)
        return true;

    window_id = VT_CreateVideoWindow(VT_VIDEO_PATH_MAIN);
    if (window_id == -1) {
        qWarning() << "[QML Video Capture Plugin]" << " " << "VT_CreateVideoWindow Failed";
        return false;
    }
    qWarning() << "[QML Video Capture Plugin]" << " " << "window_id=" << window_id;

    if (VT_AcquireVideoWindowResource(window_id, &resource_id) != VT_OK) {
        qWarning() << "[QML Video Capture Plugin]" << " " << "VT_AcquireVideoWindowResource Failed";
        return false;
    }
    qWarning() << "[QML Video Capture Plugin]" << " " << "resource_id=" << resource_id;

    context_id = VT_CreateContext(resource_id, 2);
    if (context_id == -1) {
        qWarning() << "[QML Video Capture Plugin]" << " " << "VT_CreateContext Failed";
        VT_ReleaseVideoWindowResource(resource_id);
        return false;
    }
    qWarning() << "[QML Video Capture Plugin]" << " " << "context_id=" << context_id;

    if (VT_SetTextureSourceRegion(context_id, VT_SOURCE_REGION_MAX) != VT_OK) {
        qWarning() << "[QML Video Capture Plugin]" << " " << "VT_SetTextureSourceRegion Failed";
        VT_DeleteContext(context_id);
        VT_ReleaseVideoWindowResource(resource_id);
        return false;
    }

    if (VT_RegisterEventHandler(context_id, &OnEvent, this) != VT_OK) {
        qWarning() << "[QML Video Capture Plugin]" << " " << "VT_RegisterEventHandler Failed";
        VT_DeleteContext(context_id);
        VT_ReleaseVideoWindowResource(resource_id);
        return false;
    }

    m_initialized = true;

    qWarning() << "[QML Video Capture Plugin]" << " " << "initialized";
    return true;
}

void VideoCapture::terminate()
{
    QMutexLocker locker(&mutex);
    if (!m_initialized)
        return;

    if (currentTextureId != 0 && glIsTexture(currentTextureId)) {
        if ((currentTexture && currentTextureId != currentTexture->textureId()) || currentTexture == NULL) {
            VT_DeleteTexture(context_id, currentTextureId);
        }
        currentTextureId = 0;
    }

    if (nextTextureId != 0 && glIsTexture(nextTextureId)) {
        if ((currentTexture && nextTextureId != currentTexture->textureId()) || currentTexture == NULL) {
            VT_DeleteTexture(context_id, nextTextureId);
        }
        nextTextureId = 0;
    }

    if (currentTexture && currentTexture->textureId() != 0 && glIsTexture(currentTexture->textureId())) {
        VT_DeleteTexture(context_id, currentTexture->textureId());
        if (currentTexture) {
            currentTexture = NULL;
        }
    }

    if (VT_UnRegisterEventHandler(context_id) != VT_OK) {
        qWarning() << "[QML Video Capture Plugin]" << " " << "VT_UnRegisterEventHandler error!";
    }
    VT_DeleteContext(context_id);
    VT_ReleaseVideoWindowResource(resource_id);

    m_initialized = false;
    m_isVTAvailable = false;
    context_id = 0;
    resource_id = 0;
    window_id = -1;
    qWarning() << "[QML Video Capture Plugin]" << " " << "terminated";
}

void VideoCapture::OnEvent(VT_EVENT_TYPE_T type, void* data, void* user_data)
{
    VideoCapture * capture  = (VideoCapture *)user_data;
    switch (type) {
    case VT_AVAILABLE:
        if (capture) {
            emit capture->videoFrameChanged();
            capture->m_isVTAvailable = true;
        }
        break;
    case VT_UNAVAILABLE:
        qWarning() << "[QML Video Capture Plugin]" << " " << "VT_UNAVAILABLE received";
        break;
    case VT_RESOURCE_BUSY:
        qWarning() << "[QML Video Capture Plugin]" << " " << "VT_RESOURCE_BUSY received";
        break;
    default:
        qWarning() << "[QML Video Capture Plugin]" << " " << "UNKNOWN event received";
        break;
    }
}

void VideoCapture::enabledChanged()
{
    if(isEnabled() == false){
        terminate();
    } else {
        initialize();
    }
}

void VideoCapture::acquireVideoTexture()
{
    VT_OUTPUT_INFO_T output_info;
    QMutexLocker locker(&mutex);

    if (m_isVTAvailable) {
        if (m_initialized) {

            if (nextTextureId != 0 && glIsTexture(nextTextureId) && ((currentTexture && nextTextureId != currentTexture->textureId()) || currentTexture == NULL)) {
                VT_DeleteTexture(context_id, nextTextureId);
            }

            VT_STATUS_T vtStatus = VT_GenerateTexture(resource_id, context_id, &nextTextureId, &output_info);
            if (vtStatus == VT_OK) {
                if (nextTextureId != 0 && glIsTexture(nextTextureId)) {
                    update();
                }
            } else {
                nextTextureId = 0;
            }

            m_isVTAvailable = false;
        }
    }
}

QSGNode * VideoCapture::updatePaintNode(QSGNode * oldNode, UpdatePaintNodeData * updatePaintNodeData)
{
    // recordPaintBegin();

    if(isVisible() == false)
        return oldNode;

    if(isEnabled() == false)
        return oldNode;

    if (!m_initialized) {
        QTimer::singleShot(0, this, SLOT(initialize()));
        return oldNode;
    }

    QSGNode *retSGNode = NULL;
    QSGSimpleTextureNode *simpleTextureNode = NULL;
    QSGTexture *oldTexture = NULL;
    QSGSimpleRectNode *rectNode = NULL;

    if (oldNode) {
        simpleTextureNode = dynamic_cast<QSGSimpleTextureNode*>(oldNode);
        rectNode = dynamic_cast<QSGSimpleRectNode *>(oldNode);
    }

    if (nextTextureId != 0 && glIsTexture(nextTextureId) && ((currentTexture && nextTextureId != currentTexture->textureId()) || currentTexture == NULL)) {
        currentTexture = window()->createTextureFromId(nextTextureId, boundingRect().size().toSize());
    } else if (currentTextureId != 0 && glIsTexture(currentTextureId) && ((currentTexture && currentTextureId != currentTexture->textureId()) || currentTexture == NULL)) {
        qWarning() << "[QML Video Capture Plugin]" << " " << "Next texture is not available!";
        currentTexture = window()->createTextureFromId(currentTextureId, boundingRect().size().toSize());
    }

    if(currentTexture && simpleTextureNode == NULL){
        simpleTextureNode = new QSGSimpleTextureNode();
        simpleTextureNode->setTextureCoordinatesTransform(QSGSimpleTextureNode::MirrorVertically);
        simpleTextureNode->setOwnsTexture(true);
    }

    if (!currentTexture || !simpleTextureNode) {
        if (rectNode) {
            rectNode->setRect(boundingRect());
        } else {
            rectNode = new QSGSimpleRectNode(boundingRect(), Qt::transparent);
            rectNode->material()->setFlag(QSGMaterial::Blending, false);
        }
        rectNode->markDirty(QSGNode::DirtyMaterial);
        qWarning() << "[QML Video Capture Plugin]" << " " << "Use PunchThrough";

        if (simpleTextureNode) {
            delete simpleTextureNode;
        }

        retSGNode = rectNode;
    } else {
        oldTexture = simpleTextureNode->texture();
        if (oldTexture != currentTexture) {
            simpleTextureNode->setTexture(currentTexture);
            simpleTextureNode->markDirty(QSGNode::DirtyMaterial);
            simpleTextureNode->setRect(boundingRect());

            if (nextTextureId != 0 && glIsTexture(nextTextureId) && nextTextureId == currentTexture->textureId()) {
                GLuint tempTextureId = nextTextureId;
                nextTextureId = currentTextureId;
                currentTextureId = tempTextureId;
            }
        }

        if (rectNode) {
            delete rectNode;
        }

        retSGNode = simpleTextureNode;
    }

    // recordPaintEnd();

    return retSGNode;
}

void VideoCapture::itemChange(ItemChange change, const ItemChangeData &)
{
    // The ItemSceneChange event is sent when we are first attached to a window.
    if (change == ItemSceneChange) {
        QQuickWindow *win = window();
        if (!win)   return;

        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
        win->setClearBeforeRendering(false);
    }
}

void VideoCapture::recordPaintBegin()
{
    QTime curTime = QTime::currentTime();
    if((m_paintBeginTime.isNull() == false) && (curTime.second() == m_paintBeginTime.second())){
        m_frameCount++;
    } else {
        if(m_paintBeginTime.isNull() == false){
            qWarning() << "[QML Video Capture Plugin]" << " "
                       << "FPS : " << m_frameCount;
        }
        m_frameCount = 1;
    }

    m_paintBeginTime  = curTime;
}

void VideoCapture::recordPaintEnd()
{
    QTime curTime = QTime::currentTime();
    qWarning() << "[QML Video Capture Plugin]" << " "
               << "Rendering time : " << curTime.msec() - m_paintBeginTime.msec();
}
