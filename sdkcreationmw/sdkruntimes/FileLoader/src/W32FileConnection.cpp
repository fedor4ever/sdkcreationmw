/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#include <emulator.h>
#include <uri16.h>
#include <apmrec.h>
#include <apgcli.h>
#include "ResourceHandle.h"
#include "W32FileConnection.h"
#include "W32FileReader.h"
#include "ResourceHandleManagerSymbian.h"
#include "StaticObjectsContainer.h"
#include "ResourceRequest.h"
#include <BrCtlSpecialLoadObserver.h>
#include "brctl.h"
#include "DeprecatedString.h"
HANDLE   iFh;
const TInt KFileReadChunkSize = 23920; // bytes
_LIT8( KResLoaderLatin1, "iso-8859-1" );
_LIT8( KResLoaderUCS2, "iso-10646-ucs-2" );
_LIT8( KResLoaderWapWmlc, "application/vnd.wap.wmlc" );


static int s_fileTransactionsCount = 0;

using namespace WebCore;

CW32FileConnection::CW32FileConnection(ResourceHandle* _handle) : MUrlConnection(_handle)
{
    m_fileName = 0;
    m_fileReader = 0;
    s_fileTransactionsCount++;
    m_chunkIndex = 0;
    m_charset = 0;
    m_contentType = 0;
}

CW32FileConnection::~CW32FileConnection()
{
    ::CloseHandle(iFh);
    s_fileTransactionsCount--;
    delete m_fileName;
    delete m_fileReader;
    delete m_charset;
    delete m_contentType;
//    m_file.Close();
}

TInt CW32FileConnection::submit()
{
    TRAPD(error, submitL());
    return error;
}

