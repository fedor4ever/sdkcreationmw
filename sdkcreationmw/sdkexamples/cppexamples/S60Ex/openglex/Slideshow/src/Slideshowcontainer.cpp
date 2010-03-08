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
* Description: 
*
*/


// INCLUDE FILES
#include "SlideshowContainer.h"
#include <AknUtils.h> // Pen support. 

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSlideshowContainer::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CSlideshowContainer::ConstructL(const TRect& aRect, CAknAppUi* aAppUi)
    {
    iAppUi = aAppUi;
	// Open GL hasn't been initialized
    iOpenGlInitialized = EFalse;

	// Create the native window
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

	// Describes the format, type and size of the color buffers and
	// ancillary buffers for EGLSurface
    EGLConfig config;

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

	// Pointer for EGLConfigs
    EGLConfig *configList = NULL;
    EGLint numOfConfigs   = 0;
    EGLint configSize     = 0;

    // Get the number of possible EGLConfigs
    if ( eglGetConfigs( iEglDisplay, configList, configSize, &numOfConfigs ) == EGL_FALSE )
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

    // Define properties for the wanted EGLSurface. To get the best possible
	// performance, choose an EGLConfig with a buffersize matching the current
	// window's display mode
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
        _LIT(KDModeError, "unsupported displaymode");
        User::Panic( KDModeError, 0 );
        break;
    }

    // Define properties for the wanted EGLSurface
    const EGLint attrib_list[] =
        {
				EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BUFFER_SIZE,  BufferSize,
        EGL_DEPTH_SIZE,   16,
        EGL_NONE
        };

    // Choose an EGLConfig that best matches to the properties in attrib_list
    if ( eglChooseConfig( iEglDisplay, attrib_list, configList, configSize, &numOfConfigs ) == EGL_FALSE )
		{
		_LIT(KChooseConfigFailed, "eglChooseConfig failed");
		User::Panic( KChooseConfigFailed, 0 );
		}

	// Check if no configurations were found
	if ( numOfConfigs == 0 )
	    {
		_LIT(KNoConfig, "Can't find the requested config.");
		User::Panic( KNoConfig, 0 );
		}

	// Choose the best EGLConfig. EGLConfigs returned by eglChooseConfig are
	// sorted so that the best matching EGLConfig is first in the list.
    config = configList[0];

	// Free configList, as it's not used anymore.
    User::Free( configList );

    // Create a window where the graphics are blitted
    iEglSurface = eglCreateWindowSurface( iEglDisplay, config, &Window(), NULL );
    if ( iEglSurface == NULL )
	    {
		_LIT(KCreateWindowSurfaceFailed, "eglCreateWindowSurface failed");
        User::Panic( KCreateWindowSurfaceFailed, 0 );
		}

    // Create a rendering context
    iEglContext = eglCreateContext( iEglDisplay, config, EGL_NO_CONTEXT, NULL );
    if ( iEglContext == NULL )
		{
		_LIT(KCreateContextFailed, "eglCreateContext failed");
        User::Panic( KCreateContextFailed, 0 );
		}

    // Make the context current. Binds context to the current rendering thread
    // and surface.
    if ( eglMakeCurrent( iEglDisplay, iEglSurface, iEglSurface, iEglContext ) == EGL_FALSE )
		{
		_LIT(KMakeCurrentFailed, "eglMakeCurrent failed");
        User::Panic( KMakeCurrentFailed, 0 );
		}


    TSize size;
    size = this->Size();
    iSlideshow = CSlideshow::NewL( size.iWidth, size.iHeight );
    iSlideshow->AppInitL();

    iOpenGlInitialized = ETrue;

	// Reset counters
    iFrame             = 0;
    iStartTimeTicks    = User::NTickCount();

	// Create an active object for animating the scene
    iPeriodic = CPeriodic::NewL( CActive::EPriorityIdle );
    iPeriodic->Start( 100, 100, TCallBack( CSlideshowContainer::DrawCallBack, this ) );
    }

