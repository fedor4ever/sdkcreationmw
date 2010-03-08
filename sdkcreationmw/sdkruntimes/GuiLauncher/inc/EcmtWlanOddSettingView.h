/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  this file is used for wlan ODD appliction
*
*/


#ifndef __LISTBOXSETTINGVIEW_H__
#define __LISTBOXSETTINGVIEW_H__


#include <aknview.h>
/*
 * forward declaration for CWlanSettingList
 */
class CWlanSettingList;


class CWlanOddSettingView : public CAknView
    {

public:
    /**
     * creates object for CWlanOddSettingView
     */
    static CWlanOddSettingView* NewL();
    
    /**
     * on return aIpAdds contains ip address of the device or emulator in IPv4 format
     */
    void IPv4L(TDes& aIpAdds);
    
    /**
     * destructor
     */
    ~CWlanOddSettingView();
    
public: //New functions
    void HandleSizeChange( TInt aType );

private:
    /**
     * returns view id
     */
    TUid Id() const;
    
    /** 
     * handles the command on wlan odd view
     */
    void HandleCommandL( TInt aCommand );
    void DoActivateL( const TVwsViewId& aPrevViewId,
                      TUid aCustomMessageId,
                      const TDesC8& aCustomMessage );
    void DoDeactivate();
    
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

private:
    void ConstructL();
    
    CWlanOddSettingView();
    
    void DelteJavaDirL();
    
    

private: //Data
    /**
     * to create CWlanSettingList object
     */
    CWlanSettingList* iContainer;

    };

#endif // __LISTBOXSETTINGVIEW_H__
