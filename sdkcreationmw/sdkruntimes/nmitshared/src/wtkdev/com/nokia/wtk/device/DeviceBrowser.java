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




package com.nokia.wtk.device;


/**
 * An interface to the device browser.
 * This interface must be implemented by a device simulator component,
 * if it is to support general content browsing operations.
 *
 */
public interface DeviceBrowser {

    /**
     * Loads a new document.
     *
     * @param url       the document location.
     * @param nocache   if <code>true</code> then ignore possibly-cached content.
     */
    public void load(String url, boolean nocache);

    /**
     * Reloads the current document, ignoring possibly-cached content.
     * This operation must not change the state of browser history,
     * i.e. no "push" or "pop" events.
     *
     * @exception       UnsupportedOperationException
     *                  if this method is not supported.
     *                  Implementations that do not support this method
     *                  should explicitly declare this exception.
     */
    public void reload();

    /**
     * Navigates back to the previous document in the browser history stack.
     * This operation may or may not result in a
     * {@link DeviceBrowserHistoryListener#historyPopped} notification.
     *
     * @exception       UnsupportedOperationException
     *                  if this method is not supported.
     *                  Implementations that do not support this method
     *                  should explicitly declare this exception.
     */
    public void back();

    /**
     * Navigates forward to the next document in the browser history stack.
     *
     * @exception       UnsupportedOperationException
     *                  if this method is not supported.
     *                  Implementations that do not support this method
     *                  should explicitly declare this exception.
     */
    public void forward();

    /**
     * Halts any current browser activity.
     * (e.g. pending load operation, script execution, timers)
     */
    public void stop();

}   // End of DeviceBrowser
