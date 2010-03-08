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






package com.nokia.wtk.diagnostics.browser;


import com.nokia.wtk.device.DeviceHTTPLoader;
import com.nokia.wtk.device.DeviceWMLBrowserContextListener;


/**
 * A set of browser context services.
 *
 */
class BrowserContext {

    /**
     * Creates a new <code>BrowserContext</code>.
     * @param b the Browser instance
     */
    BrowserContext(Browser b) {
        browser = b;

        // Register device browser event listeners:
        setDeviceWMLContextListenerSupported(browser.registerHostService(
            DeviceWMLBrowserContextListener.class, getDeviceWMLContextListener()));

        DeviceHTTPLoader loader = (DeviceHTTPLoader)
                                  browser.getDeviceService(DeviceHTTPLoader.class);
    }

    /**
     * Gets the <code>DeviceWMLBrowserContextListener</code> instance.
     * @return the <code>DeviceWMLBrowserContextListener</code> instance.
     */
    private DeviceWMLBrowserContextListener getDeviceWMLContextListener() {
        if (this.deviceWMLContextListener == null) {
            this.deviceWMLContextListener = new DeviceWMLBrowserContextListener() {
                public void contextVariableChanged(String name, String value) {
                    getBrowserVariables().setVariable(name, value);
                }

                public void contextVariablesCleared() {
                    getBrowserVariables().removeAll();
                }

                public void timerUpdate(boolean active, int value) {
                    //TODO
                    //getWMLTimer().setState(active, value);
                }
            };
        }
        return this.deviceWMLContextListener;
    }

    /**
     * Sets the support status for the <code>DeviceWMLContextListener</code> interface.
     * @param supported if <code>true</code>, then the current device supports
     *                  the <code>DeviceWMLContextListener</code> interface
     */
    private void setDeviceWMLContextListenerSupported(boolean supported) {
        this.deviceWMLContextListenerSupported = supported;
    }

    /**
     * Gets the support status for the <code>DeviceWMLContextListener</code> interface.
     * @return <code>true</code> if the current device supports
     *         the <code>DeviceWMLContextListener</code> interface
     */
    boolean isDeviceWMLContextListenerSupported() {
        return this.deviceWMLContextListenerSupported;
    }

    /**
     * Gets the <code>ContentLog</code> instance.
     * @return the <code>ContentLog</code> instance.
     */
    ContentLog getContentLog() {
        if (this.contentLog == null) {
            this.contentLog = new ContentLog(browser);
            // ContentLog needs this in order to dispatch content push events
            this.contentLog.setPushMessages(getPushMessages());
        }
        return this.contentLog;
    }


    /**
     * Gets the <code>PushMessages</code> instance.
     * @return the <code>PushMessages</code> instance.
     */
    PushMessages getPushMessages() {
        if (this.pushMessages == null) {
            this.pushMessages = new PushMessages(browser);
        }
        return this.pushMessages;
    }

    /**
     * Gets the <code>BrowserHistory</code> instance.
     * @return the <code>BrowserHistory</code> instance.
     */
    BrowserHistory getBrowserHistory() {
        if (this.browserHistory == null) {
            this.browserHistory = new BrowserHistory(browser);
            this.browserHistory.setContentLog(getContentLog());
        }
        return this.browserHistory;
    }

    /**
     * Gets the <code>BrowserVariables</code> instance.
     * @return the <code>BrowserVariables</code> instance.
     */
    BrowserVariables getBrowserVariables() {
        if (this.browserVariables == null) {
            this.browserVariables = new BrowserVariables();
        }
        return this.browserVariables;
    }


    /** The shared <code>BrowserContext</code> instance. */
    private static BrowserContext sharedInstance = null;


    private ContentLog contentLog = null;
    private PushMessages pushMessages = null;
    private Browser browser = null;
    private BrowserHistory browserHistory = null;
    private BrowserVariables browserVariables = null;
    private boolean deviceWMLContextListenerSupported = false;
    private DeviceWMLBrowserContextListener deviceWMLContextListener = null;
}
