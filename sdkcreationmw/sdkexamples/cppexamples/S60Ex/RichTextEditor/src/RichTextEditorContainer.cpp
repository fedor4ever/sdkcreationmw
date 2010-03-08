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


// INCLUDE FILES
#include "RichTextEditorContainer.h"
#include <txtrich.h>	//Richtext
#include <eikrted.h>  	//RichText Editor
#include <avkon.mbg> 	// Image Ids 
#include <eikenv.h> 	// ReadResource() 
#include <RichTextEditor.rsg> // Demo Text Buffer
#include "mypicture.h"   // CMyPicture 
#include <AknUtils.h>	

#ifdef __SERIES60_3X__
#include <eikapp.h>
#endif

// ================= MEMBER FUNCTIONS =======================

// Demo shows the T9 (Front End Processor) ,Scroller,
// "Insert Picture", disable Copy-Paste feature
// in RichText Editor

// CONSTANTS
_LIT( KImageBitmapFile, "picture.mbm");
//Text and images positions.
const TInt KLeftImagePositionInTheText	= 21;
const TInt KRightImagePositionInTheText	= 38;
const TInt KEndofCenterFormat			= 55;
const TInt KLeftNokia					= 6;
const TInt KRightNokia					= 11;
const TInt KKImageWidth					= 283;
const TInt KImageHeight					= 113;
//buffer size
const TInt KBufSize = 256;

// ---------------------------------------------------------
// CRTEContainer::Prepare()
// Set the Editor flags
// ---------------------------------------------------------
//
void CRTEContainer::Prepare()
	{

	// Lower Case 
	iRtEd->SetAknEditorCase(EAknEditorLowerCase);
	// Fixed case
	
	iRtEd->SetAknEditorFlags
		(
		EAknEditorFlagFixedCase| //Set up the Allowed Cases
		EAknEditorFlagEnableScrollBars // Set up the scroller
		);
	}

// ---------------------------------------------------------
//  CRTEContainer::PreparePicture()
//  Append Bitmap to iBitmap Array
// ---------------------------------------------------------
//
void CRTEContainer::PrepareBitmapL()
	{
	#ifndef __SERIES60_3X__
	TFileName name(KImageBitmapFile);
	CompleteWithAppPath( name );
	// windows build wants to install apps on rom drive ( "z:\" )
	// but we want the data from "c:\" instead
	#ifdef __WINS__
	name[ 0 ] = 'C';
	#endif

    #else // 3rd ed

    RFs fsSession;
    User::LeaveIfError(fsSession.Connect());
    CleanupClosePushL( fsSession );
    TFileName name;
    fsSession.PrivatePath( name );
    name.Append( KImageBitmapFile );

	#ifndef __WINS__
    // insert the drive to the private path
    TParsePtrC parse(
        (CEikonEnv::Static()->EikAppUi()->Application())->AppFullName() );
    name.Insert( 0, parse.Drive() );
    #endif

	#endif

	// Create and Load the Bitmap
	CFbsBitmap* bitmap = new( ELeave )CFbsBitmap;
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Load(name));
	//we have to free the memory at the Destructor() -> store the bitmap memory address
	iBitmap->AppendL(bitmap);

	CleanupStack::Pop(); //bitmap - container free bitmap at the destructor
	#ifdef __SERIES60_3X__
    CleanupStack::PopAndDestroy(1, &fsSession);
    #endif
	// last item in iBitmap array is ready to Insert
	}

// ---------------------------------------------------------
//  CRTEContainer::InsertMyPictureL()
//  Call Richtext's InsertL() with the CPicture
// ---------------------------------------------------------
//
void CRTEContainer::InsertMyPictureL(TInt aPos)
	{
	CMyPicture* picture;
	// Create a CPicture derived class which will draw our image, depending this Size
	picture = new( ELeave )CMyPicture(TSize(KKImageWidth,KImageHeight),
		*(iBitmap->At(iBitmap->Count()-1)/*process the last item of iBitmap*/));
	CleanupStack::PushL(picture);
	// Prepare the Picture header, which will be instered into the Richtext
	TPictureHeader header;
	header.iPicture =TSwizzle<CPicture>(picture);
	iRtEd->RichText()->InsertL( aPos,header); 
	CleanupStack::Pop(); // picture - Richtext take the ownership 
	}

