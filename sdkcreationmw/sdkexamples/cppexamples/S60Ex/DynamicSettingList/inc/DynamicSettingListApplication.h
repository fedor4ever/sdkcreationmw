/*
* Copyright (c) 2005 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __DYNAMICSETTINGLIST_APPLICATION_H__
#define __DYNAMICSETTINGLIST_APPLICATION_H__

// INCLUDE FILES
#include <aknapp.h>


// CLASS DECLARATION

/**
* CDynamicSettingListApplication application class.
* Provides factory to create concrete document object.
* 
*/
class CDynamicSettingListApplication : public CAknApplication
    {
private:  // Functions from base classes

    /**
    * From CApaApplication, returns application's UID (KUidDynamicSettingListApp).
    * @return The value of KUidDynamicSettingListApp.
    */
	TUid AppDllUid() const;


	/**
	* From CApaApplication, creates CDynamicSettingListDocument document object.
	* @return A pointer to the created document object.
	*/
    CApaDocument* CreateDocumentL();
    };

#endif // __DYNAMICSETTINGLIST_APPLICATION_H__

// End of file