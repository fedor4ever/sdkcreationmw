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




#ifndef __GUIENGINEAPPVIEW_H__
#define __GUIENGINEAPPVIEW_H__


// INCLUDES
#include <coecntrl.h>
#include "ShapeListManager.h"


// CONSTANTS
static const TInt KBlackPointer = 1;
static const TInt KBrushRadius = 5;
static const TInt KBrushHeight = 10;
static const TInt KBrushWidth = 10;

// FORWARD DECLARATIONS
class   CGuiEngineDocument;

// CLASS DECLARATION

/**
* CGuiEngineAppView
* An instance of class CGuiEngineAppView is the Application View object for the
* GUI / Engine example
*/
class CGuiEngineAppView : public CCoeControl
    {
    public: // Constructors and destructor


        /**
        * TBrushShapeType
        * The shape type to be drawn
        * @param ECircle user last drew a circle
        * @param  ERectangle user last drew a rectangle
        */
        enum TBrushShapeType { ECircle = 0,
                               ERectangle };
        /**
        * NewL.
        * Two-phased constructor.
        * Create a CGuiEngineAppView object, which will draw itself to aRect
        * @param aRect A pointer to the rectangle that defines the size
        *  and location of the displayable area for the view
        * @param aDocument the document
        * @return a pointer to the created instance of CGuiEngineAppView.
        */
        static CGuiEngineAppView* NewL ( const TRect& aRect,
                                        CGuiEngineDocument* aDocument );
        /**
        * NewLC
        * Two-phased constructor.
        * Create a CGuiEngineAppView object, which will draw itself to aRect
        * @param aRect A pointer to the rectangle that defines the size
        *  and location of the displayable area for the view
        * @param aDocument the document
        * @return a pointer to the created instance of CGuiEngineAppView.
        */
        static CGuiEngineAppView* NewLC( const TRect& aRect,
                                         CGuiEngineDocument* aDocument );

        /**
        * ~CGuiEngineAppView.
        * Virtual Destructor.
        */
        virtual ~CGuiEngineAppView();


    public: // New Functions

        /**
        * Clear
        * Clear the view / model
        */
        void Clear();

        /**
        * SetBrushShapeType
        * Set the type of shape that will be added to the list
        * @param aBrushShapeType the shape type of the brush
        */
        void SetBrushShapeType( TBrushShapeType aBrushShapeType );

        /**
        * BrushShapeType
        * Get the type of shape that will be added to the list
        * @return Returns the type of shape that will be added.
        */
        TBrushShapeType BrushShapeType() const;

        /**
        * ExternalizeL
        * Save the view settings to a stream
        * @param aStream the stream to save the data to.
        */
        void ExternalizeL( RWriteStream& aStream ) const;

        /**
        * InternalizeL
        * Read the view settings from a stream
        * @param aStream the stream to read the data from.
        */
        void InternalizeL( RReadStream& aStream );


    public: // from CCoeControl

        /**
        * Draw
        * Draw this to the screen.
        * @param aRect the rectangle of this view that needs updating
        */
        void Draw( const TRect& aRect ) const;

        /**
        * OfferKeyEventL
        * From CCoeControl, Handles key event.
        * @param aKeyEvent The key event.
        * @param aType The type of the event.
        * @return Indicates whether or not the key event was used
        * by this control.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType);

		/** HandleResourceChange
		* From CCoeControl. Handles a change to the control's resources.
		* @param aType A message UID value
		*/
		void HandleResourceChange( TInt aType );


    private: // Constructors and destructor

        /**
        * CGuiEngineAppView.
        * C++ default constructor.
        * @param aDocument the model
        */
        CGuiEngineAppView( CGuiEngineDocument* aDocument );

        /**
        * ConstructL.
        * 2nd phase constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL( const TRect& aRect );

    private:    // Data


        /**
        * iDocument,  The current document
        * Not owned by CGuiEngineAppView object.
        */
        CGuiEngineDocument* iDocument;

        /** iBrushShapeType The type of shape currently selected for adding */
        TBrushShapeType iBrushShapeType;

        /** iPosition The current 'cursor' position */
        TPoint iPosition;

        };


#endif // __GUIENGINEAPPVIEW_H__


// End of File
