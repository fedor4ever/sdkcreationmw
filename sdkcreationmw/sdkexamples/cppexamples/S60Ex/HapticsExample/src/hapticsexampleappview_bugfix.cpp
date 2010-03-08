/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares the control container (view) class
*
*/


#include <eikapp.h>
#include <aknutils.h>
#include <Aknnotewrappers.h>
#include <sensrvchannelfinder.h> 
#include <sensrvchannel.h>
#include <sensrvorientationsensor.h>
#include <hwrmhaptics.h>
#include <hwrmhapticslicense.h>
#include <hapticsexample.rsg>
#include "hapticsexampleappview.h"
#include "hapticsexampleball.h"
#include "hapticsexamplelicensekey.h"
#include "hapticsexampleconstants.h"

// note and warning messages
_LIT( KLoadIVTErrFormat, "Loading IVT file failed: %d" );
_LIT( KInvalidLicenseKeyText, "Invalid license key!" );
_LIT( KNoSupportedActuatorsText, "No supported actuators found!" );

_LIT( KSensorNotSupportedNoteText, "Rotation sensor data channel not supported" );


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CHapticsExampleAppView* CHapticsExampleAppView::NewL( const TRect& aRect )
    {
    CHapticsExampleAppView* self = new ( ELeave ) CHapticsExampleAppView;
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CHapticsExampleAppView::ConstructL( const TRect& aRect )
    {    
    // Create fall timer, but don't start yet
    iTicks = CPeriodic::NewL( CActive::EPriorityStandard );

    // Create a window for this application view
    CreateWindowL();
    EnableDragEvents();

    // Update object dimension
    ObjectDimension();

    // Create ball
    iBall = CHapticsExampleBall::NewL();
    iBall->SetContainerWindowL( *this );
    iBall->MakeVisible( ETrue );

    // Set the windows size
    SetRect( aRect );

    // Set objects to default positions
    SetObjects();

    // Create haptics client
    iHaptics = CHWRMHaptics::NewL( NULL, NULL );

    // Fetch supported actuators
    TUint32 actuators( 0 );
    User::LeaveIfError( iHaptics->SupportedActuators( actuators ) );

    // Open actuator of type EHWRMLogicalActuatorAny if supported
    if( EHWRMLogicalActuatorAny & actuators )
        {
        iHaptics->OpenActuatorL( EHWRMLogicalActuatorAny );
        }
    // Otherwise open actuator of type EHWRMLogicalActuatorDevice if supported
    else if ( EHWRMLogicalActuatorDevice & actuators ) 
        {
        iHaptics->OpenActuatorL( EHWRMLogicalActuatorDevice );    
        }
    else 
        {
        ErrorNoteL( KNoSupportedActuatorsText );
        // if none opened, will not continue as example is about haptics
        User::Leave( KErrGeneral );
        }

    // Set the license key
    TInt licenseErr = iHaptics->SetDeviceProperty( 
                    CHWRMHaptics::EHWRMHapticsLicensekey, KLicenseKey );
    if ( licenseErr )
        {
        ErrorNoteL( KInvalidLicenseKeyText );
        // if license setting failed, will not continue this example 
        User::Leave( KErrGeneral );
        }
        
    // Set the haptics priority                                     
    User::LeaveIfError( 
            iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsPriority,
                                         KHWRMHapticsMaxDevicePriority ) );

    // Set the strength
    User::LeaveIfError( 
            iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsStrength,
                                         iStrength ) );

    // Reserve haptics, otherwise effect playing may be ignored by player
    iHaptics->ReserveHapticsL();
    
    // Load IVT file
    TRAPD( loadErr, LoadEffectsFromFileL( KIVTFileName ) );
    if ( loadErr )
        {
        TBuf<KMaxNoteLength> noteBuf;
        noteBuf.Format( KLoadIVTErrFormat, loadErr );
        ErrorNoteL( noteBuf );
        }

    ActivateL();
    }

// ---------------------------------------------------------------------------
// Frees reserved resources
// ---------------------------------------------------------------------------
//
CHapticsExampleAppView::~CHapticsExampleAppView()
    {
    TRAP_IGNORE( SetSensorModeL( EFalse ) );
    delete iTicks;

    delete iBall;
    delete iHaptics; // Note: Also releases the reservation, deletes loaded
                     //       data buffers and stops any playing or paused 
                     //       effects
    }

// ---------------------------------------------------------------------------
// Sets a new haptics strength
// ---------------------------------------------------------------------------
//
void CHapticsExampleAppView::SetHapticsStrength( TInt aStrength )
    {
    if ( KMinStrength <= aStrength && aStrength <= KMaxStrength )
        {
        iStrength = ( KHWRMHapticsMaxStrength * aStrength ) / K100Percent;
        }

    iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsStrength,
                                 iStrength );
    }

