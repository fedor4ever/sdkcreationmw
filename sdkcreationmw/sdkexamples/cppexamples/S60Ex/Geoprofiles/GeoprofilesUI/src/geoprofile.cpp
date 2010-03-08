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
* Description:  Geo Profile Structure
*
*/


// SYSTEM INCLUDES
#include <ProEngFactory.h>
#include <mproengengine.h>
#include <mproengprofilenamearray.h>
#include <e32math.h> 
#include <geoprofilesui.rsg>
#include <stringloader.h>

// USER INCLUDES
#include "geoprofile.h"
#include "geoprofilesdebug.h"
#include "geoprofiles.hrh"

// CONSTANT DEFINTION
const TInt KMinGeoProfileRadius = 100;
_LIT( KDeliminator, ":");

// ============== MEMBER FUNCTIONS FOR GEOPROFILE ============================

// ---------------------------------------------------------------------------
// CGeoProfile::CGeoProfile
// ---------------------------------------------------------------------------
//
CGeoProfile::CGeoProfile()
    :iPositionType( EUndefined ),
    iProfileRadius( KMinGeoProfileRadius ),
    iTriggerId( 0xFFFFFFF ),
    iTriggerType( EEntryTrigger )    
    {    
    }
    
// ---------------------------------------------------------------------------
// CGeoProfile::~CGeoProfile
// ---------------------------------------------------------------------------
// 
CGeoProfile::~CGeoProfile()
    {
    GEOPROFILEDEBUG("+ CGeoProfile::~CGeoProfile" )
    
    // Delete the member variables
    delete iGeoPlaceName;
    
    delete iProfileName;  
    
    GEOPROFILEDEBUG("- CGeoProfile::~CGeoProfile" )
    }   

// ---------------------------------------------------------------------------
// CGeoProfile::NewL
// ---------------------------------------------------------------------------
// 
CGeoProfile* CGeoProfile::NewL()
    {
    TLocality locality;    
    return NewL( KNullDesC(), locality, KErrNotFound, EUndefined );                                                                  
    }
    
// ---------------------------------------------------------------------------
// CGeoProfile::NewL
// ---------------------------------------------------------------------------
// 
CGeoProfile* CGeoProfile::NewL( const TDesC&       aGeoPlaceName,
                                TLocality&         aPositionInfo,
                                TInt               aGeoProfileId,
                                TInt               aProfileRadius,
                                TInt               aTriggerId,
                                TInt               aTriggerType )
    {
    GEOPROFILEDEBUG("+ CGeoProfile::NewL" ) 
    
    CGeoProfile* self = new ( ELeave ) CGeoProfile();
    CleanupStack::PushL( self );
    self->ConstructL( aGeoPlaceName, 
                      aGeoProfileId, 
                      aPositionInfo, 
                      aProfileRadius, 
                      aTriggerId, 
                      aTriggerType );
    CleanupStack::Pop( self );
    
    GEOPROFILEDEBUG("- CGeoProfile::NewL" ) 
    return self;                                                                  
    }
    
// ---------------------------------------------------------------------------
// CGeoProfile::ConstructL
// ---------------------------------------------------------------------------
//
void CGeoProfile::ConstructL(  const TDesC&       aGeoPlaceName,
                                     TInt         aGeoProfileId,
                                     TLocality&   aPositionInfo,
                                     TInt         aProfileRadius,
                                     TInt         aTriggerId,
                                     TInt         aTriggerType )
    {
    GEOPROFILEDEBUG("+ CGeoProfile::ConstructL" ) 
    
    // Create all member variables
    iGeoPlaceName = HBufC::NewL( KGeoPlaceNameMaxLength );    
    
    iProfileName = HBufC::NewL( KGeoProfileNameMaxLength );
    iProfileName->Des().Zero();
    
    SetGeoPlaceNameL( KNullDesC());
        
    // Copy the Item Id
    SetGeoProfileIdL( aGeoProfileId );
    
    // Copy the Location information contents
    SetGeoPositionInfoL( aGeoPlaceName, aPositionInfo );
    
    // Set the profile Radius
    SetGeoProfileRadius( aProfileRadius );
    
    // Set the Trigger id
    iTriggerId = aTriggerId; 
    
    // Set Trigger Type
    SetTriggerType( aTriggerType );
    
    GEOPROFILEDEBUG("- CGeoProfile::ConstructL" )    
    }
    
