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


#ifndef __CHATSDP_ATTRIBUTE_PARSER_PAN__
#define __CHATSDP_ATTRIBUTE_PARSER_PAN__

/** Panic Category */
_LIT( KPanicSAP, "SAP" );

/** Chat application panic codes */
enum TChatSdpAttributeParserPanics 
    {
    EChatSdpAttributeParserInvalidCommand = 1,
    EChatSdpAttributeParserNoValue,
    EChatSdpAttributeParserValueIsList,
    EChatSdpAttributeParserValueTypeUnsupported
    };

inline void Panic( TChatSdpAttributeParserPanics aReason )
    { 
    User::Panic( KPanicSAP, aReason );
    }


#endif // __CHATSDP_ATTRIBUTE_PARSER_PAN__

// End of File
