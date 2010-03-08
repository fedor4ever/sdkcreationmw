/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __UEEXFORM_H__
#define __UEEXFORM_H__

// INCLUDES
#include <aknform.h>


// FORWARD DECLARATIONS
class CAknUnitEditor;

// CLASS DECLARATION

/**
 * CUEExForm
 * 
 * This is a subclass of CAknForm. In this example it is used as
 * a simple container for unit editor. The class' ProcessCommandL()
 * contains some usual use cases for the editor. The unit editor itself
 * is created by dialog/form, see resource file ueex.rss for details
 * on that. Unit editor can also be created dynamically using
 * CAknUnitEditor::NewL() and CAknUnitEditor::ConstructL() without
 * the need for any resources. In this example the unit editor is
 * created with the flag EAknUnitEditorAllowUninitialized which
 * in practice means that setting a not a number value to the editor
 * is allowed.
 *
 * Once the unit editor is created, its usage is pretty straightforward.
 * See the private DoSet and DoGet functions for details.
 */
class CUEExForm : public CAknForm
    {
public:
    /**
    * Function from MEikCommandObserver. Handle user menu commands. If a
    * leave occurs the framework generates a Symbian Leave code.
    *
    * @param aCommandId The command identifier to process.
    */ 
    void ProcessCommandL( TInt aCommandId );
    
    /**
    * Function from MEikMenuObserver. The framework 
    * calls this function. It removes the unused default form options. If a 
    * leave occurs the framework generates a Symbian Leave code. 
    * @param aResourceId The ID of the menu pane.
    * @param aMenuPane The menu pane itself.
    */
	void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
	
	/**
	* Overriden from CAknForm not to display the save changes query.
	*
	* @return @c ETrue if changes are requested to be saved and then are 
	* successfully saved. Save of data is not performed here but in 
	* @c SaveFormDataL().
	*/
    TBool QuerySaveChangesL();
    

private:
    /**
     * Shows a query and sets the value to the editor. This shows how
     * to use the function CAknUnitEditor::SetValue().
     *
     * @param aUnitEditor Pointer to the unit editor to modify
     */
    void DoSetValue( CAknUnitEditor* aUnitEditor );
    
    /**
     * Sets a not a number value to the editor. This shows how
     * to set a not a number value to the editor. Note that
     * this requires the editor to have the flag
     * EAknUnitEditorAllowUninitialized set.
     *
     * @param aUnitEditor Pointer to the unit editor to modify
     */
    void DoSetNaN( CAknUnitEditor* aUnitEditor );
    
    /**
     * Shows two queries and sets the editor's min and max
     * values accordingly. This shows how to use
     * CAknUnitEditor::SetMinimumAndMaximum().
     *
     * @param aUnitEditor Pointer to the unit editor to modify
     */
    void DoSetMinMax( CAknUnitEditor* aUnitEditor );
    
    /**
     * Sets a hardcoded localized unit to the editor. There
     * exists a set a predefined localized units for use with
     * the unit editor. These are listed in enum TAknUnitEditorUnits
     * in eikon.hrh. Although not used in this example, there's
     * TBool CAknUnitEditor::SupportsUnit() which can be used
     * to check if the editor supports a certain localized unit in
     * case the localized units are extended in future versions.
     *
     * @param aUnitEditor Pointer to the unit editor to modify
     */
    void DoSetUnit( CAknUnitEditor* aUnitEditor );
    
    /**
     * Sets a hardcoded custom unit to the editor. This shows how to set
     * a custom string as the unit. It also automatically
     * sets the unit type (obtainable with CAknUnitEditor::Unit()) as
     * EAknUnitEditorCustomUnit.
     *
     * @param aUnitEditor Pointer to the unit editor to modify
     */
    void DoSetCustomUnit( CAknUnitEditor* aUnitEditor );
    
    /**
     * Shows a query and sets the maximum fractional digits
     * of the editor. The maximum number of fractional digits
     * supported in the editor is 8. In debug builds, when
     * CAknUnitEditor::SetMaxFractionalDigits() is called with
     * a larger (or negative) value, the editor panics with
     * AKNUNITEDIT 1. 
     *
     * @param aUnitEditor Pointer to the unit editor to modify
     */
    void DoSetMaxFractionalDigits( CAknUnitEditor* aUnitEditor );
    
    /**
     * Shows a query with the current value of the editor. This uses
     * how to get the value in the editor and how to format it to a
     * string. To recreate the whole string the editor is displaying,
     * use CAknUnitEditor::Value() and CAknUnitEditor::GetUnit().
     *
     * @param aUnitEditor Pointer to the unit editor to get the value from
     */
    void DoGetValue( CAknUnitEditor* aUnitEditor );
    };

#endif // __UEEXFORM_H__
