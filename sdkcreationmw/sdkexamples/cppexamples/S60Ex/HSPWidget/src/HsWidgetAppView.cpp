/*
 ============================================================================
 Name		: HsWidgetAppView.cpp
 Author	  : 
 Copyright   : Your copyright notice
 Description : Application view implementation
 ============================================================================
 */

// INCLUDE FILES
#include <coemain.h>
#include "HsWidgetAppView.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CsPWidgetAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHsWidgetAppView* CHsWidgetAppView::NewL(const TRect& aRect)
	{
	CHsWidgetAppView* self = CHsWidgetAppView::NewLC(aRect);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CHsWidgetAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHsWidgetAppView* CHsWidgetAppView::NewLC(const TRect& aRect)
	{
	CHsWidgetAppView* self = new ( ELeave ) CHsWidgetAppView;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
	}

// -----------------------------------------------------------------------------
// CHsWidgetAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHsWidgetAppView::ConstructL(const TRect& aRect)
	{
	// Create a window for this application view
	CreateWindowL();

	// Set the windows size
	SetRect(aRect);

	// Activate the window, which makes it ready to be drawn
	ActivateL();
	}

// -----------------------------------------------------------------------------
// CHsWidgetAppView::CHsWidgetAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CHsWidgetAppView::CHsWidgetAppView()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CHsWidgetAppView::~CHsWidgetAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CHsWidgetAppView::~CHsWidgetAppView()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CHsWidgetAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CHsWidgetAppView::Draw(const TRect& /*aRect*/) const
	{
	// Get the standard graphics context
	CWindowGc& gc = SystemGc();

	// Gets the control's extent
	TRect drawRect(Rect());

	// Clears the screen
	gc.Clear(drawRect);

	}

// -----------------------------------------------------------------------------
// CHsWidgetAppView::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CHsWidgetAppView::SizeChanged()
	{
	DrawNow();
	}
// End of File
