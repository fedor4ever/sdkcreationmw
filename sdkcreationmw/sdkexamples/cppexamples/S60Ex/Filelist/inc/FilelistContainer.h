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


#ifndef FILELISTCONTAINER_H
#define FILELISTCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <eiklbo.h> // MEikListBoxObserver
#include <akngrid.h> // CAknGrid

// CONSTANTS

//Folder IDs
enum TFilelistFolderID
    {
    EPrivateFolder1 = 1,
    EPrivateFolder2
    };
//Grid text length
const TInt KLengthOfItemResourceString = 40;
// In a cell, the area of graphics
const TInt KTextAreaRatio = 3;
// Index of a graphic and text in the resource
const TInt KIndexOfGraphic = 0;
const TInt KIndexOfText = 1;
// The default text color is black.
const TInt KDefaultTextColor = 215;
// The initial number of items to be created.
const TInt KInitialNumOfItemsInSelection = 1;
//Folder path name length
const TInt KPathNameLength = 100;
//Folder name length
const TInt KFolderNameLength = 20;
//Columns in grid
const TInt KGridColumns = 3;
//Rows in grid
const TInt KGridRows = 3;
//Length of empty grid text
const TInt KEmptyGridText = 5;
//Length of error note
const TInt KErrorNoteLength = 100;


// The location of the folders changes in the emulator
// and target builds.
// paths format to private folder
#if defined( __WINS__ )
_LIT(KPrivateFolderPath1, "Z:%Sfolder1\\*.*");
_LIT(KPrivateFolderPath2, "Z:%Sfolder2\\*.*");
_LIT(KProtectedFolderPath, "Z:\\sys\\bin\\*.*");
#else
// drive-neutral path (MMC-installations safe)
_LIT(KPrivateFolderPath1, "%Sfolder1\\*.*");
_LIT(KPrivateFolderPath2, "%Sfolder2\\*.*");
_LIT(KProtectedFolderPath, "\\sys\\bin\\*.*");
#endif
//icon file
_LIT(KIconName, "\\resource\\apps\\filelist.mif");

// FORWARD DECLARATIONS
class CAknNavigationDecorator;
class CAknNavigationControlContainer;
class CDir;

