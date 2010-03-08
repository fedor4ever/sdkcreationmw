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
#include <Eikenv.h>
#include <w32std.h>

#include "MultiTexContainer.h"
#include <AknUtils.h> // Pen support. 


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMultiTexContainer::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CMultiTexContainer::ConstructL(const TRect& aRect, CAknAppUi* aAppUi)
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

    iFrame = 0;                              // Initialize frame counter

    EGLConfig config;                       // Describes the format, type and
                                             // size of the color buffers and
                                             // ancillary buffers for EGLSurface

    // Get the display for drawing graphics
    iEglDisplay = eglGetDisplay( EGL_DEFAULT_DISPLAY );
    if ( iEglDisplay == NULL )
    {
        _LIT(KGetDisplayFailed, "eglGetDisplay failed");
        User::Panic( KGetDisplayFailed, 0 );
    }

    // Initialize display
    if ( eglInitialize( iEglDisplay, NULL, NULL ) == EGL_FALSE )
    {
        _LIT(KInitializeFailed, "eglInitialize failed");
        User::Panic( KInitializeFailed, 0 );
    }

    EGLConfig *configList = NULL;            // Pointer for EGLConfigs
    TInt numOfConfigs = 0;
    TInt configSize   = 0;

    // Get the number of possible EGLConfigs
    if ( eglGetConfigs( iEglDisplay, configList, configSize, &numOfConfigs )
         == EGL_FALSE )
    {
        _LIT(KGetConfigsFailed, "eglGetConfigs failed");
        User::Panic( KGetConfigsFailed, 0 );
    }

    configSize = numOfConfigs;

    // Allocate memory for the configList
    configList = (EGLConfig*) User::Alloc( sizeof(EGLConfig)*configSize );
    if ( configList == NULL )
    {
        _LIT(KConfigAllocFailed, "config alloc failed");
        User::Panic( KConfigAllocFailed, 0 );
    }

    /* Define properties for the wanted EGLSurface
       To get the best possible performance, choose
       an EGLConfig with a buffersize matching
       the current window's display mode*/
    TDisplayMode DMode = Window().DisplayMode();
    TInt BufferSize = 0;

    switch ( DMode )
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
				EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_BUFFER_SIZE,  BufferSize,
        EGL_DEPTH_SIZE,   16,
				EGL_SAMPLE_BUFFERS, 1,
				EGL_SAMPLES,        4,
        EGL_NONE
        };

    // Define properties for requesting a non-antialiased window surface
    const EGLint attrib_list[] =
        {
				EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
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

    config = configList[0];     // Choose the best EGLConfig. EGLConfigs
                                 // returned by eglChooseConfig are sorted so
                                 // that the best matching EGLConfig is first in
                                 // the list.

    // Create Pbuffer surface
    const EGLint attrib_list2[] = { EGL_WIDTH,
                                    128,
                                    EGL_HEIGHT,
                                    128,
                                    EGL_NONE };

    iEglPbufferSurface = eglCreatePbufferSurface( iEglDisplay, config, attrib_list2 );

    if ( iEglPbufferSurface == NULL )
    {
        _LIT(KCreatePbufferSurfaceFailed, "eglCreatePbufferSurface failed");
        User::Panic( KCreatePbufferSurfaceFailed, 0 );
    }

    // Create a Pbuffer context
    iEglPbufferContext = eglCreateContext( iEglDisplay, config, EGL_NO_CONTEXT, NULL );
    if ( iEglPbufferContext == NULL )
    {
        _LIT(KCreateContextFailed, "eglCreateContext failed");
        User::Panic( KCreateContextFailed, 0 );
    }

    // Define properties for the another EGLSurface
    const EGLint attrib_list_window[] = { EGL_SURFACE_TYPE,
                                          EGL_WINDOW_BIT,
                                          EGL_BUFFER_SIZE,
                                          BufferSize,
                                          EGL_DEPTH_SIZE,
                                          16,              //16-bit depth-buffer
                                          EGL_NONE };

    // Choose an EGLConfig that best matches to the properties in attrib_list
    if ( eglChooseConfig( iEglDisplay, attrib_list_window, configList, configSize,
                          &numOfConfigs ) == EGL_FALSE )
    {
        _LIT(KChooseConfigFailed, "eglChooseConfig failed");
        User::Panic( KChooseConfigFailed, 0 );
    }

    config = configList[0];    // Choose the best EGLConfig. EGLConfigs
                                // returned by eglChooseConfig are sorted so
                                // that the best matching EGLConfig is first in
                                // the list.

    User::Free( configList ); // Free configList, not used anymore.

    // Create window surface
    iEglSurface = eglCreateWindowSurface( iEglDisplay, config, &Window(), NULL );

    if ( iEglSurface == NULL )
    {
        _LIT(KCreateWindowSurfaceFailed, "eglCreateWindowSurface failed");
        User::Panic( KCreateWindowSurfaceFailed, 0 );
    }


    // Create a window context
    iEglContext = eglCreateContext( iEglDisplay, config, iEglPbufferContext, NULL );
    if ( iEglContext == NULL )
    {
        _LIT(KCreateContextFailed, "eglCreateContext failed");
        User::Panic( KCreateContextFailed, 0 );
    }

    TSize size;
    size = this->Size();

    iMultiTex = CMultiTex::NewL( size.iWidth, size.iHeight ); // Create an instance of MultiTex
    iMultiTex->AppInitL( this );

    iOpenGlInitialized = ETrue;

    iPeriodic = CPeriodic::NewL( CActive::EPriorityIdle );    // Create an active object for
                                                              // animating the scene

    iPeriodic->Start( 100, 100, TCallBack( CMultiTexContainer::DrawCallBack, this ) );
}

