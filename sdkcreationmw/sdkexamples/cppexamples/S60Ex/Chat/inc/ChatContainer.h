/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CHATCONTAINER_H
#define CHATCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <AknQueryDialog.h>
#include <aknlists.h>
#include "Log.h"

// FORWARD DECLARATIONS
class CEikLabel;

// CONSTANTS
const TInt KMessageHeaderLen = 8;
const TInt KMessageListLen = 10;

// CLASS DECLARATION

/**
*  CChatContainer  container control class.
*  
*/
class CChatContainer : public CCoeControl, public MLog
    {
    public: // Constructors and destructor
        
        /**
        * NewL()
        * Create a CChatContainer object, which will draw itself
        * to aRect
        * @param aRect the rectangle this view will be drawn to
        * @return a pointer to the created instance of 
        * CChatContainer
        */
        static CChatContainer* NewL( const TRect& aRect );

        /**
        * NewLC()
        * Create a CChatContainer object, which will draw 
        * itself to aRect
        * @param aRect the rectangle this view will be drawn to
        * @return a pointer to the created instance of 
        * CChatContainer
        */
        static CChatContainer* NewLC( const TRect& aRect );
        
        /**
        * Destructor.
        */
        ~CChatContainer();

    public: // New functions
    
        /**
        * ContainsEntries()
        * @return ETrue if the list view contains entries
        */
        TBool ContainsEntries();

        /**
        * ClearMessageListL()
        * Clear the list of messages.
        */
        void ClearMessageListL();
    
        /**
        * ShowDataQueryL()
        * Show data query.
        * @param aQueryResourceId the id of the query
        * @param aTextResourceId for inial value
        * @param aPromptResoureId for prompt
        * @param aMaxLength sets up the length of the character
        * @param aDisplayFlag sets the query visible   
        * @param aText text to be set to the list             
        */
        void ShowDataQueryL(
            const TInt aQueryResourceId,
            const TInt aTextResourceId,
            const TInt aPromptResoureId,
            const TInt aMaxLength,
            const TBool aDisplayFlag,
            TDes& aText );
  
    public: // from MLog

        /**
        * LogL()
        * Add an entry to the log.
        * @param aText the text of the entry
        */
        void LogL( const TDesC& aText );

        /**
        * LogL()
        * Add an entry to the log.
        * @param aText the text of the entry
        * @param aExtraText extra text to append onto the entry
        */
        void LogL( const TDesC& aText, const TDesC& aExtraText );

        /**
        * LogL()
        * Add an entry to the log.
        * @param aText the text of the entry
        * @param aNumber a number to append onto the entry
        */
        void LogL( const TDesC& aText, TInt aNumber );
        
    public:  // from CCoeControl

        /**
        * OfferKeyEventL()
        * Offer the key event to the list box
        * @param aKeyEvent the key that caused the event
        * @param aType the type of event that occured
        * @return indicates whether or not the key event
        * was used by this control
        */  
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                           TEventCode aType );

        /**
        * CountComponentControls()
        * Return the number of component controls
        * @return the number of component controls
        */
        TInt CountComponentControls() const;

        /**
        * ComponentControl()
        * Return a component control
        * @param aIndex the index of the control to return
        * @return the specified component control
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;
        
       
    private: // Functions from base classes 

       /**
        * From CoeControl,SizeChanged.
        */
        void SizeChanged();

    private:    // Constructors

        /**
        * ConstructL()
        * Perform the second phase construction
        * @param aRect the rectangle this view will be drawn to
        */
        void ConstructL( const TRect& aRect );

        /**
        * CBTPointToPointAppView()
        * Perform the first phase of two phase construction 
        */
        CChatContainer();

        
    private: //data
        
        /** 
        * iDisplayDialog
        * boolean for Display Dialog 
        */
        TBool iDisplayDialog; 
    
        /** 
        * iQueryDialog
        */
        CAknQueryDialog* iQueryDialog;
        
        /**
        * iLabelResultMessage 
        */
        CEikLabel* iLabelResultMessage; 
        
        /** 
        * iListBox the control used to display the results 
        * Owned by CCChatContainer
        */
        CEikTextListBox* iListBox;

        /** 
        * iMessageList the list of messages to display 
        * Not owned by CChatContainer
        */
        CDesCArrayFlat* iMessageList;

        /** 
        * iMsgIndex 
        * the id for the next message in the list 
        */
        TInt iMsgIndex;
    };

#endif // CHATCONTAINER_H

// End of File