// ---------------------------------------------------------------------------
// Sets a new disable/enable status of haptics
// ---------------------------------------------------------------------------
//
void CHapticsExampleAppView::SetHapticsEnabled( TBool aEnabled )
    {
    iHapticsEnabled = aEnabled;
    
    iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsDisableEffects,
                                 !iHapticsEnabled );
    }

// ---------------------------------------------------------------------------
// Gets a new disable/enable status of haptics
// ---------------------------------------------------------------------------
//
TBool CHapticsExampleAppView::HapticsEnabled() const
    {
    return iHapticsEnabled;
    }

// ---------------------------------------------------------------------------
// Sets the sensor mode on/off. If the sensor mode is set on, also starts
// the sensor framework listener and starts to listen for sensor events 
// (provided that the device supports these..).
// If the sensor mode is set off (EFalse), stops the possible listener, closes
// the sensor channel and deletes the sensor channel (listener) member.
// ---------------------------------------------------------------------------
//
void CHapticsExampleAppView::SetSensorModeL( TBool aSensorModeOn )
    {
    // First, stop any effect, cancel ticks, and reset ball position.
    iHaptics->StopAllPlayingEffects();

    if ( iTicks->IsActive() )
        {
        iTicks->Cancel();
        }
    // moves ball to lower right corner of screen
    Move( iBall, 
          TPoint( iDragArea.iBr.iX - iBallDiameter,
                  iDragArea.iBr.iY - iBallDiameter), 
          EFalse );
    iBallVelocity.iX = 0;
    iBallVelocity.iY = 0;
    iScaledTiltX = 0;
    iScaledTiltY = 0;

    
    if ( aSensorModeOn )
        {    
        // Find sensor service data channel for rotation data events
        CSensrvChannelFinder* finder =
            CSensrvChannelFinder::NewLC(); 
        RSensrvChannelInfoList channelInfoList;
        CleanupClosePushL( channelInfoList );
        TSensrvChannelInfo match;
        match.iChannelType = KSensrvChannelTypeIdRotationData;
        finder->FindChannelsL( channelInfoList, match );

        if ( 0 == channelInfoList.Count() )
            {
            UserNoteL( KSensorNotSupportedNoteText );
            }
        else
            {
            // Rotation sensor data channel is supported in the device,
            // start listening for channel data
            if ( !iSensrvChannel )
                {
                iSensrvChannel = CSensrvChannel::NewL( channelInfoList[ 0 ] );
                iSensrvChannel->OpenChannelL();
                iSensrvChannel->StartDataListeningL( this, // listener 
                                                     1,    // aDesiredCount
                                                     1,    // aMaximumCount
                                                     0 );  // aBufferingPeriod 
                }

            iTicksInterval = KTicksSensorModeInterval;
            iTicks->Start( 
                    iTicksInterval, iTicksInterval, 
                    TCallBack( 
                        CHapticsExampleAppView::HandleSensorTick, this ) );
            }

        CleanupStack::PopAndDestroy( &channelInfoList );
        CleanupStack::PopAndDestroy( finder );
        }
    else // stopping the sensor mode
        {
        if ( iSensrvChannel )
            {
            iSensrvChannel->StopDataListening(); 
            iSensrvChannel->CloseChannel();
            delete iSensrvChannel;
            iSensrvChannel = NULL;
            iTicksInterval = KTicksInitialInterval;
            }
        }
    }
    

// ---------------------------------------------------------------------------
// Return true if sensor mode is active.
// ---------------------------------------------------------------------------
//
TBool CHapticsExampleAppView::SensorMode() const
    {
    return ( iSensrvChannel != NULL );
    }

// ---------------------------------------------------------------------------
// Return count of controls inside this container.
// ---------------------------------------------------------------------------
//
TInt CHapticsExampleAppView::CountComponentControls() const
    {
    return 1; // iBall instance
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// return pointer of controls inside this container
// ---------------------------------------------------------------------------
//
CCoeControl* CHapticsExampleAppView::ComponentControl( TInt /*aIndex*/ ) const
    {
    return iBall;
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// Draws to the screen.
// ---------------------------------------------------------------------------
//
void CHapticsExampleAppView::Draw( const TRect& aRect ) const
    {
    // Get the standard graphics context
    CWindowGc& gc = SystemGc();

    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KRgbBlack );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );

    // Clears the area
    gc.Clear( aRect );
    }

