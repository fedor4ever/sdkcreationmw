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

package com.nokia.epdt.core.action;
    
/**
 * ActionInterface
 */
public interface ActionInterface {
    
    /**
     * Sets the plugin's ActionListener.
     * Setting the action listener to null is the equivalent of unsetting
     * the action listener for the plugin.
     * @param l the listener to which the actions are to be delivered
     * @param uid the unique identifier
     */
    public void setActionListener(ActionListener l, Integer uid, int type);
    
    /**
     * Gets the plugin's ActionDispatcher.
     * @return the dispatcher for the plugin
     */
    public ActionEventQueue getActionEventQueue();
    
}
