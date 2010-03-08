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
* Description:  
*
*/


#ifndef __ECMTGUI_APPLICATION_H__
#define __ECMTGUI_APPLICATION_H__

// INCLUDE FILES
#include <aknapp.h>

// CLASS DECLARATIONS

/**
*  @ingroup EcmtGui
*/
class CEcmtGuiApplication : public CAknApplication
    {
	public:
	    TUid AppDllUid() const;

	protected:
	    CApaDocument* CreateDocumentL();
    };

#endif // __ECMTGUI_APPLICATION_H__