// ---------------------------------------------------------
// CSlideshowContainer::~CSlideshowContainer()
// Destructor. Releases any used resources.
// ---------------------------------------------------------
//
CSlideshowContainer::~CSlideshowContainer()
    {
    delete iPeriodic;

    /* AppExit call is made to release any allocations made in AppInit.
     This call has to be made here before we destroy the rendering context. */
    if ( iSlideshow )
		{
        iSlideshow->AppExit();
        delete iSlideshow;
		}

    eglMakeCurrent( iEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
    eglDestroySurface( iEglDisplay, iEglSurface );
    eglDestroyContext( iEglDisplay, iEglContext );
    eglTerminate( iEglDisplay );
    }

// ---------------------------------------------------------
// CSlideshowContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CSlideshowContainer::SizeChanged()
    {
    if( iOpenGlInitialized && iSlideshow )
        {
        TSize size;
        size = this->Size();

        iSlideshow->SetScreenSize( size.iWidth, size.iHeight );
        }
    }


// ---------------------------------------------------------
// CSlideshowContainer::HandleResourceChange(
//     TInt aType)
// Dynamic screen resize changes by calling the
// SetExtentToWholeScreen() method again.
// ---------------------------------------------------------
//
 void CSlideshowContainer::HandleResourceChange(TInt aType)
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
// CSlideshowContainer::CountComponentControls() const
// Return number of controls inside this container
// ---------------------------------------------------------
//
TInt CSlideshowContainer::CountComponentControls() const
    {
    return 0;
    }

// ---------------------------------------------------------
// CSlideshowContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CSlideshowContainer::ComponentControl(TInt /*aIndex*/) const
    {
    return NULL;
    }

// ---------------------------------------------------------
// CSlideshowContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CSlideshowContainer::Draw(const TRect& aRect) const
	{
    CWindowGc& gc = SystemGc();
    gc.Clear( aRect );

    if ( iSlideshow->GetState() == CSlideshow::ELoadingTextures )
		{
		_LIT(KLoading, "Loading...");
        utilOutputText(SystemGc(), KLoading, TPoint(Size().iWidth / 2, Size().iHeight / 2), KRgbBlack );
		}
	}

// ---------------------------------------------------------
// CSlideshowContainer::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CSlideshowContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    // TODO: Add any control event handler code here
    }

// ----------------------------------------------------------------------
// CSlideshowContainer::GetTimeTick
// Returns the current time.
// ----------------------------------------------------------------------
//
TTime CSlideshowContainer::GetTimeTick()
    {
    TTime time;
    time.UniversalTime();
    return time;
    }


// ---------------------------------------------------------
// CSlideshowContainer::DrawCallBack( TAny* aInstance )
// Called by the CPeriodic in order to draw the graphics
// ---------------------------------------------------------
//
TInt CSlideshowContainer::DrawCallBack( TAny* aInstance )
	{
    CSlideshowContainer* instance = (CSlideshowContainer*) aInstance;
    // Update the frame counts
    instance->iFrame++;

    // Compute the elapsed time in seconds since the startup of the example
#ifdef __WINS__

    // In the emulator the tickcount runs at 200Hz
    GLfloat timeSecs = ( (GLfloat) ( User::NTickCount() - instance->iStartTimeTicks ) ) / 200.f;

#else

    // In the device the tickcount runs at 1000hz (as intended)
    GLfloat timeSecs = ( (GLfloat) ( User::NTickCount() - instance->iStartTimeTicks ) ) / 1000.f;

#endif

    // Compute the elapsed time since last frame was drawn. Note that due to the
    // resolution of the system timer this value can be 0.0 even though new frames are being rendered.
    // This applies especially when running the emulator on a PC.
    GLfloat deltaTimeSecs = instance->iLastFrameTimeSecs - timeSecs;

    // Set the current time to be the last frame time for the upcoming frame
    instance->iLastFrameTimeSecs = timeSecs;

    if ( instance->iSlideshow->GetState() == CSlideshow::ERunning )
		{
        // Call the main OpenGL ES Symbian rendering 'loop'
        instance->iSlideshow->AppCycle( instance->iFrame, timeSecs, deltaTimeSecs );

        // Call eglSwapBuffers, which blit the graphics to the window
        eglSwapBuffers( instance->iEglDisplay, instance->iEglSurface );

        // Suspend the current thread for a short while. Give some time
        // to other threads and AOs, avoids the ViewSrv error in ARMI and
        // THUMB release builds. One may try to decrease the callback
		// function instead of this.
        if ( !(instance->iFrame % 50) )
			{
	        // Reset inactivity timer to keep the background light on
		    User::ResetInactivityTime();

            User::After(0);
			}
		}

    return 0;
	}

// End of File
