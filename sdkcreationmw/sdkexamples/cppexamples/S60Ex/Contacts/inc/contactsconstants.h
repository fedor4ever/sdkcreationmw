/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CONTACTSCONSTANTS_H__
#define __CONTACTSCONSTANTS_H__

// CONSTANTS

const TInt KMaxDatabasePathAndNameLength = 60;
const TInt KMaxTemplateNameLength = 20;
const TInt KMaxTextLength = 19;
const TInt KMaxGroupNameLength = 20;
const TInt KArraySize = 4;
const TInt KFormattersSpace = 10;
const TInt KMaxTextBoxQueryLength = 40;
_LIT( KFirstName,"FirstName") ;
_LIT( KLastName,"LastName" );
_LIT( KPhoneNumber,"123123123" );

// CAknSingleStyleListBox needs next formatters
_LIT(KListItemFormatter,"\t%S %S\t\t");
_LIT(KListOItemFormatter,"\t%S\t\t");

#endif // __CONTACTSCONSTANTS_H__
