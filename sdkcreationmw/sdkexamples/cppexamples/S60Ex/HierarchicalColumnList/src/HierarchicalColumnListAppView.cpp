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
* Description:  Implementation for HierarchicalColumnListAppView class.
*
*/


#include <coemain.h>
#include <aknsinglecolumnstyletreelist.h>
#include <stringloader.h>
#include <eikbtgpc.h>
#include <aknappui.h>
#include <aknsbasicbackgroundcontrolcontext.h>
#include <eikmenup.h>
#include <gulicon.h>

#include <avkon.mbg>
#include <akniconutils.h>

#include <avkon.rsg>
#include <HierarchicalColumnList.rsg>
#include "HierarchicalColumnList.hrh"
#include "HierarchicalColumnListAppView.h"


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Adds some items to the given list.
// ---------------------------------------------------------------------------
//
void PopulateListL( CAknSingleColumnStyleTreeList* aList )
    {
    // In this example, this flag is used to set every item is persistent,
    // which means that they are not automatically removed from the list when
    // one of the nodes in which they are located is collapsed.
    TUint32 flags = CAknSingleColumnStyleTreeList::EPersistent;

    // Add simple data row to the top-most level of the tree. The method
    // returns an identifier, which should be stored to be used later to
    // refer to the added item.
    _LIT( KSimpleDataRowText, "Simple data row" );
    TAknTreeItemID simpleItem = aList->AddSimpleDataRowL( KAknTreeIIDRoot,
        KSimpleDataRowText, flags, EFalse );


    // Add core data row to the top-most level of the tree.
    _LIT( KCoreDataRowText, "Core data row" );
    TAknTreeItemID coreItem = aList->AddCoreDataRowL( KAknTreeIIDRoot,
        KCoreDataRowText, KNullDesC(), KNullDesC(), flags, EFalse );


    // Add subtitle row to the to the top-most level of the tree.
    _LIT( KSubtitleRowText, "Subtitle row (default icons)" );
    TAknTreeItemID subtitle = aList->AddSubtitleRowL( KAknTreeIIDRoot,
        KSubtitleRowText, flags, EFalse );


    // Add subtitle row to the to the previously added subtitle row.
    // Although this is possible, the hierarchy should be restricted to
    // two levels. Otherwise, the list will get too complicated for the
    // user to use, as it offers no hints of the underlying hierarchy.
    _LIT( KEmptySubtitleRowText, "Subtitle row (empty)" );
    aList->AddSubtitleRowL( subtitle, KEmptySubtitleRowText, flags,
        EFalse );


    // Add icons to the list. The list returns an identifier for each icon
    // that is added to the list. The client should save this ID and use
    // it to set list items to use the icon. If the icon is no longer
    // required to be used by list items, it can be removed from the list
    // with CAknTreeList::RemoveIcon() method, when no longer needed.
    TInt icon = aList->AddIconL( KAknsIIDQgnIndiS60,
        AknIconUtils::AvkonIconFileName(), EMbmAvkonQgn_indi_s60,
        EMbmAvkonQgn_indi_s60_mask, EAspectRatioPreserved );

    TInt optIcon1 = aList->AddIconL( KAknsIIDQgnPropBtMisc,
        AknIconUtils::AvkonIconFileName(), EMbmAvkonQgn_prop_bt_misc,
        EMbmAvkonQgn_prop_bt_misc_mask, EAspectRatioPreserved );


    // Add subtile row with new folder icons to top-most level.
    _LIT( KSubtitleWithIconRowText, "Subtitle row (new icons)" );
    TAknTreeItemID parent = aList->AddSubtitleRowL( KAknTreeIIDRoot,
        KSubtitleWithIconRowText, flags, EFalse );
    aList->SetIcon( parent, CAknSingleColumnStyleTreeList::ECollapsedNode,
        icon, EFalse );
    aList->SetIcon( parent, CAknSingleColumnStyleTreeList::EExpandedNode,
        icon, EFalse );


    // Add simple data row to the previously added subtitle row.
    TAknTreeItemID item1 = aList->AddSimpleDataRowL( parent,
        KSimpleDataRowText, flags, EFalse );

    // Set item use pre-defined hierarhciacl list file icon.
    aList->SetIcon( item1, CAknSingleColumnStyleTreeList::ELeaf,
        AknTreeListIconID::KDefaultFileIndication, EFalse );

    // Set item use added optional icon.
    aList->SetIcon( item1, CAknSingleColumnStyleTreeList::EOptionalIcon1,
        optIcon1, EFalse );


    // Create bitmap icon with mask and add the bitmaps to the list.
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    AknIconUtils::CreateIconLC( bitmap, mask,
        AknIconUtils::AvkonIconFileName(),
        EMbmAvkonQgn_indi_mce_priority_high,
        EMbmAvkonQgn_indi_mce_priority_high_mask );

    TInt bitmapIconId = aList->AddIconL( bitmap, mask, ETrue,
        EAspectRatioPreserved );
    CleanupStack::Pop( 2 );


    _LIT( KColumn1Text, "C1 - column" );
    _LIT( KColumn2Text, "C2 - column" );
    _LIT( KColumn3Text, "C3 - column" );

    // Add core data row with two columns to the list. And set row
    // emphasised with EEmphasis flag.
    TAknTreeItemID item2 = aList->AddCoreDataRowL( parent,
        KColumn1Text, KColumn2Text,
        flags | CAknSingleColumnStyleTreeList::EEmphasis, EFalse );

    // Set item use pre-defined file icon. By default, the core data row
    // does not use any icons.
    aList->SetIcon( item2, CAknSingleColumnStyleTreeList::ELeaf,
        AknTreeListIconID::KDefaultFileIndication, EFalse );

    // Set previously added bitmap icon as item's optional icon.
    aList->SetIcon( item2, CAknSingleColumnStyleTreeList::EOptionalIcon1,
        bitmapIconId, EFalse );


    // Add core data row with three columns to the list.
    TAknTreeItemID item3 = aList->AddCoreDataRowL( parent,
        KColumn1Text, KColumn2Text, KColumn3Text, flags, EFalse );

    // Set item use pre-defined file icon.
    aList->SetIcon( item3, CAknSingleColumnStyleTreeList::ELeaf,
        AknTreeListIconID::KDefaultFileIndication, EFalse );

    // Set item emphasised.
    aList->SetEmphasis( item3, ETrue, EFalse );
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Standard construction sequence
// ---------------------------------------------------------------------------
//
CHierarchicalColumnListAppView* CHierarchicalColumnListAppView::NewL(
    const TRect& aRect )
    {
    CHierarchicalColumnListAppView* self = 
        CHierarchicalColumnListAppView::NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }


CHierarchicalColumnListAppView* CHierarchicalColumnListAppView::NewLC(
    const TRect& aRect )
    {
    CHierarchicalColumnListAppView* self = 
        new ( ELeave ) CHierarchicalColumnListAppView;
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }


CHierarchicalColumnListAppView::CHierarchicalColumnListAppView()
    : iMarkingModeEnabled( EFalse )
    {
    }


CHierarchicalColumnListAppView::~CHierarchicalColumnListAppView()
    {
    delete iBgContext;
    delete iList;
    }


void CHierarchicalColumnListAppView::ConstructL( const TRect& aRect )
    {
    // Create a window for this application view.
    CreateWindowL();

    // Init component array.
    InitComponentArrayL();
    Components().SetControlsOwnedExternally( ETrue );

    // Construct list.
    CAknSingleColumnStyleTreeList* list =
        CAknSingleColumnStyleTreeList::NewL( *this );
    
    // Use list as a component control of the view.
    Components().AppendLC( list, KCoeNoControlId );
    CleanupStack::Pop( list );
    iList = list;

    // Set the view to observer events from tree list. These events will
    // be received through MAknTreeObserver inerface.
    iList->AddObserverL( this );

    // Populate the list with some list items.
    PopulateListL( iList );

    // Construct background control context.
    iBgContext = CAknsBasicBackgroundControlContext::NewL(
        KAknsIIDQsnBgAreaMain, iAvkonAppUi->ApplicationRect(), EFalse );

    SetComponentsToInheritVisibility( ETrue );

    // Set the windows size.
    SetRect( aRect );

    // Activate the window, which makes it ready to be drawn. This makes also
    // the list ready to be drawn, as it is now component of this control.
    ActivateL();
    }


// ---------------------------------------------------------------------------
// Expands currently focused node.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppView::ExpandFocusedNode()
    {
    if ( iList )
        {
        // Get focused item ID.
        TAknTreeItemID focused = iList->FocusedItem();

        // Check that focused item is a node and then expand it.
        if ( focused != KAknTreeIIDNone && iList->IsNode( focused ) )
            {
            iList->ExpandNode( focused, ETrue );
            }
        }
    }


// ---------------------------------------------------------------------------
// Collapses currently focused node.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppView::CollapseFocusedNode()
    {
    if ( iList )
        {
        // Get focused item ID.
        TAknTreeItemID focused = iList->FocusedItem();

        // Check that focused item is a node and then collapse it.
        if ( focused != KAknTreeIIDNone && iList->IsNode( focused ) )
            {
            iList->CollapseNode( focused, ETrue );
            }
        }
    }


// ---------------------------------------------------------------------------
// Opens currently focused item.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppView::OpenFocusedItem()
    {
    if ( iList )
        {
        // Get focused item ID.
        TAknTreeItemID focused = iList->FocusedItem();

        // Get item type of focused item.
        CAknSingleColumnStyleTreeList::TItemType type =
            iList->ItemType( focused );

        // If focused item is core data row, remove emphasis from the item.
        if ( type == CAknSingleColumnStyleTreeList::ECoreDataRow )
            {
            iList->SetEmphasis( focused, EFalse, ETrue );
            }
        }
    }


// ---------------------------------------------------------------------------
// Checks whether the list is looping.
// ---------------------------------------------------------------------------
//
TBool CHierarchicalColumnListAppView::IsListLooping() const
    {
    return iList && ( iList->Flags() & KAknTreeListLooping );
    }


// ---------------------------------------------------------------------------
// Toggles the looping setting.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppView::ChangeLooping()
    {
    if ( iList )
        {
        iList->SetFlags( iList->Flags() ^ KAknTreeListLooping );
        }
    }


// ---------------------------------------------------------------------------
// Checks whether marquee scrolling is enabled in the list. When marquee
// scrolling is enabled for the hierarchical column list, marquee scolling
// is used to display the content of those subtitle and simple data rows,
// whose content is not otherwise fully visible. Marquee scolling is never
// used with core data rows.
// ---------------------------------------------------------------------------
//
TBool CHierarchicalColumnListAppView::IsMarqueeOn() const
    {
    return iList && ( iList->Flags() & KAknTreeListMarqueeScrolling );
    }


// ---------------------------------------------------------------------------
// Toggles the marquee setting.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppView::ChangeMarquee()
    {
    if ( iList )
        {
        iList->SetFlags( iList->Flags() ^ KAknTreeListMarqueeScrolling );
        }
    }


// ---------------------------------------------------------------------------
// Checks whether list is set markable. When list is set markable, the list
// list responds to the specified pointer and key events by marking and
// unmarking the items as necessary. The list client can change the marking
// of tree items through hierarchical list API, even if the list is not set
// markable.
// ---------------------------------------------------------------------------
//
TBool CHierarchicalColumnListAppView::IsMarkable() const
    {
    return iList && ( iList->Flags() & KAknTreeListMarkable );
    }


// ---------------------------------------------------------------------------
// Toggles the markable setting.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppView::ChangeMarkable()
    {
    if ( iList )
        {
        iList->SetFlags( iList->Flags() ^ KAknTreeListMarkable );
        }
    }


// ---------------------------------------------------------------------------
// Changes the used collapse/expand function indicators.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppView::ChangeFunctionIndicatorsL()
    {
    if ( iList )
        {
        iList->EnableTabModeFunctionIndicatorsL(
            !iList->TabModeFunctionIndicators() );
        }
    }


// ---------------------------------------------------------------------------
// Marks the focused list item.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppView::Mark()
    {
    if ( iList && iList->FocusedItem() != KAknTreeIIDNone )
        {
        iList->SetMarked( iList->FocusedItem(), ETrue, ETrue );
        }
    }


// ---------------------------------------------------------------------------
// Unmarks the focused list item.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppView::Unmark()
    {
    if ( iList && iList->FocusedItem() != KAknTreeIIDNone )
        {
        iList->SetMarked( iList->FocusedItem(), EFalse, ETrue );
        }
    }


// ---------------------------------------------------------------------------
// Marks every item in the list.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppView::MarkAll()
    {
    if ( iList )
        {
        // Every list item can be set marked by setting the tree root as
        // the marked item.
        iList->SetMarked( KAknTreeIIDRoot, ETrue, ETrue );
        }
    }


// ---------------------------------------------------------------------------
// Unmarks every item in the list.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppView::UnmarkAll()
    {
    if ( iList )
        {
        // The marking can be removed from every list item by specifying
        // the tree root as the unmarked item.
        iList->SetMarked( KAknTreeIIDRoot, EFalse, ETrue );
        }
    }


// ---------------------------------------------------------------------------
// Dynamically initialises a options menu pane.
// Unnecessary expand, collapse, and open commands are removed from the
// options menu depending on focused item type and state.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppView::DynInitOptionsMenuPaneL(
    CEikMenuPane* aMenuPane )
    {
    if ( iList && iList->FocusedItem() != KAknTreeIIDNone )
        {
        if ( iList->IsNode( iList->FocusedItem() ) )
            {
            // Focused item is node, remove open command and the invalid one
            // of expand and collapse commands.
            aMenuPane->DeleteMenuItem( EHclExampleCmdOpen );
            if ( iList->IsExpanded( iList->FocusedItem() ) )
                {
                aMenuPane->DeleteMenuItem( EHclExampleCmdExpand );
                }
            else
                {
                aMenuPane->DeleteMenuItem( EHclExampleCmdCollapse );
                }
            }
        else
            {
            // Focused item is a leaf, remove expand and collapse commands.
            aMenuPane->DeleteMenuItem( EHclExampleCmdExpand );
            aMenuPane->DeleteMenuItem( EHclExampleCmdCollapse );
            }
        }
    else
        {
        // No item is focused, remove all related commands.
        aMenuPane->DeleteMenuItem( EHclExampleCmdOpen );
        aMenuPane->DeleteMenuItem( EHclExampleCmdExpand );
        aMenuPane->DeleteMenuItem( EHclExampleCmdCollapse );
        }
    }


