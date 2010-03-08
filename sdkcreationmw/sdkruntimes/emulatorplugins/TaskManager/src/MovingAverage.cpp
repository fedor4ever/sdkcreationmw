/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for calculating moving average
*
*/



// INCLUDE FILES
#include    "MovingAverage.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMovingAverage<T>::CMovingAverage
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
template <class T> 
CMovingAverage<T>::CMovingAverage( TInt aLen ) : iLen( aLen )
    {
    }
    
// -----------------------------------------------------------------------------
// CMovingAverage<T>::CMovingAverage
// This default constructor without arguments is private so it is never used.
// -----------------------------------------------------------------------------
template <class T>
CMovingAverage<T>::CMovingAverage( )
	{
	}
    
// -----------------------------------------------------------------------------
// CMovingAverage<T>::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
template <class T> 
void CMovingAverage<T>::ConstructL()
    {
	iValues = new (ELeave) T[iLen];
	
	for ( TInt i(0); i<iLen; i++ )
		{
		iValues[i] = 0;
		}
		
	iSum = 0;
	iCurpos = 0;
	iReady = EFalse;
    }

// -----------------------------------------------------------------------------
// CMovingAverage<T>::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
template <class T> 
CMovingAverage<T>* CMovingAverage<T>::NewL( TInt aLen )
    {
    CMovingAverage* self = new( ELeave ) CMovingAverage( aLen );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
template <class T> 
CMovingAverage<T>::~CMovingAverage()
    {
    delete[] iValues;
    }


// -----------------------------------------------------------------------------
// CMovingAverage<T>::Insert
// 
// -----------------------------------------------------------------------------
//
template <class T>
void CMovingAverage<T>::Insert( T aSample )
    {
	iSum -= iValues[ iCurpos ];
	iValues[ iCurpos ] = aSample;
	iSum += aSample;
	iCurpos = ( ++iCurpos ) % iLen;
	
	if ( !iReady && iCurpos == 0 )
		{
		iReady = ETrue;
		}
    }

// -----------------------------------------------------------------------------
// CMovingAverage<T>::Value
// 
// -----------------------------------------------------------------------------
//
template <class T>
T CMovingAverage<T>::Value( )
	{
	if ( iReady )
		{
		return iSum / iLen;
		}
	else
		{
		return iSum / ( iCurpos + 1 );
		}
	}

// Explicit instantion of CMovingAverage for TReal type. Necessary because
// CW does not support C++ export keyword.
template class CMovingAverage<TReal>;
//  End of File  
