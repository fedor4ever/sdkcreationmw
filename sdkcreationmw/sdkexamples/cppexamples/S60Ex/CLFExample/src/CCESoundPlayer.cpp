/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "CCESoundPlayer.h"

#include <CLFExample.rsg>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCESoundPlayer::CCESoundPlayer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCESoundPlayer::CCESoundPlayer()
    {
    }

// -----------------------------------------------------------------------------
// CCESoundPlayer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCESoundPlayer::ConstructL()
    {
    // Create a player utility
    iMdaAudioPlayerUtility = CMdaAudioPlayerUtility::NewL( *this );
    }

// -----------------------------------------------------------------------------
// CCESoundPlayer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCESoundPlayer* CCESoundPlayer::NewL()
    {
    CCESoundPlayer* self = CCESoundPlayer::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCESoundPlayer::NewLC
// Two-phased constructor, pointer is left to the CleanupStack.
// -----------------------------------------------------------------------------
//
CCESoundPlayer* CCESoundPlayer::NewLC()
    {
    CCESoundPlayer* self = new (ELeave) CCESoundPlayer;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor
CCESoundPlayer::~CCESoundPlayer()
    {
    if( iPlayerState == ECEPlaying )
        {
        StopPlayback();
        }
        
    delete iMdaAudioPlayerUtility;
    }

// -----------------------------------------------------------------------------
// CCESoundPlayer::MapcInitComplete
// Defines client behaviour when opening and initialization of an audio sample 
// has completed.
// -----------------------------------------------------------------------------
//
void CCESoundPlayer::MapcInitComplete( 
                            TInt aError, 
                            const TTimeIntervalMicroSeconds& /*aDuration*/ )
	{
	iPlayerState = aError ? ECENotReady : ECEReadyToPlay;

    // Start the playback, if audio file initialization was successful
    if ( iPlayerState == ECEReadyToPlay )
	    {
	    iMdaAudioPlayerUtility->Play();
		iPlayerState = ECEPlaying;
	    }
	}

// -----------------------------------------------------------------------------
// CCESoundPlayer::MapcPlayComplete
// Defines client behaviour when playback of an audio sample has completed.
// -----------------------------------------------------------------------------
//
void CCESoundPlayer::MapcPlayComplete( TInt aError )
	{
	iPlayerState = aError ? ECENotReady : ECEReadyToPlay;	
	}

// -----------------------------------------------------------------------------
// CCESoundPlayer::StartPlayback
// Open music file and start music playback.
// -----------------------------------------------------------------------------
//
void CCESoundPlayer::StartPlaybackL( const TDesC& aFileName )
	{
	// Open the sound file
	iMdaAudioPlayerUtility->OpenFileL( aFileName );
	}

// -----------------------------------------------------------------------------
// CCESoundPlayer::StopPlayback
// Stop music playback.
// -----------------------------------------------------------------------------
//
void CCESoundPlayer::StopPlayback()
	{
	// Stop audio playback and close the audio file
	if( iMdaAudioPlayerUtility && iPlayerState == ECEPlaying )
	    {
	    iMdaAudioPlayerUtility->Stop();
	    iMdaAudioPlayerUtility->Close();
	    iPlayerState = ECENotReady;
	    }
	}

// -----------------------------------------------------------------------------
// CCESoundPlayer::PlayerState
// Get the state of the audio player.
// -----------------------------------------------------------------------------
//
TCEPlayerState CCESoundPlayer::PlayerState()
    {
    return iPlayerState;
    }

//  End of File
