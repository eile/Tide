/*********************************************************************/
/* Copyright (c) 2014, EPFL/Blue Brain Project                       */
/*                     Raphael Dumusc <raphael.dumusc@epfl.ch>       */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/*   1. Redistributions of source code must retain the above         */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer.                                                  */
/*                                                                   */
/*   2. Redistributions in binary form must reproduce the above      */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer in the documentation and/or other materials       */
/*      provided with the distribution.                              */
/*                                                                   */
/*    THIS  SOFTWARE IS PROVIDED  BY THE  UNIVERSITY OF  TEXAS AT    */
/*    AUSTIN  ``AS IS''  AND ANY  EXPRESS OR  IMPLIED WARRANTIES,    */
/*    INCLUDING, BUT  NOT LIMITED  TO, THE IMPLIED  WARRANTIES OF    */
/*    MERCHANTABILITY  AND FITNESS FOR  A PARTICULAR  PURPOSE ARE    */
/*    DISCLAIMED.  IN  NO EVENT SHALL THE UNIVERSITY  OF TEXAS AT    */
/*    AUSTIN OR CONTRIBUTORS BE  LIABLE FOR ANY DIRECT, INDIRECT,    */
/*    INCIDENTAL,  SPECIAL, EXEMPLARY,  OR  CONSEQUENTIAL DAMAGES    */
/*    (INCLUDING, BUT  NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE    */
/*    GOODS  OR  SERVICES; LOSS  OF  USE,  DATA,  OR PROFITS;  OR    */
/*    BUSINESS INTERRUPTION) HOWEVER CAUSED  AND ON ANY THEORY OF    */
/*    LIABILITY, WHETHER  IN CONTRACT, STRICT  LIABILITY, OR TORT    */
/*    (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY OUT    */
/*    OF  THE  USE OF  THIS  SOFTWARE,  EVEN  IF ADVISED  OF  THE    */
/*    POSSIBILITY OF SUCH DAMAGE.                                    */
/*                                                                   */
/* The views and conclusions contained in the software and           */
/* documentation are those of the authors and should not be          */
/* interpreted as representing official policies, either expressed   */
/* or implied, of Ecole polytechnique federale de Lausanne.          */
/*********************************************************************/

#ifndef DISPLAYGROUPRENDERER_H
#define DISPLAYGROUPRENDERER_H

#include "types.h"

#include "QmlWindowRenderer.h"

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QUuid>

class QQuickItem;

/**
 * Renders a DisplayGroup.
 */
class DisplayGroupRenderer : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor.
     * @param parentWindow the window to render the scene contents to
     * @param provider the provider of data for the windows
     * @param screenRect the rectangle this renderers is rendering wrt global
     *                   screen setup
     */
    DisplayGroupRenderer( WallWindow& parentWindow, DataProvider& provider,
                          const QRect& screenRect );

    /** Set different options used for rendering. */
    void setRenderingOptions( OptionsPtr options );

    /** Set different touchpoint's markers. */
    void setMarkers( MarkersPtr markers );

    /** Update the contents, using the channel to synchronize processes. */
    void synchronize( WallToWallChannel& channel );

    /** @return true if the renderer requires a redraw. */
    bool needRedraw() const;

public slots:
    /** Set the DisplayGroup to render, replacing the previous one. */
    void setDisplayGroup( DisplayGroupPtr displayGroup );

    /** Increment number of rendered/swapped frames for FPS display. */
    void updateRenderedFrames();

private:
    Q_DISABLE_COPY( DisplayGroupRenderer )

    QQmlEngine& _engine;
    DataProvider& _provider;
    DisplayGroupPtr _displayGroup;
    QQuickItem* _displayGroupItem;

    using QmlWindowPtr = std::shared_ptr<QmlWindowRenderer>;
    QMap<QUuid, QmlWindowPtr> _windowItems;
    QmlWindowPtr _backgroundWindowItem;

    OptionsPtr _options;
    MarkersPtr _markers;

    const QRect _screenRect;

    void _createDisplayGroupQmlItem( QQuickItem& parentItem );
    void _createWindowQmlItem( ContentWindowPtr window );
    bool _hasBackgroundChanged( const QString& newUri ) const;
    void _setBackground( ContentPtr backgroundContent );
    void _adjustBackgroundTo( const DisplayGroup& displayGroup );
};

#endif
