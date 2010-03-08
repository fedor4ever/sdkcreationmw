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

#include "EcmtWlanOddSettingList.h"
#include "EcmtGui.hrh"
#include <BAUTILS.H> 
#include  <UTF.H>
#include <aknappui.h>


//path of java dir needs to be created for wlan odd
_LIT(KWlanJavaDir,"c:\\java\\");
//path of the file which stores the ip address and port number
_LIT(KWlanFilePath,"c:\\java\\j9midpArgs.txt");

const TInt KIpPortBufLength = 70;

CWlanSettingList::CWlanSettingList()
    {  	 
    iCount = 0;
  	}
 	
CWlanSettingList::~CWlanSettingList()
  	{  	    

  	}
  	
CWlanSettingList* CWlanSettingList::NewL()
  	{
  	CWlanSettingList* self = new ( ELeave ) CWlanSettingList;
  	CleanupStack::PushL( self );
  	self->ConstructL();
  	CleanupStack::Pop( self );
  	return self;
  	}
	
void CWlanSettingList::ConstructL()
    {
    }
  	
CAknSettingItem* CWlanSettingList::CreateSettingItemL( TInt aSettingId )
    {
    CAknSettingItem* settingItem = NULL;
    
    switch( aSettingId )
        {
        case EListboxSettingUsername:
            settingItem = new ( ELeave )CAknTextSettingItem( aSettingId, iText );
            break;
        case EListboxSettingConnectMode:
            settingItem = new ( ELeave )CAknEnumeratedTextPopupSettingItem( aSettingId, iSelected );
            break;
        default:
            break;    
        }    
     
    return settingItem;
    }   
    
void CWlanSettingList::EditItemL( TInt aIndex, TBool aCalledFromMenu )
    {
    iCount++;
    TInt currentItem = ((CAknSettingItemList*)this)->ListBox()->View()->CurrentItemIndex();
    CAknSettingItemList::EditItemL( currentItem, aCalledFromMenu );
    ( *SettingItemArray() )[aIndex]->UpdateListBoxTextL();
    ( *SettingItemArray() )[aIndex]->StoreL();  

    if (iCount == 1)
        {
        iIp.Copy(( *SettingItemArray() )[aIndex]->SettingTextL());
        }
    if(iCount == 2)
        iPort.Copy(( *SettingItemArray() )[aIndex]->SettingTextL());
    SaveToFileL();
    }

void CWlanSettingList::CreateJavaDirL()
    {
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
        User::LeaveIfError(fs.MkDir (KWlanJavaDir));
        }
    else
        //if the path does not exist then create new
        User::LeaveIfError(fs.MkDir (KWlanJavaDir));
    CleanupStack::PopAndDestroy(2);//image or CFileMan object and fs
    }

void CWlanSettingList::SaveToFileL()
    {
    if (iCount == 2)
        {
        //create the directory structure for the wlan odd app c:\\c\java
        CreateJavaDirL();
        RFs fs;
        User::LeaveIfError(fs.Connect ());
        CleanupClosePushL (fs);
                
        RFile file;
        //create the file to save ip address and port number
        User::LeaveIfError(file.Create(fs, KWlanFilePath,EFileWrite));
        CleanupClosePushL (file);
       
        _LIT(KWlanTxt,"-Xrunjdwp:server=y,address=");
        _LIT(KColon,":");
        //creates the buffer to write into j9midpArgs.txt
        TBuf<KIpPortBufLength> buf;
        buf.Copy(KWlanTxt);
        buf.Append(iIp);
        buf.Append(KColon);
        buf.Append(iPort);
        //used to commit into file
        TBuf8<KIpPortBufLength> buf8;
        CnvUtfConverter::ConvertFromUnicodeToUtf8 (buf8, buf);
        
        User::LeaveIfError(file.Write (buf8));
        CleanupStack::PopAndDestroy(2);
        iIp.FillZ();
        iPort.FillZ();
        iCount = 0;
        }
   }
