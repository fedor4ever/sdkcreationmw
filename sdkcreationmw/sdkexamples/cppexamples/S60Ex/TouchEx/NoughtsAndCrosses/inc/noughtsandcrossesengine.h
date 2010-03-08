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



#ifndef __NOGUTHSANDCROSSESENGINE_H__
#define __NOGUTHSANDCROSSESENGINE_H__

// INCLUDES
#include <e32base.h>


// CONSTANTS
#define BOARD_SIZE 3


// CLASS DECLARATION
class CNoughtsAndCrossesEngine : public CTimer
	{
	public:
		enum TPlayer
			{
			ENone = -1,
			ENought,
			ECross
			};

	public:
		typedef TFixedArray<TPlayer, BOARD_SIZE> TCellContainer;
		typedef TFixedArray<TCellContainer, BOARD_SIZE> TBoard;

	public:
		class MObserver
			{
			public:
				virtual void HandleMove (TPlayer aPlayer, TUint aRow, 
					TUint aColumn) = 0;

				virtual void HandleWinnerL(TBool aHumanWins, TUint aXRow,
					TUint aXColumn, TUint aZRow, TUint aZColumn) = 0;

				virtual void HandleBoardReset() = 0;
		};

	public:
		static CNoughtsAndCrossesEngine* NewLC (TPlayer aHumanPlayer = ECross);
		/**
 		 * Two-phased constructor.
 		 */	
		static CNoughtsAndCrossesEngine* NewL (TPlayer aHumanPlayer = ECross);
		/**
		 * destructor
		 */
		virtual ~CNoughtsAndCrossesEngine();

	public:
	    /**
         * SetObserver.
         * @param aObserver.
         */
		void SetObserver (MObserver* aObserver);
		/**
		 * NewGame
		 */
		void StartNewGame (TPlayer aHumanPlayer);
		/**
		 * FirstMove
		 */
		void SetFirstMove(TBool aFirstMove);
		/**
		 * IsFirstMove
		 * Return ETrue if first move
		 */	
		TBool IsFirstMove();
		/**
		 * Winner()
		 * Return ETRue if player wins
		 */
		TBool Winner();
		/**
		 * IsDraw()
		 * Return EFalse if draw
		 */
		TBool IsDraw();
		/**
		 * CanMove()
		 * Return ETrue if move is valid
		 */
		TBool CanMove();
		/**
		 * MakeHumanMoveL(TUint aRow, TUint aColumn)
		 */
		TBool MakeHumanMoveL(TUint aRow, TUint aColumn);
		/**
		 * MakeComputerMove()
		 */
		TBool MakeComputerMove();
		/**
		 * Board()
		 * Return @
		 */
		const TBoard& Board() const;

	protected: // CTimer
		virtual void RunL();

	private:
		/**
		 * Default constructor
		 */
		CNoughtsAndCrossesEngine (TPlayer aHumanPlayer);
        /**
        * ConstructL
        * 2nd phase constructor.
        */
		void ConstructL();
		/**
		 * ClearBoard()
		 * Clear the board
		 */
		void ClearBoard();
		/**
		 * DetermineMove (TUint& aRow, TUint& aColumn)
		 * Check is the move valid
		 */
		TBool DetermineMove (TUint& aRow, TUint& aColumn) const;
		/**
		 * CheckForWinner(TPlayer& aResult)
		 */
		void CheckForWinner (TPlayer& aResult);

	private:
		TBoard iBoard;
		MObserver* iObserver;
		TPlayer iHumanPlayer;
		TPlayer iComputerPlayer;
		TPlayer iPlayerToGo;
		TUint iComputerMoveRow;
		TUint iComputerMoveColumn;
		TBool iFirstMove;

		TUint winningRowStartIndex;
		TUint winningRowEndIndex;
		TUint winningColumnStartIndex;
		TUint winningColumnEndIndex;
	};

#endif // __NOGUTHSANDCROSSESENGINE_H__

// End of File
