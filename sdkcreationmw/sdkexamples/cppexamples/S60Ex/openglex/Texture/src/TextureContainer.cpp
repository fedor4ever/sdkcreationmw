/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include "TextureContainer.h"
#include <AknUtils.h> // Pen support. 


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CTextureContainer::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// EGL code, e.g., OpenGL ES context creation, surface
// creation etc. are located here.
// ---------------------------------------------------------
//
void CTextureContainer::ConstructL(const TRect& aRect, CAknAppUi* aAppUi)
    {
    iAppUi = aAppUi;
    iOpenGlInitialized = EFalse;
    CreateWindowL();

    // Do not go to full-screen in touch devices.    
    if (AknLayoutUtils::PenEnabled())
    {
        SetRect(aRect);
    }
    else
    {
        SetExtentToWholeScreen();                // Take the whole screen into use
    }
    ActivateL();

    iFrame = 0;                              // Frame counter

    EGLConfig Config;                        // Describes the format, type and
                                             // size of the color buffers and
                                             // ancillary buffers for EGLSurface

    /* Get the display for drawing graphics*/
    iEglDisplay = eglGetDisplay( EGL_DEFAULT_DISPLAY );
    if( iEglDisplay == NULL )
    {
	    _LIT(KEglGetDisplay,    "eglGetDisplay failed");
        User::Panic( KEglGetDisplay, 0 );
    }

    /* Initialize display */
    if( eglInitialize( iEglDisplay, NULL, NULL ) == EGL_FALSE )
    {
	    _LIT(KEglInitialize,    "eglInitialize failed");
        User::Panic( KEglInitialize, 0 );
    }

    EGLConfig *configList = NULL;            // Pointer for EGLConfigs
    TInt numOfConfigs = 0;
    TInt configSize   = 0;

    /* Get the number of possible EGLConfigs */
    if( eglGetConfigs( iEglDisplay, configList, configSize, &numOfConfigs ) == EGL_FALSE )
    {
	    _LIT(KEglGetConfigs,    "eglGetconfigs failed");
        User::Panic( KEglGetConfigs, 0 );
    }

    configSize = numOfConfigs;

    /* Allocate memory for the configList */
    configList = (EGLConfig*) User::Alloc( sizeof(EGLConfig)*configSize );
    if( configList == NULL )
    {
	    _LIT(KConfigAlloc,      "config alloc failed");
        User::Panic( KConfigAlloc, 0 );
    }

    /* Define properties for the wanted EGLSurface.
       To get the best possible performance, choose
       an EGLConfig with a buffersize matching
       the current window's display mode*/
    TDisplayMode DMode = Window().DisplayMode();
    TInt BufferSize = 0;

    switch(DMode)
    {
    case(EColor4K):
        BufferSize = 12;
        break;
    case(EColor64K):
        BufferSize = 16;
        break;
    case(EColor16M):
        BufferSize = 24;
        break;
    case(EColor16MU):
    case(EColor16MA):
        BufferSize = 32;
        break;
    default:
        _LIT(KDModeError,       "unsupported displaymode");
        User::Panic( KDModeError, 0 );
        break;
    }

    // Define properties for requesting a full-screen antialiased window surface
    const EGLint attrib_list_fsaa[] =
        {
				EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BUFFER_SIZE,  BufferSize,
        EGL_DEPTH_SIZE,   16,
				EGL_SAMPLE_BUFFERS, 1,
				EGL_SAMPLES,        4,
        EGL_NONE
        };

    // Define properties for requesting a non-antialiased window surface
    const EGLint attrib_list[] =
        {
				EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BUFFER_SIZE,  BufferSize,
        EGL_DEPTH_SIZE,   16,
        EGL_NONE
        };

    // Choose an EGLConfig that best matches to the properties in attrib_list_fsaa
    if ( eglChooseConfig( iEglDisplay, attrib_list_fsaa, configList, configSize, &numOfConfigs ) == EGL_FALSE )
        {
        _LIT( KChooseConfigFailed, "eglChooseConfig failed" );
        User::Panic( KChooseConfigFailed, 0 );
        }

    // Check if no configurations were found
    if ( numOfConfigs == 0 )
        {
        // No configs with antialising were found. Try to get the non-antialiased config
		    if ( eglChooseConfig( iEglDisplay, attrib_list, configList, configSize, &numOfConfigs ) == EGL_FALSE )
			      {
	          _LIT( KChooseConfigFailed, "eglChooseConfig failed" );
	          User::Panic( KChooseConfigFailed, 0 );
			      }

		    if ( numOfConfigs == 0 )
			      {
			      // No configs found without antialiasing
	          _LIT( KNoConfig, "Can't find the requested config." );
	          User::Panic( KNoConfig, 0 );
	          }
	      }

    Config = configList[0];   // Choose the best EGLConfig. EGLConfigs
                              // returned by eglChooseConfig are sorted so
                              // that the best matching EGLConfig is first in
                              // the list.
    User::Free( configList ); // Free configList, not used anymore.

    /* Create a window where the graphics are blitted */
    iEglSurface = eglCreateWindowSurface( iEglDisplay, Config, &Window(), NULL );
    if( iEglSurface == NULL )
    {
	    _LIT(KEglCreateWindow,  "eglCreateWindow failed");
        User::Panic( KEglCreateWindow, 0 );
    }

    /* Create a rendering context */
    iEglContext = eglCreateContext( iEglDisplay, Config, EGL_NO_CONTEXT, NULL );
    if( iEglContext == NULL )
    {
	    _LIT(KEglCreateContext, "eglCreateContext failed");
        User::Panic( KEglCreateContext, 0 );
    }

    /* Make the context current. Binds context to the current rendering thread
       and surface.*/
    if( eglMakeCurrent( iEglDisplay, iEglSurface, iEglSurface, iEglContext ) == EGL_FALSE )
    {
	    _LIT(KEglMakeCurrent,   "eglMakeCurrent failed");
        User::Panic( KEglMakeCurrent, 0 );
    }

    TSize size;
    size = this->Size();

    iTexture = CTexture::NewL( size.iWidth, size.iHeight );
    iTexture->AppInitL();

    iOpenGlInitialized = ETrue;

    iPeriodic = CPeriodic::NewL( CActive::EPriorityIdle ); // Create an active object for
                                                           // animating the scene

    iPeriodic->Start( 100, 100, TCallBack( CTextureContainer::DrawCallBack, this ) );
}

