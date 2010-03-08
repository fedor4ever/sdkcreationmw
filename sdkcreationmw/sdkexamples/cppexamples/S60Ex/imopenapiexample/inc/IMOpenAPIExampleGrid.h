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


#ifndef IMOPENAPIEXAMPLEGRID_H
#define IMOPENAPIEXAMPLEGRID_H

// INCLUDES
# include <akngrid.h>
# include "IMOpenAPIExamplemsg.h"

// CLASS DECLARATION
/**
*
* @class	CIMOpenAPIExampleGrid IMOpenAPIExamplegrid.h 
* @brief	This is the graphical grid class for a IM Example application
*
*/
class CIMOpenAPIExampleGrid : public CAknGrid
	{
	public: // Constructors and destructor
	
        /**
        * ConstructL.
        * 2nd phase constructor.
        * @param aGridResource resource of the grip in resource file
        * aImageFile The file name where application images are stored
        */
		void ConstructL(TInt aGridResource, const TDesC& aImageFile);
		
        /**
        * Destructor.
        */
	    ~CIMOpenAPIExampleGrid();
	    
	public: // own functions

        /**
        * SetCellSign.
        * Put the specified graphical cell sign to the specified index in grid
        * @param aCellValue value to put in the cell
        * aCellIndex Index in grid where value need to put 0 to 8
        */
		void SetCellSign(TGameSign aCellValue, TInt aCellIndex);
		
        /**
        * ResetGrid.
        * Reset the whole grid and put empty signs
        */
		void ResetGrid();

	private: // from base classes
        /**
        * SizeChanged.
        * From CoeControl
        */
		void SizeChanged();
		
	private: // own functions

        /**
        * SetupGrid.
        * Initialized the grid
        */
		void SetupGrid();
		
        /**
        * SetupGridIconsL.
        * Initialized the grid icons with the images in given file name
        */
		void SetupGridIconsL(const TDesC& aImageFile);
		
	private: // data
        
        /**
        * iArray.
        * array of icons and text
        */
		CDesCArray* iArray;

	};

#endif	//IMOPENAPIEXAMPLEGRID_H

// End of File