// ---------------------------------------------------------
//  CRTEContainer::Demo()
//  Create Demo Text Content 
//
//  Selecting text as holding the 'ABC'+ joystick 
//  Copy/Paste working through longpress 'ABC' and selecting the 
//  softkey which labelled as our need
// 
//  Clipboard content remain even when example closed
// ---------------------------------------------------------
//
void CRTEContainer::DemoL()
	{
	iRtEd->SetCursorPosL(0,EFalse);
	// Set the Demo text to Richtext Editor
	TBuf<KBufSize> Demobuf;
	CEikonEnv::Static()->ReadResource(Demobuf, R_TYPE_II_DIALOG_TBUF_DEMO_TEXT);
	iRtEd->SetTextL(&Demobuf);
	
	// Insert a Bipmaps to Richtext 
	InsertMyPictureL(KLeftImagePositionInTheText);
	InsertMyPictureL(KRightImagePositionInTheText);
	
	//// To Compare the '\f' to EParagraphDelimiter change the comment status of the two following lines
	//iRtEd->RichText()->InsertL( KEndofCenterFormat , CEditableText::EParagraphDelimiter);
	_LIT(KFormat, "\f");
	iRtEd->RichText()->InsertL( KEndofCenterFormat , KFormat);

	// Basic Text Formatting border by EParagraphDelimiter, same as in Forum Nokia's Type - Example
	iRtEd->RichText()->InsertL( KLeftImagePositionInTheText , CEditableText::EParagraphDelimiter);
	
	// Do some color change
	SetSelectionL(KLeftImagePositionInTheText, KRightImagePositionInTheText);
	SetColor(KRgbRed);
	SetSelectionL(KLeftNokia, KRightNokia);
	SetColor(KRgbBlue);
	
	//set newline end of document
	iRtEd->RichText()->InsertL(iRtEd->Text()->DocumentLength() , CEditableText::ELineBreak);
	
	// Set the cursor end of the document
	iRtEd->SetCursorPosL(iRtEd->Text()->DocumentLength(), EFalse);
	}
	
void CRTEContainer::SetupL()
	{
    // One way to remove the selection
	//iRtEd->ClearSelectionL();
	
	// Copy-Paste Support is allowed in Edwin but we can disable
	// to do that uncomment the following line
	//iRtEd->EnableCcpuSupportL(EFalse);
	iRtEd->EnableCcpuSupportL(ETrue);
	// Another way to remove the selection.
	iRtEd->SetCursorPosL(0, EFalse);
	
	iRtEd->UpdateScrollBarsL();
	SizeChanged();
	}

// ---------------------------------------------------------
// CRTEContainer::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CRTEContainer::ConstructL(const TRect& aRect)
    {
	CreateWindowL();

	iBitmap = new (ELeave) CArrayFixFlat<CFbsBitmap *>(2); // DemoL() show two image	
	iRtEd = new (ELeave) CEikRichTextEditor;
	
	iStatusLine = new (ELeave) CEikLabel;
	iStatusLine->SetAlignment(EHCenterVCenter);

	Prepare();
	PrepareBitmapL();
	
	iRtEd->ConstructL(this,0,0,0);
	iRtEd->SetFocus(ETrue);
	
	SetRect(aRect);

	IntroL();
	ActivateL();
	SetupL();
    }

// ---------------------------------------------------------
// CRTEContainer::~CRTEContainer()
// Destructor - Free the allocated resources
// ---------------------------------------------------------

CRTEContainer::~CRTEContainer()
    {
    if(iRtEd)
		delete iRtEd;
	
	if(iStatusLine)
		delete iStatusLine;
	 	
	// Delete each image, which have reference in the Array
	for(int i=0;i<iBitmap->Count();i++)
		{
		if(iBitmap->At(i))
			delete iBitmap->At(i);
		}
	// Free the Array
	if(iBitmap)
		delete iBitmap;
    }

