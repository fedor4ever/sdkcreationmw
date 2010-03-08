/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
#include <coemain.h>
#include <aknutils.h>
#include <e32base.h>
#include <sensrvaccelerometersensor.h>
#include <sensrvtappingsensor.h>
#include <aknlistquerydialog.h>
#include <sensorexample.rsg>
#include <sensrvgeneralproperties.h>
#include "sensorexampleappview.h"

_LIT( KHexPrefix, "0x" );
const TInt KItemBufferSize( 25 );
const TInt KBallDiameter( 60 );
const TInt KEyeDiameter( 10 );
const TInt KMouthWidth( 10 );
const TSize KEyeSize( KEyeDiameter, KEyeDiameter );
const TSize KBallSize( KBallDiameter, KBallDiameter );
const TSize KMouthSizeHorizontal( ( KMouthWidth * 2 ), KMouthWidth );
const TSize KMouthSizeVertical( KMouthWidth, ( KMouthWidth * 2 ) );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSensorExampleAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSensorExampleAppView* CSensorExampleAppView::NewL( const TRect& aRect )
    {
    CSensorExampleAppView* self = CSensorExampleAppView::NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSensorExampleAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSensorExampleAppView* CSensorExampleAppView::NewLC( const TRect& aRect )
    {
    CSensorExampleAppView* self = new ( ELeave ) CSensorExampleAppView;
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// -----------------------------------------------------------------------------
// CSensorExampleAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSensorExampleAppView::ConstructL( const TRect& aRect )
    {
    // Create a window for this application view
    CreateWindowL();

    // set the font
    iFont = AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont);

    // Set the windows size
    SetRect( aRect );

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CSensorExampleAppView::CSensorExampleAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CSensorExampleAppView::CSensorExampleAppView()
    : iDeviceOrientation( iDeviceOrientation = TSensrvOrientationData::EOrientationDisplayUp ),
      iX( 0 ),
      iY( 0 ),
      iFaceColor( KRgbRed )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CSensorExampleAppView::~CSensorExampleAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CSensorExampleAppView::~CSensorExampleAppView()
    {
    // No implementation required
    }


// -----------------------------------------------------------------------------
// CSensorExampleAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CSensorExampleAppView::Draw( const TRect& /*aRect*/ ) const
    {
    // note that the whole screen is drawn everytime, so aRect-parameter is ignored
    
    CWindowGc& gc = SystemGc();// Get the standard graphics context
    TRect rect( Rect());// Gets the control's extent
    TSize screenSize( rect.Size());
    TPoint middlePoint( screenSize.iWidth / 2, screenSize.iHeight / 2);
    TPoint ballTopLeftCorner( ( ( middlePoint.iX + iX ) - ( KBallSize.iWidth / 2 ) ),
                              ( ( middlePoint.iY  + iY )- ( KBallSize.iHeight / 2 ) ) );
    TRect ballRect( ballTopLeftCorner, KBallSize );
    
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KRgbWhite );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.Clear( rect );//Clear screen
    gc.SetBrushColor( iFaceColor );
    gc.DrawEllipse( ballRect );//Draw circle for smiley
    gc.SetBrushColor( KRgbWhite );
    
    //Draw rest of smiley i.e. eyes, nose and mouth    
    TSize mouthSize;
    TRect smileyRect;
    TPoint eye1;
    TPoint eye2;
    TPoint nose;
    TPoint mouth;
    TPoint eyeTopLeftCorner;
    TPoint eyeTopRightCorner;
    TPoint eyeBottomLeftCorner;
    TPoint eyeBottomRightCorner;
    TPoint mouthTop;
    TPoint mouthBottom;
    TPoint mouthLeft;
    TPoint mouthRight;

    eyeTopLeftCorner.iX = ( ballTopLeftCorner.iX + KBallSize.iWidth / 3 ) - ( KEyeDiameter / 2 );
    eyeTopLeftCorner.iY = ( ballTopLeftCorner.iY + KBallSize.iHeight / 3 ) - ( KEyeDiameter / 2 );
    eyeTopRightCorner.iX = ( ballTopLeftCorner.iX + KBallSize.iWidth / 3 * 2 ) - ( KEyeDiameter / 2 );
    eyeTopRightCorner.iY = ( ballTopLeftCorner.iY + KBallSize.iHeight / 3 ) - ( KEyeDiameter / 2 );
    eyeBottomLeftCorner.iX = ( ballTopLeftCorner.iX + KBallSize.iWidth / 3 ) - ( KEyeDiameter / 2 );
    eyeBottomLeftCorner.iY = ( ballTopLeftCorner.iY + KBallSize.iHeight / 3 * 2 ) - ( KEyeDiameter / 2 );
    eyeBottomRightCorner.iX = ( ballTopLeftCorner.iX + KBallSize.iWidth / 3 * 2 ) - ( KEyeDiameter / 2 );
    eyeBottomRightCorner.iY = ( ballTopLeftCorner.iY + KBallSize.iHeight / 3 * 2 ) - ( KEyeDiameter / 2 );

    mouthTop.iX = ( ballTopLeftCorner.iX + KBallSize.iWidth / 2 ) - ( KMouthWidth / 2 );
    mouthTop.iY = ( ballTopLeftCorner.iY + KMouthWidth / 2 );
    mouthBottom.iX = ( ballTopLeftCorner.iX + KBallSize.iWidth / 2 ) - ( KMouthWidth / 2 );
    mouthBottom.iY = ( ballTopLeftCorner.iY + KBallSize.iHeight / 2 ) + ( KMouthWidth / 2 );
    mouthLeft.iX = ( ballTopLeftCorner.iX + KBallSize.iWidth / 2 ) + ( KMouthWidth / 2 );
    mouthLeft.iY = ( ballTopLeftCorner.iY + KBallSize.iHeight / 2 ) - ( KMouthWidth / 2 );
    mouthRight.iX = ( ballTopLeftCorner.iX + KMouthWidth / 2 );
    mouthRight.iY = ( ballTopLeftCorner.iY + KBallSize.iHeight / 2 ) - ( KMouthWidth / 2 );

    nose.iX = ( ballTopLeftCorner.iX + KBallSize.iWidth / 2 ) - ( KMouthWidth / 2 );
    nose.iY = ( ballTopLeftCorner.iY + KBallSize.iHeight / 2 ) - ( KMouthWidth / 2 );

    switch( iDeviceOrientation )
        {
        case TSensrvOrientationData::EOrientationDisplayDown:
            {
            eye1 = eyeBottomLeftCorner;
            eye2 = eyeBottomRightCorner;
            mouth = mouthTop;
            mouthSize = KMouthSizeVertical;
            }
            break;

        case TSensrvOrientationData::EOrientationDisplayLeftUp:
            {
            eye1 = eyeBottomLeftCorner;
            eye2 = eyeTopLeftCorner;
            mouth = mouthLeft;
            mouthSize = KMouthSizeHorizontal;
            }
            break;

        case TSensrvOrientationData::EOrientationDisplayRightUp:
            {
            eye1 = eyeTopRightCorner;
            eye2 = eyeBottomRightCorner;
            mouth = mouthRight;
            mouthSize = KMouthSizeHorizontal;
            }
            break;

        default:
            {
            eye1 = eyeTopLeftCorner;
            eye2 = eyeTopRightCorner;
            mouth = mouthBottom;
            mouthSize = KMouthSizeVertical;
            }
            break;
        }
    
    smileyRect.SetRect( eye1, KEyeSize );
    gc.DrawEllipse( smileyRect );
    smileyRect.SetRect( eye2, KEyeSize );
    gc.DrawEllipse( smileyRect );
    smileyRect.SetRect( nose, KEyeSize );
    gc.DrawEllipse( smileyRect );
    smileyRect.SetRect( mouth, mouthSize );
    gc.DrawEllipse( smileyRect );
    }

// -----------------------------------------------------------------------------
// CSensorExampleAppView::TappingEvent()
// Called by CSensorExampleDataListener when tapping event occurs
// -----------------------------------------------------------------------------
//
void CSensorExampleAppView::TappingEvent( TInt /*aDirection*/ )
    {
    //Alter drawing color for smiley
    if( KRgbRed == iFaceColor )
        {
        iFaceColor = KRgbBlue;
        }
    else
        {
        iFaceColor = KRgbRed;
        }

    DrawNow();
    }

// -----------------------------------------------------------------------------
// CSensorExampleAppView::AccelerometerData
// Called by CSensorExampleDataListener when accelerometer data is available
// -----------------------------------------------------------------------------
//
void CSensorExampleAppView::AccelerometerData( TInt aX, TInt aY )
    {
    iX = aX;
    iY = aY;
    DrawNow();
    }

// -----------------------------------------------------------------------------
// CSensorExampleAppView::OrientationEvent
// Called by CSensorExampleDataListener when orientation event occurs
// -----------------------------------------------------------------------------
//
void CSensorExampleAppView::OrientationEvent( 
    TSensrvOrientationData::TSensrvDeviceOrientation aDeviceOrientation )
    {
    if( TSensrvOrientationData::EOrientationUndefined != aDeviceOrientation )
        {
        iDeviceOrientation = aDeviceOrientation;
        DrawNow();
        }
    }

// -----------------------------------------------------------------------------
// CSensorExampleAppView::ShowQueryDialogL
// -----------------------------------------------------------------------------
//
TInt CSensorExampleAppView::ShowQueryDialogL( RSensrvChannelInfoList& aChannelList )
    {
    TInt selectedIndex( 0 );
    CAknListQueryDialog* dialog = new (ELeave) CAknListQueryDialog ( &selectedIndex );
    dialog->PrepareLC( R_SENSOREXAMPLE_DIALOG_CHANNELTYPEID_QUERY );
    CDesCArrayFlat* itemList = new ( ELeave ) CDesCArrayFlat( aChannelList.Count() );
    CleanupStack::PushL( itemList );

    for( TInt i=0; i<aChannelList.Count(); i++ )
        {
        switch( aChannelList[i].iChannelType )
            {
            case KSensrvChannelTypeIdAccelerometerDoubleTappingData:
                {
                itemList->AppendL( iCoeEnv->AllocReadResourceLC( R_SE_DOUBLETAPCHANNEL )->Des() );
                CleanupStack::PopAndDestroy();//R_SE_DOUBLETAPCHANNEL
                }
                break;

            case KSensrvChannelTypeIdAccelerometerXYZAxisData:
                {
                itemList->AppendL( iCoeEnv->AllocReadResourceLC( R_SE_ACCELEROMETERCHANNEL )->Des() );
                CleanupStack::PopAndDestroy();//R_SE_ACCELEROMETERCHANNEL
                }
                break;

            case  KSensrvChannelTypeIdOrientationData:
                {
                itemList->AppendL( iCoeEnv->AllocReadResourceLC( R_SE_ORIENTATIONCHANNEL )->Des() );
                CleanupStack::PopAndDestroy();//R_SE_ORIENTATIONCHANNEL
                }
                break;

            case  KSensrvChannelTypeIdRotationData:
                {
                itemList->AppendL( iCoeEnv->AllocReadResourceLC( R_SE_ROTATIONCHANNEL )->Des() );
                CleanupStack::PopAndDestroy();//R_SE_ROTATIONCHANNEL
                }
                break;

            default:
                {
                //Only number is printed for the rest of the channels.
                TBuf<KItemBufferSize> item;
                item.Append( KHexPrefix );
                item.AppendNum( aChannelList[i].iChannelType, EHex );
                itemList->AppendL( item );
                }
                break;
            }
        }

    dialog->SetOwnershipType( ELbmOwnsItemArray );
    dialog->SetItemTextArray( itemList );

    CleanupStack::Pop( itemList );

    TInt result = dialog->RunLD();

    if( result == -2 )
        {
        // Ok was pressed
        }
    else
        {
        // Cancel was pressed
        selectedIndex = KErrCancel;
        }

    return selectedIndex;
    }

// -----------------------------------------------------------------------------
// CSensorExampleAppView::ShowQueryDialogL
// -----------------------------------------------------------------------------
//
TInt CSensorExampleAppView::ShowQueryDialogL( RSensrvPropertyList& aChannelPropertyList )
    {
    TInt selectedIndex( 0 );
    CAknListQueryDialog* dialog = new (ELeave) CAknListQueryDialog ( &selectedIndex );
    dialog->PrepareLC( R_SENSOREXAMPLE_DIALOG_CHANNELPROPERTY_QUERY );
    CDesCArrayFlat* itemList = new ( ELeave ) CDesCArrayFlat( aChannelPropertyList.Count() );
    CleanupStack::PushL( itemList );

    for( TInt i=0; i<aChannelPropertyList.Count(); i++ )
        {
        TBuf<KItemBufferSize> property;

        GetPropertyStringL( aChannelPropertyList[i].GetPropertyId(),
                            aChannelPropertyList[i].PropertyItemIndex(),
                            property );

        if( ESensrvArrayPropertyInfo == aChannelPropertyList[i].GetArrayIndex() )
            {
            //Append suffix for array properties
            property.Append( iCoeEnv->AllocReadResourceLC( R_SE_ARRAYSUFFIX )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_ARRAYSUFFIX
            }
        else if( aChannelPropertyList[i].GetArrayIndex() >= 0 )
            {
            //Append array index
            TBuf<KItemBufferSize> buff;
            _LIT( KArrayIndex, " (%d)");
            buff.Format( KArrayIndex, aChannelPropertyList[i].GetArrayIndex() );
            property.Append( buff );
            }

        itemList->AppendL( property );
        }

    dialog->SetOwnershipType( ELbmOwnsItemArray );
    dialog->SetItemTextArray( itemList );

    CleanupStack::Pop( itemList );

    TInt result = dialog->RunLD();

    if( result == -2 )
        {
        // Ok was pressed
        }
    else
        {
        // Cancel was pressed
        selectedIndex = KErrCancel;
        }

    return selectedIndex;
    }

// -----------------------------------------------------------------------------
// CSensorExampleAppView::GetPropertyStringL
// -----------------------------------------------------------------------------
//
void CSensorExampleAppView::GetPropertyStringL( TSensrvPropertyId aPropertyId,
                                                TInt aItemIndex,
                                                TDes& aPropertyBuf )
    {
    switch( aPropertyId )
        {
        case KSensrvPropIdDataRate://0x00000002
            {
            aPropertyBuf.Append( iCoeEnv->AllocReadResourceLC( R_SE_DATARATE )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_DATARATE
            }
            break;

        case KSensrvPropIdAvailability://0x00000004
            {
            aPropertyBuf.Append( iCoeEnv->AllocReadResourceLC( R_SE_AVAILABILITY )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_AVAILABILITY
            }
            break;

        case KSensrvPropIdMeasureRange:// 0x00000005
            {
            aPropertyBuf.Append( iCoeEnv->AllocReadResourceLC( R_SE_MEASURERANGE )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_MEASURERANGE
            }
            break;

        case KSensrvPropIdChannelDataFormat://0x000000006
            {
            aPropertyBuf.Append( iCoeEnv->AllocReadResourceLC( R_SE_DATAFORMAT )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_DATAFORMAT
            }
            break;

        case KSensrvPropIdScaledRange://0x000000007
            {
            aPropertyBuf.Append( iCoeEnv->AllocReadResourceLC( R_SE_SCALEDRANGE )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_SCALEDRANGE
            }
            break;

        case KSensrvPropIdChannelAccuracy://0x000000008
            {
            aPropertyBuf.Append( iCoeEnv->AllocReadResourceLC( R_SE_ACCURACY )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_ACCURACY
            }
            break;

        case KSensrvPropIdChannelScale://0x000000009;
            {
            aPropertyBuf.Append( iCoeEnv->AllocReadResourceLC( R_SE_SCALE )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_SCALE
            }
            break;

        case KSensrvPropIdChannelUnit://0x0000000010
            {
            aPropertyBuf.Append( iCoeEnv->AllocReadResourceLC( R_SE_UNIT )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_UNIT
            }
            break;

        case KSensrvSensorModel://0x0000000011
            {
            aPropertyBuf.Append( iCoeEnv->AllocReadResourceLC( R_SE_MODEL )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_MODEL
            }
            break;

        case KSensrvSensorConnectionType://0x0000000012
            {
            aPropertyBuf.Append( iCoeEnv->AllocReadResourceLC( R_SE_CONNECTIONTYPE )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_CONNECTIONTYPE
            }
            break;

        case KSensrvSensorDescription://0x0000000013
            {
            aPropertyBuf.Append( iCoeEnv->AllocReadResourceLC( R_SE_DESCRIPTION )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_DESCRIPTION
            }
            break;

        case KSensrvPropIdAxisActive://0x00010001
            {
            if( TSensrvAccelerometerAxisData::EAxisX == aItemIndex )
                {
                aPropertyBuf.Append( iCoeEnv->AllocReadResourceLC( R_SE_XAXISACTIVE )->Des() );
                CleanupStack::PopAndDestroy();//R_SE_XAXISACTIVE
                }
            else if( TSensrvAccelerometerAxisData::EAxisY == aItemIndex )
                {
                aPropertyBuf.Append( iCoeEnv->AllocReadResourceLC( R_SE_YAXISACTIVE )->Des() );
                CleanupStack::PopAndDestroy();//R_SE_YAXISACTIVE
                }
            else if( TSensrvAccelerometerAxisData::EAxisZ == aItemIndex )
                {
                aPropertyBuf.Append( iCoeEnv->AllocReadResourceLC( R_SE_ZAXISACTIVE )->Des() );
                CleanupStack::PopAndDestroy();//R_SE_ZAXISACTIVE
                }
            }
            break;

        case KSensrvPropIdDblTapLatency://0x00010004
            {
            aPropertyBuf.Append( iCoeEnv->AllocReadResourceLC( R_SE_DOUBLETAPLATENCY )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_DOUBLETAPLATENCY
            }
            break;

        case KSensrvPropIdDblTapInterval://0x00010005
            {
            aPropertyBuf.Append( iCoeEnv->AllocReadResourceLC( R_SE_DOUBLETAPINTERVAL )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_DOUBLETAPINTERVAL
            }
            break;

        default:
            {
            //Only number is printed for the rest of the properties.
            aPropertyBuf.Append( KHexPrefix );
            aPropertyBuf.AppendNumUC( aPropertyId, EHex );
            }
            break;
        }

    }

// -----------------------------------------------------------------------------
// CSensorExampleAppView::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CSensorExampleAppView::SizeChanged()
    {
    DrawNow();
    }

// End of File
