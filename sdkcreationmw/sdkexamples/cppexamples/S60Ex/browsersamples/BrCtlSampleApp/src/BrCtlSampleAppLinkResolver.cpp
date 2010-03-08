/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer interface which handles resolving embedded links
*
*/


// INCLUDE FILES
#include <apmrec.h>
#include <apgcli.h>
#include <eikappui.h>
#include <eikenv.h>
#include "BrCtlSampleAppLinkResolver.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CBrCtlSampleAppLinkResolver::ResolveEmbeddedLinkL
// ---------------------------------------------------------
//
TBool 
CBrCtlSampleAppLinkResolver::ResolveEmbeddedLinkL(const TDesC& aEmbeddedUrl,
    		                                      const TDesC& /*aCurrentUrl*/,
        	                                      TBrCtlLoadContentType /*aLoadContentType*/, 
                                                  MBrCtlLinkContent& aEmbeddedLinkContent) 
    {        
    if (IsFileScheme(aEmbeddedUrl))
        {
        GetFileNameL(aEmbeddedUrl);
        HBufC8* buf = ReadFileLC(*iFileName);
        HBufC* contentType = NULL;
        TPtrC p(NULL, 0);
        contentType = RecognizeLC(*iFileName, *buf);
        aEmbeddedLinkContent.HandleResolveComplete(*contentType, p, buf);        
        CleanupStack::PopAndDestroy(2); // contentType, buf
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CBrCtlSampleAppLinkResolver::ResolveLinkL
// ---------------------------------------------------------
//
TBool 
CBrCtlSampleAppLinkResolver::ResolveLinkL(const TDesC& aUrl, 
                                          const TDesC& /*aCurrentUrl*/,
                                          MBrCtlLinkContent& aBrCtlLinkContent) 
    {        
    if (IsFileScheme(aUrl))
        {
        GetFileNameL(aUrl);
        HBufC8* buf = ReadFileLC(*iFileName);
        HBufC* contentType = NULL;
        TPtrC p(NULL, 0);
        contentType = RecognizeLC(*iFileName, *buf);
        aBrCtlLinkContent.HandleResolveComplete(*contentType, p, buf);        
        CleanupStack::PopAndDestroy(2); // contentType, buf
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CBrCtlSampleAppLinkResolver::CancelAll
// ---------------------------------------------------------
//
void 
CBrCtlSampleAppLinkResolver::CancelAll() 
    {        
    }


// ---------------------------------------------------------
// CBrCtlSampleAppLinkResolver::ConstructL
// ---------------------------------------------------------
//
void 
CBrCtlSampleAppLinkResolver::ConstructL()
    {
    }

// ---------------------------------------------------------
// CBrCtlSampleAppLinkResolver::~CBrCtlSampleAppLinkResolver
// ---------------------------------------------------------
//
CBrCtlSampleAppLinkResolver::~CBrCtlSampleAppLinkResolver()
    {
    delete iFileName;
    }


// ---------------------------------------------------------
// CBrCtlSampleAppLinkResolver::NewL
// ---------------------------------------------------------
//
CBrCtlSampleAppLinkResolver* 
CBrCtlSampleAppLinkResolver::NewL()
    {
    CBrCtlSampleAppLinkResolver* self = new(ELeave)CBrCtlSampleAppLinkResolver;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CBrCtlSampleAppLinkResolver::IsFileScheme
// ---------------------------------------------------------
//
TBool 
CBrCtlSampleAppLinkResolver::IsFileScheme(const TDesC& aFileName)
    {
    _LIT(KFileScheme, "file://");
    if (aFileName.Length() > 0 && aFileName.Ptr() != NULL)
        {
        if (aFileName.FindF(KFileScheme) == 0)
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CBrCtlSampleAppLinkResolver::ReadFile
// ---------------------------------------------------------
//
HBufC8* 
CBrCtlSampleAppLinkResolver::ReadFileLC(const TDesC& aFileName)
    {
    RFs rfs;
    RFile file;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);
    User::LeaveIfError(file.Open(rfs, aFileName, EFileRead));
    CleanupClosePushL(file);
    TInt size;
    User::LeaveIfError(file.Size(size));
    HBufC8* buf = HBufC8::NewLC(size);
    TPtr8 bufPtr(buf->Des());
    User::LeaveIfError(file.Read(bufPtr));
    CleanupStack::Pop(); // buf
    CleanupStack::PopAndDestroy(2); // file, rfs
    CleanupStack::PushL(buf);
    return buf;
    }

// ---------------------------------------------------------
// CBrCtlSampleAppLinkResolver::RecognizeL
// ---------------------------------------------------------
//
HBufC* 
CBrCtlSampleAppLinkResolver::RecognizeLC(const TDesC& aFileName, const TDesC8& aData)
    {
    TDataRecognitionResult dataType;
    RApaLsSession apaSession;
    TInt ret;
    HBufC* contentTypeString = NULL;

    User::LeaveIfError(apaSession.Connect());
    // Ask the application architecture to find the file type
    ret = apaSession.RecognizeData(aFileName, aData, dataType);
    apaSession.Close();

    if (ret == KErrNone &&
        (dataType.iConfidence == CApaDataRecognizerType::ECertain) ||
        (dataType.iConfidence == CApaDataRecognizerType::EProbable))
        {
        // If the file type was found, try to match it to a known file type
        TPtrC8 mimeTypePtr = dataType.iDataType.Des8();
        TInt len = mimeTypePtr.Length() + 1;
        contentTypeString = HBufC::NewL(len); 
        contentTypeString->Des().Copy(mimeTypePtr);
        contentTypeString->Des().ZeroTerminate();
        }
    CleanupStack::PushL(contentTypeString);
    return contentTypeString;
    }

// -----------------------------------------------------------------------------
// CBrCtlSampleAppLinkResolver::GetFileNameL
// Translate the file name from a URL to a valid file name in the system.
// -----------------------------------------------------------------------------
//
TBool 
CBrCtlSampleAppLinkResolver::GetFileNameL(const TDesC& aFileName)
    {
    // This function accepts URLs in the following format:
    // file://filename.xxx
    // file:///filename.xxx
    // file://c:/filename.xxx
    // file:///c:/filename.xxx
    //
    _LIT(KFileScheme, "file://");
    _LIT(KDefaultDrivePath, "C:\\");
    _LIT(KPathChar, "\\");
    
    TInt count;
    TInt index = 0;
    TBool drvLetter = EFalse;
    TUint16 c;
    
    // Verify the file scheme
    TPtrC urlPtr(aFileName);
    if (urlPtr.FindF(KFileScheme) != 0)
        {
        return EFalse;
        }
    urlPtr.Set(urlPtr.Mid(KFileScheme().Length()));
    
    // make sure there are enough characters in the filename before
    // trying to check them
    count = urlPtr.Length();
    if(count == 0)
        {
        return EFalse;            // no filename, so can't look at urlPtr[0]
        }
    
    // Skip the first '/' if there is one
    if (urlPtr[0] == '/')
        {
        urlPtr.Set(urlPtr.Mid(1));
        }
    count = urlPtr.Length();
    
    // Is there a drive letter?
    if(count > 1)
        {
        // can check for drive letter
        if (urlPtr[1 + index] == ':')
            {
            drvLetter = ETrue;
            }
        }
    if(drvLetter == EFalse)
        {
        // 3 additional characters for the string "c:\"
        count = urlPtr.Length() + 3;
        }
    iFileName = HBufC::NewL(count);
    if (!drvLetter)
        {
        iFileName->Des().Append(KDefaultDrivePath);
        }
    
    TBool fragment(EFalse);
    // Convert relative path containing /./ and /../ to absolute path
    for (; index < urlPtr.Length() && !fragment; index ++)
        {
        switch(urlPtr[index])
            {
            case '#':    //Check if there is a fragment '#'
                {
                fragment = ETrue;
                continue; // Just stop there
                }
                
            case '/':
                {
                iFileName->Des().Append(KPathChar);
                break;
                }
            case '.':
                {
                if (index > 1 && urlPtr[index - 1] == '/')
                    {
                    if (index < count - 1 && urlPtr[index + 1] == '/')
                        {
                        index ++; // skip  ./
                        break;
                        }
                    if (index > 2 && index < count - 3 &&
                        urlPtr[index + 1] == '.' && urlPtr[index + 2] == '/')
                        {
                        TInt i = index - 2;
                        
                        for (; i > 0 && urlPtr[i] != '/'; i--) {} // skip  /../
                        
                        iFileName->Des().SetLength(iFileName->Des().Length() - (index - i));
                        index += 2;
                        break;
                        }
                    }
                }
                // no break
                //lint -fallthrough
                
            default:
                {
                c = urlPtr[index];
                iFileName->Des().Append(&c, 1);
                break;
                }
            }	// end of switch
        }
    return ETrue;
}

// End of File  

