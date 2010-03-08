/*
 ============================================================================
 Name		: HsWidgetApplication.cpp
 Author	  : 
 Copyright   : Your copyright notice
 Description : Main application class
 ============================================================================
 */

// INCLUDE FILES
#include "HsWidget.hrh"
#include "HsWidgetDocument.h"
#include "HsWidgetApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHsWidgetApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CHsWidgetApplication::CreateDocumentL()
	{
	// Create an HsWidget document, and return a pointer to it
	return CHsWidgetDocument::NewL(*this);
	}

// -----------------------------------------------------------------------------
// CHsWidgetApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CHsWidgetApplication::AppDllUid() const
	{
	// Return the UID for the HsWidget application
	return KUidHsWidgetApp;
	}

// End of File
