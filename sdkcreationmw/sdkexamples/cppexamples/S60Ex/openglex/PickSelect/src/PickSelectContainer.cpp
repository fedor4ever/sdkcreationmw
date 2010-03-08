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
#include "PickSelectContainer.h"
#include <AknUtils.h> // Pen support. 

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPickSelectContainer::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CPickSelectContainer::ConstructL(const TRect& aRect, CAknAppUi* aAppUi)
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
        _LIT(KGetDisplayFailed, "eglGetDisplay failed");
        User::Panic( KGetDisplayFailed, 0 );
        }

    EGLConfig *configList = NULL;            // Pointer for EGLConfigs
    EGLint numOfConfigs = 0;
    EGLint configSize   = 0;

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

    /*
       Define properties for the wanted EGLSurface.
       We need to choose a 32-bit framebuffer, since the picking
       method needs an 8-bit alpha channel.
     */

    // Define properties for requesting a full-screen antialiased window surface
    const EGLint attrib_list_fsaa[] =
        {
				EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BUFFER_SIZE,  32,
        EGL_DEPTH_SIZE,   16,
				EGL_SAMPLE_BUFFERS, 1,
				EGL_SAMPLES,        4,
        EGL_NONE
        };

    // Define properties for requesting a non-antialiased window surface
    const EGLint attrib_list[] =
        {
				EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BUFFER_SIZE,  32,
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

    Config = configList[0];  // Choose the best EGLConfig. EGLConfigs
                              // returned by eglChooseConfig are sorted so
                              // that the best matching EGLConfig is first in
                              // the list.
    User::Free( configList ); // Free configList, not used anymore.

    // Create a window where the graphics are blitted
    iEglSurface = eglCreateWindowSurface( iEglDisplay, Config, &Window(), NULL );
    if ( iEglSurface == NULL )
        {
        _LIT(KCreateWindowSurfaceFailed, "eglCreateWindowSurface failed");
        User::Panic( KCreateWindowSurfaceFailed, 0 );
        }

    // Create a rendering context
    iEglContext = eglCreateContext( iEglDisplay, Config, EGL_NO_CONTEXT, NULL );
    if ( iEglContext == NULL )
        {
        _LIT(KCreateContextFailed, "eglCreateContext failed");
        User::Panic( KCreateContextFailed, 0 );
        }

    /* Make the context current. Binds context to the current rendering thread
       and surface.*/
    if ( eglMakeCurrent( iEglDisplay, iEglSurface, iEglSurface, iEglContext )
        == EGL_FALSE )
        {
        _LIT(KMakeCurrentFailed, "eglMakeCurrent failed");
        User::Panic( KMakeCurrentFailed, 0 );
        }

    TSize size;
    size = this->Size();

    iPickSelect = CPickSelect::NewL( size.iWidth, size.iHeight ); // Create an instance of PickSelect
    iPickSelect->AppInit();                                // Initialize OpenGL ES

    iOpenGlInitialized = ETrue;

    iPeriodic = CPeriodic::NewL( CActive::EPriorityIdle ); // Create an active object for
                                                           // animating the scene
    iPeriodic->Start( 100, 100,
                      TCallBack( CPickSelectContainer::DrawCallBack, this ) );
}

// Destructor
CPickSelectContainer::~CPickSelectContainer()
    {
    delete iPeriodic;

    if ( iPickSelect )
        {
        /* AppExit call is made to release
           any allocations made in AppInit. */
        iPickSelect->AppExit();
        delete iPickSelect;
        }

    eglMakeCurrent( iEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
    eglDestroySurface( iEglDisplay, iEglSurface );
    eglDestroyContext( iEglDisplay, iEglContext );
    eglTerminate( iEglDisplay );                   // Release resources associated
                                                   // with EGL and OpenGL ES
    }

// ---------------------------------------------------------
// CPickSelectContainer::SizeChanged
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CPickSelectContainer::SizeChanged()
    {
    if( iOpenGlInitialized && iPickSelect )
        {
        TSize size;
        size = this->Size();

        iPickSelect->SetScreenSize( size.iWidth, size.iHeight );
        }
    }


// ---------------------------------------------------------
// CPickSelectContainer::HandleResourceChange(
//     TInt aType)
// Dynamic screen resize changes by calling the
// SetExtentToWholeScreen() method again.
// ---------------------------------------------------------
//
 void CPickSelectContainer::HandleResourceChange(TInt aType)
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
// CPickSelectContainer::CountComponentControls
// ---------------------------------------------------------
//
TInt CPickSelectContainer::CountComponentControls() const
    {
    return 0;
    }

// ---------------------------------------------------------
// CPickSelectContainer::ComponentControl
// ---------------------------------------------------------
//
CCoeControl* CPickSelectContainer::ComponentControl(TInt /*aIndex*/ ) const
    {
    return NULL;
    }

// ---------------------------------------------------------
// CPickSelectContainer::Draw
// ---------------------------------------------------------
//
void CPickSelectContainer::Draw(const TRect& /*aRect*/ ) const
    {
    // No need to implement anything here!
    }

// ---------------------------------------------------------
// CPickSelectContainer::DrawCallBack
// Called by the CPeriodic in order to draw the graphics
// ---------------------------------------------------------
//
TInt CPickSelectContainer::DrawCallBack( TAny* aInstance )
    {
    CPickSelectContainer* instance = (CPickSelectContainer*) aInstance;
    instance->iFrame++;

    // Call the main OpenGL ES Symbian rendering 'loop'
    instance->iPickSelect->AppCycle( instance->iFrame );

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

    return 0;
    }

// ---------------------------------------------------------
// CPickSelectContainer::HandleControlEventL
// ---------------------------------------------------------
//
void CPickSelectContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    }

// End of File