// Destructor
CMultiTexContainer::~CMultiTexContainer()
{
    delete iPeriodic;

    /* AppExit call is made to release
       any allocations made in AppInit. */
    if ( iMultiTex )
    {
        iMultiTex->AppExit();
        delete iMultiTex;
    }

    eglMakeCurrent( iEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
    eglDestroySurface( iEglDisplay, iEglSurface );
    eglDestroySurface( iEglDisplay, iEglPbufferSurface );
    eglDestroyContext( iEglDisplay, iEglContext );
    eglTerminate( iEglDisplay );                   // Release resources associated
    // with EGL and OpenGL ES
}

// ---------------------------------------------------------
// CMultiTexContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CMultiTexContainer::SizeChanged()
    {
    if( iOpenGlInitialized && iMultiTex )
        {
        TSize size;
        size = this->Size();

        iMultiTex->SetScreenSize( size.iWidth, size.iHeight );
        }
    }


// ---------------------------------------------------------
// CMultiTexContainer::HandleResourceChange(
//     TInt aType)
// Dynamic screen resize changes by calling the
// SetExtentToWholeScreen() method again.
// ---------------------------------------------------------
//
 void CMultiTexContainer::HandleResourceChange(TInt aType)
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
// CMultiTexContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CMultiTexContainer::CountComponentControls() const
{
    return 0;
}

// ---------------------------------------------------------
// CMultiTexContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CMultiTexContainer::ComponentControl(TInt /*aIndex*/ ) const
{
    return NULL;
}

// ---------------------------------------------------------
// CMultiTexContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CMultiTexContainer::Draw(const TRect& /*aRect*/ ) const
{
}

// ---------------------------------------------------------
// CMultiTexContainer::DrawCallBack( TAny* aInstance )
// Called by the CPeriodic in order to draw the graphics
// ---------------------------------------------------------
//
TInt CMultiTexContainer::DrawCallBack( TAny* aInstance )
{
    CMultiTexContainer* instance = (CMultiTexContainer*) aInstance;

    if ( instance->iMultiTex->GetState() == CMultiTex::ERunning )
    {
        instance->iFrame++;

        /*=========================================================================*/
        /* Start by rendering with Pbuffer context to pbuffer and create the dynamic
           environment map. First, render to pbuffer and then copy the content
           to the second texture (iLightTexObjects[1]).                             */
        /*=========================================================================*/
        eglMakeCurrent( instance->iEglDisplay, instance->iEglPbufferSurface,
                        instance->iEglPbufferSurface, instance->iEglPbufferContext );

        instance->iMultiTex->AppCyclePbuffer( instance->iFrame );

        /*=========================================================================*/
        /* Now that we have the environment map ready, we can render the sphere with
           marble and environment map (dynamic lights) textures                    */
        /*=========================================================================*/
        eglMakeCurrent( instance->iEglDisplay, instance->iEglSurface,
                        instance->iEglSurface, instance->iEglContext );

        instance->iMultiTex->AppCycleWindow( instance->iFrame );

        // Call eglSwapBuffers, which blit the graphics to the window
        eglSwapBuffers( instance->iEglDisplay, instance->iEglSurface );

        // To keep the background light on
        if ( !(instance->iFrame%100) )
        {
            User::ResetInactivityTime();
        }

        /* Suspend the current thread for a short while. Give some time
           to other threads and AOs, avoids the ViewSrv error in ARMI and
           THUMB release builds. One may try to decrease the callback
           function instead of this. */
        if ( !(instance->iFrame%50) )
        {
            User::After(0);
        }

    }
    return 0;
}

// ---------------------------------------------------------
// CMultiTexContainer::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CMultiTexContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
{
}

// End of File
