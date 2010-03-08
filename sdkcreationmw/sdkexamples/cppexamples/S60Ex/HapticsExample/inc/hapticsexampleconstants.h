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
* Description:  Declares constants used in source files.
*
*/



#ifndef C_HAPTICSEXAMPLECONSTANTS_H
#define C_HAPTICSEXAMPLECONSTANTS_H


// --- ball size related constants ---
/* what part of minimal screen dimension ball will be at max */
const TInt KScreenFraction      = 5;
/* maximum ball size, in pixels */
const TInt KMaxBallSize = 100;

/* how many drag events should happen before dragging mode is activated */
const TInt KDragSensitivity = 3; 

// position refresh timer intervals 
const TInt KTicksInitialInterval = 50000; // 0,05s
const TInt KTicksSensorModeInterval = 50000; // 0,05s
// ball behavior parameters
const TInt KDownAcceleration = 5;
const TInt KSidewardDeceleration = 1;
const TInt KTopBounceDecrease = 2;
const TInt KBottomBounceDecrease = 2;
const TInt KHitBounce = -30;
const TInt KRotationStep = 15;
const TInt KFallInitialMagnitude = KHWRMHapticsMaxMagnitude/10; // 10% of max
const TInt KFallInitialPeriod = 150;

/* keyboard controlled pointer movements step in pixels */ 
const TInt KKeyStep = 10;

// sensor API related constants;
// See TSensrvRotationData definition and documentation for more information
const TInt KXAxisZeroPoint = 180;
const TInt KYAxisZeroPoint = 90;
const TInt KMaxTiltToRight = 6;
const TInt KMaxTiltToLeft = -6;

// IVT file definitions
_LIT( KIVTFileName, "hapticsexample.ivt" );
// effect names in KIVTFileName
_LIT8( KEffectBounce1Name, "Bounce1" );
_LIT8( KEffectBounce2Name, "Bounce2" );
_LIT8( KEffectBounce3Name, "Bounce3" );
_LIT8( KEffectBounce4Name, "Bounce4" );
_LIT8( KEffectBounce5Name, "Bounce5" );
_LIT8( KEffectBounce6Name, "Bounce6" );

const TInt KBounceLevel2Magnitude =  2;
const TInt KBounceLevel3Magnitude =  4;
const TInt KBounceLevel4Magnitude =  6;
const TInt KBounceLevel5Magnitude =  8;
const TInt KBounceLevel6Magnitude = 10;

const TInt KBounceLevel2Percent = 60;
const TInt KBounceLevel3Percent = 50;
const TInt KBounceLevel4Percent = 40;
const TInt KBounceLevel5Percent = 20;
const TInt KBounceLevel6Percent = 10;


/**
 * Maximum length of buffer used for user messages
 */
const TInt KMaxNoteLength = 100;

/**
 * 0% constant
 */
const TInt K0Percent = 0;

/**
 * 20% constant
 */
const TInt K20Percent = 20;

/**
 * 25% constant
 */
const TInt K25Percent = 25;

/**
 * 30% constant
 */
const TInt K30Percent = 30;

/**
 * 60% constant
 */
const TInt K60Percent = 60;

/**
 * 100% constant
 */
const TInt K100Percent = 100;

/*
 * Minimal Haptics Strength in percent
 */
const TInt KMinStrength =   K0Percent;

/*
 * Maximum Haptics Strength in percent
 */
const TInt KMaxStrength(K100Percent);


#endif // C_HAPTICSEXAMPLECONSTANTS_H
