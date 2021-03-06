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

#ifndef COMMANDLINEOPTIONS_H
#define COMMANDLINEOPTIONS_H

#include "CommandLineParser.h"
#include "PixelStreamerType.h"

/**
 * Command line options to pass startup parameters to a local application.
 */
class CommandLineOptions : public CommandLineParser
{
public:
    /** Construct an empty instance. */
    CommandLineOptions();

    /** Construct from command line parameters. */
    CommandLineOptions( int argc, char** argv );

    /** @copydoc CommandLineParser::parse */
    void parse( int argc, char** argv ) final;

    /** @copydoc CommandLineParser::showSyntax */
    void showSyntax( const std::string& appName ) const final;

    /** Get the arguments joined in command line format. */
    QString getCommandLine() const;

    /** Get the command line arguments in list format. */
    QStringList getCommandLineArguments() const;

    /** @name Getters */
    //@{
    PixelStreamerType getPixelStreamerType() const;
    const QString& getUrl() const;
    const QString& getStreamId() const;
    unsigned int getWidth() const;
    unsigned int getHeight() const;
    const QString& getConfiguration() const;
    //@}

    /** @name Setters */
    //@{
    void setPixelStreamerType( PixelStreamerType type );
    void setUrl( const QString& url );
    void setStreamId( const QString& id );
    void setWidth( unsigned int width );
    void setHeight( unsigned int height );
    void setConfiguration( const QString& file );
    //@}

private:
    PixelStreamerType _streamerType = PS_UNKNOWN;
    QString _url;
    QString _streamId;
    uint _width = 0;
    uint _height = 0;
    QString _configuration;

    void _fillDesc();
};

#endif
