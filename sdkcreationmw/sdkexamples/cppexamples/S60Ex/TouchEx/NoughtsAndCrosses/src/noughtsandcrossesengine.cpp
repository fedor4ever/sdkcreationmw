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
* Description: 
*
*/



// INCLUDE FILES
#include "noughtsandcrossesengine.h"


// CONSTANTS
#define TIMEWAIT 1000000

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------
// CNoughtsAndCrossesEngine::NewLC(TPlayer aHumanPlayer)
// Two-phased constructor.
// ----------------------------------------------------
//
CNoughtsAndCrossesEngine* CNoughtsAndCrossesEngine::NewLC(TPlayer aHumanPlayer)
	{
	CNoughtsAndCrossesEngine* self = new(ELeave) CNoughtsAndCrossesEngine (
		aHumanPlayer);
	CleanupStack::PushL (self);
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------
// CNoughtsAndCrossesEngine::NewL(TPlayer aHumanPlayer)
// Two-phased constructor.
// ----------------------------------------------------
//	
CNoughtsAndCrossesEngine* CNoughtsAndCrossesEngine::NewL(TPlayer aHumanPlayer)
	{
	CNoughtsAndCrossesEngine* self = CNoughtsAndCrossesEngine::NewLC(
		aHumanPlayer);
	CleanupStack::Pop();
	return self;
	}

// ----------------------------------------------------
// CNoughtsAndCrossesEngine::~CNoughtsAndCrossesEngine()
// destructor
// ----------------------------------------------------
//	
CNoughtsAndCrossesEngine::~CNoughtsAndCrossesEngine()
	{
	Cancel();
	}

// ----------------------------------------------------
// CNoughtsAndCrossesEngine::SetObserver(MObserver* aObserver)
// Set observer
// ----------------------------------------------------
//	
void CNoughtsAndCrossesEngine::SetObserver(MObserver* aObserver)
	{
	iObserver  = aObserver;
	}

// ----------------------------------------------------
// CNoughtsAndCrossesEngine::StartNewGame(TPlayer aHumanPlayer)
// New game
// ----------------------------------------------------
//
void CNoughtsAndCrossesEngine::StartNewGame(TPlayer aHumanPlayer)
	{
	SetFirstMove(ETrue);
	this->iHumanPlayer = aHumanPlayer;
	iPlayerToGo = aHumanPlayer;

	// Determine computer
	if(iHumanPlayer == ENought)
		iComputerPlayer = ECross;
	else
		iComputerPlayer = ENought;

	ClearBoard();
	if (iObserver)
		{
		iObserver->HandleBoardReset();
		}
	}

// ----------------------------------------------------
// CNoughtsAndCrossesEngine::Winner()
// Return true if player wins
// ----------------------------------------------------
//
TBool CNoughtsAndCrossesEngine::Winner()
	{
	TPlayer player;
	CheckForWinner(player);
	if(player == ENone) return EFalse;
	else return ETrue;
	}

// ----------------------------------------------------
// CNoughtsAndCrossesEngine::IsDraw()
// Return EFalse if draw
// ----------------------------------------------------
//
TBool CNoughtsAndCrossesEngine::IsDraw()
	{
	if (Winner()) return EFalse;

	// Checks for blank spaces...
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			if (iBoard[i][j] == ENone) return EFalse;
		}
	}
	return ETrue;
	}

// ----------------------------------------------------
// CNoughtsAndCrossesEngine::CanMove()
// Return ETrue if move is valid
// ----------------------------------------------------
//
TBool CNoughtsAndCrossesEngine::CanMove()
	{
	if (Winner()) return EFalse;

	// Checks for blank spaces...
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			if (iBoard[i][j] == ENone) return ETrue;
		}
	}
	return EFalse;
	}

// ----------------------------------------------------
// CNoughtsAndCrossesEngine::MakeHumanMoveL(TUint aRow, TUint aColumn)
// ----------------------------------------------------
//
TBool CNoughtsAndCrossesEngine::MakeHumanMoveL(TUint aRow, TUint aColumn)
	{
	if( aRow>=BOARD_SIZE    || aRow<0 ||
        aColumn>=BOARD_SIZE || aColumn<0 )
        {
        return EFalse;
        }
	
	if (iBoard[aRow][aColumn] != ENone)
		{
		return EFalse;
		}

	if (iPlayerToGo == iComputerPlayer) return EFalse;

	iBoard[aRow][aColumn] = iHumanPlayer;
	if (iObserver) iObserver->HandleMove (iHumanPlayer, aRow, aColumn);
	iPlayerToGo = iComputerPlayer;

    if (Winner())
		{
		if (iObserver) iObserver->HandleWinnerL(ETrue, 
			winningRowStartIndex, 
			winningColumnStartIndex,
			winningRowEndIndex, 
			winningColumnEndIndex);
		}

	return ETrue;
	}

// ----------------------------------------------------
// CNoughtsAndCrossesEngine::MakeComputerMove()
// ----------------------------------------------------
//
TBool CNoughtsAndCrossesEngine::MakeComputerMove()
	{
	if (!DetermineMove (iComputerMoveRow, iComputerMoveColumn))
		{
		return EFalse;
		}

	if (iObserver && !IsFirstMove())
		{
		After (TIMEWAIT);
		}
	else
		{
		iBoard[iComputerMoveRow][iComputerMoveColumn] = iComputerPlayer;
		}

	return ETrue;
	}

// ----------------------------------------------------
// CNoughtsAndCrossesEngine::Board() const
// ----------------------------------------------------
//
const CNoughtsAndCrossesEngine::TBoard& CNoughtsAndCrossesEngine::Board() const
	{
	return iBoard;
	}

