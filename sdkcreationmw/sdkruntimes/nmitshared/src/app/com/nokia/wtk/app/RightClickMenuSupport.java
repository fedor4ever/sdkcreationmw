/*
* Copyright (c) 2000 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.wtk.app;

/**
 * Title:        Nokia Mobile Internet Toolkit
 * Description:  Every element in the application user interface that wants to support
 * a right-click menu has to implement this interface. Whenever an
 * object is selected that supports this interface, the right-click
 * menu will be instantiated with the objects called from this interface.
 * Company:      Nokia
 * @version 1.0
 */

import java.awt.event.MouseEvent;
/**
 * Every element in the application user interface that wants to support
 * a right-click menu has to implement this interface. Whenever an
 * object is selected that supports this interface, the right-click
 * menu will be instantiated with the objects called from this interface.
 */
public interface RightClickMenuSupport {

    /**
     * This call will be made by the system when it wants to appropriatly
     * fill a right-click menu and show it. If it is appropriate (say,
     * if the implementing object is a node of a tree), perhaps the
     * implementing object should select itself. This is not manadatory;
     * if the implementing object is a text-panel, for instance, no part
     * of the selection should change. This list of actions should always
     * be consistent for the kind of object. If there is a state-change
     * that would render some actions unusuable, set the actions as
     * disabled instead of not putting them in the list.
     *
     * @param e The MouseEvent that will bring up the right-click menu.
     * @return An array of actions that will be put in the menu.
     *         The actions will be added in order to the menu,
     *         the one in array-position 0 at the top. If an element
     *         in the array is null, a separator will be put in the
     *         menu instead.
     */
    public Object[] getRightClickActions(MouseEvent e);
}
