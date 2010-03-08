/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __SENSOREXAMPLEAPPVIEW_H__
#define __SENSOREXAMPLEAPPVIEW_H__

// INCLUDES
#include <coecntrl.h>
#include <sensrvaccelerometersensor.h>
#include <sensrvorientationsensor.h>
#include <sensrvchannelinfo.h>


// CLASS DECLARATION
class CSensorExampleAppView : public CCoeControl
    {
public:

   /**
    * NewL.
    * Two-phased constructor.
    * Create a CSensorExampleAppView object, which will draw itself to aRect.
    * @param aRect The rectangle this view will be drawn to.
    * @return a pointer to the created instance of CSensorExampleAppView.
    */
    static CSensorExampleAppView* NewL( const TRect& aRect );

   /**
    * NewLC.
    * Two-phased constructor.
    * Create a CSensorExampleAppView object, which will draw itself
    * to aRect.
    * @param aRect Rectangle this view will be drawn to.
    * @return A pointer to the created instance of CSensorExampleAppView.
    */
    static CSensorExampleAppView* NewLC( const TRect& aRect );

   /**
    * ~CSensorExampleAppView
    * Virtual Destructor.
    */
    virtual ~CSensorExampleAppView();

public:  // Functions from base classes

   /**
    * From CCoeControl, Draw
    * Draw this CSensorExampleAppView to the screen.
    * If the user has given a text, it is also printed to the center of
    * the screen.
    * @param aRect the rectangle of this view that needs updating
    */
    void Draw( const TRect& aRect ) const;

   /**
    * From CoeControl, SizeChanged.
    * Called by framework when the view size is changed.
    */
    virtual void SizeChanged();

public:

   /**
    * Called by CSensorExampleDataListener when channel data is received.
    */
    void TappingEvent( TInt aDirection );
    void AccelerometerData( TInt aX, TInt aY );
    void OrientationEvent( TSensrvOrientationData::TSensrvDeviceOrientation aDeviceOrientation );

   /**
    * Show channel query dialog.
    *
    * @param aChannelList List of channels which are shown in query dialog.
    * @return index of selected channel from query dialog.
    */
    TInt ShowQueryDialogL( RSensrvChannelInfoList& aChannelList );

   /**
    * Show property query dialog
    *
    * @param aChannelPropertyList List of properties which are shown in query dialog.
    * @return index of selected property from query dialog.
    */
    TInt ShowQueryDialogL( RSensrvPropertyList& aChannelPropertyList );

   /**
    * Return name of a property
    *
    * @param aPropertyId  Property ID
    * @param aItemIndex   Item index of property
    * @param aPropertyBuf On return contains property's name
    */
    void CSensorExampleAppView::GetPropertyStringL( TSensrvPropertyId aPropertyId,
                                                    TInt aItemIndex,
                                                    TDes& aPropertyBuf );


private: // Constructors

   /**
    * ConstructL
    * 2nd phase constructor.
    * Perform the second phase construction of a
    * CSensorExampleAppView object.
    * @param aRect The rectangle this view will be drawn to.
    */
    void ConstructL( const TRect& aRect );

   /**
    * CSensorExampleAppView.
    * C++ default constructor.
    */
    CSensorExampleAppView();

private:

   /**
    * font used, when printing user-given text to the screen
    */
    const CFont* iFont;//Not own

   /**
    * Latest sensor data
    */
    TSensrvOrientationData::TSensrvDeviceOrientation iDeviceOrientation;
    TInt iX;
    TInt iY;

    TRgb iFaceColor;
    };

#endif // __SENSOREXAMPLEAPPVIEW_H__

// End of File