//========================================================================
//  CW32FileConnection::submitL
//========================================================================
TInt CW32FileConnection::submitL()
{
    TInt error(KErrNone);
    TPtrC8 urlPtr( m_handle->request().url().des() );
    m_fileName = parseFileNameL( urlPtr );
    if (m_fileName) {
    iFh = Emulator::CreateFile(m_fileName->Des().PtrZ(), GENERIC_READ,
                           FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

    if (iFh == INVALID_HANDLE_VALUE) {
      error = Emulator::LastError();
    }
    else {
        TUint32  fileSize = ::GetFileSize(iFh, NULL); //W32 call
        m_maxSize = fileSize;
    if (m_fileReader == NULL) {
        m_fileReader = CW32FileReader::NewL(iFh, KFileReadChunkSize, this);
        }
    m_fileReader->StartReading();
    }
  }
  else {
    error = KErrArgument;
  }

    User::LeaveIfError(error);
}


// -----------------------------------------------------------------------------
// CW32FileConnection::parseFileNameL
// Translate the file name from a URL to a valid file name in the system.
// -----------------------------------------------------------------------------
//
HBufC* CW32FileConnection::parseFileNameL(const TDesC8& aUrl )
{
    //To convert TPtrC8 to Tptr16
	TBufC8<200> Urlbuf(aUrl);
    HBufC16* buf16 = HBufC16::NewL(256);//delete buf16 dont forget.
    TPtr16 bufptr16(buf16->Des());
    bufptr16.Copy(Urlbuf.Des());
    TInt status;
    TUriParser parser;
    HBufC* fileName = NULL;
    status = parser.Parse( bufptr16 );
    if( status == KErrNone ) {
        // this must be the drive letter
        TPtrC host = parser.Extract( EUriHost );
        TPtrC path = parser.Extract( EUriPath );
        // add missing ":"
        fileName = HBufC::NewL( (host.Length()?host.Length()+1:0) + path.Length()+ 1/*0 terminated*/ );
        TPtr fileNamePtr( fileName->Des() );
        if( host.Length() == 1 ) {
            // fix c to c:
            fileNamePtr.Copy( host );
            fileNamePtr.Append( _L(":") );
        }
        else {
            // according to symbian uri parser in case of
            // file:///c:\foo.html
            // host: /c:\foo.html
            // path: /c:\foo.html
            // remove: / from the begenning of the path
            if( path.Length() > 2 && path[0] == '/' && path[2] == ':' ) {
                // move pointer from "/c:\" to "c:\"
                path.Set( path.Mid( 1 ) );
         
            }
        }
        fileNamePtr.Append( path );
        // fix slashes
        for( TInt i = 0; i < fileNamePtr.Length(); i++ ) {
            if( fileNamePtr[ i ] == '/' ) {
                fileNamePtr[ i ] = '\\';
            }
        }
    // then replace possible %20-sequences with blank
    TInt pos = 0;
    _LIT(KWinSP, "%20");
    _LIT(KSpace, " ");
    TInt len = (&KWinSP)->Length();
    while ((pos = fileNamePtr.FindC(KWinSP)) != KErrNotFound) {
        fileNamePtr.Replace(pos, (&KWinSP)->Length(), KSpace);
    }
//    return fileName16;
//    CleanupStack::PushL( fileName16 );
      delete buf16;
      return fileName;
}
}
void CW32FileConnection::cancel()
{
    if( m_fileReader ) {
        m_fileReader->StopReading();
    }
}

void CW32FileConnection::download(WebCore::ResourceHandle* handle,
                              const WebCore::ResourceRequest& request,
                              const WebCore::ResourceResponse& response)
{
    // stop reading and close the file
    m_fileReader->StopReading();
    m_cancelled = true;
    ::CloseHandle(iFh); //windows call
    // send data to the host application
    MBrCtlSpecialLoadObserver* loadObserver = StaticObjectsContainer::instance()->brctl()->brCtlSpecialLoadObserver();
    if(loadObserver) {
        RArray<TUint> typeArray;
        CDesCArrayFlat* desArray = new (ELeave) CDesCArrayFlat(4);
        CleanupStack::PushL(desArray);
        //
        HBufC* url = HBufC::NewLC(handle->request().url().url().length());
        url->Des().Copy(handle->request().url().des());
        typeArray.Append(EParamRequestUrl);
        desArray->AppendL(*url);

        typeArray.Append(EParamCharset);
        desArray->AppendL(*m_charset);

        typeArray.Append(EParamReceivedContentType);
        desArray->AppendL(*m_contentType);

        typeArray.Append(EParamLocalFileName);
        desArray->AppendL(*m_fileName);
        //
        loadObserver->HandleDownloadL(&typeArray, desArray);
        //
        typeArray.Close();
        desArray->Reset();
        CleanupStack::PopAndDestroy(2); // desArray, url 
    }
}

// -----------------------------------------------------------------------------
// CW32FileConnection::contentTypeL
// Determine the content type of the file.
// -----------------------------------------------------------------------------
HBufC8* CW32FileConnection::contentTypeL()
{
    TPtrC8 contentTypePtr;
    HBufC8* contentType = NULL;
    TDataRecognitionResult dataType;
    RApaLsSession apaSession;
    TInt ret;
    
    User::LeaveIfError( apaSession.Connect() );
    // Ask the application architecture to find the file type
    TPtrC8 chunkPtr;
    
    m_fileReader->GetChunkBuffer( chunkPtr );
    ret = apaSession.RecognizeData( m_fileName->Des(), chunkPtr, dataType );
    apaSession.Close();
    
    if( ret == KErrNone &&
        ( dataType.iConfidence == CApaDataRecognizerType::ECertain ) ||
        ( dataType.iConfidence == CApaDataRecognizerType::EProbable ) ) {
        // If the file type was found, try to match it to a known file type
        contentTypePtr.Set( dataType.iDataType.Des8() );
    }
    else {
        // extensions
        _LIT( KCssExt, ".css" );
        _LIT(KWbmpExt, ".wbmp");
        _LIT(KEcmaScriptExt, ".es");
        _LIT(KJavaScriptExt, ".js");
        
        TPtrC extPtr( m_fileName->Right( m_fileName->Length() -  m_fileName->LocateReverse('.') ) );
        
        if( extPtr.CompareF( KCssExt() ) == 0 ) {
            contentTypePtr.Set( _L8( "text/css" ) );
        }
        else if( extPtr.CompareF( KWbmpExt() ) == 0 ) {
            contentTypePtr.Set( _L8( "image/vnd.wap.wbmp" ) );
        }
        else if( extPtr.CompareF( KEcmaScriptExt() ) == 0 ||
            extPtr.CompareF( KJavaScriptExt() ) == 0 ) {
            contentTypePtr.Set( _L8( "text/ecmascript" ) );
        }

    }
    if( contentTypePtr.Length() ) {
        contentType = HBufC8::NewL( contentTypePtr.Length() );
        contentType->Des().Copy( contentTypePtr );
    }
    return contentType;
}

// -----------------------------------------------------------------------------
// CW32FileConnection::contentEncoding
// Determine the content encoding of the file.
// -----------------------------------------------------------------------------
//
TPtrC8 CW32FileConnection::contentEncoding(const TDesC8& aContentTypeString ) const
{
    // Assume Latin-1 for xhtml and html. ucs2 for any other
    TPtrC8 charset( KResLoaderLatin1 );

    TPtrC8 httpAppString( KResLoaderWapWmlc );
    TPtrC8 contentString( aContentTypeString );

    // Is the contentType a HTTP_application_vnd_wap_wmlc_string
    TUint index = contentString.FindF( httpAppString );
    if( index == 0 ) {
        // This is a HTTP_application_vnd_wap_wmlc_string
        charset.Set( KResLoaderUCS2 );
    }
    return charset;
}

void CW32FileConnection::response()
{
    TRAPD(error, responseL());
    if (error) {
        complete(error);
    }        
}

void CW32FileConnection::responseL()
{
    if (m_chunkIndex == 0 ) {
        HBufC8* contentType = contentTypeL();

        if( contentType ) {
            m_contentType = HBufC::NewL(contentType->Length());
            m_contentType->Des().Copy(*contentType);

            TPtrC8 contentEncodingPtr( contentEncoding( contentType->Des() ) );
            ResourceResponse response(m_handle->request().url().des(), contentType->Des(), m_maxSize, contentEncodingPtr, String() );


            const TDesC& charset = response.textEncodingName().des();
            m_charset = charset.AllocL();
            response.setHTTPStatusCode(200);

            CResourceHandleManager::self()->receivedResponse(m_handle, response, this);
        }
    }
    // If it is not Browser content, reading the file is canceled in CW32FileConnection::download().
    if (!m_cancelled) {
        TPtrC8 chunkPtr;
        m_fileReader->GetChunkBuffer( chunkPtr );    
        m_chunkIndex++;
        CResourceHandleManager::self()->receivedData(m_handle, chunkPtr, m_maxSize, this);
    }
}

void CW32FileConnection::complete(int error)
{
    CResourceHandleManager::self()->receivedFinished(m_handle, error, this);
    derefHandle();
}


// end of file
