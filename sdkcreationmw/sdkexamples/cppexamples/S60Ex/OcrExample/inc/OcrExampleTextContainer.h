/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef OCREXAMPLETESTCONTAINER_H
#define OCREXAMPLETESTCONTAINER_H

// INCLUDES
#include <coecntrl.h>

class CEikGlobalTextEditor;

/**
* TTextContainerData-struct maintains state-information of TextContainer
* This is a POD-type, but offers a constructor to initialize fields to default
* values.
*/
struct TTextContainerData
    {
    /** ctor. Sets the default values */
    TTextContainerData(): iStartPosition(0) {}

    /** position of cursor, when the view is activated */
    TInt    iStartPosition;
    };

/**
 * COcrExampleTextContainer
 * this container implements a "console view", which shows the results of
 * layout analysis and recognition operations. The text is read from the data
 * file, into which the image handling controller writes the data.
 */
class COcrExampleTextContainer : public CCoeControl
    {
    public:
		/**
		* NewL.
		* Two-phased constructor.
		* Construct a COcrExampleTextContainer using two phase construction,
		* and return a pointer to the created object
		* @return A pointer to the created instance of COcrExampleTextContainer
		*/
		static COcrExampleTextContainer* NewL(const TRect& aRect,
		                                      TTextContainerData& aData );
        /** Destructor. */
        virtual ~COcrExampleTextContainer();

        /** imports the content of datafile into read-only editor */
        void ReadTextFromFileL();

        /** sets cursor's starting position, when the view is activated */
        void AdjustCursorL();

    private:
        /** C++ constructor. */
        COcrExampleTextContainer( TTextContainerData& aData );

        /** Symbian OS default constructor. */
        void ConstructL(const TRect& aRect);

    public: // Functions from base classes

        /** From CCoeControl to activate container */
        void ActivateL();

    protected: // Functions from base classes

        /** From CCoeControl set the size and position of its controls. */
        void SizeChanged();

        /**
        * From CCoeControl return the number of controls owned
        * @return TInt number of controls
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl returns a control
        * @param aIndex index of a control
        * @return CCoeControl* pointer on a control
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
        * From CCoeControl event handling section
        * @param aKeyEvent the key event
        * @param aType the type of the event
        * @return TKeyResponse key event was used by this control or not
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

        /** From CCoeControl */
	    void Draw(const TRect& aRect) const;

    private: //data

        /** pointer to read-only editor */
	    CEikGlobalTextEditor*   iEditor;

        /** state-data of this container */
	    TTextContainerData&     iData;
    };

#endif  //OCREXAMPLETESTCONTAINER_H

// End of File
