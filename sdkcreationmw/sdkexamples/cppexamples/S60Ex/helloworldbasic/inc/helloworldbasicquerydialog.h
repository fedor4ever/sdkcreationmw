/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef HELLOWORLDQUERYDIALOG_H
#define HELLOWORLDQUERYDIALOG_H


//  INCLUDES
#include <aknquerydialog.h>

// CLASS DECLARATION

/**
*  CHelloWorldQueryDialog class.
*  This class is used to query the user for text information
*/
class CHelloWorldQueryDialog : public CAknTextQueryDialog
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @param aBuf buffer to store the input
        * @param aDefInput default input for the dialog
        */
        CHelloWorldQueryDialog( TDes& aBuf, HBufC *aDefInput );

        /**
        * Destructor.
        */
        virtual ~CHelloWorldQueryDialog() {};

    private:    // from CEikDialog

        /**
        * sets the default value to the dialog.
        */
        void  PreLayoutDynInitL();

    private:    // Data

       /**
        * the default input for the dialog
        */
        HBufC&  iDefInput;
    };

#endif // HELLOWORLDQUERYDIALOG_H

// End of File