// ---------------------------------------------------------------------------
// From class CCoeControl.
// Called by framework when the view size is changed.
// Set objects to default positions
// ---------------------------------------------------------------------------
//
void CHapticsExampleAppView::SizeChanged()
    {
    // Gets the control's extent
    TRect rect( Rect() );

    // Set area where dragging is possible
    iDragArea.SetRect( rect.iTl, rect.iBr );

    // Update object dimension
    ObjectDimension();
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// Moves objects and plays feedback during dragging
// ---------------------------------------------------------------------------
//
void CHapticsExampleAppView::HandlePointerEventL( 
        const TPointerEvent& aPointerEvent )
    {
    if ( iSensrvChannel )
        {
        // In sensor mode (when listening to sensor channel) pointer events
        // are not handled as the movement is dictated by tilting the device
        return;
        }
        
    TPoint point = aPointerEvent.iPosition;

    if ( iDragArea.Contains( point ) )
        {
        if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
            {
            if ( iBall->HitRegionContains( point, *iBall ) )
                {
                // dragging of ball started, stop the fall timer if
                if ( iTicks->IsActive() )
                    {
                    iTicks->Cancel();
                    }
                iDragState = EDownEventReceived;
                iDragStateWaitCount = 0;
                iFallingBall = EFalse;
                iDraggingOffset = iBall->Position() - point;
                }
            else
                {
                // in some cases dragging state can be still active, correct it
                iDragState = ENotDragging;
                }
            }

        if ( aPointerEvent.iType == TPointerEvent::EDrag )
            {
            if( ENotDragging != iDragState )
                {
                if ( iDragStateWaitCount++ > KDragSensitivity )  
                    {
                    // intentional dragging
                    iDragState = EDragging;
                    }
                if ( EffectActive( iFallEffectHandle ) )
                    {
                    iHaptics->StopPlayingEffect( iFallEffectHandle );
                    }
                if ( !EffectActive( iDragEffectHandle ) )
                    {
                    CHWRMHaptics::THWRMHapticsMagSweepEffect effect;
                    effect.iDuration = iHaptics->InfiniteDuration();
                    // magnitude set to 20% of max
                    effect.iMagnitude = KHWRMHapticsMaxMagnitude
                                        * K20Percent / K100Percent;
                    effect.iStyle = CHWRMHaptics::EHWRMHapticsStyleSharp;
                    effect.iAttackTime = 0;
                    effect.iAttackLevel = 0;
                    effect.iFadeTime = 0;
                    effect.iFadeLevel = 0;
                    iHaptics->PlayMagSweepEffect( effect, iDragEffectHandle );               
                    }
                
                // Move the ball according to dragging and update velocities
                TPoint oldPos( iBall->Position() );     
                Move( iBall, point + iDraggingOffset, EFalse );
                // The velocity value is multiplied by two (that gives
                // more realistic feeling when releasing the drag, i.e., the 
                // ball continues for a while along the dragging before it
                // starts to free-fall).                
                iBallVelocity.iX = ( iBall->Position().iX - oldPos.iX ) * 2;
                iBallVelocity.iY = ( iBall->Position().iY - oldPos.iY ) * 2;
                }
            }
        }

    if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        if ( EDownEventReceived == iDragState )
            {
            // Normal down-up sequence (no dragging in between) is causes
            // the ball to bounce. The bouncing is done in opposite direction
            // from "pointer event position" <--> "ball's mid point" vector
            
            // First a small bounce effect is played
            TInt tmpHandle( 0 );
            CHWRMHaptics::THWRMHapticsMagSweepEffect effect;
            effect.iDuration = 30; // in milliseconds.
            // magnitude is 30% of the maximum
            effect.iMagnitude = KHWRMHapticsMaxMagnitude
                                * K30Percent / K100Percent;
            effect.iStyle = CHWRMHaptics::EHWRMHapticsStyleSharp;
            effect.iAttackTime = 0;
            effect.iAttackLevel = 0;
            effect.iFadeTime = 0;
            effect.iFadeLevel = 0;
            iHaptics->PlayMagSweepEffect( effect, tmpHandle );

            TPoint ballMidPoint = iBall->Rect().Center();
            iBallVelocity.iX = ballMidPoint.iX - point.iX;
            iBallVelocity.iY = KHitBounce;
            }
                        
        // possible dragging is canceled if button is released
        iDragState = ENotDragging;
        iFallingBall = ETrue;

        if ( EffectActive( iDragEffectHandle ) )
            {
            iHaptics->StopPlayingEffect( iDragEffectHandle );
            }

        if ( iBall->Position().iY < ( iDragArea.iBr.iY - iBallDiameter ) ||
             iBallVelocity.iX != 0 ) 
            {
            // Start the ticks timer if not already running. That will start
            // the free falling
            if ( !iTicks->IsActive() )
                {
                iTicks->Start( 
                    iTicksInterval, 
                    iTicksInterval, 
                    TCallBack( CHapticsExampleAppView::HandleTick, this ) );
                }
            }
        }

    CCoeControl::HandlePointerEventL( aPointerEvent );
    }

