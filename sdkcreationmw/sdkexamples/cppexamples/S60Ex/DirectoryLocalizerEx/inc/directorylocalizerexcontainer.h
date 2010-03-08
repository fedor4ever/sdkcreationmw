/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Application View class, CDirectoryLocalizerExContainer -
*                shows the application state on the screen
*
*/

#ifndef BCTEST_TEMPLATECONTAINER_H
#define BCTEST_TEMPLATECONTAINER_H

#include <coecntrl.h>
#include <CDirectoryLocalizer.h>
#include <eikedwin.h>
#include <f32file.h>
#include <w32std.h>


/**
* container class
*/
class CDirectoryLocalizerExContainer: public CCoeControl
    {
public: // constructor and destructor

    /**
     * C++ default constructor.
     */
    CDirectoryLocalizerExContainer();

    /**
     * Destructor.
     */
    virtual ~CDirectoryLocalizerExContainer();

    /**
     * Symbian 2nd constructor.
     */
    void ConstructL( const TRect& aRect );

public: // new functions

    /**
    * Set component control, and container will own the control.
    * @param aControl pointer to a control.
    */
    void SetControlL( CCoeControl* aControl );

    /**
     * Delete control.
     */
    void ResetControl();

    /**
     * Create a CDirectoryLocalizer instance, and add entries using RSS Id,user can
     * use SetFullPath() function by inputting a existing path as param,then the user
     * can get the localized name.
     */
    void LocalizedFromResIdL();

    /**
     * Using a ResourceReader to read directory entries from RSS file, and Construct a
     * CDirectoryLocalizer from ResourceReader, user can use SetFullPath() function by
     * inputting a existing path as param, then the user can get the localized name.
     */
    void SetupFromResRdL();

    /**
     * Construct a CDirectoryLocalizer instance from ResourceId, user can use SetFullPath()
     * function by inputting a existing path as param, then the user can get the localized
     * name.
     */
    void SetupFromResIdL();

    /**
     * User can use the ExtraData() function after localized succefully to get the extra data from
     * resource structrue.
     */
    void GetExtraDataL();

    /**
     * User can use the Icon() function after localized succefully to get the extra data from
     * resource structrue.
     */
    void GetIconIndexL();



public: // from CCoeControl

    /**
     * Return count of component controls.
     */
    TInt CountComponentControls() const;

    /**
     * Return pointer to component control specified by index.
     * @param aIndex a index to specify a component control.
     */
    CCoeControl* ComponentControl( TInt aIndex ) const;

private: // from CCoeControl

    /**
    * From CCoeControl, Draw.
    * Fills the window's rectangle.
    * @param aRect Region of the control to be (re)drawn.
    */
    void Draw( const TRect& aRect ) const;

private: // data

    /**
     * Pointer to component control.
     * Own.
     */
    CCoeControl* iControl;

    };

#endif // BCTEST_TEMPLATECONTAINER_H