// ---------------------------------------------------------------------------
// CGeoProfile::GeoPlaceName
// ---------------------------------------------------------------------------
// 
TPtrC CGeoProfile::GeoPlaceName()
    {
    return iGeoPlaceName->Des();
    }

// ---------------------------------------------------------------------------
// CGeoProfile::ProfileName
// ---------------------------------------------------------------------------
//     
TPtrC CGeoProfile::ProfileName()
    {
    return iProfileName->Des();
    }

// ---------------------------------------------------------------------------
// void CGeoProfile::SetProfileNameL
// ---------------------------------------------------------------------------
//     
void CGeoProfile::SetProfileNameL( const TDesC&       aProfileName )
    {
    if ( aProfileName.Length() > KGeoProfileNameMaxLength )
        {
        User::Leave( KErrOverflow );
        }
        
    // Copy the corresponding profile name
    iProfileName->Des().Copy( aProfileName );    
    }
    
// ---------------------------------------------------------------------------
// TInt CGeoProfile::ItemId
// ---------------------------------------------------------------------------
// 
TInt CGeoProfile::GeoProfileId()
    {
    return iGeoProfileId;
    }

// ---------------------------------------------------------------------------
//void CGeoProfile::SetGeoProfileId
// ---------------------------------------------------------------------------
//    
void CGeoProfile::SetGeoProfileIdL( TInt               aGeoProfileId )
    {
    GEOPROFILEDEBUG("+ CGeoProfile::SetGeoProfileIdL" ) 
    
    // Obtain the list of profiles
    MProEngEngine* profileEng = ProEngFactory::NewEngineL();
    CleanupReleasePushL( *profileEng );

    MProEngProfileNameArray* profileNames = profileEng->ProfileNameArrayLC();
    TInt index = 0;
    index = profileNames->FindById( aGeoProfileId );
    if ( KErrNotFound == index )
        {
        // Setting the Default Value of index back to 0
        iGeoProfileId = profileEng->ActiveProfileId();
        index = profileNames->FindById( iGeoProfileId );
        }
    else
        {
        iGeoProfileId = profileNames->ProfileId( index );      
        }      
    SetProfileNameL( profileNames->MdcaPoint( index ));
    CleanupStack::PopAndDestroy( 2 );
    
    GEOPROFILEDEBUG("- CGeoProfile::SetGeoProfileIdL" ) 
    }

// ---------------------------------------------------------------------------
// TInt CGeoProfile::GeoPositionType
// ---------------------------------------------------------------------------
//  
TInt CGeoProfile::GeoPositionType() 
    {
    return iPositionType;
    }

// ---------------------------------------------------------------------------
// TInt CGeoProfile::GeoPositionType
// ---------------------------------------------------------------------------
//  
void CGeoProfile::SetGeoPositionType(  TInt         aPositionType ) 
    {
    iPositionType = aPositionType;
    }
    
// ---------------------------------------------------------------------------
// TLocality& CGeoProfile::GeoPositionInfo
// ---------------------------------------------------------------------------
//    
TLocality& CGeoProfile::GeoPositionInfo()
    {
    return iPositionInfo;
    }

// ---------------------------------------------------------------------------
// void CGeoProfile::SetGeoPositionInfo
// ---------------------------------------------------------------------------
//
void CGeoProfile::SetGeoPositionInfoL( const TDesC&      aGeoPlaceName,
                                             TLocality&  aPositionInfo )
    {    
    if ( !Math::IsNaN( aPositionInfo.Latitude()) && 
         !Math::IsNaN( aPositionInfo.Longitude()))
        {
        SetGeoPositionType( ECoordinate );
        iPositionInfo = aPositionInfo;
        SetGeoPlaceNameL( aGeoPlaceName );      
        }    
    }

// ---------------------------------------------------------------------------
// void CGeoProfile::GeoProfileRadius
// ---------------------------------------------------------------------------
//    
TInt CGeoProfile::GeoProfileRadius()
    {
    return iProfileRadius;
    }

// ---------------------------------------------------------------------------
// void CGeoProfile::SetGeoProfileRadius
// ---------------------------------------------------------------------------
//
void CGeoProfile::SetTriggerId( TInt  aTriggerId )
    {
    iTriggerId = aTriggerId;
    }

// ---------------------------------------------------------------------------
// TInt CGeoProfile::TriggerId
// ---------------------------------------------------------------------------
//    
TInt CGeoProfile::TriggerId()
    {
    return iTriggerId;
    }

