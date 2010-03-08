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


#ifndef __LISTBOXSETTINGLIST_H__
#define __LISTBOXSETTINGLIST_H__


#include <AknSettingItemList.h>
                             
class CWlanSettingList : public CAknSettingItemList 
    {
 
public:
    CWlanSettingList();
   	~CWlanSettingList();
   	static CWlanSettingList* NewL();
      	
public:
    /**
     * this is frame work function to create setting list box
     */
    CAknSettingItem* CreateSettingItemL( TInt aSettingId );
    /**
     * is called when user enters data or edits the stting box 
     */
    void EditItemL( TInt aIndex, TBool aCalledFromMenu );
     
private:
    void ConstructL();
    
    /**
     * this method is used to create a directory in c:
     * and then creates a file to store ip address and port number.
     */
    void CreateJavaDirL();
    
    /*
     * writes the ip address and port number into c:\java
     */
    void SaveToFileL();
    
private: // Data
    TBuf<70> iText;
    TInt iSelected;
    /**
     * used to count the number of entry in the list box
     */
    TInt iCount;
    /**
     * stores the system ip address
     */
    TBuf<30> iIp;
    /**
     * stores port number
     */
    TBuf<20> iPort;
     
    };
    
#endif // __LISTBOXSETTINGLIST_H__    
    