// CLASS DECLARATION
/**
*  CFilelistContainer  container control class.
*  
*/
class CFilelistContainer : public CCoeControl, MEikListBoxObserver
    {
    public: // Constructors and destructor

        /**
        * NewL.
        * Two-phased constructor.
        * Construct a CFilelistContainer using two phase construction,
        * and return a pointer to the created object
        * @return A pointer to the created instance of CFilelistContainer
        */
        static CFilelistContainer* NewL( const TRect& aRect );

        /**
        * NewLC.
        * Two-phased constructor.
        * Construct a CFilelistContainer using two phase construction,
        * and return a pointer to the created object
        * @return A pointer to the created instance of CFilelistContainer
        */
        static CFilelistContainer* NewLC( const TRect& aRect );


        /**
        * Destructor.
        */
        ~CFilelistContainer();

    public: // New functions
    
        /**
        * SetGridInitialParameters.
        * Sets initial parameters for the grid.
        */
        void SetGridInitialParametersL();

        /**
        * CreateGridResourceL.
        * Prepares resources for the grid.
        */
        void CreateGridResourceL();

        /**
        * LoadGraphicsL.
        * Loads graphic resources.
        */
        void LoadGraphicsL();

        /**
        * AddDataL.
        * Adds data to the grid.
        */
        void AddDataL();

        /**
        * ApplySelGridGraphicStyle.
        * Applies the layout to cells in the grid.
        */
        void ApplySelGridGraphicStyle();
        
        /**
        * SetIconSizes.
        * Sets size of the icon in iconarray.
        * @param aIconArray Iconarray
        */
		void SetIconSizes( CArrayPtr<CGulIcon>* aIconArray );
		
        /**
        * CreateFilelist
        * Read files in folder and save them to iFilelist
        */
        void CreateFilelist( );

        /**
        * ChangePrivateFolderL.
        * Retrieves data from another private folder.
        * @param aDestFolder ID for the target folder
        */
        void ChangePrivateFolderL( TInt aDestFolder );    

        /**
        * ChangeProtectedFolderL.
        * Changes to protected folder.
        */
        void ChangeProtectedFolderL();

        /**
        * ShowErrorNoteL.
        * Shows error dialog
        * @param aErr System error code
        */
        void ShowErrorNoteL( TInt aErr );

    public: // Functions from base classes
    
    private: // Functions from base classes
        /**
        * From MEikListBoxObserver,HandleListBoxEventL.
        */
        void HandleListBoxEventL( CEikListBox* aListBox, 
                TListBoxEvent aListBoxEvent );

        /**
        * From CCoeControl,ConstructL.
        * Symbian default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL( const TRect& aRect );

       /**
        * From CoeControl,SizeChanged.
        */
        void SizeChanged();

       /**
        * From CCoeControl,Draw.
        */
        void Draw( const TRect& aRect ) const;

        /**
        * From CCoeControl,CountComponentControls.
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl,ComponentControl.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl,OfferKeyEventL.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                TEventCode aType );
                
		/**
		* From CoeControl, HandleResourceChange.
        * Called by framework when the view layout is changed.
        */
//        virtual void HandleResourceChange( TInt aType );        

    private:  // New functions
        /**
        * SetFolderID.
        * Sets folderID.
        * @param aFolderID FolderID
        */
        void SetFolderID( TInt aFolderID );

        /**
        * GetFolderID.
        * Returns folder ID
        * @return The ID of the current folder. 
        */
        TInt FolderID() const;

        /**
        * SetEmptyTextL.
        * Sets text to empty grid
        */
        void SetEmptyTextL();

        /**
        * SetNaviPaneTextL.
        * Sets text to Navigation Pane
        */
        void SetNaviPaneTextL();

    private: //data
        /**
        * iGrid, the grid shown in this view.
        */
        CAknGrid* iGrid;

        /**
        * iGridM, model for iGrid
        */
        CAknGridM* iGridM;

        /**
        * iVerticalOrientation, vertical orietation status
        */
        TBool iVerticalOrientation;

        /**
        * iLeftToRight, fill order status
        */
        TBool iLeftToRight;

        /**
        * iTopToBottom, fill order status
        */
        TBool iTopToBottom;

        /**
        * iNumOfColumns, number of grid columns
        */
        TInt iNumOfColumns;

        /**
        * iNumOfRows, number of grid rows
        */
        TInt iNumOfRows;

        /**
        * iContentType, type of content
        */
        TInt iContentType;

        /**
        * iSizeOfCell, size of grid cell
        */
        TSize iSizeOfCell;

        /**
        * iNumOfItems, number of grid items
        */
        TInt iNumOfItems;

        /**
        * iVerticalScrollingType, scrolling type
        */
        CAknGridView::TScrollingType iVerticalScrollingType;

        /**
        * iHorizontalScrollingType, scrolling type
        */
        CAknGridView::TScrollingType iHorizontalScrollingType;

        /**
        * iFilelist, file list
        */
        CDir* iFilelist;

        /**
        * iBitmap, grid icon
        */
        CFbsBitmap* iBitmap;

        /**
        * iMask, mask of grid icon
        */
        CFbsBitmap* iMask;

        /**
        * iFolderID, ID to current private folder
        */
        TInt iFolderID;

        /**
        * iNaviDecorator
        */
        CAknNavigationDecorator* iNaviDecorator;

        /**
        * iNaviPane
        */
        CAknNavigationControlContainer* iNaviPane;
        
        /**
        * iSession file server session
        */
        RFs iSession;
    };

#endif

// End of File
