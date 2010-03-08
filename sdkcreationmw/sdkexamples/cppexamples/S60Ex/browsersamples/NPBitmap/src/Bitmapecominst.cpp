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
* Description:  Example for developing a plug-in
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <coecntrl.h>
#include <coeinput.h>
#include "bitmapinst.h"
#include "bitmapecommain.h"
#include <allbitmaps.mbg>

// CONSTANTS

_LIT(KSourceUrl, "sourceurl");


// ============================================================================
// CBitmapInst Class
// ============================================================================


// ----------------------------------------------------------------------------
// Two phase construction.
//
// @param aNpp - plug-in's opaque instance handle.
// @return     - plug-in object.
// ----------------------------------------------------------------------------
CBitmapInst* CBitmapInst :: NewL( NPP aNpp )
    {
    CBitmapInst* self = new ( ELeave ) CBitmapInst( aNpp );
    CleanupStack :: PushL( self );
    self->Construct();
    CleanupStack :: Pop();
    return self;
    }


// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
CBitmapInst :: CBitmapInst( NPP aNpp ) : CCoeControl(), iNpp( aNpp )
    {
    iWriteReady     = 100;
    iSourceUrl      = NULL;
    iFirstTime      = ETrue;
    iFlipBitmap     = EFalse;
    iNumDownload    = 0;
    }


// ----------------------------------------------------------------------------
// second phase constructor
// ----------------------------------------------------------------------------
void CBitmapInst :: Construct( void )
    {
    return;
    }


// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
CBitmapInst :: ~CBitmapInst( void )
    {
    if ( iFileName )
        {
	    delete iFileName;
	    iFileName = NULL;
        }
    CBitmapEcomMain* npm = ( CBitmapEcomMain* ) Dll :: Tls();
    delete iSourceUrl;
    }


// ----------------------------------------------------------------------------
// This function creates a new instance of the plug-in.
//
// @param aPluginType - the MIME type.
// @param aInstance   - the plug-in instance.
// @param aMode       - the mode
// @param aArgn       - attribute of the <object> tag; the names
// @param aArgv       - attribute of the <object> tag; the values
// @param aSaved      - this parameter is not supported.
// @return            - NPError status code.
// ----------------------------------------------------------------------------
NPError CBitmapInst :: PluginNew( 
                            NPMIMEType /*aPluginType*/, 
                            NPP aInstance, 
                            uint16 /*aMode*/, 
                            CDesC16Array* aArgn, 
                            CDesC16Array* aArgv, 
                            NPSavedData* /*aSaved*/ )
    {
    if ( aInstance->pdata != this )
        {
        return NPERR_INVALID_INSTANCE_ERROR;
        }

    // Traverse the attribute list to find the Url from which to retrieve data
    for ( TInt i = 0; i < aArgn->Count(); i++ )
        {
        TPtrC16 namePtr( (*aArgn)[i] );
        TPtrC16 valPtr( (*aArgv)[i] );

        if ( ( KSourceUrl().FindF( namePtr ) == 0 ) )
            {
            iSourceUrl = new TUint16[valPtr.Length() + 1];
            if ( iSourceUrl == NULL )
                {
                return NPERR_OUT_OF_MEMORY_ERROR;
                }
            TPtr16 ptr( iSourceUrl, 0, valPtr.Length() + 1 );
            ptr.Copy( valPtr );
            ptr.ZeroTerminate();
            break;
            }
        }
    return HandleGet();
    }


// ----------------------------------------------------------------------------
// This function deletes a plug-in instance.
// 
// @param aSave - this parameter is not supported.
// @return      - NPError status code.
// ----------------------------------------------------------------------------
NPError CBitmapInst :: PluginDestroy( NPSavedData** /*aSave*/ )
    {
    delete this;
    return NPERR_NO_ERROR;
    }


