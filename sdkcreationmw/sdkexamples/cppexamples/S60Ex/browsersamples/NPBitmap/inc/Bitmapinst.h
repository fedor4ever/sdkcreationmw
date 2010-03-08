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
* Description:  Example for developing a plugin
*
*/


#ifndef BITMAPINST_H
#define BITMAPINST_H

// INCLUDES

#include <e32std.h>
#include <coecntrl.h>
#include <npupp.h>
#include <PluginAdapterInterface.h>

// CLASS DECLARATION

/**
* CBitmapInst is the plugin control class. 
* This class specifies the plugin APIs used to create and destroy 
* plugin instances, handle events, streams, URLs etc 
* 
*/
class CBitmapInst : public CCoeControl, 
                    public MCoeControlObserver, 
                    public MPluginNotifier
    {
    public:
    	/**
		* Two phase construction.
		* @since 3.0
		* @param aNpp - plug-in's opaque instance handle.
		* @return     - plug-in object.
		*/
        static CBitmapInst* NewL ( NPP aNpp );

		/**
		* Destructor
		*/

        ~CBitmapInst();

		/**
		* This function creates a new instance of the plug-in.
		* @since 3.0
		* @param aPluginType - the MIME type.
		* @param aInstance   - the plug-in instance.
		* @param aMode       - the mode
		* @param aArgn       - attribute of the <object> tag; the names
		* @param aArgv       - attribute of the <object> tag; the values
		* @param aSaved      - this parameter is not supported.
		* @return            - NPError status code.
		*/
        NPError PluginNew(
                    NPMIMEType aPluginType, 
                    NPP aInstance, 
                    uint16 aMode, 
                    CDesC16Array* aArgn, 
                    CDesC16Array* aArgv, 
                    NPSavedData* aSaved );
  
		/**
		* This function deletes a plug-in instance.
		* @since 3.0 
		* @param aSave - this parameter is not supported.
		* @return      - NPError status code.
		*/
        NPError PluginDestroy ( NPSavedData** aSave );
  
		/**
		* This function sets the parent and the coordinates for the plug-in. It makes 
		* a call to the ExtractParentControlAndApiL API to extract the platform 
		* dependent window.The first time PluginSetWindowL() is called, it is 
		* important to construct the corresponding plug-in control. The other times 
		* this call is made, it is expected that the function simply updates the 
		* bounds of the control.
		* @since 3.0
		* @param aWindow - a plug-in window structure that contains window coordinates
		*                  and platform specific window information.
		* @return        - an error code, on success returns NPERR_NO_ERROR.
		*/
        NPError PluginSetWindowL ( NPWindow* aWindow );
        
        /**
		* This function has an empty implementation here, but the actual purpose of 
		* the function is to set values for the plugin variables. 
		* @since 3.0
		* @param aVariable - the variable whose value is to be set.
		* @param aValue    - pointer to the 32-bit parameter that contains the value.
		* @return          - NPError status code.
		*/
        NPError PluginSetValue ( NPNVariable aVariable, void *aValue );

        /**
		* This function notifies a plug-in instance of a new data stream.
		* @since 3.0 
		* @param aMimeType - the MIME type of the stream.
		* @param aStream   - the new stream object.
		* @param aSeekable - flag that indicates whether or not stream is searchable.
		* @param aStype    - the type of the stream. The plug-in sets the stream type.
		*                    Currently supported stream types are:	
		*                        NP_NORMAL
		*                        NP_ASFILE
	    *                        NP_ASFILEONLY
		* @return          - NPError status code.
		*/
        NPError PluginNewStream(
                        NPMIMEType aType, 
                        NPStream* aStream, 
                        NPBool aSeekable, 
                        uint16* aStype );

		/**
		* This function destroys the stream that was previously created to stream data
		* to the plug-in.
		* @since 3.0
		* @param aStream - the stream to be destroyed.
		* @param aReason - the reason for destroying the stream. Possible values are:
		*                      NPRES_DONE - normal completion and all data was sent to
		*                      the instance. 
		*                      NPRES_USER_BREAK - the user canceled the stream
		*                      NPRES_NETWORK_ERR - stream failed because of problems 
		*                      with the network, disk I/O error, lack of memory, or 
		*                      some other problem.        
		* @return        - NPError status code.
		*/
        NPError PluginDestroyStream ( NPStream* aStream, NPReason aReason );

		/**
		* This function passes a file name to the plug-in in which the stream data 
		* is stored. 
		* @since 3.0
		* @param aStream   - the stream
		* @param aFileName - the file name
		*/
        void PluginStreamAsFile ( NPStream* aStream, const TDesC16& aFileName );
  
		/**
		* This function writes a chunk of data to the plug-in.
		* @since 3.0
		* @param aStream - the stream
		* @param aOffset - the offset in the stream.
		* @param aLength - the size of the new data.
		* @param aBuffer - the data.
		* @return        - the number of bytes consumed by the plug-in instance.
		*/
        int32 PluginWrite(
                    NPStream* aStream, 
                    int32 aOffset, 
                    int32 aLength, 
                    void* aBuffer );

		/**
		* The browser calls the NPP_Write function with the amount of data returned 
		* from the NPP_WriteReady function.
		* @since 3.0
		* @param aStream - the stream
		* @return        - maximum data size that the plug-in can handle.
		*/
        int32 PluginWriteReady ( NPStream* aStream );
    
		/**
		* The browser calls the NPP_URLNotify function to notify the plug-in of the 
		* completion of a URL request made by the NPN_GetURLNotify function or the 
		* NPN_PostURLNotify function.
		* @since 3.0
		* @param aUrl        - url of the NPN_GetURLNotify function or of the 
		*                      NPN_PostURLNotify function request.
		* @param aReason     - reason code for completion of the request.
		* @param aNotifyData - contains the private plug-in data passed to the 
		*                      corresponding call to the NPN_GetURLNotify function.
		*/
        void PluginURLNotify(
                        const TDesC16& aUrl, 
                        NPReason aReason, 
                        void* aNotifyData );

        // Methods derived from base classes
        
		/**
		* This function implements the NotifyL() function of the MPluginNotifier 
		* interface. It notifies the plug-in of an event. 
		* @since 3.0
		* @param aCallType - the event type that is passed to the plug-in. 
		*                    Possible values are :
		*                    EEditCut, 
		*                    EEditCopy, 
		*                    EEditPaste, 
		*                    EEditDelete, 
		*                    EApplicationFocusChanged, 
		*                    ESystemNotification
		* @param aParam    - the parameter associated with the event.
		* @return          - not used.
		*/
        TInt NotifyL ( TNotificationType aCallType, TAny* aParam ); 

		/**
		* Returns the supported input capabilities which correspond to the behaviour 
		* of the OfferKeyEventL() function of the control; 
		* @since 3.0
		* @return - TCoeInputCapabilities::EAllText which supports all types of text.
		*/
        TCoeInputCapabilities InputCapabilities ( void ) const;
  
		/**
		* This function handles key events. when 'Enter' key or 'OK' key is pressed, 
		* the bitmap rendered is flipped from redflower to blueflower and viceversa.
		* @since 3.0
		* @param aKeyEvent - the key event.
		* @param aType     - type of key event:EEventKey, EEventKeyUp or EEventKeyDown
		* @return          - the function returns EKeyWasNotConsumed if it does not do 
		*                    anything in response to a key event. If it is able to 
		*                    process the event it returns EKeyWasConsumed.
		*/
        TKeyResponse OfferKeyEventL(
                            const TKeyEvent& aKeyEvent,
                            TEventCode aType );

		/**
		* This function inherited from MCoeControlObserver handles an event from an 
		* observed control.
		* @since 3.0
		* @param aControl   - pointer to the control from which the event originated.
		* @param aEventType - the event type.
		*/
        void HandleControlEventL ( CCoeControl* aControl, TCoeEvent aEventType );

		/**
		* This function handles pointer events. On clicking the left mouse button, 
		* the bitmap rendered is flipped from redflower to blueflower and viceversa.
		* @since 3.0
		* @param aPointerEvent - The pointer event.
		*/
        void HandlePointerEventL ( const TPointerEvent& aPointerEvent );
  
		/**
		* This function is called whenever a control gains or loses focus - as a 
		* result of a call to SetFocus(). 
		* @since 3.0
		* @param aDrawNow  - Contains the value that was passed to it by SetFocus().
		*/
        void FocusChanged ( TDrawNow aDrawNow );

		/**
		* This function which responds to size changes to set the size and position of 
		* the contents of the control does nothing and has an empty implementation.
		* @since 3.0
		*/
        void SizeChanged ( void );
  
    private:
		/**
		* Constructor
		*/
        CBitmapInst ( NPP aNpp );

		/**
		* second phase constructor
		*/
        void Construct();

		/**
		* This function draws the control. When iFlipBitmap is true it renders one 
		* bitmap, and when iFlipBitmap is false it renders another one.
		* @since 3.0
		* @param aRect - The region of the control to be redrawn. Co-ordinates are 
		*                relative to the control's origin (top left corner).
		*/
        void Draw ( const TRect& aRect ) const; // derived from CCoeControl

		/**
		* This function calls the NPN_GetURL function to ask the browser to deliver 
		* the data to the plug-in instance in a new stream.
		* @since 3.0
		* @return - if the url is NULL returns NPERR_GENERIC_ERROR, otherwise returns
		*           NPError status code from calling the NPN_GetURL function.
		*/
        NPError HandleGet ( void );

        NPStream*   iStream;
        TInt        iWriteReady;
        TUint16*    iSourceUrl;
        NPP         iNpp;
        TBool       iFirstTime;
        TBool       iFlipBitmap;
        HBufC*      iFileName;
        TInt        iNumDownload;
    };

#endif // BITMAPINST_H

// End of File

