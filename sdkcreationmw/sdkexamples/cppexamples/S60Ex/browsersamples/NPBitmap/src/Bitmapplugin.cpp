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
#include "bitmapplugin.h"
#include "bitmapecommain.h"
#include "bitmapinst.h"


// CONSTANTS
 _LIT(KMIMEDescription, "application/mbm;mbm;|image/mbm;mbm;|image/gif;gif");
 _LIT(KPluginName,"S60 Bitmap Graphics Plug-in");
 _LIT(KPluginDescription,"Example plug-in to demonstrate usage of different plug-in APIs");

// ============================================================================
// LOCAL FUNCTIONS
// ============================================================================

// ----------------------------------------------------------------------------
// This function returns pointer to the plug-in object which is stored in the 
// pdata member of the plugin instance handle.
//
// @param aInstance - the plug-in instance.
// @return          - pointer to the plug-in object.
// ----------------------------------------------------------------------------

static CBitmapInst* Inst( NPP aInstance )
    {
    CBitmapInst* inst = ( CBitmapInst* )aInstance->pdata;
    return inst;
    }


// ----------------------------------------------------------------------------
// The browser calls the PluginDestroy function to delete an instance of a 
// plug-in previously constructed by PluginNew(). When this function is called, 
// the browser will have already cleaned up all other instance-related objects
// that it created.
//
// @param aInstance - the instance originally passed to PluginNew().
// @param aSave     - optional parameter to save data for reuse by a new 
//                    plug-in instance with the same URL. Saved data is passed 
//                    to the NP_New() call of the new plug-in instance. The buf 
//                    field of the aSave variable should always be allocated 
//                    using NPN_MemAlloc() because the browser is responsible 
//                    for deleting this value.To ensure that the browser does 
//                    not crash or leak memory when the saved data is 
//                    discarded, the buf field should be a flat structure with 
//                    no allocated substructures since the browser is not aware
//                    of the structure of buf to free the individual allocated 
//                    members of the structure.
// @return          - since this is a destructor function, it should always 
//                    return NPERR_NO_ERROR.
// ----------------------------------------------------------------------------
NPError PluginDestroy( NPP aInstance, NPSavedData** aSave )
    {
    return Inst( aInstance )->PluginDestroy( aSave );
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
NPError PluginDestroyStream( NPP aInstance, NPStream* aStream, NPReason aReason )
    {
    return Inst( aInstance )->PluginDestroyStream( aStream, aReason );
    }

// ----------------------------------------------------------------------------
// The browser uses the PluginGetValue function to determine the name and 
// description of the plug-in.
//
// @param aInstance - the plug-in instance.
// @param aVariable - the variable requested. Standard values in Symbian are:
//                    NPPVpluginNameString
//                    NPPVpluginDescriptionString
//                    NPPVpluginWindowBool
//                    NPPVpluginProgressBar
// @param aValue    - the return value requested.
// @return          - an error code; on success returns NPERR_NO_ERROR.
// ----------------------------------------------------------------------------
NPError PluginGetValue( NPP /*aInstance*/, NPPVariable aVariable, void *aValue )
    {
    const char** retValue = (const char**)aValue;

    switch ( aVariable )
        {
        case NPPVpluginNameString:
            {
            *( ( const TDesC** )retValue ) = &KPluginName;
            break;
            }
        case NPPVpluginDescriptionString:
            {
            *( ( const TDesC** )retValue ) = &KPluginDescription;
            break;
            }
        case NPPVpluginWindowBool:
        case NPPVpluginTransparentBool:
        default:
            {
            *retValue = NULL;
            break;
            }
        }
    return NPERR_NO_ERROR;
    }

// ----------------------------------------------------------------------------
// The browser calls PluginNew function to create a plug-in instance based on a 
// MIME type. The plug-in instance created can store its private data as a 
// member (pdata) of the instance argument passed.
//
// @param aPluginType - the MIME type of the plug-in.
// @param aInstance   - the pdata member variable is used by the plug-in to 
//                      store instance-specific private data. Commonly used to 
//                      store a pointer to an object, which is used for 
//                      operating on the plug-in in the future.
// @param aMode       - identifies the display mode of the plug-in.
//                          NP_EMBED - specifies that the plug-in was created 
//                          using an embed tag within a Web document and that 
//                          the plug-in is to be displayed within a Web document.
//                          NP_FULL - specifies that the plug-in was created by 
//                          opening a document requiring plug-in support for 
//                          display as the top level document and consumes the
//                          entire document window.
// @param aArgn       - string list containing all the names of all parameters 
//                      included within the document embed tag.
// @param aArgv       - a string list containing values corresponding to the 
//                      parameters passed in the argn by means of the embed tag
// @param aSaved      - Not supported.
// @return            - an error code. Upon success, returns NPERR_NO_ERROR.
// ----------------------------------------------------------------------------
NPError PluginNew( 
            NPMIMEType aPluginType, NPP aInstance, uint16 aMode, 
            CDesC16Array* aArgn, CDesC16Array* aArgv, NPSavedData* aSaved )
    {
    CBitmapInst* inst = NULL;
    TRAPD( ret, inst = CBitmapInst :: NewL( aInstance ) );
    if ( ret )
        {
        return NPERR_OUT_OF_MEMORY_ERROR;
        }
    aInstance->pdata = inst;
    Inst( aInstance )->PluginNew( aPluginType, aInstance, aMode, 
                                         aArgn, aArgv, aSaved );
	return NPERR_NO_ERROR;
    }


// ----------------------------------------------------------------------------
// This function notifies a plug-in instance of a new data stream.
// 
// @param aInstance - the plug-in instance originally used in PluginNew().
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
NPError PluginNewStream( 
            NPP aInstance, NPMIMEType aType, NPStream* aStream, 
            NPBool aSeekable, uint16* aStype )
    {
    return Inst( aInstance )->PluginNewStream( aType, aStream, aSeekable, 
                                               aStype );
    }


// ----------------------------------------------------------------------------
// This function sets value for a plugin variable. 
//
// @param aInstance - the plug-in instance originally used in PluginNew().
// @param aVariable - the variable whose value is to be set.
// @param aValue    - pointer to the 32-bit parameter that contains the value.
// @return          - NPError status code.
// ----------------------------------------------------------------------------
NPError PluginSetValue( NPP aInstance, NPNVariable aVariable, void *aValue )
    {
    return Inst( aInstance )->PluginSetValue( aVariable, aValue );
    }


// ----------------------------------------------------------------------------
// The browser calls the NPP_SetWindow() function to set the parent and the 
// coordinates for the plug-in. The first time this function is called, it is 
// important to construct the corresponding plug-in control.The other times 
// this call is made, it is expected that the function simply updates the 
// bounds of the control.
//
// @param aInstance - the plug-in instance originally used in PluginNew().
// @param aWindow   - a plug-in window structure that contains window 
//                    coordinates and platform specific window information.
// @return          - an error code; on success returns NPERR_NO_ERROR.
// ----------------------------------------------------------------------------
NPError PluginSetWindowL( NPP aInstance, NPWindow* aWindow )
    {
    return Inst( aInstance )->PluginSetWindowL( aWindow );
    }


// ----------------------------------------------------------------------------
// This function passes a file name to the plug-in in which the stream data 
// is stored. 
//
// @param aInstance - the plug-in instance originally used in PluginNew().
// @param aStream   - the stream
// @param aFileName - the file name
// ----------------------------------------------------------------------------
void PluginStreamAsFile( 
                NPP aInstance, NPStream* aStream, const TDesC16& aFileName )
    {
    Inst( aInstance )->PluginStreamAsFile( aStream, aFileName );
    }


// ----------------------------------------------------------------------------
// The browser calls the NPP_URLNotify() function to notify the plug-in of the 
// completion of a URL request made by the NPN_GetURLNotify() function or the 
// NPN_PostURLNotify() function.
//
// @param aInstance   - the plug-in instance originally used in PluginNew().
// @param aUrl        - url of the NPN_GetURLNotify() function or of the 
//                      NPN_PostURLNotify() function request.
// @param aReason     - reason code for completion of the request.
// @param aNotifyData - contains the private plug-in data passed to the 
//                      corresponding call to the NPN_GetURLNotify() function.
// ----------------------------------------------------------------------------
void PluginURLNotify( 
            NPP aInstance, const TDesC16& aUrl, NPReason aReason, 
            void* aNotifyData )
    {
    Inst( aInstance )->PluginURLNotify( aUrl, aReason, aNotifyData );
    }


// ----------------------------------------------------------------------------
// This function writes a chunk of data to the plug-in.
//
// @param aInstance - the plug-in instance originally used in PluginNew().
// @param aStream - the stream.
// @param aOffset - the offset in the stream.
// @param aLength - the size of the new data.
// @param aBuffer - the data.
// @return        - the number of bytes consumed by the plug-in instance.
// ----------------------------------------------------------------------------
int32 PluginWrite( 
            NPP aInstance, NPStream* aStream, int32 aOffset, int32 aLength, 
            void* aBuffer )
    {
    return Inst( aInstance )->PluginWrite( aStream, aOffset, aLength, aBuffer );
    }


// ----------------------------------------------------------------------------
// The browser calls the NPP_Write() function with the amount of data returned 
// from the NPP_WriteReady() function.
//
// @param aInstance - the plug-in instance originally used in PluginNew().
// @param aStream   - the stream
// @return          - maximum data size that the plug-in can handle.
// ----------------------------------------------------------------------------
int32 PluginWriteReady( NPP aInstance, NPStream* aStream )
    {
    return Inst( aInstance )->PluginWriteReady( aStream );
    }


// ----------------------------------------------------------------------------
// The initialization function of the plug-in exchanges function pointers 
// between the browser and the plug-in, allowing calls to and from the plug-in. 
// This function also initializes plug-in library global data. 
// All unimplemented functions should be set to NULL in the function table.
// For example, in the S60 and Series 90 platforms,JRI support is not 
// available. Therefore, aPpf->javaClass should be set to NULL.
//
// @param aPpf - allocated but uninitialized structure containing function 
//               pointers to implemented plug-in functions.
// @return     - an error code, on success returns NPERR_NO_ERROR.
// ----------------------------------------------------------------------------
EXPORT_C NPError InitializeFuncs(NPPluginFuncs* aPpf)
    {
    aPpf->size          = sizeof ( NPPluginFuncs );
	aPpf->version       = 1;
    aPpf->newp          = PluginNew;
    aPpf->destroy       = PluginDestroy;
    aPpf->setwindow     = PluginSetWindowL;
    aPpf->newstream     = PluginNewStream;
    aPpf->destroystream = PluginDestroyStream;
    aPpf->asfile        = PluginStreamAsFile;
    aPpf->writeready    = PluginWriteReady;
    aPpf->write         = PluginWrite;
    aPpf->print         = NULL;
    aPpf->event         = NULL;
    aPpf->urlnotify     = PluginURLNotify;
    aPpf->javaClass     = NULL;
    aPpf->getvalue      = PluginGetValue;
    aPpf->setvalue      = PluginSetValue;

    return NPERR_NO_ERROR;
    }


// ----------------------------------------------------------------------------
// This function has no input parameters and no return value. It is called to 
// allow the plug-in to clean up its data before the plug-in library is unloaded.
// ----------------------------------------------------------------------------
EXPORT_C const TDesC* NPP_GetMIMEDescription(void)
{
    return &KMIMEDescription;
  
}


// ----------------------------------------------------------------------------
// The browser uses the PluginGetValue function to determine the name and 
// description of the plug-in.
//
// @param aInstance - the plug-in instance.
// @param aVariable - the variable requested. Standard values in Symbian are:
//                    NPPVpluginNameString
//                    NPPVpluginDescriptionString
//                    NPPVpluginWindowBool
//                    NPPVpluginProgressBar
// @param aValue    - the return value requested.
// @return          - an error code; on success returns NPERR_NO_ERROR.
// ----------------------------------------------------------------------------
EXPORT_C NPError NPP_GetValue(void* instance, NPPVariable aVariable, void* aValue)
{
    return PluginGetValue((NPP)instance, aVariable , aValue);
}


EXPORT_C void NPP_Shutdown( void )
    {
	CBitmapEcomMain* npm = ( CBitmapEcomMain* )Dll :: Tls();
    delete npm;
	Dll :: SetTls( NULL );
    }

