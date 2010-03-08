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


#ifndef HEADER_DEBUG_AGENT_UI_APPLICATION
#define HEADER_DEBUG_AGENT_UI_APPLICATION

#include <eikapp.h>

/**
Application class.
Use by framework to construct application.
Creates document.
*/
class CDebugAgentApplication : public CEikApplication 
    {
private:
    CApaDocument* CreateDocumentL();
    TUid AppDllUid() const;
    };

#endif
