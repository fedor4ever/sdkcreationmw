/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CCONTCONTAINER_H__
#define __CCONTCONTAINER_H__

// INCLUDES
#include <coecntrl.h>
#include <eiklbx.h>
#include <aknsfld.h>
#include <aknutils.h>
#include <aknlists.h>
#include <cntdb.h>
#include <badesca.h>
#include <eiktxlbm.h>

// CONSTANTS

#include "ContactsConstants.h"

// CLASS DECLARATION

/**
*  CContContainer container control class.
*
*/
class CContContainer : public CCoeControl,
                              MCoeControlObserver,
                              MEikListBoxObserver
    {

    public: // Constructors and destructor

        /**
        * Default constructor
        */
        void ConstructL( const TRect& aRect );

        /**
        * Destructor.
        * Destructor is always virtual
        */
        virtual ~CContContainer();

        /**
        * Constructor
        */
        CContContainer();

    public: // New functions

        /**
        * AddNewContactDlgL
        * The method adds new contact
        */
        void AddNewContactDlgL();

        /**
        * EditContactDlgL
        * The method edits contact
        */
        void EditContactDlgL();

        /**
        * DeleteContactDlgL
        * The method deletes contact
        */
        void DeleteContactDlgL();

        /**
        * AddContactToGrpL
        * The method adds contact to selected contact group
        */
        void AddContactToGrpL();

        /**
        * GetSelectedContactL
        * The method stores selected contact and open group
        * list to the listbox
        */
        void GetSelectedContactL();

        /**
        * CreateContactTemplateL
        * The method creates new contact template
        */
        void CreateContactTemplateL();

        /**
        * CreateContactGroupL
        * The method Creates new contact group
        */
        void CreateContactGroupL();

        /**
        * DeleteContactGroupL
        * The method deletes contact group
        */
        void DeleteContactGroupL();

        /**
        * ShowContactsL
        * The method shows contacts in the listbox
        */
        void ShowContactsL();

        /**
        * ShowContactGroupsL
        * The method shows contact groups in the listbox
        */
        void ShowContactGroupsL();

        /**
        * IsContactsOnListBox
        * The method returns true if contacs are in the listbox
        * @return status of listbox
        */
        TBool IsContactsOnListBox();

        /**
        * IfContactDbSelected
        * The method returns true if contacs are opened.
        * @return status of contact database attribute
        */
        TBool IfContactDbSelected();

        /**
        * CanAddContactToGroupL
        * The method returns ETrue is contact database
        * has been opened and contains at least one contact item
        * @return status of contact group
        */
        TBool CanAddContactToGroupL();

        /**
        * OpenDefaultDatabaseL
        * The method opens default contact database
        */
        void OpenDefaultDatabaseL();

        /**
        * OpenContactDatabase
        * The method stores name of the contact database
        * complete path
        * to the variable iDbName
        * @param aDbName name of the contact database
        */
        void UpdateContactDbName( TDesC& aDbName );

    private: // Functions from base classes

        /**
        * From CoeControl,SizeChanged
        * Called by framework when the view size is changed
        */
        void SizeChanged();

        /**
        * From CoeControl,CountComponentControls
        * @return Number of controls inside this container
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl,ComponentControl
        * @param aIndex The index of the control queried
        * @return Pointer to the control queried
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl,Draw
        * @param aRect rectangle for drawing
        */
        void Draw( const TRect& aRect ) const;

        /**
        * From CCoeControl
        * event handling section
        * e.g Listbox events
        * @param aControl name of the contact database
        * @param aEventType The event type
        */
        void HandleControlEventL( CCoeControl* aControl,TCoeEvent aEventType );

        /**
        * From CCoeControl
        * event handling section
        * e.g Listbox events
        * @param aKeyEvent Event to handled.
        * @param aType Type of the key event.
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed).
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * From MEikListBoxObserver,HandleListBoxEventL.
        * Handles listbox events.
        * @param aListBox, pointer to listbox - not in use
        * @param aEventType, event which happened
        */
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

    private: // New functions

        /**
        * UpdateListBoxL
        * The method updates listbox
        */
        void UpdateListBoxL();

        /**
        * ReadContactsFromDbL
        * The method reads contacts from contact database
        */
        void ReadContactsFromDbL();

        /**
        * ReadContactGroupsFromDbL
        * The method reads contact groups from contact database
        */
        void ReadContactGroupsFromDbL();
        /**
        * ShowDeleteConfirmationQueryL
        * The method shows confirmation query display
        * @param aTextResourceId ID for text string
        * @return Status for query
        */
        TBool ShowDeleteConfirmationQueryL( const TInt aTextResourceId );

        /**
        * OpenTextBoxQueryL
        * The method shows text query display
        * @param aTextResourceId  ID for text string
        * @param aTextData pointer of return textdata.
        * @param aTextLength max length for text
        * @return Status for query
        */
        TBool OpenTextBoxQueryL( const TInt aTextResourceId,
                                 TPtr aTextData,
						         TInt aTextLength = KMaxTextLength );


        /**
        * ShowInformationNoteNoDbL
        * The method shows no contact database selected information note
        */
        void ShowInformationNoteNoDbL();

    private: //data
        /**
        * iContactDb the contact database
        * Owned by CContContainer object
        */
        CContactDatabase* iContactDb;

        /**
        * iListBox the listbox
        * Owned by CContContainer object, instantiated in
        * ConstructL, deleted in destructor
        */
        CEikTextListBox* iListBox;

        /**
        * iContactArray the array of the contacts
        * Owned by CContContainer object, instantiated in
        * ConstructL, deleted in destructor
        */
        CDesCArray*       iContactArray;

        /**
        * iContactGroupArray the array of the contacts
        * Owned by CContContainer object, instantiated in
        * ConstructL, deleted in destructor
        */
        CDesCArray*       iContactGroupArray;

        /**
        * iDbName the name of the contact database
        * variable contains path and name
        */
        TBuf<KMaxDatabasePathAndNameLength>	  iDbName;

        /**
        * iSelectedItemId the id of the selected item
        */
        TContactItemId	  iSelectedItemId;
    };

#endif // __CCONTCONTAINER_H__

// End of File