// ----------------------------------------------------------------------------
// This function sets the parent and the coordinates for the plug-in. 
// The first time PluginSetWindowL() is called, it is 
// important to construct the corresponding plug-in control. The other times 
// this call is made, it is expected that the function simply updates the 
// bounds of the control.
//
// @param aWindow - a plug-in window structure that contains window coordinates
//                  and platform specific window information.
// @return        - an error code, on success returns NPERR_NO_ERROR.
// ----------------------------------------------------------------------------
NPError CBitmapInst :: PluginSetWindowL( NPWindow* aWindow )
    {
    MPluginAdapterPTR pPluginAdapter( NULL );
    pPluginAdapter = static_cast<MPluginAdapter*>( aWindow->window );
    CCoeControl* pParentControl = pPluginAdapter->GetParentControl();

	if ( !pParentControl )
		return NPERR_NO_ERROR;

    if ( iFirstTime )
        {
	    CreateWindowL( pParentControl );
        pPluginAdapter->SetPluginNotifier( this );
        pPluginAdapter->PluginConstructedL( this );
        }
  
    TPoint point( aWindow->x, aWindow->y );
    TSize size( aWindow->width, aWindow->height );
    SetExtent( point, size );
  
    if ( iFirstTime )
        {
        ActivateL();
        iFirstTime = EFalse;
        }

    return NPERR_NO_ERROR;
    }


// ----------------------------------------------------------------------------
// This function has an empty implementation here, but the actual purpose of 
// the function is to set values for the plugin variables. 
//
// @param aVariable - the variable whose value is to be set.
// @param aValue    - pointer to the 32-bit parameter that contains the value.
// @return          - NPError status code.
// ----------------------------------------------------------------------------
NPError CBitmapInst :: PluginSetValue( 
                            NPNVariable /*aVariable*/, 
                            void* /*aValue*/ )
    {
    return NPERR_NO_ERROR;
    }


// ----------------------------------------------------------------------------
// This function notifies a plug-in instance of a new data stream.
// 
// @param aMimeType - the MIME type of the stream.
// @param aStream   - the new stream object.
// @param aSeekable - flag that indicates whether or not stream is searchable.
// @param aStype    - the type of the stream. The plug-in sets the stream type.
//                    Currently supported stream types are:	
//                        NP_NORMAL
//                        NP_ASFILE
 //                       NP_ASFILEONLY
// @return          - NPError status code.
// ----------------------------------------------------------------------------
NPError CBitmapInst :: PluginNewStream( 
                            NPMIMEType /*aMimeType*/, 
                            NPStream* aStream, 
                            NPBool /*aSeekable*/, 
                            uint16* aStype )
    {
    *aStype         = NP_ASFILE;
    iStream         = aStream;
    iStream->pdata  = this;
    return NPERR_NO_ERROR;
    }


// ----------------------------------------------------------------------------
// This function destroys the stream that was previously created to stream data
// to the plug-in.
//
// @param aStream - the stream to be destroyed.
// @param aReason - the reason for destroying the stream. Possible values are:
//                      NPRES_DONE - normal completion and all data was sent to
//                      the instance. 
//                      NPRES_USER_BREAK - the user canceled the stream
//                      NPRES_NETWORK_ERR - stream failed because of problems 
//                      with the network, disk I/O error, lack of memory, or 
//                      some other problem.        
// @return        - NPError status code.
// ----------------------------------------------------------------------------
NPError CBitmapInst :: PluginDestroyStream( 
                                NPStream* /*aStream*/, 
                                NPReason /*aReason*/ )
    {
    return NPERR_NO_ERROR;
    }


// ----------------------------------------------------------------------------
// This function passes a file name to the plug-in in which the stream data 
// is stored. 
//
// @param aStream   - the stream
// @param aFileName - the file name
// ----------------------------------------------------------------------------
void CBitmapInst :: PluginStreamAsFile( 
                            NPStream* aStream, 
                            const TDesC16& aFileName )
    {
    if ( ( aStream->pdata != this ) || ( aStream != iStream ) 
                                    || ( iNumDownload > 0 ) )
        {
        return;
        }    
    iFileName = aFileName.Alloc();
    iNumDownload ++;
    }


