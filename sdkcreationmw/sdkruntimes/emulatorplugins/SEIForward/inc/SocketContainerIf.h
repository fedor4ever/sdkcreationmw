/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SEI Forward Plugin
*
*/


#ifndef MSOCKETCONTAINERIF_H
#define MSOCKETCONTAINERIF_H

// FORWARD DECLARATIONS

class CSymbianSocket;

/**
* Socket container interface
*/
class MSocketContainer
    {
    public:
		virtual void Remove( CSymbianSocket* aSocket ) = 0;
		virtual void ForwardL( CSymbianSocket* aSocket, TPtrC8 aMessage ) = 0;
    };

#endif
        
// End of File
