/*********************************************************************/
/* Copyright (c) 2013-2016, EPFL/Blue Brain Project                  */
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

#include "PixelStreamerLauncher.h"

#include "log.h"
#include "CommandLineOptions.h"
#include "PixelStreamWindowManager.h"
#include "MasterConfiguration.h"

#include <QCoreApplication>
#include <QDir>

namespace
{
#ifdef _WIN32
const QString LOCALSTREAMER_BIN( "localstreamer.exe" );
const QString LAUNCHER_BIN( "tideLauncher.exe" );
#else
const QString LOCALSTREAMER_BIN( "localstreamer" );
const QString LAUNCHER_BIN( "tideLauncher" );
#endif

const QSize WEBBROWSER_DEFAULT_SIZE( 1280, 1024 );
}

const QString PixelStreamerLauncher::launcherUri = QString( "Launcher" );

PixelStreamerLauncher::PixelStreamerLauncher( PixelStreamWindowManager& windowManager,
                                              const MasterConfiguration& config )
    : _windowManager( windowManager )
    , _config( config )
{
    connect( &_windowManager, SIGNAL( pixelStreamWindowClosed( QString )),
             this, SLOT( _dereferenceLocalStreamer( QString )),
             Qt::QueuedConnection );
}

void PixelStreamerLauncher::openWebBrowser( const QPointF pos, const QSize size,
                                            const QString url )
{
    static int webbrowserCounter = 0;
    const QString& uri = QString( "WebBrowser_%1" ).arg( webbrowserCounter++ );

    const QSize viewportSize = !size.isEmpty() ? size : WEBBROWSER_DEFAULT_SIZE;
    _windowManager.openWindow( uri, pos, viewportSize, true );

    CommandLineOptions options;
    options.setPixelStreamerType( PS_WEBKIT );
    options.setStreamname( uri );
    options.setUrl( url );
    options.setWidth( viewportSize.width( ));
    options.setHeight( viewportSize.height( ));

    _processes.insert( uri );
    const QString command = _getLocalStreamerBin() + QString( ' ' ) +
                            options.getCommandLine();
    emit start( command, QDir::currentPath(), QStringList( ));
}

void PixelStreamerLauncher::openLauncher()
{
    const QString& uri = launcherUri;
    if( _processes.count( uri ))
    {
        _windowManager.showWindow( uri );
        return;
    }

    const qreal width = 0.25 * _config.getTotalWidth();
    const QSize launcherSize( width, 0.75 * width );
    const qreal x = 0.25 * _config.getTotalWidth();
    const qreal y = 0.35 * _config.getTotalHeight();
    const QPointF centerPos( x, y );

    _windowManager.openWindow( uri, centerPos, launcherSize );
    _processes.insert( uri );

    CommandLineOptions options;
    options.setStreamname( launcherUri );
    options.setConfiguration( _config.getFilename( ));
    options.setWidth( launcherSize.width( ));
    options.setHeight( launcherSize.height( ));

    QStringList env;
    if( !_config.getLauncherDisplay().isEmpty( ))
        env.append( QString("DISPLAY=%1").arg( _config.getLauncherDisplay( )));

    const QString command = _getLauncherBin() + QString( ' ' ) +
                            options.getCommandLine();
    emit start( command, QDir::currentPath(), env );
}

void PixelStreamerLauncher::hideLauncher()
{
    _windowManager.hideWindow( launcherUri );
}

void PixelStreamerLauncher::_dereferenceLocalStreamer( const QString uri )
{
    _processes.erase( uri );
}

QString PixelStreamerLauncher::_getLocalStreamerBin() const
{
    const QString& appDir = QCoreApplication::applicationDirPath();
    return QString( "%1/%2" ).arg( appDir, LOCALSTREAMER_BIN );
}

QString PixelStreamerLauncher::_getLauncherBin() const
{
    const QString& appDir = QCoreApplication::applicationDirPath();
    return QString( "%1/%2" ).arg( appDir, LAUNCHER_BIN );
}
