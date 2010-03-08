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
* Description:  DRM support
 *
*/


// INCLUDE FILES

#include "EcmtMenubar.h"
#include "EcmtWin32.h"
#include "httpfile.h"

#include <s32mem.h>
#include <eikenv.h>
#include <apmstd.h>
#include <DRMMessageParser.h>
#include <DocumentHandler.h>
#include <AiwGenericParam.h>

// MACROS
#define __S8(x) _S8(x)
#define __S(x) _S(x)

// (Some) DRM content types defined in Oma2Agent.h:
#define KOma1DrmMessageContentType "application/vnd.oma.drm.message"
#define KOma1XmlRoContentType      "application/vnd.oma.drm.rights+xml"
#define KOma1WbxmlRoContentType    "application/vnd.oma.drm.rights+wbxml"
#define KOma1DcfContentType        "application/vnd.oma.drm.content"

typedef TInt (*ExtFunL) (CDRMMessageParser*, HBufC8 *& );
typedef struct _DrmType {
    const TText* ext;
    const TText8* contentType;
    ExtFunL  funL;
    TBool processContent;
} DrmType;

static TInt ParseDRC(CDRMMessageParser* parser, HBufC8 *& drm)
{
    RPointerArray<CDRMRights> rightsDetail;
    TInt err = parser->ProcessRightsObject(*drm, rightsDetail);
    if (err == KErrNone)
    {
        CEikonEnv* env = CEikonEnv::Static();
        if (env)
        {
            _LIT(KMessage,"DRM rights received");
            env->InfoMsg(KMessage);
        }
    }
    return err;
}

static TInt ParseDCF(CDRMMessageParser*, HBufC8 *&)
{
    //return parser->ProcessContentDataL(*drm);
    return KErrNone;
}

static TInt ParseDM(CDRMMessageParser* parser, HBufC8 *& drm)
{
    return parser->ProcessMessage(drm);
}

static TInt ParseDR(CDRMMessageParser*, HBufC8*&)
{
    return KErrNone;
}

/**
 * Detects whether the file is a DRM file and attempts to handle it.
 * Returns ETrue on success, EFalse on failure or if the file does 
 * not seem to be a DRM file. Leaves in case of a catastrophic failure.
 * Takes Windows file name as a parameter.
 */
