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


// INCLUDE FILES
#include "IMOpenAPIExampleGrid.h"
#include <akniconarray.h> 
#include <aknlists.h> 
#include <barsread.h> 
#include <stringloader.h> 
#include <e32des16.h> 

// CONSTANTS
const TInt KNumberOfIcons(7);

// ----------------------------------------------------------
// CIMOpenAPIExampleGrid::ConstructL(TInt aGridResource, const TDesC& aImageFile)
// Construct a grid with an image file and grid resource
// ----------------------------------------------------------
//
void CIMOpenAPIExampleGrid::ConstructL(TInt aGridResource, const TDesC& aImageFile)
	{
    // Construct the grid from resource
	TResourceReader reader;
	CEikonEnv::Static()->CreateResourceReaderLC(reader, aGridResource);
	ConstructFromResourceL(reader);
	CleanupStack::PopAndDestroy(); // reader

	// Set up the icons
	SetupGridIconsL (aImageFile);

	// set up text and icons in array
    iArray = static_cast<CDesCArray*>(this->Model()->ItemTextArray());
    ResetGrid();

	// Draw the grid
	SizeChanged();
    
	}

// ----------------------------------------------------------
// CIMOpenAPIExampleGrid::~CIMOpenAPIExampleGrid()
// Destructor
// ----------------------------------------------------------
//
CIMOpenAPIExampleGrid::~CIMOpenAPIExampleGrid()
    {
    }

// ----------------------------------------------------------
// CIMOpenAPIExampleGrid::SetupGridIconsL(const TDesC& aImageFile)
// setup grid icons reading from an mbm file
// ----------------------------------------------------------
//
void CIMOpenAPIExampleGrid::SetupGridIconsL(const TDesC& aImageFile)
	{

	// Create an array of icons, reading them from the file
	CArrayPtr<CGulIcon>* icons = new(ELeave) CAknIconArray(KNumberOfIcons);
	CleanupStack::PushL(icons);

	icons->AppendL( iEikonEnv->CreateIconL( aImageFile, 0, 3 ) );
	icons->AppendL( iEikonEnv->CreateIconL( aImageFile, 1, 3 ) );
	icons->AppendL( iEikonEnv->CreateIconL( aImageFile, 2, 3 ) );

	
	
	CleanupStack::Pop(icons);
	// passing ownership of icons
	ItemDrawer()->FormattedCellData()->SetIconArray(icons);
	
	}

// ----------------------------------------------------------
// CIMOpenAPIExampleGrid::SetupGrid()
// setup the grid layout
// ----------------------------------------------------------
//
void CIMOpenAPIExampleGrid::SetupGrid()
	{
	// Setup text foreground and background colours to default
	AknListBoxLayouts::SetupStandardGrid(*this);


	// Get local copies of data we will need	
	CFormattedCellListBoxItemDrawer* itemDrawer = this->ItemDrawer();
	TInt cellWidth = ColumnWidth();
	TInt cellHeight = ItemHeight();

		
	// Set up graphics subcells
	AknListBoxLayouts::SetupFormGfxCell(*this,  //the grid
										itemDrawer, // the grid's drawer
										0, // index of the graphic within item strings
										1, // left position
										1, // top position
										0, // right - unused
										0, // bottom - unused
										cellWidth-4, // width of graphic
										cellHeight-4, // height of graphic
										TPoint (0, 0), // start position
										TPoint (cellWidth , cellHeight)); // end position
    CEikScrollBarFrame* frame = ScrollBarFrame();

	if (frame)
		{
		frame->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff);
		}

	}

// ----------------------------------------------------------
// CIMOpenAPIExampleGrid::SizeChanged()
// Called by framework when the view size is changed and by ConstructL to intialise the grid
// ----------------------------------------------------------
//
void CIMOpenAPIExampleGrid::SizeChanged()
	{
    CAknGrid::SizeChanged();
	SetupGrid();
	}

// ----------------------------------------------------------
// CIMOpenAPIExampleGrid::SetCellSign(TGameSign aCellValue, TInt aCellIndex)
// sets up a single cell sign
// ----------------------------------------------------------
//
void CIMOpenAPIExampleGrid::SetCellSign(TGameSign aCellValue, TInt aCellIndex)
	{
    _LIT(KFormatString, "%d");
	TBuf<1> cellValueString;
	cellValueString.Format(KFormatString, aCellValue);

    // return if index is out of bound
    if ( (aCellIndex>8) || (aCellIndex<0) )
        return;
    iArray->Delete(aCellIndex);
	iArray->InsertL(aCellIndex,cellValueString);
    HandleItemAdditionL();
	}

// ----------------------------------------------------------
// CIMOpenAPIExampleGrid::ResetGrid()
// resets all cell signs to empty
// ----------------------------------------------------------
//
void CIMOpenAPIExampleGrid::ResetGrid()
	{
    _LIT(KFormatString, "%d");
	TBuf<2> cellValueString;

    for (TInt i=0; i<9 ; i++)
        {
        cellValueString.Format(KFormatString, EEmptySign);
        iArray->Delete(i);
	    iArray->InsertL(i,cellValueString);
        }
    HandleItemAdditionL();
	}

// End of File	

