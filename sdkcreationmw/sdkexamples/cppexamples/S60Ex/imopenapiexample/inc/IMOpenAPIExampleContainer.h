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


#ifndef IMOPENAPIEXAMPLECONTAINER_H
#define IMOPENAPIEXAMPLECONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <aknview.h>
#include <e32base.h>
#include <stringloader.h>
#include <EIKMENUB.H>
#include <IMOpenAPIExample.rsg>
#include "IMOpenAPIExamplemsg.h"
   
// FORWARD DECLARATIONS
class CEikLabel;
class CIMOpenAPIExampleGrid;
class CIMOpenAPIExamplemodel;

// CLASS DECLARATION

/**
* CIMOpenAPIExampleContainer  container control class. Most important aspect
* of the class is holding a graphical grid, which displays game actions 
*  
*/
class CIMOpenAPIExampleContainer : public CCoeControl, MCoeControlObserver
    {
    public: // Constructors and destructor
        
        /**
        * EPOC default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect);

        /**
        * Destructor.
        */
        ~CIMOpenAPIExampleContainer();

    public: // Own Functions
    
        /**
        * SetCellSign.
        * Sets the graphical sign of given cell, either empty,cross or circle
        * @param aCellSign sign of the cell
        * aCellIndex position of the cell in the matrix 0 to 8
        */
		void SetCellSign(const TGameSign aCellSign, const TInt aCellIndex);
		
        /**
        * SetMySign.
        * Sets the players own graphical sign either cross or zero
        * @param aMySign Users own sign
        */
		void SetMySign(const TGameSign aMySign);
		
        /**
        * SetGameView.
        * Sets the complete game view according the the give state of the game
        * @param aDesiredView state of the game
        */
		void SetGameView(const TGameState aDesiredView);
		
        /**
        * SetModel.
        * Sets the pointer to model
        * @param aModel pointer to CIMOpenAPIExamplemodel
        */
        void SetModel(CIMOpenAPIExamplemodel* aModel);

        /**
        * PlayMove.
        * Plays a move at focused grid index only if that position is empty
        */
        void PlayMove();

    public: // Functions from base classes
    
        /**
        * ActivateL.
        * From CCoeControl
        */
		void ActivateL();

       /**
        * From CoeControl,SizeChanged.
        */
        void SizeChanged();

       /**
        * From CoeControl,CountComponentControls.
        */
        TInt CountComponentControls() const;

       /**
        * From CCoeControl,ComponentControl.
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

       /**
        * From CCoeControl,Draw.
        */
        void Draw(const TRect& aRect) const;

       	
		/**
		* From MCoeControlObserver
		* Acts upon changes in the hosted control's state. 
		*
		* @param	aControl	The control changing its state
		* @param	aEventType	The type of control event 
		*/
        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);

        /**
        * From CCoeControl.
        * Sets the complete game view according the the give state of the game
        */
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
															TEventCode aType);

	private:	// own methods
	
        /**
        * LoadMySignBMPL.
        * Load the MBM file with given filename. MBM file contains images
        * used in this application
        * @param aImageFile filename
        */
		void LoadMySignBMPL(const TDesC& aImageFile);


    private: //data
    
        /**
        * iTurnIndicator.
        * Pointer to CEikLabel, a text label which shows whos turn it is
        */
        CEikLabel* iTurnIndicator;
        
        /**
        * iSignIndicator.
        * Pointer to CEikLabel, shows the players own sign text label
        */
        CEikLabel* iSignIndicator;
        
        /**
        * iCrossSign.
        * Pointer to CFbsBitmap, Graphic of cross sign
        */
		CFbsBitmap* iCrossSign;
		
        /**
        * iZeroSign.
        * Pointer to CFbsBitmap, Graphic of circle sign
        */
		CFbsBitmap* iZeroSign;

        /**
        * iEmptySign.
        * Pointer to CFbsBitmap, Graphic of an empty sign
        */
		CFbsBitmap* iEmptySign;
		
        /**
        * iMySignBMP.
        * Pointer to CFbsBitmap, Graphic of user's own sign, cross or circle
        */
		CFbsBitmap* iMySignBMP;
		
        /**
        * iMySign.
        * User's own sign in enum format
        */
		TGameSign iMySign;
		
        /**
        * iImageFileName.
        * Name of the image file where images used by this application are stored
        */
        TFileName iImageFileName;
        
        /**
        * iModel.
        * Pointer to CIMOpenAPIExamplemodel
        */
        CIMOpenAPIExamplemodel* iModel;
        
        /**
        * iGamesGrid.
        * Pointer to CIMOpenAPIExampleGrid, container hold this object
        */
        CIMOpenAPIExampleGrid* iGamesGrid;
     
        /**
        * iCba.
        * To help draw screen correctly
        */
        CEikButtonGroupContainer* iCba;
    };



#endif //IMOPENAPIEXAMPLECONTAINER_H

// End of File

