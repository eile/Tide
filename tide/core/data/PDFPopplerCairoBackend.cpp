/*********************************************************************/
/* Copyright (c) 2016, EPFL/Blue Brain Project                       */
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

#include <poppler.h> // Must come before any Qt include

#include "PDFPopplerCairoBackend.h"

#include "CairoWrappers.h"

struct PopplerPageDeleter
{
    void operator()( PopplerPage* page ) { g_object_unref( page ); }
};
typedef std::unique_ptr<PopplerPage, PopplerPageDeleter> PopplerPagePtr;

struct PopplerDocDeleter
{
    void operator()( PopplerDocument* doc ) { g_object_unref( doc ); }
};
typedef std::unique_ptr<PopplerDocument, PopplerDocDeleter> PopplerDocumentPtr;

struct PDFPopplerCairoBackend::Impl
{
    PopplerDocumentPtr document;
    PopplerPagePtr page;
};

std::string _getFilepath( const QString& uri )
{
    return QString( "file://" ).append( uri ).toStdString();
}

PDFPopplerCairoBackend::PDFPopplerCairoBackend( const QString& uri )
    : _impl( new Impl )
{
    GError* gerror = nullptr;
    _impl->document.reset( poppler_document_new_from_file(
                               _getFilepath( uri ).c_str(), NULL, &gerror ));
    if( !_impl->document )
        throw std::runtime_error( gerror->message );
    if( !setPage( 0 ))
        throw std::runtime_error( "Could not open first page" );
}

PDFPopplerCairoBackend::~PDFPopplerCairoBackend() {}

QSize PDFPopplerCairoBackend::getSize() const
{
    QSizeF size;
    poppler_page_get_size( _impl->page.get(), &size.rwidth(), &size.rheight( ));
    return size.toSize();
}

int PDFPopplerCairoBackend::getPageCount() const
{
    return poppler_document_get_n_pages( _impl->document.get( ));
}

bool PDFPopplerCairoBackend::setPage( const int pageNumber )
{
    PopplerPagePtr page( poppler_document_get_page( _impl->document.get(),
                                                    pageNumber ));
    if( !page )
        return false;

    _impl->page = std::move( page );
    return true;
}

void _threadSafeRenderPage( PopplerPage* page, cairo_t* context )
{
    // The first call to poppler_page_render() is not thread-safe and concurrent
    // access at this point results in a segfault in liblcms2: cmsGetColorSpace.
    // Only lock the first operation to work around this problem.
    // Observed on Ubuntu 14.04 with Poppler: 0.24.5-2 and liblcms2: 2.5-0.
    static bool _firstRenderingFinished{ false };
    if( !_firstRenderingFinished )
    {
        static std::mutex _mutex;
        const std::lock_guard< std::mutex > lock{ _mutex };
        poppler_page_render( page, context );
        _firstRenderingFinished = true;
    }
    else
        poppler_page_render( page, context );
}

QImage PDFPopplerCairoBackend::renderToImage( const QSize& imageSize,
                                              const QRectF& region ) const
{
    const QSizeF pageSize( getSize( ));

    const qreal zoomX = 1.0 / region.width();
    const qreal zoomY = 1.0 / region.height();

    const qreal resX = imageSize.width() / pageSize.width();
    const qreal resY = imageSize.height() / pageSize.height();

    const QPointF topLeft( region.x() * pageSize.width(),
                           region.y() * pageSize.height( ));

    // For correct rendering of PDF, it is first rendered to a transparent
    // image (all alpha = 0).
    QImage image( imageSize, QImage::Format_ARGB32 );
    image.fill( 0u );

    CairoSurfacePtr surface( cairo_image_surface_create_for_data(
                                 image.bits(), CAIRO_FORMAT_ARGB32,
                                 image.width(), image.height(),
                                 4 * image.width( )));

    CairoPtr context( cairo_create( surface.get( )));

    cairo_scale( context.get(), zoomX * resX, zoomY * resY );
    cairo_translate( context.get(), -topLeft.x(), -topLeft.y( ));

    cairo_save( context.get( ));
    _threadSafeRenderPage( _impl->page.get(), context.get( ));
    cairo_restore( context.get( ));

    // Then the image is painted on top of a white "page".
    // Instead of creating a second image, painting it white, then painting the
    // PDF image over it we can use the CAIRO_OPERATOR_DEST_OVER operator to
    // achieve the same effect with the one image.
    cairo_set_operator( context.get(), CAIRO_OPERATOR_DEST_OVER );
    cairo_set_source_rgb( context.get(), 1, 1, 1 );
    cairo_paint( context.get( ));

    return image;
}
