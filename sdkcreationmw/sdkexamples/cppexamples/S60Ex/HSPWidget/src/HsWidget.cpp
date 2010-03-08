/*
 ============================================================================
 Name		: HsWidget.cpp
 Author	  : 
 Copyright   : Your copyright notice
 Description : Main application class
 ============================================================================
 */

// INCLUDE FILES
#include <eikstart.h>
#include "HsWidgetApplication.h"

LOCAL_C CApaApplication* NewApplication()
	{
	return new CHsWidgetApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}

