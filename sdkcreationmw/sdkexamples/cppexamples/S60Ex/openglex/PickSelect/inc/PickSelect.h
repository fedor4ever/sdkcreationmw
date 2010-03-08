/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDES
#include <e32base.h> // for CBase definition
#include <GLES/gl.h> // OpenGL ES header file

// MACROS
#define POLYGON_SELECTION 1
#define OBJECT_SELECTION  2
#define FRUSTUM_LEFT   -1.f     //left vertical clipping plane
#define FRUSTUM_RIGHT   1.f     //right vertical clipping plane
#define FRUSTUM_BOTTOM -1.f     //bottom horizontal clipping plane
#define FRUSTUM_TOP     1.f     //top horizontal clipping plane
#define FRUSTUM_NEAR    3.f     //near depth clipping plane
#define FRUSTUM_FAR  1000.f     //far depth clipping plane


// CLASS DECLARATION

/**
 * Class that does the actual OpenGL ES rendering.
 */
class CPickSelect : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Factory method for creating a new CPickSelect object.
         */
        static CPickSelect* NewL( TUint aWidth, TUint aHeight);

        /**
         * Destructor. Does nothing.
         */
          virtual ~CPickSelect();

    public: // New functions

        /**
         * Initializes OpenGL ES, sets the vertex and color
         * arrays and pointers etc.
         */
        void AppInit( void );

        /**
         * Called upon application exit. Does nothing.
         */
        void AppExit( void );

        /**
         * Renders one frame.
         * @param aFrame Number of the frame to be rendered.
         */
        void AppCycle( TInt aFrame );

        /**
         * Updates cursor position.
         */
        void UpdateCursorPosition( void );

        /**
         * Draws the cursor.
         */
        void DrawCursor( void );

        /**
         * Draw a single cube and white lines around the selected polygon
         * if one is selected.
         * @param aFrame The number of the frame to be rendered.
         */
        void DrawSingleBox( TInt aFrame );

        /**
         * Draw two rotating boxes.
         * The selected cube, if there is one, is draw in wireframe mode.
         * @param aFrame The number of the frame to be rendered.
         */
        void DrawMultipleBoxes( TInt aFrame );


        /**
         * Sets the move up flag to the given value.
         * @param move True if cursor should move upwards each frame.
         *             False if cursor shouldn't move up.
         */
        void MoveUp( TBool move );

        /**
         * Sets the move down flag to the given value.
         * @param move True if cursor should move downwards each frame.
         *             False if cursor shouldn't move down.
         */
        void MoveDown( TBool move );

        /**
         * Sets the move left flag to the given value.
         * @param move True if cursor should move left each frame.
         *             False if cursor shouldn't move left.
         */
        void MoveLeft( TBool move );

        /**
         * Sets the move right flag to the given value.
         * @param move True if cursor should move right each frame.
         *             False if cursor shouldn't move right.
         */
        void MoveRight( TBool move );

        /**
         * Tries to select the object currently under the cursor.
         * Uses the cursor position to get the current pixel's alpha value and stores
         * it to iSelectionIdx. The iSelectionIdx is then used to check if a polygon or
         * an object is chosen.
         */
        void SelectObject( void );

        /**
         * Changes selection mode to polygon selection.
         */
        void SetPolygonSelection( void );

        /**
         * Changes selection mode to object selection.
         */
        void SetObjectSelection( void );

        /**
         * Notifies that the screen size has dynamically changed during execution of
         * this program. Resets the viewport to this new size.
         * @param aWidth New width of the screen.
         * @param aHeight New height of the screen.
         */
        void SetScreenSize( TUint aWidth, TUint aHeight );

    protected: // New functions

        /**
         * Standard constructor that must never Leave.
         * Stores the given screen width and height.
         * @param aWidth Width of the screen.
         * @param aHeight Height of the screen.
         */
        CPickSelect( TUint aWidth, TUint aHeight );

        /**
         * Second phase contructor. Does nothing.
         */
        void ConstructL( void );

    private: // Data

        /** Width of the screen */
        TUint iScreenWidth;

        /** Height of the screen */
        TUint iScreenHeight;

        /** Current selection mode (either POLYGON_SELECTION or OBJECT_SELECTION). */
        TInt iSelectionMode;

        /** Whether cursor should move up every frame or not. */
        TBool iMoveUp;

        /** Whether cursor should move down every frame or not. */
        TBool iMoveDown;

        /** Whether cursor should move left every frame or not. */
        TBool iMoveLeft;

        /** Whether cursor should move right every frame or not. */
        TBool iMoveRight;

        /** Selection ID. */
        TInt iSelectionIdx;

        /** Current X axis rotation of the cube(s). */
        GLfloat iCubeRotX;

        /** Current Y axis rotation of the cube(s). */
        GLfloat iCubeRotY;

        /** Current X coordinate of the cursor. */
        GLint iCursorPosX;

        /** Current Y coordinate of the cursor. */
        GLint iCursorPosY;


        /** Currently selected pelected polygon. */
        GLbyte iSelectedPoly[3];

        /** Current screen aspect ratio (width/height). */
        GLfloat iAspectRatio;
    };
