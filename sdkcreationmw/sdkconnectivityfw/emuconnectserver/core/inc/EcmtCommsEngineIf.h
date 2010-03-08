/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface of Communications Engine 
*
*/


#ifndef MECMTMCOMMSENGINEIF_H
#define MECMTMCOMMSENGINEIF_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MEcmtCommsWriter;
class MEcmtCommsReader;

/**
*  @ingroup EcmtCore
*/
enum TEcmtCommsObjectStatus
    {
    EReady,
    EInError
    };

/**
*  @ingroup EcmtCore
*/
class MEcmtCommsEngineObserver
    {
    public:
        virtual void HandleCommsEngineNotify( TEcmtCommsObjectStatus aStatus, TInt aCode ) = 0;
    };

/**
*  @ingroup EcmtCore
*/
class MEcmtCommsWriterObserver
    {
    public:
        virtual void HandleCommsWriterNotify( TEcmtCommsObjectStatus aStatus ) = 0;
    };

/**
*  @ingroup EcmtCore
*/
class MEcmtCommsWriter
    {
    public:
        virtual TBool WriterReady( ) const = 0;
        virtual void IssueWrite( const TDesC8& aData ) = 0;
        virtual void SetWriterObserver( MEcmtCommsWriterObserver& aObserver ) = 0;
    };
    
/**
*  @ingroup EcmtCore
*/
class MEcmtCommsReaderObserver
    {
    public:
        virtual void HandleCommsReaderNotify( TEcmtCommsObjectStatus aStatus ) = 0;
    };

/**
*  @ingroup EcmtCore
*/
class MEcmtCommsReader
    {
    public:
        virtual TBool ReaderReady( ) const = 0;
        virtual void IssueRead( TDes8& aData ) = 0;
        virtual void SetReaderObserver( MEcmtCommsReaderObserver& aObserver ) = 0;
    };

#endif
        
// End of File
