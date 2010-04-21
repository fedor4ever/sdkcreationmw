/*
* Copyright (c) 2006 - 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include <mda\common\audio.h>
#include <mmf\common\mmfutilities.h>
#include <MdaAudioInputStream.h>	// audio input stream
#include <MdaAudioOutputStream.h>	// audio output stream
#include <s32file.h>				// RFileWriteStream and RFileReadStream

#include "AudioStreamEngine.h"
#include "audiostream.pan"

// Audio data buffer size.
// In both 3rd Edition and 2nd Edition the total buffer (iStreamBuffer) size is 
// KFrameSizePCM * KFrameCountPCM = 40960 bytes. This will contain 2560 ms 
// of 16-bit audio data. 
// In 3rd Edition the KFrameSizePCM is 4096 bytes, because CMdaAudioInputStream::ReadL() 
// returns audio data in 4096-byte chunks. In 2nd Edition, ReadL() returns data in 320-byte
// chunks.


const TInt KFrameSizePCM = 4096;
const TInt KFrameCountPCM = 10;

// Audio data buffer size for AMR encoding. For AMR, the buffer size is the same in
// both 2nd and 3rd Edition devices (20 ms per frame, a total of 2560 ms in 128 frames).
const TInt KFrameSizeAMR = 14;
const TInt KFrameCountAMR = 128;

// Header data for an AMR-encoded audio file
const TInt KAMRHeaderLength=6;
const TUint8 KAMRNBHeader[KAMRHeaderLength] = { 0x23, 0x21, 0x41, 0x4d, 0x52, 0x0a };

// Files to store the sample audio clips
_LIT(KAudioFilePCM, "sample.aud");
_LIT(KAudioFileAMR, "sample.amr");

#ifdef __WINS__
// The path to the sample files in 2nd Ed emulator
_LIT(KEmulatorPath, "c:\\system\\apps\\audiostream\\");
#endif

CAudioStreamEngine* CAudioStreamEngine::NewL(CAudioStreamAppUi* aAppUi)
	{
	CAudioStreamEngine* self = CAudioStreamEngine::NewLC(aAppUi);
    CleanupStack::Pop(self);
	return self;
	}

CAudioStreamEngine* CAudioStreamEngine::NewLC(CAudioStreamAppUi* aAppUi)
	{
	CAudioStreamEngine* self = new (ELeave) CAudioStreamEngine(aAppUi);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// Standard EPOC 2nd phase constructor
void CAudioStreamEngine::ConstructL()
	{
	// Construct streams. We need to construct these here, so that at least the input stream
	// exists if SetEncodingL() is called before any recording has taken place 
	iInputStream = CMdaAudioInputStream::NewL(*this);
	iOutputStream = CMdaAudioOutputStream::NewL(*this);

	// Get a handle to the RFs session to be used (owned by CEikonEnv, NOT to be closed
	// when this application exits!)		
	iFs = CEikonEnv::Static()->FsSession();

	// Save the default encoding for later reference (the encoding is the same for
	// both input and output streams).
	iDefaultEncoding = iInputStream->DataType();
	// At first we are using the default encoding.
	iCurrentEncoding = iDefaultEncoding;

	// Stream buffer allocation (by default for PCM)
	iStreamBuffer = HBufC8::NewMaxL(iFrameSize * iFrameCount);
	iStreamStart=0;
	iStreamEnd=iFrameCount - 1;
	


   	User::LeaveIfError( iFs.CreatePrivatePath( EDriveC ) );
		User::LeaveIfError( iFs.SetSessionToPrivate( EDriveC ) );
	}

// ----------------------------------------------------------------------------
// CAudioStreamEngine::CAudioStreamEngine(
//     CAudioStreamAppUi* aAppUi)
//
// onstructor
// ----------------------------------------------------------------------------
CAudioStreamEngine::CAudioStreamEngine(CAudioStreamAppUi* aAppUi)
: iAppUi(aAppUi), iUseAMR(EFalse), iAudioFile(KAudioFilePCM), iFrameSize(KFrameSizePCM), 
iFrameCount(KFrameCountPCM), iStreamBuffer(0), iFramePtr(0,0), iBufferOK(EFalse)
	{
	// By default we use PCM and initialise the instance variables accordingly above.
		
	// Initial audio stream properties for input and output, 8KHz mono. 
	// These settings could also be set/changed using method SetAudioPropertiesL() of
	// the input and output streams.
	iStreamSettings.iChannels=TMdaAudioDataSettings::EChannelsMono;
	iStreamSettings.iSampleRate=TMdaAudioDataSettings::ESampleRate8000Hz;
	}

// ----------------------------------------------------------------------------
// CAudioStreamEngine::~CAudioStreamEngine()
//
// destructor
// ----------------------------------------------------------------------------
CAudioStreamEngine::~CAudioStreamEngine()
	{ 
	// close and delete streams
	if (iInputStream)
		{
		if (iInputStatus!=ENotReady) iInputStream->Stop();
	    delete iInputStream;
    	iInputStream=NULL;
		}
	if (iOutputStream)
		{
		if (iOutputStatus!=ENotReady) iOutputStream->Stop();
	    delete iOutputStream;
    	iOutputStream=NULL;
		}
	if (iStreamBuffer)
		{
		delete iStreamBuffer;
		iStreamBuffer = NULL;
		}
	}


// ----------------------------------------------------------------------------
// CAudioStreamEngine::Play()
//
// plays the audio data contained in the buffer
// ----------------------------------------------------------------------------
void CAudioStreamEngine::Play()
	{
	ShowMessage(_L("Play "), ETrue);
	// if either stream is active, return
	if (iInputStatus!=ENotReady || iOutputStatus!=ENotReady) 
		{
	    ShowMessage(_L("Stream in use, \ncannot play audio."), ETrue);
		return;
		}

	if(!iBufferOK)
	    {
	    ShowMessage(_L("Nothing to play - \nrecord or load \na file first."), ETrue);
	    return;
	    }
		
	// Open output stream.
	// Upon completion will receive callback in 
	// MMdaAudioOutputStreamCallback::MaoscOpenComplete().

	iOutputStream->Open(&iStreamSettings);
	}


// ----------------------------------------------------------------------------
// CAudioStreamEngine::Record()
//
// records audio data into the buffer
// ----------------------------------------------------------------------------
void CAudioStreamEngine::Record()
	{
	// If either stream is active, return
	if (iInputStatus!=ENotReady || iOutputStatus!=ENotReady) 
	{
	    ShowMessage(_L("Stream in use, \ncannot record audio."), ETrue);
		return;
	}

	// Open input stream.
	// Upon completion will receive callback in 
	// MMdaAudioInputStreamCallback::MaiscOpenComplete().

	iInputStream->Open(&iStreamSettings);
	}

// ----------------------------------------------------------------------------
// CAudioStreamEngine::Stop()
//
// stops playing/recording
// ----------------------------------------------------------------------------
void CAudioStreamEngine::Stop()
	{
	// if input or output streams are active, close them
	if (iInputStatus!=ENotReady) 
		{
		iInputStream->Stop();
		ShowMessage(_L("\nRecording stopped!"), EFalse);
		iBufferOK = ETrue;
		}		
	if (iOutputStatus!=ENotReady) 
		{
		iOutputStream->Stop();
		ShowMessage(_L("\nPlayback stopped!"), ETrue);
		}
	}


// ----------------------------------------------------------------------------
// CAudioStreamEngine::LoadAudioFileL()
//
// loads the audio data from a file into the buffer
// ----------------------------------------------------------------------------
void CAudioStreamEngine::LoadAudioFileL()
	{
	RFileReadStream audiofile;

	// open file
	TFileName fileName;
	fileName.Copy(iAudioFilePath);
	fileName.Append(iAudioFile);

	TInt err = audiofile.Open(iFs, fileName, EFileRead|EFileStream);
	iStreamBuffer->Des().FillZ(iFrameCount * iFrameSize);  // Empty the stream buffer
	if (err==KErrNone) 
		{
		// file opened ok, proceed reading
		if (iUseAMR)
			{
			// Read the AMR header (the first 6 bytes). We don't need to save/use the header,
			// since while playback we already know it's an AMR-NB encoded stream.
			TBuf8<KAMRHeaderLength> temp;
			audiofile.ReadL(temp, KAMRHeaderLength);
			}

		TUint idx=0;
		while (idx < iFrameCount)
			{
			TRAPD(fstatus, audiofile.ReadL(GetFrame(idx), iFrameSize));
			if (fstatus!=KErrNone)
				break;
			idx++;
			}
		iStreamStart=0;
		iStreamEnd=idx-1;
		ShowMessage(_L("Loading complete!"), ETrue);
		iBufferOK = ETrue;	
		}	
	else 
		{
		// failed to open file
		ShowMessage(_L("Error loading \naudio sample!"), ETrue); 
		iBufferOK = EFalse;
		}
	audiofile.Close();
	}


// ----------------------------------------------------------------------------
// CAudioStreamEngine::SaveAudioFileL()
//
// saves the audio data in the buffer into a file
// ----------------------------------------------------------------------------
void CAudioStreamEngine::SaveAudioFileL()
	{
	if (!iBufferOK)
	{
		// In case the encoding was changed between recording and trying to save the file
		ShowMessage(_L("Recorded buffer does not \nmatch current encoding."), ETrue);	
		ShowMessage(_L("\nPlease re-record and \ntry again."), EFalse);	
		return;
	}
	RFileWriteStream audiofile;

	// Check for free space for saving the sample
	TVolumeInfo volinfo;
	TInt err=iFs.Volume(volinfo,EDriveC);
	if ( volinfo.iFree<(iFrameCount*iFrameSize))
		{
		// Not enough free space on drive for saving, report and exit
		ShowMessage(_L("Cannot save file:\nnot enough space!"), ETrue);	
		return;
		}

	TFileName fileName;
	fileName.Copy(iAudioFilePath);
	fileName.Append(iAudioFile);
	err = audiofile.Replace(iFs, fileName, EFileWrite|EFileStream);
	if (err==KErrNone) 
		{
		if (iUseAMR)
			{
				// Write the six-byte AMR header, so that the file can be used by other
				// applications as well.
				for (int i = 0; i < KAMRHeaderLength; i++)
					audiofile.WriteUint8L(KAMRNBHeader[i]);
			}
			
		// File opened ok, proceed writing.
		// Write audio data directly from iStreamBuffer
		for (TUint idx=iStreamStart; idx<=iStreamEnd; idx++)//iFrameCount; idx++)
			{
			audiofile.WriteL(GetFrame(idx));
			}
		ShowMessage(_L("Saving complete!"), ETrue);	
		}	
	else 
		{
		// failed to open file
		ShowMessage(_L("Error saving \naudio sample!"), ETrue);	
		}
	audiofile.Close();
	}

// ----------------------------------------------------------------------------
// CAudioStreamEngine::SetEncodingL(TBool aAmr)
//
// If argument is ETrue, AMR-NB encoding will be used in audio input/output.
// If EFalse, the default PCM is used. If the platform does not support AMR-NB,
// PCM will be used no matter what the argument's value is.
// ----------------------------------------------------------------------------
void CAudioStreamEngine::SetEncodingL(TBool aAmr)
	{
	// Act only if the new encoding differs from the current one
	if (iUseAMR != aAmr)
		{
		iUseAMR = aAmr;
		if (iUseAMR)
			{
			// Try to set AMR-NB encoding, this will indicate whether it is supported
			// by the platform or not.
			TRAPD(err, iInputStream->SetDataTypeL(KMMFFourCCCodeAMR));
			if (err != KErrNone)
				{
				ShowMessage(_L("AMR-NB not supported,\nusing PCM."), ETrue);	
				iCurrentEncoding = iDefaultEncoding;
				iUseAMR = EFalse;
				// We do not need to invalidate the buffer or change buffer settings, 
				// since the encoding was not changed -> just return.
				return;  
				}
			else
				{
				iCurrentEncoding = KMMFFourCCCodeAMR;
				iAudioFile.Zero();  // Empty the audio file name				
				iAudioFile.Append(KAudioFileAMR);
				iFrameCount = KFrameCountAMR;
				iFrameSize = KFrameSizeAMR;
				ShowMessage(_L("Encoding set to AMR-NB."), ETrue);	
				}
			}
		else
			{
			// If we get here, the encoding has previously been changed to AMR. Switch back to
			// PCM.
			iCurrentEncoding = iDefaultEncoding;
			iAudioFile.Zero();  // Empty the audio file name				
			iAudioFile.Append(KAudioFilePCM);
			iFrameCount = KFrameCountPCM;
			iFrameSize = KFrameSizePCM;
			ShowMessage(_L("Encoding set to PCM."), ETrue);	
			}

		// Make sure the user re-records or reloads the audio file, so that we do not 
		// accidentally try to play PCM data using AMR or vice versa.
		iBufferOK = EFalse;	
		if (iStreamBuffer) delete iStreamBuffer;
		iStreamBuffer = NULL; // In case the following NewL leaves
		iStreamBuffer = HBufC8::NewMaxL(iFrameSize * iFrameCount);
		iStreamStart=0;
		iStreamEnd=iFrameCount - 1;
		}	
	}

// ----------------------------------------------------------------------------
// CAudioStreamEngine::ShowMessage(
//     const TDesC& aMsg, TBool aReset=EFalse)
//
// displays text referenced by aMsg in the label, will append the aMsg in the 
// existing text in label if aReset is EFalse, otherwise will reset the label 
// text.
// ----------------------------------------------------------------------------
void CAudioStreamEngine::ShowMessage(const TDesC& aMsg, TBool aReset=EFalse)
	{
	if (aReset) 	// if ETrue, clear the message on the label prior to output
		iMsg.Zero();
	iMsg.Append(aMsg);
	TRAPD(error, iAppUi->GetView()->ShowMessageL(iMsg));
	PanicIfError(error);
	}

// ----------------------------------------------------------------------------
// TPtr8& CAudioStreamEngine::GetFrame(TUint aFrameIdx)
//
// Returns a modifiable pointer to a single frame inside the audio buffer 
// ----------------------------------------------------------------------------
TPtr8& CAudioStreamEngine::GetFrame(TUint aFrameIdx)
	{
	  __ASSERT_ALWAYS(aFrameIdx < iFrameCount, 
  									User::Panic(_L("AudioStreamEx"), 1));
  								
	  iFramePtr.Set((TUint8*)(iStreamBuffer->Ptr() + (aFrameIdx * iFrameSize)),
  								 iFrameSize,
  								 iFrameSize);
	  return iFramePtr;
	}

// ----------------------------------------------------------------------------
// TPtr8& CAudioStreamEngine::GetPlaybackFrames(TUint aLastFrame)
//
// Returns a modifiable pointer to the requested frames inside the audio buffer
// (from the first frame to aLastFrame). 
// ----------------------------------------------------------------------------
TPtr8& CAudioStreamEngine::GetPlaybackFrames(TUint aLastFrame)
	{
	__ASSERT_ALWAYS(aLastFrame < iFrameCount, 
  								User::Panic(_L("AudioStreamEx"), 2));
  								
	iFramePtr.Set((TUint8*)(iStreamBuffer->Ptr()),
  								 (aLastFrame + 1) * iFrameSize,
  								 (aLastFrame + 1) * iFrameSize);
  	return iFramePtr;
	}


//
// MMdaAudioInputStream callbacks (MMdaAudioInputStreamCallback)
//
// ----------------------------------------------------------------------------
// CAudioStreamEngine::MaiscOpenComplete(
//     TInt aError)
//
// called upon completion of CMdaAudioInputStream::Open(),
// if the stream was opened succesfully (aError==KErrNone), it's ready for use.
// upon succesful open, the first audio data block will be read from the input
// stream.
// ----------------------------------------------------------------------------
void CAudioStreamEngine::MaiscOpenComplete(TInt aError)
	{
	if (aError==KErrNone) 
		{
		// Input stream opened succesfully, set status
		iInputStatus = EOpen;
		// Set the data type (encoding)
		TRAPD(error, iInputStream->SetDataTypeL(iCurrentEncoding));
		PanicIfError(error);

		// set stream input gain to maximum
		iInputStream->SetGain(iInputStream->MaxGain());	
		// set stream priority to normal and time sensitive
		iInputStream->SetPriority(EPriorityNormal, EMdaPriorityPreferenceTime);				
		ShowMessage(_L("Recording..."), ETrue);
		
		// Emtpy the buffer and issue ReadL() to read the first audio data block, 
		// subsequent calls to ReadL() will be issued 
		// in MMdaAudioInputStreamCallback::MaiscBufferCopied()
		iStreamBuffer->Des().FillZ(iFrameCount * iFrameSize);
		iStreamIdx=0;
		TRAPD(error2, iInputStream->ReadL(GetFrame(iStreamIdx)));
		PanicIfError(error2);
		} 
	else 
		{
		// input stream open failed
		iInputStatus = ENotReady;
		ShowMessage(_L("Recording failed!"), ETrue);
		}
	}

// ----------------------------------------------------------------------------
// CAudioStreamEngine::MaiscBufferCopied(
//     TInt aError, const TDesC8& aBuffer)
//
// called when a block of audio data has been read and is available at the 
// buffer reference *aBuffer.  calls to ReadL() will be issued until all blocks
// in the audio data buffer (iStreamBuffer) are filled.
// ----------------------------------------------------------------------------
void CAudioStreamEngine::MaiscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	
	if (aError==KErrNone) 
		{
		// stop recording if at the end of the buffer
		iStreamIdx++;
		if (iStreamIdx == iFrameCount)
		    {
		    ShowMessage(_L("\nRecording complete!"), EFalse);
		    iStreamEnd = iStreamIdx - 1;
	    	iBufferOK = ETrue;
			iInputStatus = ENotReady;
			// NOTE: In 2nd Edition we MUST NOT call iInputStream->Stop() here, because
			// this will cause a crash on 2nd Edition, FP1 devices.
			// Since iInputStream->Stop() is not called, the callback method
			// MaiscRecordComplete() will not be called either after exiting this method.
			// In 3rd Edition, however, iInputStream->Stop() MUST be called in order to reach
			// MaiscRecordComplete(), otherwise the stream will "hang".
			
		    return;
		  	}		
		
		// issue ReadL() for next frame		
		TRAPD(error, iInputStream->ReadL(GetFrame(iStreamIdx)));
		PanicIfError(error);
		}
	else if (aError==KErrAbort) 
		{
		// Recording was aborted, due to call to CMdaAudioInputStream::Stop()
		// This KErrAbort will occur each time the Stop() method in this class is executed.
		// Also, MaiscRecordComplete() will be called after exiting this method.
	    iStreamEnd = iStreamIdx - 1;
	    iBufferOK = ETrue;
		iInputStatus = ENotReady;
		}
	else 
		{
		ShowMessage(_L("\nError reading data \nfrom input"), EFalse);
		iInputStatus = ENotReady;
		}
	}

// ----------------------------------------------------------------------------
// CAudioStreamEngine::MaiscRecordComplete(
//     TInt aError)
//
// called when input stream is closed by CMdaAudioInputStream::Stop()
// ----------------------------------------------------------------------------
void CAudioStreamEngine::MaiscRecordComplete(TInt aError)
	{	
	iInputStatus = ENotReady;
	if (aError==KErrNone) 
		{
		// normal stream closure
		}
	else 
		{
		// completed with error(s)
		}

    // see comments in MaiscBufferCopied() regarding stopping the stream.
    iInputStream->Stop();
	}


// MMdaAudioOutputStream callbacks (MMdaAudioOutputStreamCallback)

// ----------------------------------------------------------------------------
// CAudioStreamEngine::MaoscOpenComplete(
//     TInt aError)
//
// called upon completion of CMdaAudioOutputStream::Open(),
// if the stream was opened succesfully (aError==KErrNone), it's ready for use.
// upon succesful open, the first audio data block will be written to the 
// output stream.
// ----------------------------------------------------------------------------
void CAudioStreamEngine::MaoscOpenComplete(TInt aError)
	{
	if (aError==KErrNone) 
		{
		// output stream opened succesfully, set status
		iOutputStatus = EOpen;
		// Set the data type (encoding). Should not fail, since we already
		// have tested support for this encoding in SetEncodingL with the 
		// corresponding input stream!
		TRAPD(error, iOutputStream->SetDataTypeL(iCurrentEncoding));
		PanicIfError(error);
		
		// set volume to 1/4th of stream max volume
		iOutputStream->SetVolume(iOutputStream->MaxVolume()/4);
		// set stream priority to normal and time sensitive
		iOutputStream->SetPriority(EPriorityNormal, 
			EMdaPriorityPreferenceTime);				
		ShowMessage(_L("Playing "), ETrue);

		if (iUseAMR)
			{
			// In case of AMR, the whole recorded/loaded buffer is played back at once, not frame by frame. 
			// The buffer might not be fully recorded, so we will only play back the part
			// that is filled with data.
			iStreamIdx = iStreamEnd;
			TRAPD(error2, iOutputStream->WriteL(GetPlaybackFrames(iStreamEnd)));
			PanicIfError(error2);
			}
		else
			{
			// PCM needs to be played back frame by frame, otherwise some older devices might
			// run into buffer overflow situations.
			iStreamIdx = 0;
			TRAPD(error3, iOutputStream->WriteL(GetFrame(iStreamIdx)));
			PanicIfError(error3);
			}
		}
	else 
		{
		// output stream open failed
		iOutputStatus = ENotReady;
		ShowMessage(_L("Playback failed!"), ETrue);
		}		
	}

// ----------------------------------------------------------------------------
// CAudioStreamEngine::MaoscBufferCopied(
//     TInt aError, const TDesC8& aBuffer)
//
// called when a block of audio data has been written to MMF. calls to WriteL() 
// will be issued until all blocks in the audio data buffer (iStreamBuffer) are 
// written.
// ----------------------------------------------------------------------------
void CAudioStreamEngine::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{	
	if (aError==KErrNone) 
		{
		if (iStreamIdx==iStreamEnd)
			{
			ShowMessage(_L("\nPlayback complete!"), EFalse);
			iOutputStatus = ENotReady;
			// NOTE: In 2nd Edition we MUST NOT call iOutputStream->Stop() here, because
			// this will cause a crash on 2nd Edition, FP1 devices.
			// Since iOutputStream->Stop() is not called, the callback method
			// MaiscRecordComplete() will not be called either after exiting this method.
			// In 3rd Edition, however, iOutputStream->Stop() MUST be called in order to reach
			// MaiscRecordComplete(), otherwise the stream will "hang".
			
				iOutputStream->Stop();
			
			}
		else 
			{
			iStreamIdx++;
			TRAPD(error, iOutputStream->WriteL(GetFrame(iStreamIdx)));	
			PanicIfError(error);
			}
		}
	else if (aError==KErrAbort) 
		{
		// Playing was aborted, due to call to CMdaAudioOutputStream::Stop().
		// MaoscRecordComplete() will be called after exiting this method.
		iOutputStatus = ENotReady;
		}
	else 
		{
		ShowMessage(_L("\nError writing data \nto output"), EFalse);			
		iOutputStatus = ENotReady;
		}
	}


// ----------------------------------------------------------------------------
// CAudioStreamEngine::MaoscPlayComplete(
//     TInt aError)
//
// called when output stream is closed by CMdaAudioOutputStream::Stop() or if 
// end of audio data has been reached, in this case KErrUnderflow will be 
// returned.
// ----------------------------------------------------------------------------
void CAudioStreamEngine::MaoscPlayComplete(TInt aError)
	{
	iOutputStatus = ENotReady;
	if (aError==KErrNone) 
		{
		// normal stream closure
		}	
	else if (aError==KErrUnderflow) 
		{
		// end of audio data stream was reached because of stream underflow,
		}
	else 
		{
		// completed with error(s)
		}	
	}

// END OF FILE

