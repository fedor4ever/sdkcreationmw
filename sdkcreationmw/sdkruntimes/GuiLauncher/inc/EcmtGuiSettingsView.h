/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  View class for setting list.
*
*/


#ifndef __ECMTGUI_SETTINGSVIEW_H__
#define __ECMTGUI_SETTINGSVIEW_H__


#include <aknview.h>

class CEcmtGuiSettingList;

/**
 *  View class for Setting List.
 *
 *  @since S60 v3.1
 */
class CEcmtGuiSettingsView : public CAknView
{

public:

    /**
     * Two-phased constructor.
     */
    static CEcmtGuiSettingsView* NewL();

public: //New functions
    /**
     * Called by HandleResourceChangeL() from CEcmtGuiAppUi when layout 
     * is changed.
     *
     * @param aType Type of resources that have changed
     */
    void HandleSizeChange( TInt aType );

private:

    /**
     * From CAknView.
     * Returns views id.
     *
     * @return Id for this view.
     */
    TUid Id() const;

    /**
     * From CAknView.
     * Handles commands
     *
     * @param aCommand A command to be handled.
     */
    void HandleCommandL( TInt aCommand );

    /**
     * From CAknView.
     * Activates view
     *
     * @param aPrevViewId Specifies the view previously active.
     * @param aCustomMessageId Specifies the message type.
     * @param aCustomMessage The activation message.
     */
    void DoActivateL( const TVwsViewId& aPrevViewId,
                      TUid aCustomMessageId,
                      const TDesC8& aCustomMessage );

    /**
     * From CAknView.
     * Deactivates view.
     */
    void DoDeactivate();
    
    /**
     * this method gives the IP address of the devicein IPv4 format
     */
 //  void IPv4L(TDes& aIpAdds);

private:

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
    /**
     * C++ default constructor.
     */
    CEcmtGuiSettingsView();
    
    /**
     * Destructor.
     */
    ~CEcmtGuiSettingsView();

private: //Data

    /**
     * Container.
     * Own.
     */
    CEcmtGuiSettingList* iContainer;

    };

#endif // __ECMTGUI_SETTINGSVIEW_H__
