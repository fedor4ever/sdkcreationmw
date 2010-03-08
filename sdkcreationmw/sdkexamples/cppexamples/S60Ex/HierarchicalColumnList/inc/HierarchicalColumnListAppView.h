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
* Description:  View class for the example application.
*
*/


#ifndef C_HIERARCHICALCOLUMNLISTAPPVIEW_H
#define C_HIERARCHICALCOLUMNLISTAPPVIEW_H


#include <coecntrl.h>
#include <akntreelistobserver.h>

class CAknSingleColumnStyleTreeList;
class CAknsBasicBackgroundControlContext;


/**
 *  An instance of the Application View object for the HierarchicalColumnList 
 *  application.
 *
 *  This class acts as a contaner for @c CAknSingleColumnStyleTreeList
 *  instance, and provides a bacground control context for the list through
 *  mop-chain. Class realizes the @c MAknTreeListObserver interface, so that
 *  it can register itself as an observer of the list in orded to receive
 *  list events.
 */
class CHierarchicalColumnListAppView : public CCoeControl,
                                       public MAknTreeListObserver
    {

public:

    /**
     * Two phased constructor. Creates a CHierarchicalColumnListAppView object,
     * which will draw itself to aRect.
     *
     * @param aRect the rectangle this view will be drawn to
     * @return a pointer to the created instance of CHierarchicalColumnListAppView
     */
    static CHierarchicalColumnListAppView* NewL( const TRect& aRect );

    /**
     * Two phased constructor. Creates a CHierarchicalColumnListAppView object,
     * which will draw itself to aRect.
     *
     * @param aRect the rectangle this view will be drawn to
     * @return a pointer to the created instance of CHierarchicalColumnListAppView
     */
    static CHierarchicalColumnListAppView* NewLC( const TRect& aRect );

    /**
     * Destructor. Destroys the object and releases all memory objects.
     */
    ~CHierarchicalColumnListAppView();

    /**
     * Expands currently focused node in the displayed list.
     */
    void ExpandFocusedNode();

    /**
     * Collapses currently focused node in the displayed list.
     */
    void CollapseFocusedNode();

    /**
     * Opens currently focused item in the displayed list.
     */
    void OpenFocusedItem();

    /**
     * Checks whether the list is looping.
     */
    TBool IsListLooping() const;

    /**
     * Toggles the looping setting.
     */
    void ChangeLooping();

    /**
     * Checks whether marquee scrolling is enabled in the list.
     */
    TBool IsMarqueeOn() const;

    /**
     * Toggles the marquee setting.
     */
    void ChangeMarquee();

    /**
     * Checks whether list is set markable.
     */
    TBool IsMarkable() const;

    /**
     * Toggles the markable setting.
     */
    void ChangeMarkable();

    /**
     * Changes the used collapse/expand function indicators.
     */
    void ChangeFunctionIndicatorsL();

    /**
     * Marks the focused list item.
     */
    void Mark();

    /**
     * Unmarks the focused list item.
     */
    void Unmark();
    
    /**
     * Marks every list item.
     */
    void MarkAll();

    /**
     * Unmarks every list item.
     */
    void UnmarkAll();

    /**
     * Dynamically initialises a menu pane.
     *
     * @param aMenuPane Menu pane.
     */
    void DynInitOptionsMenuPaneL( CEikMenuPane* aMenuPane );

    /**
     * Dynamically initialises a settings menu pane.
     *
     * @param aMenuPane Menu pane.
     */
    void DynInitSettingsMenuPaneL( CEikMenuPane* aMenuPane );
    
    /**
     * Dynamically initialises a marking menu pane.
     *
     * @param aMenuPane Menu pane.
     */
    void DynInitMarkingMenuPaneL( CEikMenuPane* aMenuPane );

// from base class CCoeControl

    /**
     * From CCoeControl.
     * Handles key events.
     */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
        TEventCode aType );

// from base class MAknTreeListObserver
 
    /**
     * From MAknTreeListObserver.
     * Handles tree list events.
     */
    TBool HandleTreeListEvent( CAknTreeList& aList, TAknTreeItemID aItem,
        TEvent aEvent );

protected:

// from base class CCoeControl

    
	// Change of focus
    void FocusChanged( TDrawNow aDrawNow );

	// Change of size and position
	void SizeChanged();

    /**
     * Retrieves an object of the same type as that encapsulated in aId.
     * 
     * @param aId An encapsulated object type ID.
     */
    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );

private:

    /**
     * Second phase constructor. Performs the second phase construction of a
     * CHierarchicalColumnListAppView object.
     *
     * @param aRect the rectangle this view will be drawn to
     */
    void ConstructL( const TRect& aRect );

    /**
     * C++ constructor. Performs the first phase of two phase construction.
     */
    CHierarchicalColumnListAppView();

    /**
     * Updates the command and label in MSK depending on the curryntly
     * focused hierarchical list item.
     *
     * @param aList Reference to an instance of CAknTreeList.
     * @param aFocusedItem Item identifier of focused list item.
     */
    void UpdateMskCommand( CAknTreeList& aList, TAknTreeItemID aFocusedItem );

    /**
     * Change the specified command and label to MSK.
     *
     * @param aCommandId Command ID.
     * @param aLabelResourceId Resource ID for the MSK label.
     */
    void ChangeMskCommand( TInt aCommandId, TInt aLabelResourceId );

// from base class CCoeControl

    /**
     * From CCoeControl.
     * Draws this CHierarchicalColumnListAppView to the screen.
     *
     * @param aRect the rectangle of this view that needs updating.
     */
    void Draw( const TRect& aRect ) const;

private: // data

    /**
     * Hierarchical column list.
     * Own.
     */
    CAknSingleColumnStyleTreeList* iList;

    /**
     * Background control context.
     * Own.
     */
    CAknsBasicBackgroundControlContext* iBgContext;

    /**
     * Marking mode enabled. This member variable is used to store the
     * information whether the used hierarhcical column list is being
     * in marking mode, where it controls the MSK labels and application
     * cannot change them.
     */
    TBool iMarkingModeEnabled;

    };


#endif // C_HIERARCHICALCOLUMNLISTAPPVIEW_H
