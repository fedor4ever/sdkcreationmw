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

import java.lang.reflect.Method;

import com.nokia.wtk.device.Device;
import com.nokia.wtk.device.DeviceBrowser;
import com.nokia.wtk.device.DeviceContentReceived;
import com.nokia.wtk.device.DeviceFileSystem;
import com.nokia.wtk.device.DeviceHTTPLoader;
import com.nokia.wtk.device.DeviceWMLBrowser;

/**
 * Browser-related services for the currently-selected device simulator.
 *
 */
public class Browser {

    /**
     * Creates a new <code>Browser</code>.
     */
    public Browser(Device d) {
        if (d == null)
            throw new NullPointerException("device");
        device = d;
        init();
    }

    /**
     * Initializes this <code>Browser</code> instance.
     */
    private void init() {
        // Check for WML browser first:
        deviceBrowser =
            (DeviceBrowser) getDeviceService(DeviceWMLBrowser.class);
        if (deviceBrowser == null) {
            // ... else check for generic browser:
            deviceBrowser =
                (DeviceBrowser) getDeviceService(DeviceBrowser.class);
        }
        deviceLoader =
            (DeviceHTTPLoader) getDeviceService(DeviceHTTPLoader.class);
        deviceFilesystem =
            (DeviceFileSystem) getDeviceService(DeviceFileSystem.class);
        // Notify browser context of its new WML browser, if any:
        getBrowserVariables().setWMLBrowser(getWMLBrowser());
    }

    /**
     * Gets a device service from the current device.
     * @param type the service type
     * @return a device service instance for the current device.
     */
    public Object getDeviceService(Class type) {
        return device.getInstanceOf(type);
    }

    /**
     * Registers a host service.
     * @param type the service type
     * @param service the service instance or <code>null</code>.
     * @return  <code>true</code> if device supports it, otherwise
     *          <code>false</code>
     */
    public boolean registerHostService(Class type, Object service) {
        return device.setInstanceOf(type, service);
    }

    /**
     * Gets the <code>BrowserContext</code> instance.
     * @return          the <code>BrowserContext</code> instance.
     */
    private BrowserContext getBrowserContext() {
        if (this.browserContext == null) {
            this.browserContext = new BrowserContext(this);
        }
        return this.browserContext;
    }

    /**
     * Gets the <code>ContentLog</code> instance.
     * @return          the <code>ContentLog</code> instance.
     */
    public ContentLog getContentLog() {
        return getBrowserContext().getContentLog();
    }

    /**
     * Gets the <code>PushMessages</code> instance.
     * @return          the <code>PushMessages</code> instance.
     */
    public PushMessages getPushMessages() {
        return getBrowserContext().getPushMessages();
    }

    /**
     * Gets the <code>BrowserHistory</code> instance.
     * @return          the <code>BrowserHistory</code> instance.
     */
    public BrowserHistory getBrowserHistory() {
        return getBrowserContext().getBrowserHistory();
    }

    /**
     * Gets the <code>BrowserVariables</code> instance.
     * @return          the <code>BrowserVariables</code> instance.
     */
    public BrowserVariables getBrowserVariables() {
        return getBrowserContext().getBrowserVariables();
    }

    /**
     * Gets the current DeviceBrowser.
     * @return          the current DeviceBrowser or <code>null</code>.
     */
    private DeviceBrowser getDeviceBrowser() {
        return this.deviceBrowser;
    }

    /**
     * Checks whether the current device browser service is defined.
     * @return          <code>true</code> if the current device browser service is defined.
     */
    public boolean isBrowserSupported() {
        return (getDeviceBrowser() != null);
    }

    /**
     * Checks whether the current device browser supports WML browser features.
     * @return          <code>true</code> if the current device browser is WML.
     */
    public boolean isWMLBrowserSupported() {
        return (getDeviceBrowser() instanceof DeviceWMLBrowser);
    }

    /**
     * Gets the current <code>DeviceWMLBrowser</code> if applicable.
     * @return          the current <code>DeviceWMLBrowser</code> or <code>null</code>.
     */
    private DeviceWMLBrowser getWMLBrowser() {
        return isWMLBrowserSupported()
            ? (DeviceWMLBrowser) getDeviceBrowser()
            : null;
    }