// ---------------------------------------------------------------------------
// From class CCoeControl.
// Enables feedback when layout is changed
// ---------------------------------------------------------------------------
//
void CHapticsExampleAppView::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        // Set ball to default position
        SetObjects();
        }
    }

// ---------------------------------------------------------------------------
// From class CCoeControl.
// Handles key events (navigation keys)
// ---------------------------------------------------------------------------
//
TKeyResponse CHapticsExampleAppView::OfferKeyEventL(
        const TKeyEvent &aKeyEvent, TEventCode /*aType*/ )
    {
    TKeyResponse rVal( EKeyWasConsumed );
    
    TInt xMove(0);
    TInt yMove(0);
    switch ( aKeyEvent.iCode )
        {
        // Arrow keys are used for moving
        case EKeyUpArrow:
            yMove = -KKeyStep;
            break;    
        case EKeyDownArrow:
            yMove = KKeyStep;
            break;    
        case EKeyRightArrow:
            xMove = KKeyStep;
            break;    
        case EKeyLeftArrow:
            xMove = -KKeyStep;
            break;    

        // "Ok" (central navikey) key triggers ball falling
        case EKeyOK:
            //flow through
        case EKeyEnter:
            {
            // emulate pointer release
            TPoint newPos = iBall->Position();
            TPointerEvent emulatedEvent;
            emulatedEvent.iType = TPointerEvent::EButton1Up;
            emulatedEvent.iPosition = newPos;
            iDragState = EDownEventReceived; 
            HandlePointerEventL( emulatedEvent );
            iDragState = ENotDragging;
            }
            break;
            
        default:
            rVal = EKeyWasNotConsumed;
            break;
        }

    if ( xMove || yMove )
        {
        // emulate dragging
        TPoint newPos = iBall->Position() + TPoint(xMove, yMove);
        TPointerEvent emulatedEvent;
        emulatedEvent.iType = TPointerEvent::EDrag;
        emulatedEvent.iPosition = newPos;
        TDragState oldState = iDragState;
        iDragState = EDragging; 
        HandlePointerEventL( emulatedEvent );
        iDragState = oldState;
        }
    
    return rVal;
    }

