/*
 ============================================================================
 Name		: HsWidgetApplication.h
 Author	  : 
 Copyright   : Your copyright notice
 Description : Declares main application class.
 ============================================================================
 */

#ifndef __HSWIDGETAPPLICATION_H__
#define __HSWIDGETAPPLICATION_H__

// INCLUDES
#include <aknapp.h>
#include "HsWidget.hrh"

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidHsWidgetApp =
	{
	_UID3
	};

// CLASS DECLARATION

/**
 * CHsWidgetApplication application class.
 * Provides factory to create concrete document object.
 * An instance of CHsWidgetApplication is the application part of the
 * AVKON application framework for the HsWidget example application.
 */
class CHsWidgetApplication : public CAknApplication
	{
public:
	// Functions from base classes

	/**
	 * From CApaApplication, AppDllUid.
	 * @return Application's UID (KUidHsWidgetApp).
	 */
	TUid AppDllUid() const;

protected:
	// Functions from base classes

	/**
	 * From CApaApplication, CreateDocumentL.
	 * Creates CHsWidgetDocument document object. The returned
	 * pointer in not owned by the CHsWidgetApplication object.
	 * @return A pointer to the created document object.
	 */
	CApaDocument* CreateDocumentL();
	};

#endif // __HSWIDGETAPPLICATION_H__
// End of File
