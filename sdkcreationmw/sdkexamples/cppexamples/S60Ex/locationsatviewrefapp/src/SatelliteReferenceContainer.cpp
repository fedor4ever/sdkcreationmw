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
#include "SatelliteReferenceContainer.h"

#include <eiklabel.h>  // for example label control

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSatelliteReferenceContainer::ConstructL(const TRect& aRect)
// Second phase constructor
// ---------------------------------------------------------
//

void CSatelliteReferenceContainer::ConstructL(const TRect& aRect, CAknAppUi* aAppUi)
    {
    CreateWindowL();           
    SetRect(aRect);
    ActivateL();
    iAppUi = aAppUi;
    }

// ------------------------------------------------------------
// CSatelliteReferenceContainer::~CSatelliteReferenceContainer()
// Destructor
// ------------------------------------------------------------
//
CSatelliteReferenceContainer::~CSatelliteReferenceContainer()
    {    
    }

// ---------------------------------------------------------
// CSatelliteReferenceContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CSatelliteReferenceContainer::SizeChanged()
    {
    }
    
// ---------------------------------------------------------
// CSatelliteReferenceContainer::HandleResourceChange()
// ---------------------------------------------------------
//
void CSatelliteReferenceContainer::HandleResourceChange(TInt aType)
	{
	if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        SetRect(iAppUi->ClientRect());
        }        	
	}

// ---------------------------------------------------------
// CSatelliteReferenceContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CSatelliteReferenceContainer::CountComponentControls() const
    {
    return 0; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CSatelliteReferenceContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CSatelliteReferenceContainer::ComponentControl(TInt /*aIndex*/) const
    {
    return NULL;
    }

// ---------------------------------------------------------
// CSatelliteReferenceContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CSatelliteReferenceContainer::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle(CGraphicsContext::ENullPen);
    gc.SetBrushColor(KRgbGray);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.DrawRect(aRect);
    }

// ---------------------------------------------------------
// CSatelliteReferenceContainer::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CSatelliteReferenceContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    }

// End of File  
