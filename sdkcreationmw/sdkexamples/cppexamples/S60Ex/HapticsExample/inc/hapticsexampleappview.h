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
* Description:  Declares the control container (view) class for the 
*               Haptics Example
*
*/


#ifndef C_HAPTICSEXAMPLEAPPVIEW_H
#define C_HAPTICSEXAMPLEAPPVIEW_H

#include <coecntrl.h>
#include <sensrvdatalistener.h>

class CSensrvChannel;
class CHWRMHaptics;
class CHapticsExampleBall;

typedef TPoint TVelocity;

/**
* Application view class. Also initializes and configures Haptics, 
* controls ball and performs drawing.  
*/
class CHapticsExampleAppView : public CCoeControl,
                               public MSensrvDataListener 
    {
public:

    /**
     * Two-phased constructor.
     * Create a CHapticsExampleAppView object that draws itself to aRect.
     * @param aRect The rectangle this view will be drawn to.
     * @return a pointer to the created instance of CHapticsExampleAppView.
     */
    static CHapticsExampleAppView* NewL( const TRect& aRect );

    /**
     * Virtual Destructor.
     */
    virtual ~CHapticsExampleAppView();

    /**
     * Method for setting the haptics strength.
     * @param aStrength The new strength in percents.
     */
    void SetHapticsStrength( TInt aStrength );

    /**
     * Method for enabling/disabling the haptics.
     * @param aEnabled Boolean value indicating whether the haptics is (ETrue)
     *                 to be enabled or not (EFalse).
     */
    void SetHapticsEnabled( TBool aEnabled );

    /**
     * Getter method for haptics enable/disable status
     * @return ETrue if haptics is enabled, EFalse otherwise
     */
    TBool HapticsEnabled() const;
     
    /**
     * Sets the sensor mode on/off
     * @param aSensorModeOn ETrue if sensor mode is activated.
     */
    void SetSensorModeL( TBool aSensorModeOn );  
     
    /**
     * returns true if sensor mode is On.
     * @return true if sensor mode is On
     */
    TBool SensorMode() const;
    
public:
    /**
     * From CoeControl.
     * @return count of controls inside this container.
     */
    TInt CountComponentControls() const;

    /**
     * From CCoeControl.
     * @param aIndex number of the control.
     * @return Pointer of controls inside this container.
     */
    CCoeControl* ComponentControl( TInt aIndex ) const;

    /**
     * From CCoeControl.
     * Draws to the screen.
     * @param aRect the rectangle of this view that needs updating
     */
    void Draw( const TRect& aRect ) const;

    /**
     * From CoeControl.
     * Called by framework when the view size is changed.
     */
    void SizeChanged();

    /**
     * From CCoeControl.
     * @param aPointerEvent The pointer event.
     */
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    /**
     * From CCoeControl.
     * @param aType The type of resources that have changed.
     */
    void HandleResourceChange( TInt aType );

    /**
     * From CCoeControl
     * @param aKeyEvent The key event (of TKeyEvent type).
     * @param aType  Type of the event (up/down).
     * @return TKeyResponse Whether the key was consumed or not by the control
     */
    TKeyResponse OfferKeyEventL( const TKeyEvent &aKeyEvent, TEventCode aType );

    
public: // from MSensrvDataListener

    /**
     * Implementation of MSensrvDataListener interface.
     * 
     * @param aChannel Reference to the related channel object
     * @param aCount Data object count in receiving buffer.
     * @param aDataLost Number of lost data items. Data loss can occur if
     */
    void DataReceived( CSensrvChannel& aChannel, TInt aCount, TInt aDataLost ); 

    /**
     * Implementation of MSensrvDataListener interface.
     * 
     * @param aChannel Reference to the related channel object
     * @param aError Error code.
     */
    void DataError( CSensrvChannel& aChannel, TSensrvErrorSeverity aError );
    
    /**
     * Implementation of MSensrvDataListener interface.
     * 
     * @param aInterfaceUid Identifier of the interface to be retrieved
     * @param aInterface A reference to a pointer that retrieves 
     *        the specified interface.
     */
    void GetDataListenerInterfaceL( TUid aInterfaceUid, TAny*& aInterface );                        

private:

    /**
     * C++ default constructor.
     */
    CHapticsExampleAppView();

    /**
     * 2nd phase constructor.
     * Perform the second phase construction of a
     * CHapticsExampleAppView object.
     * @param aRect The rectangle this view will be drawn to.
     */
    void ConstructL( const TRect& aRect );

    /**
     * Sets the ball object to default position
     */
    void SetObjects();

    /**
     * Moves the control snapping to imaginary grid lines
     * @param aControl Control which is moved.
     * @param aPoint Point where control is moved to.
     * @param aPlayFallEffect Whether or not to play falling effect during moving.
     */
    void Move( CCoeControl* aControl, TPoint aPoint, TBool aPlayFallEffect );

    /**
     * Calculates object's side length related to screen size
     */
    void ObjectDimension();
    
    /**
     * Loads effects from a file to buffer and to haptics system
     * @param aFileName Name of the file that contains effect definitions
     */
    void LoadEffectsFromFileL( const TDesC& aFileName );

    /**
     * Checks that effect is being played.
     * @return true if effect is being played. 
     * If an error occurred when checking state returns EFalse. 
     */
    TBool EffectActive( TInt aEffectHandle );
    
    /**
     * Static method that is called when timer tick occurs in touch mode
     */
    static TInt HandleTick( TAny* aPtr );

    /** 
     * Method for handling the tick within an object (i.e., non-static)
     * in touch mode
     */
    void DoHandleTick(); 

    /**
     * Static method that is called when timer tick occurs in sensor mode
     */
    static TInt HandleSensorTick( TAny* aPtr );

    /** 
     * Method for handling the tick within an object (i.e., non-static)
     * in sensor mode
     */
    void DoHandleSensorTick(); 

    /**
     * Updates falling velocity during free-falling
     */
    void UpdateFallVelocity(); 

    /**
     * Updates velocity in sensor mode
     */
    void UpdateSensorModeVelocity(); 

    /**
     * Checks whether the ball has bounced from a wall and updates its 
     * velocity accordingly. Also plays bounce effect if needed.
     */
    void CheckBounce();
     
    /**
     * Methods for checking whether the Y-axis movement has stopped
     */
    TBool FallStopped();

    /**
     * Updates the effect during falling. Depending on the falling velocity
     * the effect parameters are changed.
     */
    void UpdateFallEffect();

    /**
     * Plays bounce effect.
     * @param aBounceVelocity in percent (0..100)
     */
    void PlayBounceEffect( TInt aBounceVelocity ); 
    
    /** 
     * Shows an info note containing the given text
     */
    void UserNoteL( const TDesC& aText ) const; 

    /** 
     * Shows an error note containing the given text
     */
    void ErrorNoteL( const TDesC& aText ) const;
    
private:
    
    /**
     * Enumeration for the current state of the dragging
     */
    enum TDragState
        {
        ENotDragging,
        EDownEventReceived,
        EDragging
        };

    /**
     * CPeriodic "timer" providing the ticks.
     * Owned.
     */
    CPeriodic* iTicks;

    /**
     * Offset from the object's top left corner to the point of touch
     */
    TPoint  iDraggingOffset;

    /**
     * Area where dragging is possible
     */
    TRect  iDragArea;

    /**
     * Flag indicating ball is dragged
     */
    TDragState iDragState;

    /** 
     * Drag events since touch down.
     */
    TInt iDragStateWaitCount;

    /**
     * Flag indicating ball is falling
     */
    TBool iFallingBall;

    /**
     * Ball diameter in pixels
     */
    TInt  iBallDiameter;

    /**
     * Ball object
     * Owned.
     */
    CHapticsExampleBall* iBall;

    /**
     * Haptics client instance. 
     * Owned.
     */
    CHWRMHaptics* iHaptics;

    /** 
     * The Strength percentile
     */
    TInt iStrength;
    
    /**
     * Boolean member that indicates whether or not the haptics
     * is enabled.
     */ 
    TBool iHapticsEnabled;

    /**
     * Handle for the loaded effect file
     */
    TInt iEffectFileHandle;

    /**
     * Interval for refreshing the ball position during falling
     */
    TInt iTicksInterval;

    /**
     * Next ball position during falling
     */
    TPoint iNextPosition; 

    /**
     * Flag indicating that haptics effect for dragging is being played
     */
    TInt iDragEffectHandle; 
    
    /**
     * Haptics effect handle for haptics during falling
     */
    TInt iFallEffectHandle;
    
    /**
     * Current velocity.
     */
    TVelocity iBallVelocity;
    
    /**
     * Tick round counter
     */
    TInt iRound;

    /**
     * Channel for listening events from sensor framework.
     * Owned.
     */
    CSensrvChannel* iSensrvChannel; 

    /**
     * Last known scaled X-axis tilt. 
     * Only applicable in Sensor mode.
     */
    TInt iScaledTiltX;

    /**
     * Last known scaled Y-axis tilt. 
     * Only applicable in Sensor mode.
     */
    TInt iScaledTiltY;
    };

#endif // C_HAPTICSEXAMPLEAPPVIEW_H

// End of File