// ---------------------------------------------------------
// CRTEContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CRTEContainer::SizeChanged()
    {
	TRect rect = Rect();
	TInt StatusLineHeight = iStatusLine->Font()->HeightInPixels()+6;
	iStatusLine->SetExtent(TPoint(0, 0),TSize(rect.Width(), StatusLineHeight));
	
	#ifdef __SERIES60_3X__
	TRect ScrollBarRect = iRtEd->ScrollBarFrame()->VerticalScrollBar()->Rect();
	//In 3rd edition CEikRichTextEditor draw the view for the whole rect and
	//the scrollbar doesn't show. That is a reason why CEikRichTextEditor Width() is
	//rect.Width()-ScrollBarRect.Width()
	iRtEd->SetExtent(TPoint(0,StatusLineHeight), 
		TSize(rect.Width()-ScrollBarRect.Width(), rect.Height()-StatusLineHeight));
	#else
    iRtEd->SetExtent(TPoint(0,StatusLineHeight), 
    	TSize(rect.Width(), rect.Height()-StatusLineHeight));
    #endif
    }

// ---------------------------------------------------------
// CRTEContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CRTEContainer::CountComponentControls() const
    {
    return 2; // return number of controls inside this container
    }

// ---------------------------------------------------------
// CRTEContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CRTEContainer::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
			return iRtEd;
		case 1:
			return iStatusLine;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CRTEContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CRTEContainer::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle(CGraphicsContext::ENullPen);
    gc.SetBrushColor(KRgbWhite);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.DrawRect(aRect);
    }

// ---------------------------------------------------------
// OfferKeyEventL() 
// Distribute the key event to the Editor
// ---------------------------------------------------------
//
TKeyResponse CRTEContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
	{
	if (iRtEd->IsFocused())
		{
		return iRtEd->OfferKeyEventL(aKeyEvent, aType);	
		}
	else
		{
		return EKeyWasNotConsumed;
		}
	}

// ----------------------------------------------------
// SetAlignment(CParaFormat::TAlignment aAlignment)
//
// Alignment formatting apply to the actual paragraph
// If there is only one, it apply for the whole text
//
// We expect one of the following input parameter value:
//	CParaFormat::ELeftAlign
//	CParaFormat::ECenterAlign
//	CParaFormat::ERightAlign
//
// ----------------------------------------------------
void CRTEContainer::SetAlignment(CParaFormat::TAlignment aAlignment)
	{
	CParaFormat     paraFormat;
	TParaFormatMask paraFormatMask;

	paraFormatMask.SetAttrib(EAttAlignment);    // set the mask
	paraFormat.iHorizontalAlignment = aAlignment;

	iRtEd->ApplyParaFormatL(&paraFormat, paraFormatMask);
	
	Echo(R_TYPE_DIALOG_TBUF_INDICATOR_ALIGNMENT);
	}

// ----------------------------------------------------
// SetSelectionL(TInt aCursorPos,TInt aAnchorPos)
// Select a text in the Rich text editor
// ----------------------------------------------------
void CRTEContainer::SetSelectionL(TInt aCursorPos,TInt aAnchorPos)
	{
	iRtEd->SetSelectionL(aCursorPos,aAnchorPos);
	}

// ----------------------------------------------------
// SetCharacterBIUAttrib(CEikGlobalTextEditor::TFontStyleFlags aFontFlags)
// 
// Character formating apply for the selected text
// If nothing is selected it apply to actual word
// We expect one of the following input parameter value:
//	CEikGlobalTextEditor::EBold
//	CEikGlobalTextEditor::EItalic
//	CEikGlobalTextEditor::EUnderline
// ----------------------------------------------------

void CRTEContainer::SetCharacterBIUAttribL(CEikGlobalTextEditor::TFontStyleFlags aFontFlags)
	{
	iRtEd->BoldItalicUnderlineEventL(aFontFlags);
	Echo(R_TYPE_DIALOG_TBUF_INDICATOR_CHARACTER_FORMAT);
	}
	
#ifdef __SERIES60_3X__
// ----------------------------------------------------
// SetFont(TInt aFontId)
// Change the text's font in the editor 
// to the one specified as input parameter for the function 
// ----------------------------------------------------
void CRTEContainer::SetFont(TInt aFontId)
	{	
	// Get a logical font to base my font on:
	const CFont* logicalFont = AknLayoutUtils::FontFromId(aFontId);
	// Extract font information
	TFontSpec fontspec = logicalFont->FontSpecInTwips();

    TCharFormat charFormat(fontspec.iTypeface.iName, fontspec.iHeight);
    TCharFormatMask charFormatMask;

    charFormatMask.SetAttrib(EAttFontTypeface);
    charFormatMask.SetAttrib(EAttFontHeight);

	iRtEd->ApplyCharFormatL(charFormat, charFormatMask);

	Echo(R_TYPE_DIALOG_TBUF_INDICATOR_FONT);
	}
