/*
 ============================================================================
 Name		: HsWidgetDocument.h
 Author	  : 
 Copyright   : Your copyright notice
 Description : Declares document class for application.
 ============================================================================
 */

#ifndef __HSWIDGETDOCUMENT_H__
#define __HSWIDGETDOCUMENT_H__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CHsWidgetAppUi;
class CEikApplication;

// CLASS DECLARATION

/**
 * CHsWidgetDocument application class.
 * An instance of class CHsWidgetDocument is the Document part of the
 * AVKON application framework for the HsWidget example application.
 */
class CHsWidgetDocument : public CAknDocument
	{
public:
	// Constructors and destructor

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Construct a CHsWidgetDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CHsWidgetDocument.
	 */
	static CHsWidgetDocument* NewL(CEikApplication& aApp);

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Construct a CHsWidgetDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CHsWidgetDocument.
	 */
	static CHsWidgetDocument* NewLC(CEikApplication& aApp);

	/**
	 * ~CHsWidgetDocument
	 * Virtual Destructor.
	 */
	virtual ~CHsWidgetDocument();

public:
	// Functions from base classes

	/**
	 * CreateAppUiL
	 * From CEikDocument, CreateAppUiL.
	 * Create a CHsWidgetAppUi object and return a pointer to it.
	 * The object returned is owned by the Uikon framework.
	 * @return Pointer to created instance of AppUi.
	 */
	CEikAppUi* CreateAppUiL();

private:
	// Constructors

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CHsWidgetDocument.
	 * C++ default constructor.
	 * @param aApp Application creating this document.
	 */
	CHsWidgetDocument(CEikApplication& aApp);

	};

#endif // __HSWIDGETDOCUMENT_H__
// End of File