    /**
      * Checks whether the current device browser supports a DeviceFileSystem.
      * @return          <code>true</code> if the current device browser
      * exports a DeviceFileSystem area.
      */
    public boolean isDeviceFileSystemSupported() {
        return (getDeviceFileSystem() != null);
    }

    /**
     * Gets the current <code>DeviceFileSystem</code> if applicable.
     * @return          the current <code>DeviceFileSystem</code> or <code>null</code>.
     */
    public DeviceFileSystem getDeviceFileSystem() {
        return deviceFilesystem;
    }

    /**
     * Gets the current <code>DeviceHTTPLoader</code> if applicable.
     * @return          the current <code>DeviceHTTPLoader</code> or <code>null</code>.
     */
    private DeviceHTTPLoader getLoader() {
        return this.deviceLoader;
    }

    /**
     * @returns <code>true</code> if the current device supports the browser load operation.
     */
    public boolean isLoadSupported() {
        return Browser.isMethodSupported(
            getDeviceBrowser(),
            "load",
            new Class[] { String.class, Boolean.TYPE });
    }

    /**
     * Loads a new document.
     *
     * @param url       the document location.
     * @param nocache   if <code>true</code> then ignore possibly-cached content.
     */
    public synchronized void load(String url, boolean nocache) {
        if ((url != null) && isLoadSupported()) {
            try {
                getDeviceBrowser().load(url, nocache);
            } catch (UnsupportedOperationException ex) {}
        }
    }

    /**
     * Loads a new document.
     *
     * @param url       the document location.
     */
    public void load(String url) {
        load(url, true);
    }

    /**
     * @returns <code>true</code> if the current device supports the browser reload operation.
     */
    public boolean isReloadSupported() {
        return Browser.isMethodSupported(getDeviceBrowser(), "reload");
    }

    /**
     * Reloads the current document, ignoring possibly-cached content.
     */
    public synchronized void reload() {
        if (isReloadSupported()) {
            try {
                getDeviceBrowser().reload();
            } catch (UnsupportedOperationException ex) {}
        }
    }

    /**
     * @returns <code>true</code> if the current device supports the loader push operation.
     */
    public boolean isPushMessageSupported() {
        return Browser.isMethodSupported(
            getLoader(),
            "pushContent",
            new Class[] { DeviceContentReceived.class });
    }

    /**
     * Pushes a message onto the currently selected device.
     * This will inject push content into the device loader,
     * with the same effect as if content had been pushed
     * onto the device in the normal fashion.
     * @param message   the message content to be pushed.
     */
    public void pushMessage(DeviceContentReceived message) {
        if (isPushMessageSupported()) {
            try {
                getLoader().pushContent(message);
            } catch (UnsupportedOperationException ex) {}
        }
    }

    /**
     * @returns <code>true</code> if the current device supports the browser back operation.
     */
    public boolean isBackSupported() {
        return Browser.isMethodSupported(getDeviceBrowser(), "back");
    }

    /**
     * Navigates back to the previous document in the browser history stack.
     * This operation may or may not result in a {@link DeviceBrowserListener#historyPopped} event.
     *
     * @exception       UnsupportedOperationException
     *                  if this method is not supported.
     */
    public synchronized void back() {
        if (isBackSupported()) {
            try {
                getDeviceBrowser().back();
            } catch (UnsupportedOperationException ex) {}
        }
    }

    /**
     * @returns <code>true</code> if the current device supports the browser forward operation.
     */
    public boolean isForwardSupported() {
        return Browser.isMethodSupported(getDeviceBrowser(), "forward");
    }

    /**
     * Navigates forward to the next document in the browser history stack.
     */
    public synchronized void forward() {
        if (isForwardSupported()) {
            try {
                getDeviceBrowser().forward();
            } catch (UnsupportedOperationException ex) {}
        }
    }

