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

#ifndef __W32FILECONNECTION_H__
#define __W32FILECONNECTION_H_

#include <e32base.h>
#include "UrlConnection.h"

class CW32FileReader;

class CW32FileConnection : public MUrlConnection
{
public:
    /**
    */
    CW32FileConnection(WebCore::ResourceHandle* _handle);
    /**
    */
    virtual ~CW32FileConnection();
    /**
    */
    virtual int submit();
    /**
    */
    virtual void cancel();
    /**
    */
    virtual void download(WebCore::ResourceHandle* handle, const WebCore::ResourceRequest& request, const WebCore::ResourceResponse& response);

    virtual int totalContentSize() { return m_maxSize; }
    /**
    */
    void response();
    /**
    */
    void complete(int error);

private:
    /**
    */
    TInt submitL();
    /**
    */
    void responseL();
    /**
    */
    HBufC* parseFileNameL(const TDesC8& aUrl );
    /**
    * Determine the content type of the file.
    */
    HBufC8* contentTypeL();

    /**
    * Determine the content encoding of the file.
    */
    TPtrC8 contentEncoding(const TDesC8& aContentTypeString ) const;
private:
    HBufC* m_fileName;
    int m_maxSize;
    CW32FileReader* m_fileReader;
    int m_chunkIndex;
	HBufC* m_charset;
	HBufC* m_contentType;
};


#endif // __W32FileConnection_H__
// end of file