// ----------------------------------------------------------------------------
// This function writes a chunk of data to the plug-in.
//
// @param aStream - the stream
// @param aOffset - the offset in the stream.
// @param aLength - the size of the new data.
// @param aBuffer - the data.
// @return        - the number of bytes consumed by the plug-in instance.
// ----------------------------------------------------------------------------
int32 CBitmapInst :: PluginWrite( 
                        NPStream* aStream, 
                        int32 /*aOffset*/, 
                        int32 aLength, 
                        void* /*aBuffer*/ )
    {
    if ( aStream->pdata != this || aStream != iStream )
        {
        return 0;
        }

    return aLength;
    }


// ----------------------------------------------------------------------------
// The browser calls the NPP_Write function with the amount of data returned 
// from the NPP_WriteReady function.
//
// @param aStream - the stream
// @return        - maximum data size that the plug-in can handle.
// ----------------------------------------------------------------------------
int32 CBitmapInst :: PluginWriteReady( NPStream* /*aStream*/ )
    {
    return iWriteReady;
    }


// ----------------------------------------------------------------------------
// The browser calls the NPP_URLNotify function to notify the plug-in of the 
// completion of a URL request made by the NPN_GetURLNotify function or the 
// NPN_PostURLNotify function.
//
// @param aUrl        - url of the NPN_GetURLNotify function or of the 
//                      NPN_PostURLNotify function request.
// @param aReason     - reason code for completion of the request.
// @param aNotifyData - contains the private plug-in data passed to the 
//                      corresponding call to the NPN_GetURLNotify function.
// ----------------------------------------------------------------------------
void CBitmapInst :: PluginURLNotify( 
                        const TDesC16& /*aUrl*/, 
                        NPReason       /*aReason*/, 
                        void*          /*aNotifyData*/ )
    {
    return;
    }


// ----------------------------------------------------------------------------
// This function draws the control. When iFlipBitmap is true it renders one 
// bitmap, and when iFlipBitmap is false it renders another one.
//
// @param aRect - The region of the control to be redrawn. Co-ordinates are 
//                relative to the control's origin (top left corner).
// ----------------------------------------------------------------------------
void CBitmapInst :: Draw( const TRect& aRect ) const
    {   
    TInt ret( KErrNone );
    CFbsBitmap* bitmap = NULL;
  
    bitmap = new CFbsBitmap();
    if ( !bitmap )
        {
        return;
        }

    if ( iFileName )
		{
		if( iFlipBitmap )
			{
			ret = bitmap->Load( iFileName->Des(), EMbmAllbitmapsRedflower, ETrue );
			}
		else 
			{
			ret = bitmap->Load( iFileName->Des(), EMbmAllbitmapsBlueflower, ETrue );
			}
		}
    if ( ret != KErrNone )
        {
        delete ( bitmap );
        bitmap = NULL;
        return;
        }

    SystemGc().Clear();
    SystemGc().DrawBitmap( aRect, bitmap );
    delete ( bitmap );
    bitmap = NULL;
    }
// ----------------------------------------------------------------------------
// This function implements the NotifyL() function of the MPluginNotifier 
// interface. It notifies the plug-in of an event. 
//
// @param aCallType - the event type that is passed to the plug-in. 
//                    Possible values are :
//                    EEditCut, 
//                    EEditCopy, 
//                    EEditPaste, 
//                    EEditDelete, 
//                    EApplicationFocusChanged, 
//                    ESystemNotification
// @param aParam    - the parameter associated with the event.
// @return          - not used.
// ----------------------------------------------------------------------------
TInt CBitmapInst :: NotifyL( TNotificationType aCallType, TAny* /*aParam*/ )
    {
    switch ( aCallType )
        {
        case EApplicationFocusChanged:
            {
			if ( iFlipBitmap )
                {
		        iFlipBitmap = EFalse;
                }
	        else 
                {
		        iFlipBitmap = ETrue;
                }
            DrawNow();
            break;
            }
        default:
            break;
        }
    return 0;
    }


// ----------------------------------------------------------------------------
// Returns the supported input capabilities which correspond to the behaviour 
// of the OfferKeyEventL() function of the control; 
//
// @return - TCoeInputCapabilities::EAllText which supports all types of text.
// ----------------------------------------------------------------------------