// ---------------------------------------------------------------------------
// Dynamically initialises a settings menu pane.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppView::DynInitSettingsMenuPaneL(
    CEikMenuPane* aMenuPane )
    {
    // List looping setting. The menu symbol is set for the item
    // depending whether looping is enabled for the list.
    if ( IsListLooping() )
        {
        aMenuPane->ItemData( EHclExampleCmdChangeLooping ).iFlags
            |= EEikMenuItemSymbolOn;
        }
    else
        {
        aMenuPane->ItemData( EHclExampleCmdChangeLooping ).iFlags
            &= ~EEikMenuItemSymbolOn; 
        }

    // Marquee scrolling setting. The menu symbol is set for the item
    // depending whether marquee scrolling is enabled for the list.
    if ( IsMarqueeOn() )
        {
        aMenuPane->ItemData( EHclExampleCmdChangeMarquee ).iFlags
            |= EEikMenuItemSymbolOn;
        }
    else
        {
        aMenuPane->ItemData( EHclExampleCmdChangeMarquee ).iFlags
            &= ~EEikMenuItemSymbolOn;
        }

    // Markable list setting. The menu symbol is set for the item
    // depending whether list is set markable.
    if ( IsMarkable() )
        {
        aMenuPane->ItemData( EHclExampleCmdChangeMarkable ).iFlags
            |= EEikMenuItemSymbolOn;
        }
    else
        {
        aMenuPane->ItemData( EHclExampleCmdChangeMarkable).iFlags
            &= ~EEikMenuItemSymbolOn;
        }

    // Tabulator mode expand/collapse function indicator setting.
    // The menu symbol is set for the item depending whether tabulator
    // mode function indicators are enabled for the list.
    if ( iList && iList->TabModeFunctionIndicators() )
        {
        aMenuPane->ItemData( EHclExampleCmdChangeIndicators ).iFlags
            |= EEikMenuItemSymbolOn;
        }
    else
        {
        aMenuPane->ItemData( EHclExampleCmdChangeIndicators ).iFlags
            &= ~EEikMenuItemSymbolOn; 
        }
    }


