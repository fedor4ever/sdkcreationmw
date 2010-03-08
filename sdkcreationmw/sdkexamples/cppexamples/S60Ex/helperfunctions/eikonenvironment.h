/*
* Copyright (c) 2004, 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __EIKONENVIRONMENT_H__
#define __EIKONENVIRONMENT_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CEikAppUi;
class CEikonEnv;
class CEikDocument;
class CCoeControl;
class RFs;

// CLASS DECLARATION
/**
* NEikonEnvironment
* Useful functions based around the Eikon environment
*/
namespace NEikonEnvironment
    {
        /**
        * EikAppUi()
        * the appui
        * @return a reference to the CEikAppUi
        */
        CEikAppUi& EikAppUi();

        /**
        * EikEnv()
        * the eikon environment
        * @return a reference to the CEikonEnv
        */
        CEikonEnv& EikEnv();

        /**
        * EikDocument()
        * the document
        * @return a reference to CEikDocument
        */
        CEikDocument& EikDocument();

        /**
        * AddToStackL()
        * add a control to the control stack
        * @param aControl the control to add
        */
        void AddToStackL( CCoeControl& aControl );

        /**
        * AddToStackL()
        * add a control to the control stack given a priority and flags
        * @param aControl the control to add
        * @param aPriority the priority
        * @param aStackingFlags the flags
        */
        void AddToStackL( CCoeControl& aControl,
                          TInt aPriority,
                          TInt aStackingFlags );

        /**
        * RemoveFromStack()
        * remove a control from the control stack
        * @param aControl the control to remove
        */
        void RemoveFromStack( CCoeControl& aControl );

        /**
        * AppFullName()
        * the full name of the application
        * @return the name
        */
        TFileName AppFullName();

        /**
        * ApplicationDriveAndPath()
        * the drive and path of the application
        * @return the drive and path
        */
        TFileName ApplicationDriveAndPath();

        /**
        * AddPath()
        * convert a filename to a filename with the drive and
        * path of the application added
        * @return the drive, path and filename
        * @param aFileName the file name to convert
        */
        TFileName AddPath( const TDesC& aFileName );

        /**
        * MessageBox()
        * Display a message box
        * @param the message to display
        */
        void MessageBox( const TDesC&  aMessage );

        /**
        * FlushWindowServer()
        * flush the windows server to ensure all changes to the
        * display are reflected on the screen
        */
        void FlushWindowServer();

        /**
        * TheRFs()
        * the eikon frameworks file server connection
        * @return a reference to the RFs
        */
        RFs& TheRFs();

    }


#endif //__EIKONENVIRONMENT_H__

// End of File