TCoeInputCapabilities CBitmapInst :: InputCapabilities ( void ) const
    {
	return TCoeInputCapabilities( TCoeInputCapabilities::EAllText );
    }


// ----------------------------------------------------------------------------
// This function handles key events. when 'Enter' key or 'OK' key is pressed, 
// the bitmap rendered is flipped from redflower to blueflower and viceversa.
//
// @param aKeyEvent - the key event.
// @param aType     - type of key event:EEventKey, EEventKeyUp or EEventKeyDown
// @return          - the function returns EKeyWasNotConsumed if it does not do 
//                    anything in response to a key event. If it is able to 
//                    process the event it returns EKeyWasConsumed.
// ----------------------------------------------------------------------------
TKeyResponse CBitmapInst :: OfferKeyEventL( 
                                const TKeyEvent& aKeyEvent, 
                                TEventCode aType )
    {
	if ( aType == EEventKeyUp )
	    {
		switch ( aKeyEvent.iScanCode )
		    {
		    case EStdKeyEnter:
		    case EStdKeyDevice3:
                {
				if ( iFlipBitmap )
                    {
		            iFlipBitmap = EFalse;
                    }
	            else 
                    {
		            iFlipBitmap = ETrue;
                    }
			    DrawNow();
			    return EKeyWasConsumed;
                }
		    default:
			    return EKeyWasNotConsumed;
		    }
	    }
	return EKeyWasNotConsumed;
    }


// ----------------------------------------------------------------------------
// This function inherited from MCoeControlObserver handles an event from an 
// observed control.
//
// @param aControl   - pointer to the control from which the event originated.
// @param aEventType - the event type.
// ----------------------------------------------------------------------------
void CBitmapInst :: HandleControlEventL( 
                            CCoeControl* aControl,
                            TCoeEvent aEventType )
    {
    if ( aControl != (( MPluginAdapter* ) ( iNpp->ndata ))->GetParentControl() )
        {
        return;
        }

    switch ( aEventType )
        {
        case EEventRequestFocus:
	    default:
	        break;
        }
    }


// ----------------------------------------------------------------------------
// This function handles pointer events. On clicking the left mouse button, 
// the bitmap rendered is flipped from redflower to blueflower and viceversa.
//
// @param aPointerEvent - The pointer event.
// ----------------------------------------------------------------------------
void CBitmapInst :: HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    switch ( aPointerEvent.iType )
        {
        case TPointerEvent::EButton1Down:
            {
			if ( iFlipBitmap )
                {
		        iFlipBitmap = EFalse;
                }
	        else 
                {
		        iFlipBitmap = ETrue;
                }
	        DrawNow();
	        break;
            }       
        default:
	        break;
        }
    }


// ----------------------------------------------------------------------------
// This function is called whenever a control gains or loses focus - as a 
// result of a call to SetFocus(). 
//
// @param aDrawNow  - Contains the value that was passed to it by SetFocus().
// ----------------------------------------------------------------------------
void CBitmapInst :: FocusChanged( TDrawNow /*aDrawNow*/ )
    {
    DrawNow();
    }


// ----------------------------------------------------------------------------
// This function which responds to size changes to set the size and position of 
// the contents of the control does nothing and has an empty implementation.
// ----------------------------------------------------------------------------
void CBitmapInst :: SizeChanged( void )
    {
    return;
    }


// ----------------------------------------------------------------------------
// This function calls the NPN_GetURL function to ask the browser to deliver 
// the data to the plug-in instance in a new stream.
//
// @return - if the url is NULL returns NPERR_GENERIC_ERROR, otherwise returns
//           NPError status code from calling the NPN_GetURL function.
// ----------------------------------------------------------------------------
NPError CBitmapInst :: HandleGet( void )
    {
    CBitmapEcomMain* npm = ( CBitmapEcomMain* ) Dll :: Tls();
    TPtrC url;

    if ( iSourceUrl ) // iSourceUrl is non-null
        {
        url.Set( iSourceUrl );
        return ( npm->Funcs()->geturl( iNpp, url, NULL ) );
        }
    else
        {
        return NPERR_GENERIC_ERROR;
        }
    }

