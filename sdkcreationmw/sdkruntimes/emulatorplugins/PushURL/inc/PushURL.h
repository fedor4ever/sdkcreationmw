/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef _PUSHURLPLUGIN_H_
#define _PUSHURLPLUGIN_H_

#include <e32base.h>
#include "EcmtPlugin.h"
#include "EcmtMessagingIf.h"

class CDocumentHandler;

class CPushURL : public CBase,
                 public MEcmtPlugin,
                 public MEcmtMessageEventObserver
{
  public:
    virtual ~CPushURL ();
    static  MEcmtPlugin*  NewL ();
    /**
     * From MEcmtPlugin
     */
    void    BindMessagingL (MEcmtMessaging* aMessaging);
    
    /**
     * From MecmtMessageEventObserver
     */
    void    HandleNotifyL (const CEcmtMessageEvent& aEvent);

  private:
    CPushURL ();
    void LoadUrlL (const TDesC&  aUrl, const TUid&  browser_uid);
    void LoadFileToDocHandlerL (const TDesC&  aUrl, TBool aPrefix);
    
    TUid  iUid;
    MEcmtMessaging*   iMessaging;
    CDocumentHandler* iDocHandler;
};


#endif /* _PUSHURLPLUGIN_H_ */



