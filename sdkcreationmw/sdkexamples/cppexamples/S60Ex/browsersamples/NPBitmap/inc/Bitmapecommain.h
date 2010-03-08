/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Example for developing a plugin
*
*/


#ifndef BITMAPECOMMAIN_H
#define BITMAPECOMMAIN_H

// INCLUDES

#include <e32std.h>
#include <npupp.h>
#include "bitmapplugin.h"
#include "CEcomBrowserPluginInterface.h"

// DATA TYPES

typedef MPluginAdapter *MPluginAdapterPTR;
typedef CCoeControl *CCoeControlPTR;

// CLASS DECLARATION

/**
*  CBitmapEcomMain class.
*/
class CBitmapEcomMain : public CEcomBrowserPluginInterface
    {
    public:
    	/**
        * Two-phased constructor.
        */
        static CBitmapEcomMain* NewL (TAny* aInitParam);

		/**
        * Destructor.
        */
        ~CBitmapEcomMain();

		/**
        * Accessor method for retrieving iNPNfunctions
        */
        NPNetscapeFuncs* Funcs ( void ) { return iNPNfunctions; }

    private:
    
    	/**
    	* Private constuctor
    	*/
        CBitmapEcomMain ( NPNetscapeFuncs* aNPNfunctions );
		
		/**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL (NPPluginFuncs* aPluginFuncs );

        NPNetscapeFuncs* iNPNfunctions;
    };


#endif // BITMAPECOMMAIN_H

// End of File

