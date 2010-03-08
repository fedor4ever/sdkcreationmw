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


// INCLUDE FILES
#include <aknquerydialog.h>
#include <aknnotewrappers.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>
#include <contacts.rsg>

#include <aknlists.h>
#include <avkon.hrh>
#include <CPbkContactEngine.h>
#include <CPbkViewState.h>
#include <CPbkContactEditorDlg.h>
#include <CPbkContactItem.h>
#include <f32file.h>
#include <rpbkviewresourcefile.h>
#include <stringloader.h>

#include "CContContainer.h"
#include "contacts.hrh"


// ================= MEMBER FUNCTIONS =======================
//
// ---------------------------------------------------------
// CContContainer::CContContainer()
// constructor
// ---------------------------------------------------------
//
CContContainer::CContContainer()
    {
    iContactDb = NULL;
    iSelectedItemId = -1;
    }

// ---------------------------------------------------------
// CContContainer::ConstructL()
// Two phased constructor
// ---------------------------------------------------------
//
void CContContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();

    iContactArray = new (ELeave) CDesCArrayFlat( KArraySize );
    iContactGroupArray = new (ELeave) CDesCArrayFlat( KArraySize );

    // Create Listbox
    iListBox = new (ELeave) CAknSingleStyleListBox;

    if ( iListBox )
        {
        iListBox->ConstructL( NULL, EAknListBoxSelectionList );
        iListBox->Model()->SetItemTextArray( iContactArray );
        iListBox->CreateScrollBarFrameL();
        iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
        iListBox->SetListBoxObserver( this );
        iListBox->HandleItemAdditionL();
        }
    SizeChanged();

    SetRect( aRect );

    ActivateL();
    }

// ---------------------------------------------------------
// CContContainer::~CContContainer ()
// Destructor
// ---------------------------------------------------------
//
CContContainer::~CContContainer()
    {
    if ( iListBox )
        {
        delete iListBox;
        }

    if ( iContactDb )
        {
        iContactDb->CloseTables();
        delete iContactDb;
        }

    if ( iContactArray )
        {
        delete iContactArray;
        }

    if ( iContactGroupArray )
        {
        delete iContactGroupArray;
        }
    }

// ---------------------------------------------------------
// CContContainer::SizeChanged()
// Called by framework when the view size is changed.
// ---------------------------------------------------------
//
void CContContainer::SizeChanged()
    {
    // We want the list box to begin from top of the main pane
    // and fill at least its minimum size.
	TPoint coordListBox;
	AknLayoutUtils::LayoutMetricsPosition(AknLayoutUtils::EMainPane, coordListBox);
	iListBox->SetExtent( coordListBox, iListBox->MinimumSize() );
    }

// ---------------------------------------------------------
// CContContainer::CountComponentControls()
// Return the number of controls inside this container. We
// have only the list box.
// ---------------------------------------------------------
//
TInt CContContainer::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CContContainer::ComponentControl()
// ---------------------------------------------------------
//
CCoeControl* CContContainer::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
            if ( iListBox )
                {
                return iListBox;
                }

        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CContContainer::Draw()
// ---------------------------------------------------------
//
void CContContainer::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KRgbWhite );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( aRect );
    }

// ----------------------------------------------------------------------------
// CContContainer::OfferKeyEventL()
// Handles the key events.
// ----------------------------------------------------------------------------
//
TKeyResponse CContContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                              TEventCode aType )
	{
    if ( aType != EEventKey )
        {
        return EKeyWasNotConsumed;
        }

	if ( iListBox )
        {
        return iListBox->OfferKeyEventL( aKeyEvent, aType );
        }

	return EKeyWasNotConsumed;

    }

