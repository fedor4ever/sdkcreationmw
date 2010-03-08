/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Override dialogs provided by the Browser Control
*
*/



#ifndef BRCTLSAMPLEAPPDIALOGSPROVIDER_H
#define BRCTLSAMPLEAPPDIALOGSPROVIDER_H

// INCLUDES
#include <BrCtlInterface.h>

// FORWARD DECLARATIONS
class CBrCtlSampleAppContainer;

// CLASS DECLARATION

/**
* CBrCtlSampleAppDialogsProvider class.
* This class inherits from the MBrCtlDialogsProvider and is used to modify the default
* dialogs that the Browser Control provides
*/
class CBrCtlSampleAppDialogsProvider : public CBase, public MBrCtlDialogsProvider
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CBrCtlSampleAppDialogsProvider* NewL(CBrCtlSampleAppContainer* aContainer);

        /**
        * Destructor.
        */
        ~CBrCtlSampleAppDialogsProvider();

        /**
        * Notify the user of an error
        * @since 2.8
        * @param aErrCode The error that occured
        * @return void
        */
        virtual void DialogNotifyErrorL(TInt /*aErrCode*/)
            {
            };

        /**
        * Notify the user of an http error
        * @since 2.8
        * @param aErrCode The error that occured
        * @param aUri The uri of the request that failed
        * @return void
        */
        virtual void DialogNotifyHttpErrorL(TInt /*aErrCode*/, const TDesC& /*aUri*/)
            {
            };

        /**
        * File selection dialog
        * @since 2.8
        * @param aStartPath The initial displayed directory (drive and path)
        * @param aRootPath The top most directory that the user can go up to (currently set to NULL indicating no restriction)
        * @param aSelectedFileName The selected file name. Returned on cleanup stack. Browser control will free the buffer
        * @return ETrue if file selected; else EFalse (user canceled, etc)
        */
        virtual TBool DialogFileSelectLC(const TDesC& /*aStartPath*/,
                                        const TDesC& /*aRootPath*/,
                                        HBufC*& /*aSelectedFileName*/)
            {
            return EFalse;
            }

        /**
        * List selection dialog
        * @since 2.8
        * @param aTitle The title, could be empty
        * @param aBrCtlSelectOptionType The type of the list box 
        * @param aOptions A list of options to display
        * @return EFalse if the user canceled, ETrue otherwise
        */
        virtual TBool DialogSelectOptionL(const TDesC& /*aTitle*/, 
                                          TBrCtlSelectOptionType /*aBrCtlSelectOptionType*/, 
                                          CArrayFix<TBrCtlSelectOptionData>& /*aOptions*/)
            {
            return EFalse;
            }

        /**
        * User Authentication dialog. User name and password are returned on cleanup stack
        * @since 2.8
        * @param aUrl The url requiring authentication
        * @param aRealm The realm requiring authentication
        * @param aDefaultUserName The user name that was used before for this realm and path, if any
        * @param aReturnedUserName The user name entered by the user
        * @param aReturnedPasswd The password entered by the user
        * @param aBasicAuthentication A flag if basic authentication was used
        * @return EFalse if the user canceled, ETrue otherwise
        */
        virtual TBool DialogUserAuthenticationLC(const TDesC& /*aUrl*/, 
                                                 const TDesC& /*aRealm*/, 
                                                 const TDesC& /*aDefaultUserName*/, 
                                                 HBufC*& /*aReturnedUserName*/, 
                                                 HBufC*& /*aReturnedPasswd*/,
                                                 TBool /*aBasicAuthentication*/ = EFalse)                                                 
            {
            return EFalse;
            }


        /**
        * Display a note to the user. No softkeys
        * @since 2.8
        * @param aMessage The message to display
        * @return void
        */
        virtual void DialogNoteL(const TDesC& /*aMessage*/)
            {
            }

        /**
        * Display a note to the user with ok softkey only
        * @since 2.8
        * @param aTitle The title, could be empty
        * @param aMessage The message to display
        * @return void
        */
        void DialogAlertL(const TDesC& aTitle, const TDesC& aMessage); 

        /**
        * Display confirmation message to the user
        * @since 2.8
        * @param aTitle The title, could be empty
        * @param aMessage The message to display
        * @param aYesMessage The text to display on left softkey
        * @param aNoMessage The text to display on right softkey
        * @return EFalse if the user canceled, ETrue otherwise
        */
        virtual TBool DialogConfirmL(const TDesC& /*aTitle*/,
                                     const TDesC& /*aMessage*/,
                                     const TDesC& /*aYesMessage*/,
                                     const TDesC& /*aNoMessage*/)
            {
            return EFalse;
            }

        /**
        * Display input dialog to the user
        * @since 2.8
        * @param aTitle The title, could be empty
        * @param aMessage The message to display
        * @param aDefaultInput The default input if available
        * @param aReturnedInput The input entered by the user. Returned on the cleanup stack
        * @return EFalse if the user canceled, ETrue otherwise
        */
        virtual TBool DialogPromptLC(const TDesC& /*aTitle*/,
                                     const TDesC& /*aMessage*/,
                                     const TDesC& /*aDefaultInput*/,
                                     HBufC*& /*aReturnedInput*/)
            {
            return EFalse;
            }

        /**
        * Display object info and ask confirmation before download
        * @since 2.8
        * @param aBrCtlObjectInfo The object info
        * @return EFalse if the user canceled, ETrue otherwise
        */
        virtual TBool DialogDownloadObjectL(CBrCtlObjectInfo* /*aBrCtlObjectInfo*/)
            {
            return EFalse;
            }

        /**
        * DIsplay the images that appear in the current page
        * @since 2.8
        * @param aPageImages The images that appear in this page
        * @return vois
        */
        virtual void DialogDisplayPageImagesL(CArrayFixFlat<TBrCtlImageCarrier>& /*aPageImages*/)
            {
            }

        /**
        * Cancel any dialog that is being displayed due to e.g. Browser exit or page was destroyed
        * @since 2.8
        * @return void
        */
        virtual void CancelAll()
            {
            }

        /**
        * Display search on page dialog
        * @since 3.0
        * @return void
        */
        virtual void DialogFindL() 
            {
            } 

    private:
	    /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(CBrCtlSampleAppContainer* aContainer);

    private:   // data
        CBrCtlSampleAppContainer* iContainer;
    };

#endif      // BRCTLSAMPLEAPPOBSERVER_H
            
// End of File

