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
#include "SimpleCubePbufferContainer.h"
#include <Eikenv.h>
#include <w32std.h>
#include <AknUtils.h> // Pen support. 

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSimpleCubePbufferContainer::ConstructL
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CSimpleCubePbufferContainer::ConstructL(const TRect& aRect, CAknAppUi* aAppUi)
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

    iConfig = configList[0];   // Choose the best EGLConfig. EGLConfigs
                              // returned by eglChooseConfig are sorted so
                              // that the best matching EGLConfig is first in
                              // the list.
    User::Free( configList ); // Free configList, not used anymore.


    TSize size;
    size = this->Size();

    // Create a Symbian bitmap where the graphics from the Pbuffer are copied
    iPixmap = new CWsBitmap( iEikonEnv->WsSession() );
    iPixmap->Create( Window().Size(), DMode);

    const EGLint attrib_list2[] = { EGL_WIDTH,
                                    size.iWidth,
                                    EGL_HEIGHT,
                                    size.iHeight,
                                    EGL_NONE };

    iEglSurface = eglCreatePbufferSurface( iEglDisplay, iConfig, attrib_list2 );

    if ( iEglSurface == NULL )
        {
        _LIT(KCreatePbufferSurfaceFailed, "eglCreatePbufferSurface failed");
        User::Panic( KCreatePbufferSurfaceFailed, 0 );
        }

    // Create a rendering context
    iEglContext = eglCreateContext( iEglDisplay, iConfig, EGL_NO_CONTEXT, NULL );
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


    // Create an instance of SimpleCubePbuffer
    iSimpleCubePbuffer = CSimpleCubePbuffer::NewL( size.iWidth, size.iHeight );
    iSimpleCubePbuffer->AppInit();                           // Initialize OpenGL ES

    iOpenGlInitialized = ETrue;

    iPeriodic = CPeriodic::NewL( CActive::EPriorityIdle );  // Create an active object for
                                                            // animating the scene
    iPeriodic->Start( 100, 100, TCallBack( CSimpleCubePbufferContainer::DrawCallBack, this ) );
    }

// Destructor
CSimpleCubePbufferContainer::~CSimpleCubePbufferContainer()
    {
    delete iPeriodic;

    /* AppExit call is made to release
       any allocations made in AppInit. */
    if ( iSimpleCubePbuffer )
        {
        iSimpleCubePbuffer->AppExit();
        delete iSimpleCubePbuffer;
        }

    eglMakeCurrent( iEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
    eglDestroySurface( iEglDisplay, iEglSurface );
    eglDestroyContext( iEglDisplay, iEglContext );
    eglTerminate( iEglDisplay );                   // Release resources associated
                                                   // with EGL and OpenGL ES

    delete iPixmap;
    }

// ---------------------------------------------------------
// CSimpleCubePbufferContainer::SizeChanged
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CSimpleCubePbufferContainer::SizeChanged()
    {
    if( iOpenGlInitialized && iSimpleCubePbuffer )
        {
        TSize size;
        size = this->Size();

        // Delete previous context etc.
        eglMakeCurrent( iEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
        eglDestroySurface( iEglDisplay, iEglSurface );
        eglDestroyContext( iEglDisplay, iEglContext );
        if( iPixmap )
            {
            delete iPixmap;
            }


        // Recreate the Symbian bitmap where the graphics from the Pbuffer are copied

        iPixmap = new CWsBitmap( iEikonEnv->WsSession() );
        iPixmap->Create( Window().Size(), Window().DisplayMode() );

        // Recreate and bind the new surface and rendering context
        const EGLint attrib_list2[] = { EGL_WIDTH,
                                        size.iWidth,
                                        EGL_HEIGHT,
                                        size.iHeight,
                                        EGL_NONE };

        iEglSurface = eglCreatePbufferSurface( iEglDisplay, iConfig, attrib_list2 );

        if ( iEglSurface == NULL )
            {
            _LIT(KCreatePbufferSurfaceFailed, "eglCreatePbufferSurface failed");
            User::Panic( KCreatePbufferSurfaceFailed, 0 );
            }

        // Create a rendering context
        iEglContext = eglCreateContext( iEglDisplay, iConfig, EGL_NO_CONTEXT, NULL );
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


        // Notify the renderer class
        iSimpleCubePbuffer->SetScreenSize( size.iWidth, size.iHeight );
        }
    }


// ---------------------------------------------------------
// CSimpleCubePbufferContainer::HandleResourceChange(
//     TInt aType)
// Dynamic screen resize changes by calling the
// SetExtentToWholeScreen() method again.
// ---------------------------------------------------------
//
 void CSimpleCubePbufferContainer::HandleResourceChange(TInt aType)
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
// CSimpleCubePbufferContainer::CountComponentControls
// ---------------------------------------------------------
//
TInt CSimpleCubePbufferContainer::CountComponentControls() const
    {
    return 0;
    }

// ---------------------------------------------------------
// CSimpleCubePbufferContainer::ComponentControl
// ---------------------------------------------------------
//
CCoeControl* CSimpleCubePbufferContainer::ComponentControl(TInt /*aIndex*/) const
    {
    return NULL;
    }
// ---------------------------------------------------------
// CSimpleCubePbufferContainer::Draw
// ---------------------------------------------------------
//
void CSimpleCubePbufferContainer::Draw(const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();
    gc.BitBlt( TPoint( 0, 0 ), iPixmap );
    }

// ---------------------------------------------------------
// CSimpleCubePbufferContainer::DrawCallBack
// Called by the CPeriodic in order to draw the graphics
// ---------------------------------------------------------
//
int CSimpleCubePbufferContainer::DrawCallBack( TAny* aInstance )
    {
    CSimpleCubePbufferContainer* instance
      = (CSimpleCubePbufferContainer*) aInstance;
    instance->iFrame++;

    // Call the main OpenGL ES Symbian rendering 'loop'
    instance->iSimpleCubePbuffer->AppCycle( instance->iFrame );

    // Copy contents of Pbuffer surface to a native Pixmap
    eglCopyBuffers( instance->iEglDisplay, instance->iEglSurface, instance->iPixmap );

    /* eglSwapBuffers is not called when using Pixmap surface.
       Instead, standard Symbian BitBlt has to be used. */
    instance->DrawNow( );

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
// CSimpleCubePbufferContainer::HandleControlEventL
// ---------------------------------------------------------
//
void CSimpleCubePbufferContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    }


// End of File
