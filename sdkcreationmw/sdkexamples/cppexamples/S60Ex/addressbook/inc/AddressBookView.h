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
* Description:  Application View class, CAddressBookView - 
*				 shows the application state on the screen
*
*/


#ifndef ADDRESSBOOKVIEW_H
#define ADDRESSBOOKVIEW_H

// INCLUDES
#include <coecntrl.h>
#include <aknlists.h>

// CONSTANTS
_LIT(KMultiImagePathName, "\\recource\\apps\\ADDRESSBOOK.MBM");

// AddressBook item ("vCard") constants:
_LIT8(KLastname, "Family Name:");
_LIT8(KFirstname, "Given Name:");
_LIT8(KMiddlename, "Middle Name:");
_LIT8(KExtAddr, "Extra address:");
_LIT8(KStreet, "Street:");
_LIT8(KLocality, "Locality:");
_LIT8(KRegion, "Region:");
_LIT8(KPCode, "Post code:");
_LIT8(KCountry, "Country:");
_LIT8(KMobile, "Mobile:");
_LIT8(KFax, "Fax:");
_LIT8(KPhone, "Phone:");
_LIT8(KEmail, "Email:");
_LIT8(KTitle, "Title:");
_LIT8(KOrganization, "Organization:");
_LIT8(KUnit, "Unit:");

_LIT8(KRowFormat, "\t%S\t%S\t");

_LIT8(KTab,"\t");
_LIT8(KSpace," ");
_LIT8(KHasPhoneChar,"X");

_LIT(KAboutString1, "Liberty Enabled Address Book.");
_LIT(KAboutString2, "Web Service Demo Client");
_LIT(KAboutString3, "by Nokia.");
_LIT(KAboutString4, "Server functionality powered");
_LIT(KAboutString5, "by Trustgenix.");

// FORWARD DECLARATIONS
class CAddressBookContact;
class CAddressBookEngine;

// CLASS DECLARATION
/**
* CAddressBookView  container control class,
* shows the application state on the screen
*/
class CAddressBookView : public CCoeControl, MCoeControlObserver, MEikListBoxObserver
    {
	public: // Constructors and destructor
	
		/**
        * By default Symbian 2nd phase constructor is private.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect);
      
		/**
		* C++ default constructor.
		*/
		CAddressBookView(CAddressBookEngine& aEngine);
		
		/**
        * Destructor.
        */
        ~CAddressBookView();

    	// New functions
    
    	/**
    	* Updates bufer containing the contact information when search was finished 
    	* @param aContactArray - descriptor on the array connecting finding information
    	*/
		void UpdateL(RPointerArray<CAddressBookContact>& aContactArray);
		
		/**
		* is needed to browse Contact information
		*/
		void OpenItemL();
		
		/**
		* It is called when Contact information was browsed and needed to be closed
		*/  
		void CloseItem();
		
		/**
		* It is called when need to open or close some item to change common button area
		* @param aResourceId - Id of Resource
		* @param aDrawNow - ENoDrawNow - Redraw of the control is not required immediately. 
							EDrawNow - Redraw control immediately.
		*/
		void ChangeCbaL(TInt aResourceId, TDrawNow aDrawNow);
				
		/**
		* Shows information about Address Book aplication.
		* @param aAboutVisible - ETrue in case that to show information on the screen
		* @return aAboutVisible
		*/
		bool ShowAbout(TBool aAboutVisible = ETrue);
		
		/**
		* Is needed to add current contact into PnoneBook database.
		* @return descriptor on current contact
		*/
		CAddressBookContact* CurrentContact();
		
		/**
		* Creates card (full contact information) about a person who was found
		* @param aValue - value of field that is needed to locate in the aArray 
		* @param aFieldName - name of field whose value contains in aValue
		* @param aArray - pointer on the array where will be located this card
		*/
		void AppendRowL(TPtrC8 aValue, const TDesC8& aFieldName, CDesCArray& aArray);
	
        // Functions from base classes
	
		/**
		* From MEikListBoxObserver, is called when user want to browse 
		* the card (full contact information) about employees who was found
		* @param aEventType - type of event
		*/ 
		void HandleListBoxEventL(CEikListBox*, TListBoxEvent aEventType);

	private: 
		
       	/**
        * From CCoeControl, is called by framework when the view size is changed
        */
        void SizeChanged();

       	/**
        * From CCoeControl, is called by the active scheduler to controll count of components
        * @return 1
        */
        TInt CountComponentControls() const;

       	/**
        * From CCoeControl, is called by the active scheduler to controll components
        * @param aIndex 
        * @return - iDetailList/iSearchList(if aIndex == 0)/NULL (if aInex != 0)
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

       	/**
        * From CCoeControl, draws rects
        * @param aRect - pointer on rectangle that has to be drawn
        */
        void Draw(const TRect& aRect) const;
       	
		/**
		* From MCoeControlObserver, acts upon changes in the hosted control's state. 
		* @param aControl	- The control changing its state
		* @param aEventType	- The type of control event 
		*/
        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);

		/**
		* From CCoeControl the key event handler function, is called every time 
		* when user presses some button.
		* @param aKeyEvent - Event to handled.
        * @param aType - Type of the key event. 
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed). 
        */
		virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,	TEventCode aType );
        
	private: // Data
        
		CAddressBookEngine& iEngine;
		TBool iDetailedView;
		CEikColumnListBox* iSearchList; 	
		CEikTextListBox* iDetailList; 	
		CDesCArray* iItemList;
		const CFont* iFont;
	};

#endif // ADDRESSBOOKVIEW_H

// End of File

