/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef LAYOUT_FORM_H
#define LAYOUT_FORM_H

#include <aknform.h> // CAknForm

class TOCRLayoutBlockInfo;
class TOCRLayoutSetting;
class TRegionInfo;

/**
 * namespace contains various utility classes
 */
namespace NOCRUtils
{
   /**
    * CLayoutForm is a tailored form-dialog to read various settings from user.
    * In OCR Example application forms are used to inquiry settings in
    * layout analysis and block recognition operations. This same form-class
    * can be used in all three cases.
    * The whole operation is executed with one static function.
    *
    * This form sets up a dialog for choosing correct settings for operation,
    * and the dialog has only Ok-softkey enabled. When user presses Ok, the
    * current settings are saved to appropriate datastructure.
    */
    class CLayoutForm : public CAknForm
        {
        public:
            /**
            * Creates, executes and destroys the form dialog.
            * This variant is used to ask settings for Layout Analysis.
            * @param aInfo data-structure to store settings
            * @param aResourceId resource-id of the form-dialog resource
            * @return status from ExecuteLD()
            */
            static TInt ExecuteFormLD(TOCRLayoutBlockInfo& aInfo,
                                      TInt aResourceId);
            /**
            * Creates, executes and destroys the form dialog.
            * This variant is used to ask settings for Block Recognition.
            * @param aInfo data-structure to store settings
            * @param aResourceId resource-id of the form-dialog resource
            * @return status from ExecuteLD()
            */
            static TInt ExecuteFormLD(TOCRLayoutSetting& aInfo,
                                      TInt aResourceId);
            /**
            * Creates, executes and destroys the form dialog.
            * This variant is used to ask settings for Special Region Recognition.
            * @param aInfo data-structure to store settings
            * @param aResourceId resource-id of the form-dialog resource
            * @return status from ExecuteLD()
            */
            static TInt ExecuteFormLD(TRegionInfo& aInfo,
                                      TInt aResourceId);
        protected:
            /**
            * implements a workaround to cause form to exit after pressing Ok
            */
            TBool OkToExitL( TInt aButtonId );

        private:

            /**
            * from CAknForm
            * Normally called by the framework whenever the 'Save' menu item is
            * selected, and by the QuerySaveChangesL
            * method when the user answers yes to the save query.
            * Saves the data from the forms controls, into the data-structures.
            * Note that in this particular form-class data is always implicitly
            * saved without asking the user.
            * @return always returns ETrue
            */
        	TBool SaveFormDataL();

            /**
            * @return always returns ETrue to trigger call to SaveFormDataL()
            */
        	TBool QuerySaveChangesL();

        private:

           /**
            * constructor
            * note that only one data-structure is allowed
            */
        	CLayoutForm( TOCRLayoutBlockInfo *aLayoutBlockInfo,
        	             TOCRLayoutSetting   *aLayoutSetting = NULL,
        	             TRegionInfo         *aRegionInfo = NULL)
        	    : iBlock(aLayoutBlockInfo)
        	    , iLayout(aLayoutSetting)
        	    , iRegion(aRegionInfo)
            {}

        private:
            /** settings for Block Recognition operation */
        	TOCRLayoutBlockInfo *iBlock;

            /** settings for Layout Analysis operation */
        	TOCRLayoutSetting   *iLayout;

            /** settings for Special Region Recognition operation */
        	TRegionInfo         *iRegion;
    	};

} // namespace NOCRUtils

#endif
