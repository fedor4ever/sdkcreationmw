/*
 ============================================================================
 Name		: HsWidgetDocument.cpp
 Author	  : 
 Copyright   : Your copyright notice
 Description : CHsWidgetDocument implementation
 ============================================================================
 */

// INCLUDE FILES
#include "HsWidgetAppUi.h"
#include "HsWidgetDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHsWidgetDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHsWidgetDocument* CHsWidgetDocument::NewL(CEikApplication& aApp)
	{
	CHsWidgetDocument* self = NewLC(aApp);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CHsWidgetDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHsWidgetDocument* CHsWidgetDocument::NewLC(CEikApplication& aApp)
	{
	CHsWidgetDocument* self = new ( ELeave ) CHsWidgetDocument( aApp );

	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CHsWidgetDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHsWidgetDocument::ConstructL()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CHsWidgetDocument::CHsWidgetDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CHsWidgetDocument::CHsWidgetDocument(CEikApplication& aApp) :
	CAknDocument(aApp)
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CHsWidgetDocument::~CHsWidgetDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CHsWidgetDocument::~CHsWidgetDocument()
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CHsWidgetDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CHsWidgetDocument::CreateAppUiL()
	{
	// Create the application user interface, and return a pointer to it;
	// the framework takes ownership of this object
	return new ( ELeave )CHsWidgetAppUi;
	}

// End of File