// ---------------------------------------------------------------------------
// Handles rotation data channel notifications from sensor framework
// Note: Not interested in lost data.
// ---------------------------------------------------------------------------
//
void CHapticsExampleAppView::DataReceived( 
    CSensrvChannel& aChannel, TInt aCount, TInt /* aDataLost */ )
    {
    if ( KSensrvChannelTypeIdRotationData ==
         aChannel.GetChannelInfo().iChannelType )
        {
        TSensrvRotationData rotData;

        // skip data from the channel until last available rotation data
        for ( TInt i = 0; i < aCount; ++i )
            {
            TPckgBuf<TSensrvRotationData> rotationDataPackage;
            aChannel.GetData( rotationDataPackage );
            rotData = rotationDataPackage();
            }
        
        if ( TSensrvRotationData::KSensrvRotationUndefined 
             != rotData.iDeviceRotationAboutYAxis )
            {
            iScaledTiltX = 
                        ( rotData.iDeviceRotationAboutYAxis - KXAxisZeroPoint ) 
                        / KRotationStep;
            if ( KMaxTiltToLeft > iScaledTiltX )
                {
                iScaledTiltX = KMaxTiltToLeft;
                }
            else if ( KMaxTiltToRight < iScaledTiltX )
                {
                iScaledTiltX = KMaxTiltToRight;
                }
            }
        if ( TSensrvRotationData::KSensrvRotationUndefined 
             != rotData.iDeviceRotationAboutXAxis )
            {
            iScaledTiltY = 
                        ( KYAxisZeroPoint - rotData.iDeviceRotationAboutXAxis ) 
                        / KRotationStep;
            if ( KMaxTiltToLeft > iScaledTiltY )
                {
                iScaledTiltY = KMaxTiltToLeft;
                }
            else if ( KMaxTiltToRight < iScaledTiltY )
                {
                iScaledTiltY = KMaxTiltToRight;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Handles errors from sensor framework
// ---------------------------------------------------------------------------
//
void CHapticsExampleAppView::DataError( CSensrvChannel& /*aChannel*/, 
                                        TSensrvErrorSeverity /*aError*/ )
    {
    
    }

// ---------------------------------------------------------------------------
// From sensor framework's callback interface. Not supported. 
// ---------------------------------------------------------------------------
//
void CHapticsExampleAppView::GetDataListenerInterfaceL( TUid /* aInterfaceUid */, 
                                                        TAny*& aInterface )
    {
    aInterface = NULL;
    }

// ---------------------------------------------------------------------------
// C++ default constructor
// ---------------------------------------------------------------------------
//
CHapticsExampleAppView::CHapticsExampleAppView()
    : iDragState( ENotDragging ),
      iStrength( KHWRMHapticsMaxStrength / 4 ), // 25% of strength 
      iHapticsEnabled( ETrue ),
      iTicksInterval( KTicksInitialInterval )
    {
    
    }

// ---------------------------------------------------------------------------
// Sets the ball to default position
// ---------------------------------------------------------------------------
//
void CHapticsExampleAppView::SetObjects()
    {        
    TPoint ballPos = TPoint( iDragArea.iBr.iX - iBallDiameter,
                             iDragArea.iBr.iY - iBallDiameter );
                             
    iBall->SetExtent( ballPos,
                      TSize( iBallDiameter, iBallDiameter ) );

    iBallVelocity.iX = 0;
    iBallVelocity.iY = 0;
    iScaledTiltX = 0;
    iScaledTiltY = 0;
    }

// ---------------------------------------------------------------------------
// Adjusts aPoint to match the grid and moves the control if needed
// ---------------------------------------------------------------------------
//
void CHapticsExampleAppView::Move( 
        CCoeControl* aControl, TPoint aPoint, TBool aPlayFallEffect )
    {
    if ( aPlayFallEffect )
        {
        // Update the falling effect
        UpdateFallEffect();
        }
        
    // adjust new position to be within the drag area;
    // in sensor mode also play bounce effect and set the velocity to zero
    if ( aPoint.iY > ( iDragArea.iBr.iY - iBallDiameter ) )
        {
        aPoint.iY = iDragArea.iBr.iY - iBallDiameter;
        if ( iSensrvChannel )
            {
            PlayBounceEffect( iBallVelocity.iY );
            iBallVelocity.iY = 0;
            }
        }
    else if ( aPoint.iY <  iDragArea.iTl.iY )
        {
        aPoint.iY = iDragArea.iTl.iY;
        if ( iSensrvChannel )
            {
            PlayBounceEffect( iBallVelocity.iY );
            iBallVelocity.iY = 0;
            }
        }
    if ( aPoint.iX > ( iDragArea.iBr.iX - iBallDiameter ) )
        {
        aPoint.iX = iDragArea.iBr.iX - iBallDiameter;
        if ( iSensrvChannel )
            {
            PlayBounceEffect( iBallVelocity.iX );
            iBallVelocity.iX = 0;
            }
        }
    else if ( aPoint.iX < iDragArea.iTl.iX )
        {
        aPoint.iX = iDragArea.iTl.iX;
        if ( iSensrvChannel )
            {
            PlayBounceEffect( iBallVelocity.iX );
            iBallVelocity.iX = 0;
            }
        }
    
    // re-draw in new position
    if ( aPoint != aControl->Position() )
        {
        // Define the changed area
        TRect drawRect = TRect( aPoint, aControl->Size() );
        drawRect.BoundingRect( aControl->Rect() );

        // set control to new position
        aControl->SetPosition( aPoint );

        // draw only changed area
        DrawNow( drawRect );
        }
    }


// ---------------------------------------------------------------------------
// Calculates objects dimesion related to screen size.
// ---------------------------------------------------------------------------
//
void CHapticsExampleAppView::ObjectDimension()
    {
    TPixelsAndRotation screenSize;
    iCoeEnv->ScreenDevice()->GetDefaultScreenSizeAndRotation( screenSize );

    TInt ballSize = Min( screenSize.iPixelSize.iWidth, 
                         screenSize.iPixelSize.iHeight ) / KScreenFraction;
    if ( ballSize > KMaxBallSize )
        {
        ballSize = KMaxBallSize; // limit it to KMaxBallSize
        }

    iBallDiameter = ballSize;
    }

// ---------------------------------------------------------------------------
// Loads effects from a file into a local buffer and then into haptics
// ---------------------------------------------------------------------------
// 
void CHapticsExampleAppView::LoadEffectsFromFileL( const TDesC& aFileName ) 
    {
    RFs fs;
    TFileName filePath;
    iEffectFileHandle = 0; // if loading fails, stays 0;

    User::LeaveIfError( fs.Connect() );  
    CleanupClosePushL( fs );

    // set application private path as default path
    User::LeaveIfError( fs.PrivatePath( filePath ) );
    User::LeaveIfError( fs.SetSessionPath( filePath ) ); 
    
    RFile file;
    User::LeaveIfError( file.Open( fs, aFileName, EFileRead ) );
    CleanupClosePushL( file );
    TInt fileSize( 0 );
    file.Size( fileSize );
    HBufC8* effectDataBuf = HBufC8::NewLC( fileSize );
    TPtr8 effectDataBufPtr = effectDataBuf->Des();
    User::LeaveIfError( file.Read( effectDataBufPtr ) );
    // Haptics effect data is being loaded
    User::LeaveIfError( iHaptics->LoadEffectData( *effectDataBuf, 
                                                  iEffectFileHandle ) );
    CleanupStack::PopAndDestroy( effectDataBuf );
    CleanupStack::PopAndDestroy( &file );
    CleanupStack::PopAndDestroy( &fs );
    }

// ---------------------------------------------------------------------------
// Checks effect state 
// ---------------------------------------------------------------------------
//
TBool CHapticsExampleAppView::EffectActive( TInt aEffectHandle )
    {
    TInt result = EFalse;
    TInt effectState( CHWRMHaptics::EHWRMHapticsEffectNotPlaying );
    TInt err = iHaptics->GetEffectState( aEffectHandle, effectState );
    if ( effectState == CHWRMHaptics::EHWRMHapticsEffectPlaying && !err )
        {
        result = ETrue;
        }

    return result;
    }

// ---------------------------------------------------------------------------
// Static callback method for handling CPeriodic timer's ticks
// ---------------------------------------------------------------------------
//
TInt CHapticsExampleAppView::HandleTick( TAny* aPtr )
    {
    static_cast<CHapticsExampleAppView*>( aPtr )->DoHandleTick();
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Method for handling the tick within an object (i.e., non-static)
// ---------------------------------------------------------------------------
// 
void CHapticsExampleAppView::DoHandleTick()
    {
    // Update fall velocity
    UpdateFallVelocity();

    // Move ball to new position
    Move( iBall, 
            TPoint( iBall->Position().iX + iBallVelocity.iX,
                    iBall->Position().iY + iBallVelocity.iY ), 
           ETrue );
    
    // Check if there's need for bounce (i.e., ball hits top, bottom or sides)
    CheckBounce();

    // If the ball has stopped cancel timer and stop Haptics fall effect 
    if ( iBall->Position().iY == iDragArea.iBr.iY - iBallDiameter &&
         iBallVelocity == TVelocity( 0,0 ) )
        {
        if ( iTicks->IsActive() )
            {
            iTicks->Cancel();
            }
        if ( EffectActive( iFallEffectHandle ) )
            {
            iHaptics->StopPlayingEffect( iFallEffectHandle );
            }    
        }
    ++iRound;
    User::ResetInactivityTime();
    }

// ---------------------------------------------------------------------------
// Static callback method for handling CPeriodic timer's ticks in sensor mode
// ---------------------------------------------------------------------------
//
TInt CHapticsExampleAppView::HandleSensorTick( TAny* aPtr )
    {
    static_cast<CHapticsExampleAppView*>( aPtr )->DoHandleSensorTick();
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Method for handling the tick within an object (i.e., non-static) in
// sensor mode
// ---------------------------------------------------------------------------
// 
void CHapticsExampleAppView::DoHandleSensorTick()
    {
    UpdateSensorModeVelocity();

    Move( iBall, 
          TPoint( iBall->Position().iX + iBallVelocity.iX,
                  iBall->Position().iY + iBallVelocity.iY ), 
          ETrue );
    User::ResetInactivityTime();
    }

// ---------------------------------------------------------------------------
// Updates the falling (Y-axis) velocity
// ---------------------------------------------------------------------------
// 
void CHapticsExampleAppView::UpdateFallVelocity()
    {
    // Velocity is increased if the Y-axis movement hasn't stopped yet
    if ( !FallStopped() )
        {
        iBallVelocity.iY += KDownAcceleration;
        }
    }

// ---------------------------------------------------------------------------
// Updates the velocity in sensor mode (both X- and Y-axis)
// ---------------------------------------------------------------------------
// 
void CHapticsExampleAppView::UpdateSensorModeVelocity()
    {
    TPoint ballPos( iBall->Position() );

    if ( iScaledTiltX )
        {
        if ( ( 0 < iScaledTiltX && 
               ballPos.iX < ( iDragArea.iBr.iX - iBallDiameter ) ) ||
             ( 0 > iScaledTiltX && ballPos.iX > iDragArea.iTl.iX ) )
            {
            // X-axis, change velocity, but only if not resting on tilted side
            iBallVelocity.iX += iScaledTiltX;
            }
        }
    if ( iScaledTiltY )
        {
        if ( ( 0 < iScaledTiltY &&
               ballPos.iY < ( iDragArea.iBr.iY - iBallDiameter ) ) ||
             ( 0 > iScaledTiltY && ballPos.iY > iDragArea.iTl.iY ) )
            {
            // Y-axis, change velocity, but only if not resting on tilted side
            iBallVelocity.iY += iScaledTiltY;
            }
        }
    }

// ---------------------------------------------------------------------------
// Checks bounces (ball hitting the walls) and plays bounce effect if needed 
// ---------------------------------------------------------------------------
// 
void CHapticsExampleAppView::CheckBounce()
    {
    TPoint ballPos( iBall->Position() );
    TInt bounceVelocity( 0 );
    
    // First, check the bottom and top bounces 
    if ( ballPos.iY >= ( iDragArea.iBr.iY - iBallDiameter ) )
        {
        // the bottom of the drag area has been hit 
        if ( 0 != iBallVelocity.iY )
            {
            // bounce effect needed
            bounceVelocity = iBallVelocity.iY;
            }
        // if the velocity is very small stop the Y axis movement 
        // (i.e., set velocity to zero)
        if ( iBallVelocity.iY < KDownAcceleration )
            {
            iBallVelocity.iY = 0;
            }
        // otherwise change the direction (and also slow down the ball)
        else 
            {
            iBallVelocity.iY -= KBottomBounceDecrease;
            iBallVelocity.iY = -iBallVelocity.iY;
            }
        }
    else if ( ballPos.iY <= iDragArea.iTl.iY )
        {
        // the top of the drag area has been hit => bounce effect needed
        bounceVelocity = Abs( iBallVelocity.iY );
        // change the direction and slow down the ball
        iBallVelocity.iY += KTopBounceDecrease;
        iBallVelocity.iY = -iBallVelocity.iY;
        }

    // Then check the left and right side bounces 
    if ( ballPos.iX < ( iDragArea.iBr.iX - iBallDiameter ) &&
         ballPos.iX > iDragArea.iTl.iX )
        {
        // No bounce from the sides
        if ( 0 != iBallVelocity.iX )
            {
            // reduce the X-axis (absolute) speed (we're using same amount as
            // from side bounces here).
            // Note: When Y-axis movement has stopped, the X-axis 
            //       speed is reduced every round, otherwise just 
            //       every fourth round
            if ( ( 0 == iBallVelocity.iY && 
                   ballPos.iY == ( iDragArea.iBr.iY - iBallDiameter ) ) ||
                 0 == iRound % 4 )
                {
                iBallVelocity.iX += ( iBallVelocity.iX < 0 ? 
                                      KSidewardDeceleration : 
                                      -KSidewardDeceleration );
                }

            }
        }
    else
        {
        // the left or right side of the drag area has been hit => bounce
        // effect needed 
        if ( 0 != iBallVelocity.iX )
            {
            TInt bounceVelocityX = Abs( iBallVelocity.iX );
            if ( bounceVelocityX > bounceVelocity )
                {
                bounceVelocity = bounceVelocityX;
                }
            }
        // change direction
        iBallVelocity.iX = -iBallVelocity.iX;
        }
     
    PlayBounceEffect( bounceVelocity );
    }



// ---------------------------------------------------------------------------
// Checks whether ball has stopped in Y-axis direction
// ---------------------------------------------------------------------------
//
TBool CHapticsExampleAppView::FallStopped()
    {
    TBool ballStopped = ( iBallVelocity.iY == 0 ); 
    TBool ballInLowestPos = iBall->Rect().iBr.iY == iDragArea.iBr.iY; 
    return ballStopped && ballInLowestPos; 
    }

// ---------------------------------------------------------------------------
// Modifies and plays the fall effect 
// ---------------------------------------------------------------------------
//
void CHapticsExampleAppView::UpdateFallEffect()
    {
    TBool fallEffectActive = EffectActive( iFallEffectHandle );
    if ( iSensrvChannel && iBallVelocity == TVelocity( 0, 0 ) )
        {
        // if there's no movement while in sensor mode, 
        // there should not be any effect playing
        if ( fallEffectActive )
            {
            iHaptics->StopPlayingEffect( iFallEffectHandle );
            }
        }
    else // ball is moving, start or update effect playing
        {
        CHWRMHaptics::THWRMHapticsPeriodicEffect effect;
        effect.iDuration = iHaptics->InfiniteDuration();
        effect.iStyle = CHWRMHaptics::EHWRMHapticsStyleSmooth;
        effect.iAttackTime = 0;
        effect.iAttackLevel = 0;
        effect.iFadeTime = 0;
        effect.iFadeLevel = 0;
        
        // If this is start of the fall, start the haptics effect playing
        if ( !fallEffectActive )
            {
            effect.iMagnitude = KFallInitialMagnitude;
            effect.iPeriod = KFallInitialPeriod;
            iHaptics->PlayPeriodicEffect( effect, iFallEffectHandle ); 
            }
        else
            {
            // otherwise modify the existing fall effect
            TInt xySpeed = 
                Abs( iBallVelocity.iY ) + Abs( iBallVelocity.iX );
            // change magnitude in 200 unit steps according to XY speed
            effect.iMagnitude = KFallInitialMagnitude + xySpeed * 200;
            TInt limit = KHWRMHapticsMaxMagnitude / 4; // 25% of max magnitude
            if ( effect.iMagnitude > limit )
                {
                effect.iMagnitude = limit;
                }
            effect.iPeriod = KFallInitialPeriod - xySpeed * 4; // in ms
            iHaptics->ModifyPlayingPeriodicEffect( iFallEffectHandle, 
                                                   effect );
            }
        }
    }

// ---------------------------------------------------------------------------
// Plays a bounce effect based on the bounce velocity
// ---------------------------------------------------------------------------
//
void CHapticsExampleAppView::PlayBounceEffect( TInt aBounceVelocity )
    {
    if ( 0 != aBounceVelocity )
        {
        if ( iEffectFileHandle )
            {
            const TDesC8 ptr = 
                ( aBounceVelocity < KBounceLevel6Percent ? KEffectBounce6Name :
                  aBounceVelocity < KBounceLevel5Percent ? KEffectBounce5Name :
                  aBounceVelocity < KBounceLevel4Percent ? KEffectBounce4Name :
                  aBounceVelocity < KBounceLevel3Percent ? KEffectBounce3Name :
                  aBounceVelocity < KBounceLevel2Percent ? KEffectBounce2Name : 
                  KEffectBounce1Name );
    
            TInt effectIndex( 0 );
            if ( KErrNone == iHaptics->GetEffectIndexFromName( 
                    iEffectFileHandle, ptr, effectIndex ) )
                {
                TInt tmpEffectHandle( 0 );
                iHaptics->PlayEffect( 
                    iEffectFileHandle, effectIndex, tmpEffectHandle );
                }
            }
        else
            {                 
            // If there's no bounce effect file, the bounce magnitude depends
            // on the velocity. The less velocity, the less magnitude
            TInt magnitude( KHWRMHapticsMaxMagnitude );
            magnitude /= 
                aBounceVelocity < KBounceLevel6Percent ? KBounceLevel6Magnitude :
                aBounceVelocity < KBounceLevel5Percent ? KBounceLevel5Magnitude :
                aBounceVelocity < KBounceLevel4Percent ? KBounceLevel4Magnitude :
                aBounceVelocity < KBounceLevel3Percent ? KBounceLevel3Magnitude :
                aBounceVelocity < KBounceLevel2Percent ? KBounceLevel2Magnitude : 
                1;
            TInt tmpHandle( 0 );
            CHWRMHaptics::THWRMHapticsMagSweepEffect effect;
            effect.iDuration = 50; // in milliseconds.
            effect.iMagnitude = magnitude;
            effect.iStyle = CHWRMHaptics::EHWRMHapticsStyleSharp;
            effect.iAttackTime = 0;
            effect.iAttackLevel = 0;
            effect.iFadeTime = 0;
            effect.iFadeLevel = 0;
            iHaptics->PlayMagSweepEffect( effect, tmpHandle );
            }
        }
    }

// ---------------------------------------------------------------------------
// Shows a note to the screen
// ---------------------------------------------------------------------------
//  
void CHapticsExampleAppView::UserNoteL( const TDesC& aText ) const
    {
    CAknInformationNote* note = new( ELeave ) CAknInformationNote();
    note->ExecuteLD( aText );    
    }

// ---------------------------------------------------------------------------
// Shows an error note on the screen
// ---------------------------------------------------------------------------
//  
void CHapticsExampleAppView::ErrorNoteL( const TDesC& aText ) const
    {
    CAknErrorNote* errorNote = new( ELeave ) CAknErrorNote( ETrue ); // waiting
    errorNote->ExecuteLD( aText );    
    }

// End of File
