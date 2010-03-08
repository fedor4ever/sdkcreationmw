/*
 ============================================================================
 Name		: HsWidgetAppUi.h
 Author	  : 
 Copyright   : Your copyright notice
 Description : Declares UI class for application.
 ============================================================================
 */

#ifndef __HSWIDGETAPPUI_H__
#define __HSWIDGETAPPUI_H__

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CHsWidgetAppView;
class CHsWidgetExample;

// CLASS DECLARATION
/**
 * CHsWidgetAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CHsWidgetAppUi : public CAknAppUi
	{
public:
	// Constructors and destructor

	/**
	 * ConstructL.
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CHsWidgetAppUi.
	 * C++ default constructor. This needs to be public due to
	 * the way the framework constructs the AppUi
	 */
	CHsWidgetAppUi();

	/**
	 * ~CHsWidgetAppUi.
	 * Virtual Destructor.
	 */
	virtual ~CHsWidgetAppUi();
	
	/**
	 * DynInitMenuPaneL.
	 * Dynamically initialises a menu pane..
	 * @param aResourceId
	 * @param aMenuPane The in-memory representation of the menu bar. 
	 */
	void DynInitMenuPaneL( TInt aResourceId,CEikMenuPane* aMenuPane );

private:
	// Functions from base classes

	/**
	 * From CEikAppUi, HandleCommandL.
	 * Takes care of command handling.
	 * @param aCommand Command to be handled.
	 */
	void HandleCommandL(TInt aCommand);

	/**
	 *  HandleStatusPaneSizeChange.
	 *  Called by the framework when the application status pane
	 *  size is changed.
	 */
	void HandleStatusPaneSizeChange();

	/**
	 *  From CCoeAppUi, HelpContextL.
	 *  Provides help context for the application.
	 *  size is chaluenged.
	 */
	CArrayFix<TCoeHelpContext>* HelpContextL() const;

private:
	// Data

	/**
	 * The application view
	 * Owned by CHsWidgetAppUi
	 */
	CHsWidgetAppView* iAppView;
	
	CHsWidgetExample* iWidgetExample;
	};

#endif // __HSWIDGETAPPUI_H__
// End of File