    /**
     * @returns <code>true</code> if the current device supports the browser stop operation.
     */
    public boolean isStopSupported() {
        return Browser.isMethodSupported(getDeviceBrowser(), "stop");
    }

    /**
     * Halts any current browser activity.
     * (e.g. pending load operation, script execution, timers)
     */
    public synchronized void stop() {
        if (isStopSupported()) {
            try {
                getDeviceBrowser().stop();
            } catch (UnsupportedOperationException ex) {}
        }
    }

    /**
     * @returns <code>true</code> if the current device supports the WML browser refresh operation.
     */
    public boolean isRefreshSupported() {
        return Browser.isMethodSupported(getWMLBrowser(), "refresh");
    }

    /**
     * Redisplays the current document using possibly updated WML browser context.
     */
    public synchronized void refresh() {
        if (isRefreshSupported()) {
            try {
                getWMLBrowser().refresh();
            } catch (UnsupportedOperationException ex) {}
        }
    }

    /**
     * @returns <code>true</code> if the current device supports the WML browser newcontext operation.
     */
    public boolean isNewContextSupported() {
        return Browser.isMethodSupported(getWMLBrowser(), "newContext");
    }

    /**
     * Clears the current WML browser context.
     */
    public synchronized void newContext() {
        if (isNewContextSupported()) {
            try {
                getWMLBrowser().newContext();
            } catch (UnsupportedOperationException ex) {}
        }
    }

    /**
     * Gets the support status for the <code>DeviceWMLContextListener</code> interface.
     * @return <code>true</code> if the current device supports
     *         the <code>DeviceWMLContextListener</code> interface
     */
    public boolean isDeviceWMLContextListenerSupported() {
        return getBrowserContext().isDeviceWMLContextListenerSupported();
    }

    /**
     * Checks whether a service method is supported.
     * Specifically, this utility checks whether a service is valid,
     * a method is defined and an <code>UnsupportedOperationException</code>
     * is NOT a declared exception.
     * @param service the service instance (may be <code>null</code>)
     * @param methodName the method name
     * @param methodParams the method parameter types (may be <code>null</code>)
     * @returns <code>true</code> if a service method is supported.
     */
    //TODO: Move this method to another service class for general "Current Device Services".
    static boolean isMethodSupported(
        Object service,
        String methodName,
        Class[] methodParams) {
        boolean methodSupported = false;
        if ((service != null) && (methodName != null)) {
            try {
                if (methodParams == null) {
                    methodParams = new Class[] {};
                }
                Method m =
                    service.getClass().getMethod(methodName, methodParams);
                if (m != null) {
                    Class[] exceptions = m.getExceptionTypes();
                    methodSupported = true;
                    for (int i = 0;
                        methodSupported && (i < exceptions.length);
                        i++) {
                        if (UnsupportedOperationException
                            .class
                            .isAssignableFrom(exceptions[i])) {
                            methodSupported = false;
                        }
                    }
                }
            } catch (NoSuchMethodException ex) {}
        }
        return methodSupported;
    }

    /**
     * Checks whether a service method is supported.
     * Specifically, this utility checks whether a service is valid,
     * a method is defined and an <code>UnsupportedOperationException</code>
     * is NOT a declared exception.
     * @param service the service instance (may be <code>null</code>)
     * @param methodName the method name
     * @returns <code>true</code> if a service method is supported.
     */
    //TODO: Move this method to another service class for general "Current Device Services".
    static boolean isMethodSupported(Object service, String methodName) {
        return isMethodSupported(service, methodName, new Class[] {});
    }

    /**
     * Creates dummy Browser object for unit testing
     */
    public static Browser createDummyBrowser() {
        return new Browser(new Device() {
            public void openDevice() {}
            public void closeDevice() {}
            public Object getInstanceOf(Class c) {
                return null;
            }
            public boolean setInstanceOf(Class c, Object o) {
                return false;
            }
        });
    }

    private Device device;
    private DeviceBrowser deviceBrowser;
    private DeviceHTTPLoader deviceLoader;
    private BrowserContext browserContext;
    private DeviceFileSystem deviceFilesystem;
} // End of Browser