// ---------------------------------------------------------------------------
// void CGeoProfile::SetGeoProfileRadius
// ---------------------------------------------------------------------------
//
void CGeoProfile::SetGeoProfileRadius( TInt  aRadius )
    {
    if ( aRadius < KMinGeoProfileRadius )
        {
        iProfileRadius = KMinGeoProfileRadius; 
        }
    else
        {
        iProfileRadius = aRadius;  
        }
    }

// ---------------------------------------------------------------------------
// TInt CGeoProfile::TriggerType
// ---------------------------------------------------------------------------
//
TInt CGeoProfile::TriggerType()
    {
    return iTriggerType;    
    }

// ---------------------------------------------------------------------------
// void CGeoProfile::SetTriggerType
// ---------------------------------------------------------------------------
//
void CGeoProfile::SetTriggerType( TInt  aTriggerType )
    {
    iTriggerType = aTriggerType;
    }

// ---------------------------------------------------------------------------
// void CGeoProfile::SetGeoProfileNameL
// ---------------------------------------------------------------------------
//
void CGeoProfile::SetGeoPlaceNameL( const TDesC&   aGeoPlaceName )
    {    
    if ( aGeoPlaceName.Length() > KGeoPlaceNameMaxLength )
        {
        User::Leave( KErrOverflow );
        }
    
    if ( !aGeoPlaceName.Length())
        {
        HBufC* placeName = StringLoader::LoadLC( R_LBL_GEOPROFILES_DEFINE );    
        iGeoPlaceName->Des().Copy( placeName->Des());    
        CleanupStack::PopAndDestroy( placeName );
        SetGeoPositionType( EUndefined );       
        }
    else
        {
        // Copy the corresponding profile name
        iGeoPlaceName->Des().Copy( aGeoPlaceName );             
        }  
    }

// ---------------------------------------------------------------------------
// void CGeoProfile::PartialInternalizeL
// ---------------------------------------------------------------------------
//    
void CGeoProfile::PartialInternalizeL( const TDesC&   aInputBuffer )
    {
    GEOPROFILEDEBUG("+ CGeoProfile::PartialInternalizeL" ) 
    
    TInt32 value = 0;
    TLex lexer( aInputBuffer );
    
    // Read Profile Id   
    User::LeaveIfError( lexer.Val( value ));
    SetGeoProfileIdL( value );
    lexer.Inc();
    
    // Read the Position Type Info
    User::LeaveIfError( lexer.Val( value ));
    SetGeoPositionType( value );
    lexer.Inc();    
    
    // Write the Horizontal Accuracy
    User::LeaveIfError( lexer.Val( value ));
    if ( value == -1 )
        {
        TRealX nan;
        nan.SetNaN();
        iPositionInfo.SetHorizontalAccuracy( nan );        
        }
    else
        {
        iPositionInfo.SetHorizontalAccuracy( value );      
        }
    lexer.Inc();        
        
    // Set the profile radius
    User::LeaveIfError( lexer.Val( value ));
    SetGeoProfileRadius( value );
    lexer.Inc();
    
    GEOPROFILEDEBUG("- CGeoProfile::PartialInternalizeL" )     
    }

// ---------------------------------------------------------------------------
// HBufC* CGeoProfile::PartialExternalizeL
// ---------------------------------------------------------------------------
//     
HBufC* CGeoProfile::PartialExternalizeLC()
    {
    GEOPROFILEDEBUG("+ CGeoProfile::PartialExternalizeLC" ) 
    
    HBufC* buffer = HBufC::NewLC( 256 );
    TPtr bufferPtr = buffer->Des();

    // Write Geo Profile Id
    bufferPtr.AppendNum( iGeoProfileId );
    bufferPtr.Append( KDeliminator );
    
    // Write Position Type
    bufferPtr.AppendNum( iPositionType );
    bufferPtr.Append( KDeliminator );
        
    // Write the Horizontal Accuracy
    if ( Math::IsNaN( iPositionInfo.HorizontalAccuracy()))
        {
        bufferPtr.AppendNum( -1 );    
        }
    else
        {
       bufferPtr.AppendNum( iPositionInfo.HorizontalAccuracy());
        }
    bufferPtr.Append( KDeliminator );
        
    // Write the Radius
    bufferPtr.AppendNum( iProfileRadius ); 
    bufferPtr.Append( KDeliminator );
    
    GEOPROFILEDEBUG("- CGeoProfile::PartialExternalizeLC" ) 
    
    return buffer;
    }
