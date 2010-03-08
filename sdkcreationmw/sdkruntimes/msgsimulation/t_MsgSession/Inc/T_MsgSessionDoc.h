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

#ifndef __T_MSGSESSIONDOCUMENT_H__
#define __T_MSGSESSIONDOCUMENT_H__


//  Include Files

#include <eikdoc.h>     // CEikDocument


//  Class Definitions

class CT_MsgSessionDocument : public CEikDocument 
    {
public:     // new functions
    CT_MsgSessionDocument(CEikApplication& aApp);

protected:  // from CEikDocument
    CEikAppUi* CreateAppUiL();
    };

#endif  // __T_MSGSESSIONDOCUMENT_H__

// End of file