// Destructor
CTextureContainer::~CTextureContainer()
{
    delete iPeriodic;

    /* AppExit call is made to release any allocations made in AppInit.
     This call has to be made here before we destroy the rendering context. */
    if ( !(iTexture == NULL) )
        {
        iTexture->AppExit();
        delete iTexture;
        }

    eglMakeCurrent( iEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
    eglDestroySurface( iEglDisplay, iEglSurface );
    eglDestroyContext( iEglDisplay, iEglContext );
    eglTerminate( iEglDisplay );                   // Release resources associated
                                                   // with EGL and OpenGL ES
}

// ---------------------------------------------------------
// CTextureContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CTextureContainer::SizeChanged()
    {
    if( iOpenGlInitialized && iTexture )
        {
        TSize size;
        size = this->Size();

        iTexture->SetScreenSize( size.iWidth, size.iHeight );
        }
    }


// ---------------------------------------------------------
// CTextureContainer::HandleResourceChange(
//     TInt aType)
// Dynamic screen resize changes by calling the
// SetExtentToWholeScreen() method again.
// ---------------------------------------------------------
//
 void CTextureContainer::HandleResourceChange(TInt aType)
    {
	switch( aType )
    	{
	    case KEikDynamicLayoutVariantSwitch:
	        // Do not go full-screen in touch devices.    
            if (AknLayoutUtils::PenEnabled())
            {
                SetRect(iAppUi->ClientRect());
            }
            else
            {
                SetExtentToWholeScreen();                // Take the whole screen into use
            }
	        break;
	    }


    }

// ---------------------------------------------------------
// CTextureContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CTextureContainer::CountComponentControls() const
{
    return 0;
}

// ---------------------------------------------------------
// CTextureContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CTextureContainer::ComponentControl(TInt /*aIndex*/) const
{
    return NULL;
}

// ---------------------------------------------------------
// CTextureContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CTextureContainer::Draw(const TRect& /*aRect*/ ) const
{
    // Nothing to do here...
}

// ---------------------------------------------------------
// CTextureContainer::DrawCallBack( TAny* aInstance )
// Called by the CPeriodic in order to draw the graphics
// ---------------------------------------------------------
//
TInt CTextureContainer::DrawCallBack( TAny* aInstance )
{
    CTextureContainer* instance = (CTextureContainer*) aInstance;
    if ( instance->iTexture->GetState() == CTexture::ERunning )
		{
	    /* Check if the textures are loaded before doing any rendering. */
	    instance->iFrame++;

	    /* Call the main OpenGL ES Symbian rendering 'loop' */
	    instance->iTexture->AppCycle( instance->iFrame );

	    /* Call eglSwapBuffers, which blit the graphics to the window */
	    eglSwapBuffers( instance->iEglDisplay, instance->iEglSurface );

	    /* To keep the background light on */
	    if(!(instance->iFrame%100))  User::ResetInactivityTime();

	    /* Suspend the current thread for a short while. Give some time
	       to other threads and AOs, avoids the ViewSrv error in ARMI and
	       THUMB release builds. One may try to decrease the callback
	       function instead of this. */
	    if(!(instance->iFrame%50))   User::After(0);
		}
    return 0;
}

// ---------------------------------------------------------
// CTextureContainer::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CTextureContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
{
}

// End of File
