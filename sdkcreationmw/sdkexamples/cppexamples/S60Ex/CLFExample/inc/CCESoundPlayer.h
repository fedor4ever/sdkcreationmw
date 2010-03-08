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



#ifndef __CCESOUNDPLAYER_H__
#define __CCESOUNDPLAYER_H__

//  INCLUDES
#include <e32base.h>
#include <MdaAudioSamplePlayer.h>

#include "CLFExample.hrh"

// CLASS DECLARATION

/**
*  Sound player for CLF Example Application.
*  Plays audio files in the application.
*/
class CCESoundPlayer 
    : public CBase,
      public MMdaAudioPlayerCallback
    {
    public:  // Constructors and destructor

		/**
		 * Creates and returns a new instance of this class.
		 * @return Pointer to the CCESoundPlayer object
		 */
	    static CCESoundPlayer* NewL();

		/**
		 * Creates and returns a new instance of this class. The returned
         * object is left on the cleanup stack.
		 * @return Pointer to the CCESoundPlayer object
		 */
	    static CCESoundPlayer* NewLC();

        /**
        * Destructor.
        */
        virtual ~CCESoundPlayer();

    private:    // Constructors
        /**
        * C++ default constructor.
        */
        CCESoundPlayer();

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    protected:  // Functions from base classes

        /**
        * From MMdaAudioPlayerCallback. Defines client behaviour 
        * when opening and initialization of an audio sample has 
        * completed.
        */
		void MapcInitComplete( TInt aError, 
		                       const TTimeIntervalMicroSeconds& aDuration );

        /**
        * From MMdaAudioPlayerCallback. Defines client behaviour 
        * when playback of an audio sample has completed.
        */
		void MapcPlayComplete( TInt aError );

    public:     // New functions

        /**
        * Open music file and start music playback.
        * @param aFileName Path and file name of the audio file
        */
        void StartPlaybackL( const TDesC& aFileName );
                
        /**
        * Stop music playback.
        */
        void StopPlayback();
        
        /**
        * Get the state of the audio player.
        * @return State of the audio player
        */
        TCEPlayerState PlayerState();
        
    private:    // Data
        
        // Audio player utility (owned)
        CMdaAudioPlayerUtility* iMdaAudioPlayerUtility;
        
        // State of the audio player
        TCEPlayerState iPlayerState;
        
        // For waiting the audio player initialization to complete
        CActiveSchedulerWait iWait;
        
    };

#endif // __CCESOUNDPLAYER_H__

// End of File