// ---------------------------------------------------------------------------
// CContContainer::HandleListBoxEventL()
// Handles listboxevents.
// ---------------------------------------------------------------------------
//
void CContContainer::HandleListBoxEventL( CEikListBox* /*aListBox */,
    TListBoxEvent aEventType )
    {
    switch( aEventType )
        {
        case EEventEnterKeyPressed: // Flow through
        case EEventItemClicked:
            EditContactDlgL();
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CContContainer::HandleControlEventL()
// ---------------------------------------------------------
//
void CContContainer::HandleControlEventL(
    CCoeControl* /*Control*/,TCoeEvent /*aEventType*/)
    {
    }

// ---------------------------------------------------------
// CContContainer::UpdateContactDbName()
// Store current database path and name.
// ---------------------------------------------------------
//
void CContContainer::UpdateContactDbName( TDesC& aDbName )
    {
    iDbName = aDbName;
    }

// ----------------------------------------------------
// CContContainer::UpdateListBoxL()
// Updates listbox.
// ----------------------------------------------------
//
void CContContainer::UpdateListBoxL()
    {
    iListBox->HandleItemAdditionL();
    iListBox->SetCurrentItemIndexAndDraw( 0 );
    SizeChanged();
    DrawNow();
    }

// ----------------------------------------------------
// CContContainer::IsContactsOnListBox()
// The function returns state of the listbox.
// If contacts on the listbox has been selected
// the function returns ETrue.
// ----------------------------------------------------
//
TBool CContContainer::IsContactsOnListBox()
    {
    CDesCArray* itemArray = (CDesCArray*) iListBox->Model()->ItemTextArray();

    if ( iContactArray == itemArray )
        {
	    return ETrue;
	    }

    return EFalse;
    }
// ----------------------------------------------------
// CContContainer::ShowContactsL()
// Show contacts in the listbox.
// ----------------------------------------------------
//
void CContContainer::ShowContactsL()
    {

    if ( !IfContactDbSelected() )
        {
        ShowInformationNoteNoDbL();
        return;
        }

    if ( IsContactsOnListBox() )
        {
        return;
        }

    iListBox->Model()->SetItemTextArray( iContactArray );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    UpdateListBoxL();
    }

// ----------------------------------------------------
// CContContainer::ShowContactGroupsL()
// Show contact groups in the listbox.
// ----------------------------------------------------
//
void CContContainer::ShowContactGroupsL()
	{

    if ( !IfContactDbSelected() )
        {
        ShowInformationNoteNoDbL();
        return;
        }

    if ( !IsContactsOnListBox() )
	    {
	    return;
	    }

    iListBox->Model()->SetItemTextArray( iContactGroupArray );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    UpdateListBoxL();
    }



// ----------------------------------------------------
// CContContainer::ShowInformationNoteNoDbL()
// Show information not if contact database
// did not opened.
// ----------------------------------------------------
//
void CContContainer::ShowInformationNoteNoDbL()
    {
    HBufC* textResource = StringLoader::LoadLC( R_CONTACTS_NO_DB_SELECTED );
    CAknInformationNote* informationNote = new( ELeave ) CAknInformationNote;
    informationNote->ExecuteLD( textResource->Des() );
    CleanupStack::PopAndDestroy( textResource );

    }
// ----------------------------------------------------
// CContContainer::IfContactDbSelected()
// The function checks is contact database
// has been opened.
// ----------------------------------------------------
//
TBool CContContainer::IfContactDbSelected()
    {

    if ( iContactDb )
	    {
	    return ETrue;
	    }

    return EFalse;
    }

// ----------------------------------------------------
// CContContainer::CanAddContactToGroupL()
// The function returns ETrue is contact database
// has been opened and contains at least one contact item.
// ----------------------------------------------------
//
TBool CContContainer::CanAddContactToGroupL()
    {
    if ( !IfContactDbSelected() )
	    {
        ShowInformationNoteNoDbL();
	    return EFalse;
	    }

    if ( !iContactArray->Count() )
        {
        HBufC* textResource = StringLoader::LoadLC( R_CONTACTS_NO_CONTACT_SELECTED );
        CAknInformationNote* informationNote = new( ELeave ) CAknInformationNote;
        informationNote->ExecuteLD( textResource->Des() );
        CleanupStack::PopAndDestroy( textResource );
        return EFalse;
        }

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CContContainer::OpenDefaultDatabaseL()
// Open default contact database.
// Database is same as what Phonebook uses.
// ----------------------------------------------------------------------------
//
void CContContainer::OpenDefaultDatabaseL()
    {
    // Read name of the default database
    _LIT( KOrgContactFile,"" );
    TBuf<KMaxDatabasePathAndNameLength> orgContactFile( KOrgContactFile );
    CContactDatabase::GetDefaultNameL( orgContactFile );
    orgContactFile.LowerCase();

    if ( iContactDb )
        {
        if ( !iDbName.Compare( orgContactFile ) )
            {
            return;
            }
        iContactDb->CloseTables();
        delete iContactDb;
        iContactDb = NULL;
        }

    TRAPD(err, iContactDb = CContactDatabase::OpenL( orgContactFile ););
    // Check if database already exist
    if ( err == KErrNotFound )
        {

        HBufC* text = StringLoader::LoadLC( R_CONTACTS_DB_NOT_FOUND );
        CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
        informationNote->ExecuteLD( *text );
        CleanupStack::PopAndDestroy( text );
        iContactDb = NULL;
        ReadContactsFromDbL();// Update iContactArray and iContactGroupArray

        TBuf<15> noContactFile( KNullDesC );
        UpdateContactDbName( noContactFile );

        UpdateListBoxL();
        }
    else
        {
        // Update iContactArray and iContactGroupArray
        ReadContactsFromDbL();
        UpdateContactDbName( orgContactFile );
        UpdateListBoxL();
        }
    }

// ----------------------------------------------------------------------------
// CContContainer::ShowDeleteConfirmationQueryL()
// The display of Delete Confirmation.
// ----------------------------------------------------------------------------
//
TBool CContContainer::ShowDeleteConfirmationQueryL( const TInt aTextResourceId )
    {
    HBufC* prompt = StringLoader::LoadLC( aTextResourceId );

    CAknQueryDialog* dlg = CAknQueryDialog::NewL();

    dlg->SetPromptL( *prompt );
    TInt retCode( dlg->ExecuteLD( R_CONTACTS_CONFIRMATION_QUERY ) );

    CleanupStack::PopAndDestroy(); //prompt
    return retCode;
    }

// ----------------------------------------------------------------------------
// CContContainer::OpenTextBoxQueryL()
// Display of Data Query.
// ----------------------------------------------------------------------------
//
TBool CContContainer::OpenTextBoxQueryL(const TInt aTextResourceId,
                                               TPtr aTextData,
											   TInt aTextLength)
    {
    HBufC* prompt = StringLoader::LoadLC( aTextResourceId );
    // aTextData is smaller than 40 so this does not crash
    TBuf<KMaxTextBoxQueryLength> text = aTextData;

    CAknTextQueryDialog* dlg = new (ELeave) CAknTextQueryDialog( text,
                                                CAknQueryDialog::ENoTone );

    CleanupStack::PushL( dlg );
    dlg->SetPromptL( *prompt );
    dlg->SetMaxLength( aTextLength );

    TBool answer( dlg->ExecuteLD( R_CONTACTS_TEXTBOX_QUERY ) );
    CleanupStack::Pop( dlg );
    CleanupStack::PopAndDestroy( prompt );
    aTextData = text;
    return answer;
    }

// ----------------------------------------------------
// CContContainer::EditContactDlgL()
// Opens CPbkContactEditorDlg for contact.
// ----------------------------------------------------
//
void CContContainer::EditContactDlgL()
    {
    if ( !IfContactDbSelected() )
        {
        ShowInformationNoteNoDbL();
        return;
        }

    // Check if contacts list on the list box
    if ( !IsContactsOnListBox() )
        {
        return;
        }

    // The caller does not take ownership of this object.
    // so do not push it onto the CleanupStack.
    const CContactIdArray* contacts = iContactDb->SortedItemsL();

    TInt currentItemIndex( iListBox->CurrentItemIndex() );
    TInt count( iContactArray->Count() );
    if ( currentItemIndex < 0 || currentItemIndex >= count )
        {
        HBufC* textResource = StringLoader::LoadLC( R_CONTACTS_NO_CONTACT_SELECTED );
        CAknInformationNote* informationNote = new( ELeave ) CAknInformationNote;
        informationNote->ExecuteLD( textResource->Des() );
        CleanupStack::PopAndDestroy( textResource );
	    return;
        }

    TContactItemId itemId( ((*contacts)[currentItemIndex]) );

    CPbkContactEngine* pbkContactEng = CPbkContactEngine::NewL( &iCoeEnv->FsSession());
    CleanupStack::PushL( pbkContactEng );
    // load the contacts dialog resources
    CCoeEnv *env = CEikonEnv::Static();
    RPbkViewResourceFile pbkRes( *env );
    pbkRes.OpenL();

    CPbkContactItem* aContactItem = pbkContactEng->OpenContactL( itemId );
    CleanupStack::PushL( aContactItem );

    // Open the contacts dialog
    // third parameter: set to true if aContactItem is a
    // new contact (not yet in the database)
    // fourth parameter index of field to focus initially
    // fifth paramter if true sets the contact initially as "edited".
    CPbkContactEditorDlg* pbkContactDlg =
                    CPbkContactEditorDlg::NewL( *pbkContactEng,
                                                *aContactItem,
                                                EFalse, -1,
                                                EFalse );

    CleanupStack::PushL( pbkContactDlg );
    pbkContactDlg->SetMopParent( iAvkonAppUi );

    TInt res( KErrNone );
    TRAPD( err, res = pbkContactDlg->ExecuteLD() );

    pbkRes.Close();
    pbkContactEng->CloseContactL( aContactItem->Id() );

    CleanupStack::Pop( pbkContactDlg );
    CleanupStack::PopAndDestroy( 2 ); //aContactItem, pbkContactEng

    // Update iContactArray and iContactGroupArray
    ReadContactsFromDbL();
    UpdateListBoxL();
    }

// ----------------------------------------------------
// CContContainer::GetSelectedContactL()
// Set selected contact's item id to
// variable and changes listbox data to contact groups.
// ----------------------------------------------------
//
void CContContainer::GetSelectedContactL()
    {
    if ( !IfContactDbSelected() )
        {
        ShowInformationNoteNoDbL();
        return;
        }

    if ( !IsContactsOnListBox() )
        {
	    return;
        }

    iContactDb->SetDbViewContactType( KUidContactCard );
    // The caller does not take ownership of this object.
    // so do not push it onto the CleanupStack
    const CContactIdArray* contacts = iContactDb->SortedItemsL();

    TInt currentItemIndex( iListBox->CurrentItemIndex() );
    TInt count( iContactArray->Count() );
    if ( currentItemIndex < 0 || currentItemIndex >= count )
        {
	    return;
        }

    iSelectedItemId = ((*contacts)[currentItemIndex]);

    ShowContactGroupsL();
    }

// ----------------------------------------------------
// CContContainer::AddContactToGrp()
// Add selected contact to contact group.
// ----------------------------------------------------

void CContContainer::AddContactToGrpL()
    {
    if ( !IfContactDbSelected() )
        {
        ShowInformationNoteNoDbL();
        return;
        }

    if ( iSelectedItemId == -1 )
	    {
	    return;
	    }

    TInt currentItemIndex( iListBox->CurrentItemIndex() );
    TInt count( iContactGroupArray->Count() );
    if ( currentItemIndex < 0 || currentItemIndex >= count )
        {
        return;
        }

    // The caller takes ownership of the returned object.
    // groupIdList is NULL if there are no groups in the database.
    CContactIdArray* groupIdList = iContactDb->GetGroupIdListL();
    if ( !groupIdList )
        {
	    return;
        }

    CleanupStack::PushL( groupIdList );

    TContactItemId groupItemId = ((*groupIdList)[currentItemIndex]);
    iContactDb->AddContactToGroupL( iSelectedItemId , groupItemId );

    iSelectedItemId = -1;

    CleanupStack::PopAndDestroy( groupIdList );
    }
// ----------------------------------------------------
// CContContainer::AddNewContactDlg()
// Open CPbkContactEditorDlg dialog
// for new contact item.
// ----------------------------------------------------
//
void CContContainer::AddNewContactDlgL()
    {

    if ( !IfContactDbSelected() )
        {
        ShowInformationNoteNoDbL();
        return;
        }

    if ( !IsContactsOnListBox() )
        {
        return;
        }

	CPbkContactEngine* pbkContactEng = CPbkContactEngine::NewL( &iCoeEnv->FsSession());
    // Put object to the stack
    CleanupStack::PushL( pbkContactEng );

    // load the contacts dialog resources
    CCoeEnv *env = CEikonEnv::Static();
    RPbkViewResourceFile pbkRes( *env );
    pbkRes.OpenL();

    //create a new empty contact
    // The caller takes ownership of this object.
    // so push it onto the CleanupStack
    CPbkContactItem* aContactItem = pbkContactEng->CreateEmptyContactL();
    CleanupStack::PushL( aContactItem );

    // launch the contact dialog
    CPbkContactEditorDlg* pbkContactDlg =
                CPbkContactEditorDlg::NewL( *pbkContactEng,
                                            *aContactItem,
                                            ETrue, -1,
                                            ETrue );

    CleanupStack::PushL( pbkContactDlg );

    pbkContactDlg->SetMopParent( iAvkonAppUi );

    TInt res( KErrNone );

    // Show contact dialog
    TRAPD( err, res = pbkContactDlg->ExecuteLD() );

    pbkRes.Close();

    CleanupStack::Pop( pbkContactDlg );
    CleanupStack::PopAndDestroy( 2 ); //aContactItem, pbkContactEng

    // Update internal database connection
    iContactDb->CloseTables();
    delete iContactDb;
    iContactDb = CContactDatabase::OpenL( iDbName );

    // Update iContactArray and iContactGroupArray
    ReadContactsFromDbL();
    // Update display
    UpdateListBoxL();
    }

// ----------------------------------------------------
// CContContainer::DeleteContactDlg()
// Delete contact from contact database.
// ----------------------------------------------------
//
void CContContainer::DeleteContactDlgL()
    {

    if ( !IfContactDbSelected() )
        {
        ShowInformationNoteNoDbL();
        return;
        }

    if ( !IsContactsOnListBox() )
        {
        return;
        }

    // The caller does not take ownership of this object.
    // so do not push it onto the CleanupStack

    TInt currentItemIndex( iListBox->CurrentItemIndex() );
    TInt count( iContactArray->Count() );
    if (currentItemIndex < 0 || currentItemIndex >= count )
        {
        HBufC* textResource = StringLoader::LoadLC( R_CONTACTS_NO_CONTACT_SELECTED );
        CAknInformationNote* informationNote = new( ELeave ) CAknInformationNote;
        informationNote->ExecuteLD( textResource->Des() );
        CleanupStack::PopAndDestroy( textResource );
        return;
        }

    // Ask about deletion
    if ( !ShowDeleteConfirmationQueryL( R_CONTACTS_DELETE_CONTACT_MESSAGE ) )
        {
        return;
        }

    // Sets the type of contact items to be included in
    // sorted views of the database
    iContactDb->SetDbViewContactType( KUidContactCard );

    const CContactIdArray* contacts = iContactDb->SortedItemsL();
    TContactItemId itemId( ( (*contacts)[currentItemIndex] ) );
                                        
	CPbkContactEngine* pbkContactEng = CPbkContactEngine::NewL( &iCoeEnv->FsSession());                                              
    CleanupStack::PushL( pbkContactEng );
    pbkContactEng->DeleteContactL( itemId );
    CleanupStack::PopAndDestroy( pbkContactEng );

    iContactDb->CloseTables();
    delete iContactDb;
    iContactDb = NULL;
    iContactDb = CContactDatabase::OpenL( iDbName );

    // Update iContactArray and iContactGroupArray
    ReadContactsFromDbL();
    UpdateListBoxL();
    }


// ----------------------------------------------------
// CContContainer::CreateContactTemplateL()
// Creates new contact template to contact database.
// ----------------------------------------------------
//
void CContContainer::CreateContactTemplateL()
    {

    if ( !IfContactDbSelected() )
        {
        ShowInformationNoteNoDbL();
        return;
        }

    HBufC* templateName = HBufC::NewLC( KMaxTemplateNameLength );
    TPtr ptr( templateName->Des() );
    if( !OpenTextBoxQueryL( R_CONTACTS_CREATE_TEMPLATE_MESSAGE,
                                ptr,
                                KMaxTextLength ) )
	    {
	    CleanupStack::PopAndDestroy( 1 ); // templateName
	    return;
	    }

    // The caller takes ownership of this object.
    // so push it onto the CleanupStack
    CContactItem* contactTemplate =
                 iContactDb->CreateContactCardTemplateL( templateName->Des() );
    CleanupStack::PushL( contactTemplate );
    CContactItemField* givenNameField =
                   CContactItemField::NewLC( KStorageTypeText,
                                            KUidContactFieldGivenName );
    CContactItemField* familyNameField =
                   CContactItemField::NewLC(KStorageTypeText,
                                            KUidContactFieldFamilyName );
    CContactItemField* phoneNumberField =
                   CContactItemField::NewLC(KStorageTypeText,
                                            KUidContactFieldPhoneNumber );

    // Add data to fields
    givenNameField->TextStorage()->SetTextL( KFirstName );
    givenNameField->SetMapping( KUidContactFieldVCardMapUnusedN );
    familyNameField->TextStorage()->SetTextL( KLastName );
    familyNameField->SetMapping( KUidContactFieldVCardMapUnusedN );
    phoneNumberField->TextStorage()->SetTextL( KPhoneNumber );
    phoneNumberField->SetMapping( KUidContactFieldVCardMapTEL );

    // Add fields to a contact template
    contactTemplate->AddFieldL( *givenNameField );
    contactTemplate->AddFieldL( *familyNameField );
    contactTemplate->AddFieldL( *phoneNumberField );

    // Item takes the ownership of fields
    CleanupStack::Pop( 3 ); // givenNameField, familyNameField, phoneNumberField
    CleanupStack::PopAndDestroy( 2 );// templateName, contactTemplate

    // Update iContactArray and iContactGroupArray
    ReadContactsFromDbL();
    UpdateListBoxL();
	}

// ----------------------------------------------------
// CContContainer::CreateContactGroupL()
// Create new contact group.
// ----------------------------------------------------
//
void CContContainer::CreateContactGroupL()
	{

    if ( !IfContactDbSelected() )
        {
        ShowInformationNoteNoDbL();
        return;
        }

    if ( IsContactsOnListBox() )
        {
        return;
        }
           
	CPbkContactEngine* pbkContactEng = CPbkContactEngine::NewL( &iCoeEnv->FsSession());                                            
    CleanupStack::PushL( pbkContactEng );

    // Open dialog
    HBufC* groupName = HBufC::NewLC( KMaxGroupNameLength );
    TPtr ptr( groupName->Des() );
    if ( OpenTextBoxQueryL( R_CONTACTS_CREATE_GROUP_MESSAGE,
                            ptr,
                            KMaxTextLength ) )
	    {
	    //Create contactGroup
	    CContactGroup* contactGroup =
                    pbkContactEng->CreateContactGroupL( groupName->Des() );
        delete contactGroup;
	    }

    CleanupStack::PopAndDestroy( 2 ); // pbkContactEng, groupName

    iContactDb->CloseTables();
    delete iContactDb;
    iContactDb = NULL;
    iContactDb = CContactDatabase::OpenL( iDbName );

    // Update iContactArray and iContactGroupArray
    ReadContactsFromDbL();
    UpdateListBoxL();
    }

// ----------------------------------------------------
// CContContainer::DeleteContactGroupL()
// Delete contact group from contact database.
// ----------------------------------------------------
//
void CContContainer::DeleteContactGroupL()
	{

    if ( !IfContactDbSelected() )
        {
        ShowInformationNoteNoDbL();
        return;
        }

    if ( IsContactsOnListBox() )
        {
        return;
        }

    TInt currentItemIndex( iListBox->CurrentItemIndex() );
    TInt count( iContactGroupArray->Count() );
    if ( currentItemIndex < 0 || currentItemIndex >= count )
        {
        return;
        }

    // The caller takes ownership of the returned object.
    // groupIdList is NULL if there are no groups in the database.
    CContactIdArray* groupIdList = iContactDb->GetGroupIdListL();
    if ( !groupIdList )
        {
        return;
        }

    CleanupStack::PushL( groupIdList );

	CPbkContactEngine* pbkContactEng = CPbkContactEngine::NewL( &iCoeEnv->FsSession());                                             
    CleanupStack::PushL(pbkContactEng);

    TContactItemId itemId = ((*groupIdList)[currentItemIndex]);

    // Ask about deletion
    if (ShowDeleteConfirmationQueryL( R_CONTACTS_DELETE_GROUP_MESSAGE ) )
        {
        pbkContactEng->DeleteContactGroupL( itemId );
        }

    CleanupStack::PopAndDestroy( 2 ); //pbkContactEng , groupIdList

    iContactDb->CloseTables();
    delete iContactDb;
    iContactDb = NULL;
    iContactDb = CContactDatabase::OpenL( iDbName );

    // Update iContactArray and iContactGroupArray
    ReadContactsFromDbL();
    UpdateListBoxL();
	}

// ---------------------------------------------------------
// CContContainer::ReadContactsFromDbL()
// Read contacts from contact database
// and sort contacts by Family Name and
// Given Name. Add sorted items to
// iContactArray.
// ---------------------------------------------------------
//
void CContContainer::ReadContactsFromDbL()
    {
    ReadContactGroupsFromDbL();

    // Delete old contact from listbox
    TInt count( iContactArray->Count() );
    if ( count )
        {
        // Clear array. Start to index 0
	    iContactArray->Delete( 0, count );
        }

    if ( !IfContactDbSelected() )
        {
	    return;
        }

    iContactDb->SetDbViewContactType( KUidContactCard );

    TFieldType aFieldType1( KUidContactFieldFamilyName );
    TFieldType aFieldType2( KUidContactFieldGivenName );
    CContactDatabase::TSortPref sortPref1( aFieldType1 );
    CContactDatabase::TSortPref sortPref2( aFieldType2 );

    // Sort contacts by Family and Given Name
    CArrayFixFlat<CContactDatabase::TSortPref>* aSortOrder =
                    new (ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(2);

    CleanupStack::PushL( aSortOrder );
    aSortOrder->AppendL( sortPref1 );
    aSortOrder->AppendL( sortPref2 );

    // The database takes ownership of the sort order array passed in
    iContactDb->SortL( aSortOrder );

    // The caller does not take ownership of this object.
    // so do not push it onto the CleanupStack
    const CContactIdArray* contacts = iContactDb->SortedItemsL();

    // Go through each contact item and
    // make items for listbox
    const TInt nc( contacts->Count() );

    for ( TInt i( nc-1 ); i >= 0; i-- ) //For each ContactId
        {
        CContactItem* contact = NULL;
        // The caller takes ownership of the returned object.
        // So push it onto the CleanupStack
        contact = iContactDb->OpenContactL( (*contacts)[i] );
        CleanupStack::PushL( contact );

        CContactItemFieldSet& fieldSet = contact->CardFields();

        HBufC* firstNameBuf = NULL;
        HBufC* lastNameBuf = NULL;

        // Get first name
        TInt findpos( fieldSet.Find( KUidContactFieldGivenName ) );

        // Check that the first name field is actually there.
        if ( (findpos > -1) || (findpos >= fieldSet.Count()) )
            {
            CContactItemField& firstNameField = fieldSet[findpos];
            CContactTextField* firstName = firstNameField.TextStorage();
            firstNameBuf = firstName->Text().AllocLC();
            }

	    // Get last name
        findpos = fieldSet.Find( KUidContactFieldFamilyName );

        // Check that the last name field is actually there.
        if ( (findpos > -1) || (findpos >= fieldSet.Count()) )
            {
            CContactItemField& lastNameField = fieldSet[ findpos ];
            CContactTextField* lastName = lastNameField.TextStorage();
            lastNameBuf = lastName->Text().AllocLC();
            }

        TInt len( 0 );

        if ( firstNameBuf )
            {
            len+=firstNameBuf->Length();
            }

        if ( lastNameBuf )
            {
            len+=lastNameBuf->Length();
            }

	    // Create new buffer and add space for formatters
	    HBufC* combinedDes = HBufC::NewLC( len + KFormattersSpace );
        if ( ( firstNameBuf ) && ( lastNameBuf ) )
            {
            combinedDes->Des().Format( KListItemFormatter,
                                       lastNameBuf,
                                       firstNameBuf );
            }
        else
            {
            if ( firstNameBuf )
                {
                combinedDes->Des().Format( KListOItemFormatter, firstNameBuf );
                }
            else if ( lastNameBuf )
                {
                combinedDes->Des().Format( KListOItemFormatter, lastNameBuf );
                }
            }

        // Insert a contacts name into the array at the specified position.
        // If the specified position is the same as the current number of
        // descriptor elements in the array, this has the effect of
        // appending the element.
        iContactArray->InsertL( 0, *combinedDes );

        CleanupStack::PopAndDestroy( combinedDes );

        if ( lastNameBuf )
            {
            CleanupStack::PopAndDestroy( lastNameBuf );
            }

        if ( firstNameBuf )
            {
            CleanupStack::PopAndDestroy( firstNameBuf );
            }

        iContactDb->CloseContactL( contact->Id() );
        CleanupStack::PopAndDestroy( contact );

        }

    CleanupStack::Pop( aSortOrder );
    }

// ---------------------------------------------------------
// CContContainer::ReadContactGroupsFromDbL()
// Read contact groups from contact database.
// Do not sort contact groups. Add group items
// to iContactGroupArray.
// ---------------------------------------------------------
//
void CContContainer::ReadContactGroupsFromDbL()
    {
    TInt count( iContactGroupArray->Count() );
    if ( count )
        {
        // Clear array. Start to index 0
        iContactGroupArray->Delete( 0, count );
        }
    if ( !IfContactDbSelected() )
        {
        return;
        }

	//The caller takes ownership of the returned object.
    // groupIdList is NULL if there are no groups in the database.
    CContactIdArray* groupIdList = iContactDb->GetGroupIdListL();
    if( !groupIdList )
        {
        return;
        }

    CleanupStack::PushL( groupIdList );

    CContactGroup* contactGroup = NULL;
    const TInt nc( groupIdList->Count() );
    for ( TInt i( nc-1 ); i >= 0; i-- ) //For each Contact Group Id
        {
        CContactItem* contact=NULL;

        // The caller takes ownership of the returned object.
        // So push it onto the CleanupStack
        contact = iContactDb->OpenContactL( ((*groupIdList)[i]) );
        CleanupStack::PushL( contact );

        contactGroup = (CContactGroup*) contact;
        TInt len( contactGroup->GetGroupLabelL().Length() );
        HBufC* groupLabelBuf = HBufC::NewLC( len + KFormattersSpace );
        HBufC* combinedDes = HBufC::NewLC( len + KFormattersSpace );

        *groupLabelBuf = contactGroup->GetGroupLabelL();
        combinedDes->Des().Format( KListOItemFormatter, groupLabelBuf );

        // Insert a group name into the array at the specified position.
        // If the specified position is the same as the current number of
        // descriptor elements in the array, this has the effect of
        // appending the element.
        iContactGroupArray->InsertL( 0, *combinedDes );

	    CleanupStack::PopAndDestroy( combinedDes );
	    CleanupStack::PopAndDestroy( groupLabelBuf );

	    iContactDb->CloseContactL( contact->Id() );
	    CleanupStack::PopAndDestroy( contact );
	    }

    CleanupStack::PopAndDestroy( groupIdList );
    }

// End of File
