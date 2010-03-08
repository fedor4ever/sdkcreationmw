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


#ifndef __T_MSGSESSIONAPPUI_H___
#define __T_MSGSESSIONAPPUI_H__


//  Include Files

#include <eikappui.h>       // CEikAppUi


//  Forward Declarations

class CT_MsgSessionView;
class CTest;


//  Class Definitions

class CT_MsgSessionAppUi : public CEikAppUi
    {
public:     
    // new functions
    CT_MsgSessionAppUi();
    void ConstructL();
    ~CT_MsgSessionAppUi();

protected:  
    // from CEikAppui
    void HandleCommandL(TInt aCommand);

private:    
    // new functions
    void HandleFileNewL();
    void HandleFileSaveL();

private:    
    // data
    CT_MsgSessionView*    iView;
	CTest *iTest;
    };

#endif  // __T_MSGSESSIONAPPUI_H__

// End of file