TBool CEcmtMenubar::LoadFileAsDrmL(const TDesC& aFileName)
{
    static const DrmType drmTypes[] = {
        {_S("dm"),  __S8(KOma1DrmMessageContentType), ParseDM, ETrue},
        {_S("dcf"), __S8(KOma1DcfContentType), ParseDCF, ETrue},
        {_S("odf"), __S8(KOma1DcfContentType), ParseDCF, ETrue},
        {_S("drc"), __S8(KOma1WbxmlRoContentType), ParseDRC, EFalse},
        {_S("dr"),  __S8(KOma1XmlRoContentType), ParseDRC, EFalse}
    };

    // First check if this is an HTTP response file.
    ParseHttpResponse* resp = NULL;
    const DrmType* drmType = NULL;
    char* fname = STRING_ToMultiByteN(aFileName.Ptr(),aFileName.Length());
    if (fname)
    {
        resp = HTTP_ParseFile(fname, HttpFalse);
        if (resp)
        {
            if (HTTP_ResponseStatus(resp) == HTTP_STATUS_CODE_OK)
            {
                const HttpContent* content = HTTP_ResponseContent(resp);
                if (content && content->type)
                {
                    for (TInt i=0; i<COUNT(drmTypes) && !drmType; i++)
                    {
                        const char* type = (char*)drmTypes[i].contentType;
                        if (!strcmp(type, content->type))
                        {
                            drmType = drmTypes + i;
                        }
                    }
                }
            }
            if (!drmType)
            {
                HTTP_ResponseDelete(resp);
                resp = NULL;
            }
        }
        MEM_Free(fname);
    }
    
    // Now check if this file has one of the DRM extensions
    if (!drmType)
    {
        TPtrC fileExt(FileExt(aFileName));
        for (TInt i=0; i<COUNT(drmTypes) && !drmType; i++)
        {
            if (fileExt.CompareF(TPtrC(drmTypes[i].ext)) == 0) {
                drmType = drmTypes + i;
            }
        }
    }

    TBool ok = EFalse;
    if (drmType)
    {
        HBufC8* drm;
        if (resp)
        {
            // We have to copy the data. Sorry
            const HttpContent* hc = HTTP_ResponseContent(resp);
            drm = HBufC8::New(hc->size); // No leaving, please
            if (drm) drm->Des().Append(TPtrC8((TText8*)hc->data, hc->size));
            HTTP_ResponseDelete(resp);
            User::LeaveIfNull(drm);
            CleanupStack::PushL(drm);
        }
        else
        {
            // Read the file into memory
            drm = ReadExtFileLC(aFileName);
        }

        // Set up the parser
        TMemBuf memBuf;
        TUint8* ptr = &(drm->Des()[0]);
        memBuf.Set(ptr, ptr + drm->Length());
        RWriteStream stream(&memBuf);
        CDRMMessageParser* parser = CDRMMessageParser::NewL();
        CleanupStack::Pop(drm);

        // Process the message. Note that CDRMMessageParser may reallocate
        // the message.
        TInt err = drmType->funL(parser, drm);
        delete parser;

        // Push possibly reallocated content to the cleanup stack
        // Now it's pure content, with rights and other garbage 
        // stripped off. CDRMMessageParser saves rights in the DRM 
        // database.
        CleanupStack::PushL(drm);

        // If an error was returned, it's probably not a DRM content
        if (err == KErrNone && drmType->processContent)
        {
            // Create a temporary file
            RFs& fs = FsSessionL();
            TPtrC tempDir(__S(SDK_TEMP_DIR));
            err = fs.MkDirAll(tempDir);
            if (err == KErrAlreadyExists) err = KErrNone;
            User::LeaveIfError(err);

            // Allocate buffer for the file name
            TPtrC filePart(ExtractFilePart(aFileName));
            HBufC* fname = HBufC::NewLC(tempDir.Length()+filePart.Length()+1);

            // Format the file name
            TPtr fnamePtr(fname->Des());
            fnamePtr.Copy(tempDir);
            fnamePtr.Append(filePart);

            // Create a new file in the Symbian temporary directory.
            RFile f;
            User::LeaveIfError(f.Replace(fs, fnamePtr, EFileWrite));

            // Write the data and close the file
            CleanupClosePushL(f);
            User::LeaveIfError(f.Write(*drm));
            CleanupStack::PopAndDestroy(&f);

            // Try to do something with it
            TPtrC8 drmFileTypePtr(drmType->contentType);
            TDataType dataType(drmFileTypePtr);
            TText16* url = ConvertFileNameToUrl(fnamePtr.PtrZ(), FILE_SCHEME);
            User::LeaveIfNull(url);
            CleanupStack::PushL(TCleanupItem(FreeOnLeave,url));
            TPtrC urlPtr(url);

            // Initialize document handler
            if (iDocHandler)
            {
                delete iDocHandler;
                iDocHandler = NULL;
            }
            iDocHandler = CDocumentHandler::NewL();
            CAiwGenericParamList* gpl = CAiwGenericParamList::NewLC();
            TAiwVariant yes(ETrue);
            gpl->AppendL(TAiwGenericParam(EGenericParamURL,TAiwVariant(urlPtr)));
            gpl->AppendL(TAiwGenericParam(EGenericParamAllowSave, yes));
            gpl->AppendL(TAiwGenericParam(EGenericParamAllowMove, yes));

            iDocHandler->OpenTempFileL(*fname, f);
            CleanupClosePushL(f);
            iDocHandler->OpenFileEmbeddedL(f, dataType, *gpl);

            // Cleanup
            CleanupStack::PopAndDestroy(&f);
            CleanupStack::PopAndDestroy(gpl);
            CleanupStack::PopAndDestroy(url);
            CleanupStack::PopAndDestroy(fname);
        }
        CleanupStack::PopAndDestroy(drm);
        ok = (err == KErrNone);
    }
        
    return ok;
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