// ----------------------------------------------------
// CNoughtsAndCrossesEngine::RunL()
// CTimer
// ----------------------------------------------------
//
void CNoughtsAndCrossesEngine::RunL()
	{
	iBoard[iComputerMoveRow][iComputerMoveColumn] = iComputerPlayer;
	iObserver->HandleMove (iComputerPlayer, iComputerMoveRow, iComputerMoveColumn);
	iPlayerToGo = iHumanPlayer;

	if (Winner())
		{
		if (iObserver)
			{
			iObserver->HandleWinnerL(EFalse, 
				winningRowStartIndex, 
				winningColumnStartIndex,
				winningRowEndIndex, 
				winningColumnEndIndex);
			}
		}
	}

// ----------------------------------------------------
// CNoughtsAndCrossesEngine::CNoughtsAndCrossesEngine (TPlayer aHumanPlayer)
// Default constructor
// ----------------------------------------------------
//
CNoughtsAndCrossesEngine::CNoughtsAndCrossesEngine (TPlayer aHumanPlayer) :
	CTimer (CActive::EPriorityStandard), iObserver (NULL),
	iHumanPlayer (aHumanPlayer), iPlayerToGo (aHumanPlayer)
	{
	ClearBoard();
	CActiveScheduler::Add (this);
	
	// Determine computer player
	if(iHumanPlayer == ENought)
		iComputerPlayer = ECross;
	else
		iComputerPlayer = ENought;
	}

// ----------------------------------------------------
// CNoughtsAndCrossesEngine::ConstructL()
// Symbian OS default constructor can leave.
// ----------------------------------------------------
//
void CNoughtsAndCrossesEngine::ConstructL()
	{
	CTimer::ConstructL();
	}

// ----------------------------------------------------
// CNoughtsAndCrossesEngine::ClearBoard()
// Clear the board
// ----------------------------------------------------
//	
void CNoughtsAndCrossesEngine::ClearBoard()
	{
	for (TInt i = 0; i < BOARD_SIZE; ++i)
		{
		for (TInt j = 0; j < BOARD_SIZE; ++j)
			{
			iBoard[i][j] = ENone;
			}
		}
	}

// ----------------------------------------------------
// CNoughtsAndCrossesEngine::DetermineMove(TUint& aRow, TUint& aColumn)
// Check is the move valid
// ----------------------------------------------------
//
TBool CNoughtsAndCrossesEngine::DetermineMove (TUint& aRow, TUint& aColumn) const
	{
	for (int i = 0; i < BOARD_SIZE; ++i)
		{
		for (int j = 0; j < BOARD_SIZE; ++j)
			{
			if (iBoard[i][j] == ENone)
				{
				aRow = i;
				aColumn = j;
				return ETrue;
				}
			}
		}
	return EFalse;
	}

// ----------------------------------------------------
// CNoughtsAndCrossesEngine::CheckForWinner(TPlayer& aResult)
// ----------------------------------------------------
//
void CNoughtsAndCrossesEngine::CheckForWinner (TPlayer& aResult)
	{
	int i, j;
	aResult = ENone;

	// Check rows
	for (i = 0; i < BOARD_SIZE; ++i)
		{
		if(iBoard[i][0] != ENone)
			{
			for (j = BOARD_SIZE-1; j > 0; --j)
				{
				if (iBoard[i][j] != iBoard[i][j-1]) break;
				if (j == 1)
					{
					aResult = iBoard[i][j];
					winningRowStartIndex = i;
					winningRowEndIndex = i;
					winningColumnStartIndex = 0;
					winningColumnEndIndex = BOARD_SIZE-1;
					}
				}
			}
		}
		
	// Check columns
	for (j = 0; j < BOARD_SIZE; ++j)
		{
		if(iBoard[0][j] != ENone)
			{
			for (i = BOARD_SIZE-1; i > 0; --i)
				{
				if (iBoard[i][j] != iBoard[i-1][j]) break;
				if (i == 1)
					{
					aResult = iBoard[i][j];
					winningRowStartIndex = 0;
					winningRowEndIndex = BOARD_SIZE-1;
					winningColumnStartIndex = j;
					winningColumnEndIndex = j;
					}
				}
			}
		}

	// Check diagonals
	for (i = BOARD_SIZE-1; i > 0; --i)
		{
		if(iBoard[0][0] != ENone)
			{
			if (iBoard[i][i] != iBoard[i-1][i-1]) break;
			if (i == 1)
				{
				aResult = iBoard[i][i];
				winningRowStartIndex = 0;
				winningRowEndIndex = BOARD_SIZE-1;
				winningColumnStartIndex = 0;
				winningColumnEndIndex = BOARD_SIZE-1;
				}
			}
		}

	for (i = BOARD_SIZE-1; i > 0; --i)
		{
		if(iBoard[BOARD_SIZE-1][0] != ENone)
			{
			if (iBoard[i][(BOARD_SIZE-i)-1] != iBoard[i-1][BOARD_SIZE-i]) break;
			if (i == 1)
				{
				aResult = iBoard[BOARD_SIZE-1][0];
				winningRowStartIndex = BOARD_SIZE-1;
				winningRowEndIndex = 0;
				winningColumnStartIndex = 0;
				winningColumnEndIndex = BOARD_SIZE-1;
				}
			}
		}
    }

// ----------------------------------------------------
// CNoughtsAndCrossesEngine::SetFirstMove(TBool aFirstMove)
// ----------------------------------------------------
//
void CNoughtsAndCrossesEngine::SetFirstMove(TBool aFirstMove)
    {
    iFirstMove  = aFirstMove;
    }

// ----------------------------------------------------
// CNoughtsAndCrossesEngine::IsFirstMove()
// ----------------------------------------------------
//
TBool CNoughtsAndCrossesEngine::IsFirstMove()
	{
	return iFirstMove;
	}

// End of File
