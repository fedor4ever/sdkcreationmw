/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  List box model for Geo Profiles Application UI.
*
*/


// SYSTEM INCLUDES 

// USER INCLUDES
#include "geoprofileslbmodel.h"
#include "geoprofile.h"

// CONSTANT DEFINTION
const TInt KGeoProfilesBufferSize = 256;
    
// ---------------------------------------------------------------------------
// CGeoProfilesLBModel::CGeoProfilesLBModel
// ---------------------------------------------------------------------------
//
CGeoProfilesLBModel::CGeoProfilesLBModel( 
                    RPointerArray<CGeoProfile>*	 aGeoProfiles )
    :iGeoProfiles( aGeoProfiles )
    {    
    }

// ---------------------------------------------------------------------------
// CGeoProfilesLBModel::~CGeoProfilesLBModel
// ---------------------------------------------------------------------------
//    
CGeoProfilesLBModel::~CGeoProfilesLBModel()
    {
    // Delete the Conversion buffer
    delete iBuffer;   
    }

// ---------------------------------------------------------------------------
// CGeoProfilesLBModel::NewL
// ---------------------------------------------------------------------------
// 
CGeoProfilesLBModel* CGeoProfilesLBModel::NewL( 
                         RPointerArray<CGeoProfile>*	 aGeoProfiles )
    {
    CGeoProfilesLBModel* self = new ( ELeave ) CGeoProfilesLBModel( aGeoProfiles );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// void CGeoProfilesLBModel::ConstructL
// ---------------------------------------------------------------------------
//
void CGeoProfilesLBModel::ConstructL()
    {
    // Allocate the Conversion buffer
    iBuffer = HBufC::NewL( KGeoProfilesBufferSize );
    }

// ---------------------------------------------------------------------------
// void CGeoProfilesLBModel::UpdateModelL
// ---------------------------------------------------------------------------
//
void CGeoProfilesLBModel::UpdateModel( 
                        RPointerArray<CGeoProfile>*	 aGeoProfiles )
    {
    iGeoProfiles = aGeoProfiles;
    }

// ---------------------------------------------------------------------------
// TInt CGeoProfilesLBModel::MdcaCount
// ---------------------------------------------------------------------------
//
TInt CGeoProfilesLBModel::MdcaCount() const
    {
    return iGeoProfiles->Count();   	
    }
    
// ---------------------------------------------------------------------------
// TPtrC16 CGeoProfilesLBModel::MdcaPoint
// ---------------------------------------------------------------------------
//
TPtrC16 CGeoProfilesLBModel::MdcaPoint(  TInt aIndex  ) const
    {
    // Create formatted item string.
    // list item string format: // Format "\t%S\t\t%S"
	TPtr16	buf( iBuffer->Des());
	buf.Zero();
	    
    if ( iGeoProfiles->Count() < aIndex )
        {
        return buf;
        }
       
    // Pack the contents into the Buffer. We dont have to re-allocate the buffer
    // everytime. Currently the string length is restricted to 256. 
    // 
    // !!! Could be a potential problem !!!!!
		    
    // Tabulator Defintion
    _LIT( KTab, "\t" );
    
	// Append the Tab Key
	buf.Append( KTab );    
	
	// Append the Geo Profile Name
	buf.Append( (*iGeoProfiles)[ aIndex ]->GeoPlaceName());
	
	// Append the Tab Key
	buf.Append( KTab );
	
	// Append the Tab Key
	buf.Append( KTab );
		
	// Append the Geo Profile Name
	buf.Append( (*iGeoProfiles)[ aIndex ]->ProfileName());

    return buf;
    
    }
