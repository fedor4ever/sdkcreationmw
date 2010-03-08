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

#include <EcmtGui.rsg>

#include "EcmtWlanOddSettingView.h"
#include "EcmtWlanOddSettingList.h"
#include "EcmtGui.hrh"
#include "EcmtGuiAppUi.h"
#include <aknnotewrappers.h> 
#include <ES_SOCK.H>
#include <BAUTILS.H>
#include <aknmessagequerydialog.h> //for ip address message query

CWlanOddSettingView* CWlanOddSettingView::NewL()
    {
    CWlanOddSettingView* self = new (ELeave) CWlanOddSettingView;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CWlanOddSettingView::CWlanOddSettingView()
    {
    }

CWlanOddSettingView::~CWlanOddSettingView()
	{
	if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        }     
	}


TUid CWlanOddSettingView::Id() const 
    { 
    return TUid::Uid( EListboxSettingView ); 
    }


void CWlanOddSettingView::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EListboxCmdChange:
            iContainer->EditItemL( 0, ETrue );
            break;
        case EIp:
            {
            TBuf<20> ipAdds;
            TRAPD(err,IPv4L(ipAdds));
            if(KErrNone == err)
                {
                HBufC* buf;
                buf = ipAdds.AllocL();
                CleanupStack::PushL(buf);
                CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(*buf);
                dlg->PrepareLC(R_IP_ADDRESS_DIALOG);
                dlg->RunLD();
                CleanupStack::PopAndDestroy(buf);
                }
             }
             break;
        case EDeleteJavaDir:
            {
            User::InfoPrint(_L("deleting java dir"));
            DelteJavaDirL();
            }
            break;
        case EAknSoftkeyBack:
            AppUi()->ActivateLocalViewL( TUid::Uid( EEcmtLogViewId ) );               
            break;
        default:
            AppUi()->HandleCommandL(aCommand);  
            break;
        }
    }


void CWlanOddSettingView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                      TUid /*aCustomMessageId*/,
                      const TDesC8& /*aCustomMessage*/ )
    {    
    iContainer = CWlanSettingList::NewL();
    
    iContainer->SetMopParent( this );
    iContainer->ConstructFromResourceL( R_LISTBOX_SETTING_ITEM_LIST );
    AppUi()->AddToStackL( *this, iContainer );
    
    iContainer->MakeVisible( ETrue );
    iContainer->SetRect( ClientRect() );
    iContainer->ActivateL();
    }

void CWlanOddSettingView::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }    
    }

void CWlanOddSettingView::ConstructL()
    {
    BaseConstructL(R_LISTBOX_SETTINGS_VIEW);    
    }

void CWlanOddSettingView::DynInitMenuPaneL( TInt aResourceId, 
                                           CEikMenuPane* aMenuPane )
    {

    }   


void CWlanOddSettingView::HandleSizeChange( TInt aType )
    {
    if( iContainer )
        {
        iContainer->HandleResourceChange( aType );
        
        if( aType==KEikDynamicLayoutVariantSwitch )
            {        
            TRect rect;
            AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
            iContainer->SetRect(rect);
            }
        }         
    } 
//this function is used to get the ip address of the device
void CWlanOddSettingView::IPv4L(TDes& aIpAdds)
    {
    TBuf<20> deviPAdds;
    RSocketServ socketServ;
    TInetAddr inetAddr;
    RSocket sock;
    User::LeaveIfError(socketServ.Connect());
    User::LeaveIfError(sock.Open(socketServ, KAfInet, KSockStream, KProtocolInetTcp));
    //Get IP information from the socket
    TSoInetInterfaceInfo ifInfo;
    TPckg<TSoInetInterfaceInfo> ifInfoPkg(ifInfo);
    TSoInetIfQuery ifQuery;
    TPckg<TSoInetIfQuery> ifQueryPkg(ifQuery);
    User::LeaveIfError(sock.SetOpt(KSoInetEnumInterfaces, KSolInetIfCtrl));
    while(sock.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, ifInfoPkg) == KErrNone)
        {
        ifQuery.iName = ifInfo.iName;
        TInt err = sock.GetOpt(KSoInetIfQueryByName, KSolInetIfQuery, ifQueryPkg);
        if( err == KErrNone )
            {
              if (!ifInfo.iAddress.IsUnspecified() && !ifInfo.iAddress.IsLoopback() && !ifInfo.iAddress.IsLinkLocal())
              {
              inetAddr = ifInfo.iAddress;
              inetAddr.ConvertToV4();
              inetAddr.Output(deviPAdds);
              aIpAdds.Copy(deviPAdds);
              }
            }
         else if( err !=KErrNone )
             {
             sock.Close();
             socketServ.Close();
             }
        } // while
        sock.Close();
        socketServ.Close();
   }

void CWlanOddSettingView::DelteJavaDirL()
    {
    //path of java dir needs to be created for wlan odd
    _LIT(KWlanJavaDir,"c:\\java\\");
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL (fs);
    CFileMan* image = CFileMan::NewL (fs);
    CleanupStack::PushL (image);
    TBool folderExist = EFalse; 
    //check the path exists or not
    folderExist = BaflUtils::FolderExists (fs, KWlanJavaDir);
    //if exists then remove the directory and create new
    if(folderExist)
        {
        User::LeaveIfError(image->RmDir (KWlanJavaDir));
        }
    CleanupStack::PopAndDestroy(2);//image or CFileMan object and fs    
    }