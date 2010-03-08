/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __ECMTGUI_APPUI_H__
#define __ECMTGUI_APPUI_H__

// INCLUDE FILES
#include <aknviewappui.h>
#include "EcmtClient.h"
#include "EcmtServerObserver.h"
#include <aknnotewrappers.h>
/**
 *  @defgroup EcmtGUi Gui
 *  The Gui component is used in the device to show to the user selected
 *  information on the status of the Ecmt Framework. It also launches the
 *  Ecmt Server process using the REcmt class.
 */

// FORWARD DECLARATIONS
class CEcmtGuiLogView;
class CEcmtGuiSettingsView;
class CWlanOddSettingView;

//Define this if you want to enable settings under emulator environment
//#define _ENABLE_SETINGS_ON_WINS_
// CLASS DECLARATION

/**
 *  @ingroup EcmtGui
 */
class CEcmtGuiAppUi : public CAknViewAppUi, public MEcmtServerObserver
{


public:
    void ConstructL();
    CEcmtGuiAppUi();
    ~CEcmtGuiAppUi();

public:
    void HandleCommandL(TInt aCommand);
    void HandleResourceChangeL(TInt aType);

		/**
     * From MEcmtServerObserver. Handles the notification messages
     * received from Ecmt server and displays them in the view.
     * @param aMsg Notification message from Ecmt server.
     * @param aError Completion code of message reception from server.
     * Either KErrNone if notification message was received succesfully
     * or one of the system wide error codes.
     */
    void HandleEcmtServerNotification( const TDesC& aMsg, TInt aError );

private:
    /**
     * Connects to Ecmt using REcmt::Connect() and displays
     * status on view.
     */
    void ConnectEcmtL();

    /**
     * Disconnects from Ecmt using REcmt::Close() and displays
     * status on view.
     */
    void DisconnectEcmt();

private:
    CEcmtGuiLogView* iLogView;
    CEcmtGuiSettingsView* iSettingsView;
    /**
     * pointer to CWlanOddSettingView. used to create new view for wlan odd application
     */
    CWlanOddSettingView* iWlanOdd;
    
    REcmt iEcmt;
    TBool iConnected; 
		enum { KMaxLogMsgLen = 128 };

};

#endif // __ECMTGUI_APPUI_H__

