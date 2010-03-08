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
* Description:  Container class for log view.
*
*/


#ifndef __ECMTGUI_LOGCONTAINER_H__
#define __ECMTGUI_LOGCONTAINER_H__


#include <aknlists.h>
#include <coecntrl.h>


/**
 *  View class for the LogContainer.
 *
 *  @since S60 v3.1
 */
class CEcmtGuiLogContainer : public CCoeControl
    {
        
public: 

    /**
     * C++ default constructor.
     */
  	CEcmtGuiLogContainer( CDesCArrayFlat& aMessageList );
  	
  	/**
     * Destructor.
     */
  	~CEcmtGuiLogContainer();
  	
  	/**
     * Symbian 2nd phase constructor.
     */
    void ConstructL(const TRect& aRect );
	
		/**
     * Two-phased constructor.
     */
    static CEcmtGuiLogContainer* NewL(const TRect& aRect );

    
private: // From CCoeControl
           
    /**
     * From CoeControl.
     * Responds to changes to the size and position of the contents of this 
     * control.
     */
    void SizeChanged();
    
    /**
     * From CoeControl.
     * Gets the number of controls contained in a compound control.
     *
     * @return The number of component controls contained by this control.
     */
    TInt CountComponentControls() const;
    
    /**
     * From CoeControl.
     * Gets an indexed component of a compound control.
     *
     * @param aIndex The index of the control.
     * @return The component control with an index of aIndex.
     */
    CCoeControl* ComponentControl(TInt aIndex) const;
    
    /**
     * From CoeControl.
     * Draws the control.
     *
     * @param aRect The region of the control to be redrawn.
     */
    void Draw(const TRect& /*aRect*/) const;

public: // New functions

    /**
     * Clears logs from the listbox.
     */
    void ClearMessageListL();
    
    /**
     * Adds message to the listbox and to the message list iMessageList.
     *
     * @param aText Message to the listbox.
     * @param aAutoScroll Autoscroll listbox after appending message.
     */
    void AddMessageL(const TDesC& aText, TBool aAutoScroll);
    
    /**
     * Calls AddMessageL
     *
     * @param aMsg Message.
     */
    void LogMessageL( const TDesC& aMsg );
    
    /**
     * Loads message from resource and calls AddMessageL.
     *
     * @param aMsgResId Message resource Id.
     */
    void LogMessageL( TInt aMsgResId );

private: // Data

    /**
     * Listbox control.
     * Own.
     */
    CAknDoubleStyleListBox* iListBox;
    
    /**
     * Max length for message.
     */
    enum { KMaxLogMsgLen = 128 };
    
    /**
     * Buffer for storing last log message.
     */
    TBuf<KMaxLogMsgLen> iLastMsg;
    
    /**
     * Boolean indicating is listbox automatically scrollable.
     */
    TBool iAutoScroll;
    
    /**
     * A reference to message list, which is owned by the log view class.
     */
    CDesCArrayFlat& iMessageList;
    };

#endif // __ECMTGUI_LOGCONTAINER_H__
