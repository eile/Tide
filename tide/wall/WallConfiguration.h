/*********************************************************************/
/* Copyright (c) 2013-2017, EPFL/Blue Brain Project                  */
/*                          Raphael Dumusc <raphael.dumusc@epfl.ch>  */
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

#ifndef WALLCONFIGURATION_H
#define WALLCONFIGURATION_H

#include "Configuration.h"

#include <QPoint>

/**
 * Read the parameters needed to setup a Wall process from an xml file.
 *
 * @warning: this class can only be used AFTER creating a QApplication.
 */
class WallConfiguration : public Configuration
{
public:
    /**
     * Constructor.
     * @param filename \see Configuration
     * @param processIndex MPI index in the range [1;n] of the process
     * @throw std::runtime_error if the file could not be read
     */
    WallConfiguration( const QString& filename, int processIndex );

    /** Get the index of the process. */
    int getProcessIndex() const;

    /** Get the name of the host on which this process is running. */
    const QString& getHost() const;

    /**
     * Get the display identifier in string format matching the current Linux
     * DISPLAY env_var.
     */
    const QString& getDisplay() const;

    /** @return the number of wall processes running on the same host. */
    int getProcessCountForHost() const;

    /**
     * Get the global index for the screen.
     * @return index starting at {0,0} from the top-left
     */
    const QPoint& getGlobalScreenIndex() const;

    /** Get the coordinates of the screen in pixel units. */
    const QPoint& getWindowPos() const;

    /** @return the stereo mode for this process. */
    deflect::View getStereoMode() const;

private:
    const int _processIndex;

    QString _host;
    QString _display;
    int _processCountForHost = 0;

    QPoint _screenPosition;
    QPoint _screenGlobalIndex;

    deflect::View _stereoMode = deflect::View::mono;

    void _loadWallSettings( int processIndex );
};

#endif
