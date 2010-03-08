/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Windows specific class and utilities
*
*/


#ifndef _MECMTMENUBARCALLBACKS_H_
#define _MECMTMENUBARCALLBACKS_H_

/**
 * Defines callbacks invoked by EcmtMenubar when various events occur. 
 * The callbacks are invoked in context of a Symbian thread
 */
class MEcmtMenubarCallbacks
{
public:
    virtual void EcmtServerListening(const unsigned short* params) = 0;
    virtual void EcmtServerConnected() = 0;
    virtual void EcmtServerDisconnected() = 0;
};
    
#endif	// _MECMTMENUBARCALLBACKS_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
