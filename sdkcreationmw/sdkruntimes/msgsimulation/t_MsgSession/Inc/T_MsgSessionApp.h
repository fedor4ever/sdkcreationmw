/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_MSGSESSIONAPPLICATION_H__
#define __T_MSGSESSIONAPPLICATION_H__

//  Include Files

#include <eikapp.h>     // CEikApplication

//  Constants

const TUid KUidT_MsgSessionApp = { 0x101f8c30 };


//  Forward Declarations

class CT_MsgSessionDocument;


//  Class Definitions

class CT_MsgSessionApplication : public CEikApplication
    {
private:    
     // from CEikApplication
     CApaDocument* CreateDocumentL();
     TUid AppDllUid() const;
    };

#endif  // __T_MSGSESSIONAPPLICATION_H__

// End of file
