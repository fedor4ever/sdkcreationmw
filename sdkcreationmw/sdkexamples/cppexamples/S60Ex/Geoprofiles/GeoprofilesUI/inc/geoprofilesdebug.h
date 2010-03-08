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
* Description:  Geo Profile Debug
*
*/


#ifndef _GEOPROFILESDEBUG_H_
#define _GEOPROFILESDEBUG_H_

// INCLUDES
#include <e32std.h>
#define GEOPROFILE_DEBUG

// MACROS
#define DEBUG(TEXT) 				    { \
									    _LIT( KText, #TEXT ); \
									    Debug(KText); \
									    }
									 
#define DEBUG1(TEXT, ARG1) 			    { \
									    _LIT( KText, #TEXT ); \
									    Debug( KText, (ARG1) ); \
									    }
									 
#define DEBUG2(TEXT, ARG1, ARG2)        { \
									    _LIT( KText, #TEXT ); \
									    Debug( KText,(ARG1), (ARG2) ); \
									    }
									 
#define DEBUG3(TEXT, ARG1, ARG2, ARG3)  { \
									    _LIT( KText, #TEXT ); \
									    Debug( KText, (ARG1), (ARG2), (ARG3) ); \
									    }


void Debug( TRefByValue<const TDesC> aText, ...);


#ifdef GEOPROFILE_DEBUG
									 
#define GEOPROFILEDEBUG( TEXT )                     DEBUG( TEXT);
#define GEOPROFILEDEBUG1( TEXT, ARG1 )              DEBUG1( TEXT, ARG1 );
#define GEOPROFILEDEBUG2( TEXT, ARG1, ARG2 )        DEBUG2( TEXT, ARG1, ARG2 );
#define GEOPROFILEDEBUG3( TEXT, ARG1, ARG2, ARG3)   DEBUG3( TEXT, ARG1, ARG2, ARG3 );

#else

#define GEOPROFILEDEBUG( TEXT )                     ;
#define GEOPROFILEDEBUG1( TEXT, ARG1 )              ;
#define GEOPROFILEIDEBUG2( TEXT, ARG1, ARG2 )       ;
#define GEOPROFILEDEBUG3( TEXT, ARG1, ARG2, ARG3 )  ;

#endif

#endif // _GEOPROFILESDEBUG_H_