#else
// ----------------------------------------------------
// SetFont(const CFont *aFont)
// Change the text's font in the editor 
// to the one specified as input parameter for the function 
// ----------------------------------------------------
void CRTEContainer::SetFont(const CFont *aFont)
	{	
		/*
	TCursorSelection cs = iRtEd->Selection();
	
	SetSelectionL(cs.iCursorPos, cs.iAnchorPos-1);
	*/
    TFontSpec fontspec = aFont->FontSpecInTwips();
    TCharFormat charFormat( fontspec.iTypeface.iName, fontspec.iHeight );
    TCharFormatMask charFormatMask;

    charFormatMask.SetAttrib(EAttFontTypeface);
    charFormatMask.SetAttrib(EAttFontHeight);
	iRtEd->ApplyCharFormatL(charFormat, charFormatMask);

	Echo(R_TYPE_DIALOG_TBUF_INDICATOR_FONT);
	}
#endif
// ----------------------------------------------------
// Strike()
// Toggle between strikethrough or not strikethrough text
// It depend only bool switch in the code, which turn to opposite in every 
// case Strikethrough selected from the menu.
// ----------------------------------------------------
void CRTEContainer::Strike()
	{
    TCharFormat charFormat;
	TCharFormatMask charFormatMask;

	iStrike=!iStrike;
	charFormat.iFontPresentation.iStrikethrough=iStrike?EStrikethroughOn:EStrikethroughOff;
    charFormatMask.SetAttrib(EAttFontStrikethrough);
	iRtEd->ApplyCharFormatL(charFormat, charFormatMask);

	if(iStrike)
		{
		Echo(R_TYPE_DIALOG_TBUF_INDICATOR_STRIKE_ON);
		}
	else
		{
		Echo(R_TYPE_DIALOG_TBUF_INDICATOR_STRIKE_OFF);
		}
	}
	
// ----------------------------------------------------
// SetColor(TRgb aColor)
// Set the selected text color based on the input parameter
// If nothing selected, text color changed only the cursor position.
// ----------------------------------------------------
void CRTEContainer::SetColor(TRgb aColor)
	{
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;

	charFormat.iFontPresentation.iTextColor = aColor;
	charFormatMask.SetAttrib(EAttColor);
	iRtEd->ApplyCharFormatL(charFormat, charFormatMask);
	
//	iRtEd->SetBackgroundColorL(aColor); 
//  It is possible to change the background color - if foreground and background 
//  color is same, the text became invisible.
	}

// ----------------------------------------------------
// Echo(TInt aMessage)
// Echo the last operation to the status line
// ----------------------------------------------------
void CRTEContainer::Echo(TInt aMessage)
	{
	TBuf<32> buf;
	CEikonEnv::Static()->ReadResource(buf, aMessage);
	iStatusLine->SetTextL(buf);
	}

// ----------------------------------------------------
// IntroL()
// Create Intro Text.
// ----------------------------------------------------
void CRTEContainer::IntroL()
	{
	TBuf<KBufSize> Introbuf;
	CEikonEnv::Static()->ReadResource(Introbuf, R_TYPE_DIALOG_TBUF_INTRO_TEXT);

	TBuf<KBufSize> HelpText;
	CEikonEnv::Static()->ReadResource(HelpText, R_TYPE_DIALOG_TBUF_INTRO_HELP_TEXT);
	HelpText.Append(CEditableText::ELineBreak);

	//Start the center format.
	SetAlignment(CParaFormat::ECenterAlign);
	
	//Set the text.
	iRtEd->SetTextL(&Introbuf);
	iRtEd->RichText()->InsertL(iRtEd->Text()->DocumentLength()/* at the end*/, HelpText);
	
	Echo(R_TYPE_DIALOG_TBUF_INDICATOR_DEMO);
	}

// End of File
