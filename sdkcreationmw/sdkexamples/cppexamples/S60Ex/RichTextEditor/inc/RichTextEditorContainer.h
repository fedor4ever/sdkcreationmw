/*
* Copyright (c) 2006 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef RTECONTAINER_H
#define RTECONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <eikrted.h>

#include "eiklabel.h"  //CEikLabel

#include "mypicture.h"
// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  CRTEContainer  container control class.
*/
class CRTEContainer : public CCoeControl
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
        ~CRTEContainer();

       /**
        * Toggle -left-center-right- alignment within the actual paragraph
        */
	void SetAlignment(CParaFormat::TAlignment );
		//! Select an area of text
	void SetSelectionL(TInt,TInt);
		//! Swicht on/off the Bold/Italic/Underline attributes
	void SetCharacterBIUAttribL(CEikGlobalTextEditor::TFontStyleFlags );

	#ifdef __SERIES60_3X__
       /**
		* Change the text's font from 3rd Edition onwards
		*/
		void SetFont(TInt aFontId);
	#else
		//! Change the text's font
		void SetFont(const CFont* );
	#endif
       /**
        * Toggle between strikethrough or not strikethrough text
        */
	void Strike();
       /**
		* Change the text's color
		*/
	void SetColor(TRgb );
		//! Demonstrate the how program use some Richtext feature
	void IntroL();

       /**
        * Create Demo Content
        */
		void DemoL();


    public: // Functions from base classes

    private:
	// New functions

		//! Prepare the Richtext Editor, set the Editor flags
		void Prepare();

		//! Apply the final settings after the Editor is activated
		void SetupL();

		//! Append Bitmap to iBitmap Array
		void PrepareBitmapL();
		//! Call Richtext's InsertL() with the CPicture
		void InsertMyPictureL(TInt aPos);

		//! Echo the last engine operation to the status line
	void Echo(TInt );


		// Functions from base classes

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
        * From CCoeControl,OfferKeyEventL.
        */
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

    private: //data

		CEikRichTextEditor* iRtEd; // Rich Text Editor
		CArrayFixFlat<CFbsBitmap*>* iBitmap; // Reference Array for Images
		CEikLabel* iStatusLine; // Status line Label
		TBool iStrike;// Strikethrough Switch
    };

#endif

// End of File