// ---------------------------------------------------------------------------
// Dynamically initialises a marking menu pane.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppView::DynInitMarkingMenuPaneL(
    CEikMenuPane* aMenuPane )
    {
    if ( iList && iList->FocusedItem() != KAknTreeIIDNone )
        {
        if ( iList->IsMarked( iList->FocusedItem() ) )
            {
            aMenuPane->DeleteMenuItem( EAknCmdMark );
            }
        else
            {
            aMenuPane->DeleteMenuItem( EAknCmdUnmark );
            }
        }
    else
        {
        aMenuPane->DeleteMenuItem( EAknCmdMark );
        aMenuPane->DeleteMenuItem( EAknCmdUnmark );
        }
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// Passes key event to the list.
// ---------------------------------------------------------------------------
//
TKeyResponse CHierarchicalColumnListAppView::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    if ( iList )
        {
        return iList->OfferKeyEventL( aKeyEvent, aType );
        }
    else
        {
        return EKeyWasNotConsumed;
        }
    }


// ---------------------------------------------------------------------------
// From class MAknTreeListObserver.
// Handles the events from the hierarchical list. In this example application,
// the received events are mainly used to update the MSK commands.
// ---------------------------------------------------------------------------
//
TBool CHierarchicalColumnListAppView::HandleTreeListEvent( CAknTreeList& aList,
    TAknTreeItemID aItem, TEvent aEvent )
    {
    // This event is received from hierarchical list, when one of the nodes
    // in the list is being expanded. In this example, the list contains only
    // a static structure, but when the list content is being updated
    // dynamically, the content of the node should be updated on this event.
    if ( aEvent == ENodeExpanded )
        {
        // If the expanded node is currently focused and the list is not in
        // the marking mode, the MSK command is updated.
        if ( aItem == aList.FocusedItem() && !iMarkingModeEnabled )
            {
            ChangeMskCommand( EHclExampleCmdCollapse,
                R_QTN_MSK_COLLAPSE );
            }
        }

    // This event is received from hierarchical list, when one of the nodes
    // in the list is collapsed. The item ID of collapsed node is received
    // in aItem parameter.
    else if ( aEvent == ENodeCollapsed )
        {
        // If the collapsed node is currently focused and the list is not in
        // the marking mode, the MSK command is updated.
        if ( aItem == aList.FocusedItem() && !iMarkingModeEnabled )
            {
            ChangeMskCommand( EHclExampleCmdExpand,
                R_QTN_MSK_EXPAND );
            }
        }

    // EItemFocused event is received from the list, when the focused item
    // changes. The list also sents this event, when it gains or loses focus.
    else if ( aEvent == EItemFocused )
        {
        // MSK command is updated on focus change events, if the list is not
        // in the marking mode.
        if ( !iMarkingModeEnabled )
            {
            UpdateMskCommand( aList, aItem );
            }
        }

    // EItemSelected event is received when a leaf item (simple data row or
    // core data row in hierarchical column list) is selected.
    else if ( aEvent == EItemSelected )
        {
        OpenFocusedItem();
        }

    // EItemRemoved event is received when an item is being removed from
    // the list.
    else if ( aEvent == EItemRemoved )
        {
        }

    // EMarkinModeEnabled event is received when hierarchical list has
    // entered in marking mode. In marking mode, the list controls the
    // mark/unmark commands in MSK, and the list client is not allowed
    // to change MSK commands.
    else if ( aEvent == EMarkingModeEnabled )
        {
        iMarkingModeEnabled = ETrue;
        }

    // The EMarkingModeDisabled event is received when hierarchical list
    // exits the marking mode and it no longer controls the MSK commands.
    // At this point, the client should resume to update them itself.
    else if ( aEvent == EMarkingModeDisabled )
        {
        iMarkingModeEnabled = EFalse;
        UpdateMskCommand( aList, aList.FocusedItem() );
        }

    return KErrNone;
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppView::FocusChanged( TDrawNow aDrawNow )
    {
    // When application view gets notification of focus change, the focus
    // state is updated also to the hierarchical column list. This is
    // required to get list show the highlight for focused item and to
    // respond to key events.
    if ( iList )
        {
        iList->SetFocus( IsFocused(), aDrawNow );
        }
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// Handles size change.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppView::SizeChanged()
    {
    // Get the rectangle set for the application view and update the list
    // and background control context to occupy the same area.
    TRect rect = Rect();

    if ( iList )
        {
        iList->SetRect( rect );
        }

    if ( iBgContext )
        {
        iBgContext->SetRect( rect );
        }
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// Supplies background control context.
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CHierarchicalColumnListAppView::MopSupplyObject( TTypeUid aId )
    {
    if ( aId.iUid == MAknsControlContext::ETypeId && iBgContext )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject( aId );
    }


// ---------------------------------------------------------------------------
// Updates MSK command and label.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppView::UpdateMskCommand( CAknTreeList& aList,
    TAknTreeItemID aFocusedItem )
    {
    TInt resourceId = NULL;
    TInt command = NULL;

    // Determine the MSK command depending on the focused item type and state.
    if ( aFocusedItem == KAknTreeIIDNone )
        {
        // No item focused.
        }
    else if ( aList.IsNode( aFocusedItem ) )
        {
        if ( aList.IsExpanded( aFocusedItem ) )
            {
            resourceId = R_QTN_MSK_COLLAPSE;
            command = EHclExampleCmdCollapse;
            }
        else
            {
            resourceId = R_QTN_MSK_EXPAND;
            command = EHclExampleCmdExpand;
            }
        }
    else if ( aList.IsLeaf( aFocusedItem ) )
        {
        resourceId = R_QTN_MSK_OPEN;
        command = EHclExampleCmdOpen;
        }

    if ( resourceId )
        {
        ChangeMskCommand( command, resourceId );
        }
    }


// ---------------------------------------------------------------------------
// Changes MSK command and label.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppView::ChangeMskCommand( TInt aCommandId,
    TInt aLabelResourceId )
    {
    // The MSK commands should not be changed when controlled by the list.
    __ASSERT_DEBUG( !iMarkingModeEnabled, User::Invariant() );
    
    // Get cba and update the given label and command to MSK.
    CEikButtonGroupContainer* cba = iAvkonAppUi->Cba();
    if ( cba )
        {
        TBuf<20> label;
        StringLoader::Load( label, aLabelResourceId, iCoeEnv );
        cba->SetCommandL( 3, aCommandId, label );
        cba->DrawDeferred();
        }
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// Draw this application's view to the screen.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppView::Draw( const TRect& /*aRect*/ ) const
    {
    // Get the standard graphics context.
    CWindowGc& gc = SystemGc();
    
    // Gets the control's extent.
    TRect rect = Rect();
    
    // Clears the screen.
    gc.Clear( rect );
    }


