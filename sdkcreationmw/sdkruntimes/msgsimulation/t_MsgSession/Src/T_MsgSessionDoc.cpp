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


//  Include Files 

#include "T_MsgSessionDoc.h"      // CT_MsgSessionDocument
#include "T_MsgSessionAppUi.h"    // CT_MsgSessionAppUi

/****************************************************************************
*   CT_MsgSessionDocument
*****************************************************************************
*
*
*
****************************************************************************/
CT_MsgSessionDocument::CT_MsgSessionDocument(CEikApplication& aApp) : CEikDocument(aApp)
    {
    }

CEikAppUi* CT_MsgSessionDocument::CreateAppUiL()
    {
    return new (ELeave) CT_MsgSessionAppUi;
    }
// End of file
