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





package com.nokia.wtk.diagnostics.debug;

import java.awt.Point;
import java.util.Properties;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;

import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.app.ApplicationWindow;
import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.device.Device;
import com.nokia.wtk.device.DeviceWindowProperties;
import com.nokia.wtk.diagnostics.gui.MainWindow;
import com.nokia.wtk.diagnostics.views.AbstractViewFactoryInterface;
import com.nokia.wtk.util.PropertySet;

/**
 * The main application class for the NMIT DebugViews
 */
public class DebugViews extends AppEnvironment {

    // Window properties
    private static final String WindowProperties = "window.";

    // Private member variables:
    private ApplicationWindow applicationWindow;
    private MainWindow mainWindow;

    /**
     * Creates <code>DebugViews</code>. Note that this constructor does NOT
     * make the application window visible because the caller may need to do
     * something after creating <code>DebugViews</code> but before showing
     * the application window.
     *
     * @param device the device to attach to
     * @param props  the debug views property set
     */
    public DebugViews(Device device, final PropertySet props) {

        // Initialize AppEnvironment
        AppEnvironment.appEnvironmentImplementation = this;
        setRightClickMenuEnabled(true);
        setResourceFileName("resources/Diagnostics.properties");
        try {
            UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
        } catch (UnsupportedLookAndFeelException u) {} catch (IllegalAccessException i) {} catch (InstantiationException in) {} catch (ClassNotFoundException c) {}

        // Create Main window
        PropertySet windowProps = props.createPropertySet(WindowProperties);
        Browser browser = new Browser(device);

        // This setInstanceOf call will probably be ignored by Series40
        // disagnostics, but it's necessary in Series60 world. It shouldn't
        // harm Series40.
        device.setInstanceOf(Browser.class, browser);
        mainWindow = new MainWindow(browser, windowProps);
        applicationWindow = mainWindow.getApplicationWindow();

        props
            .addPropertyChangeListener(
                DeviceWindowProperties.DeviceWindowLocation,
                new PropertyChangeListener() {
            public void propertyChange(PropertyChangeEvent ev) {
                Point location =
                    (Point) props.getValue(
                        DeviceWindowProperties.DeviceWindowLocation);
                DebugViews.this.moveWindow(location);
            }
        });
        // synchronize the titles
        props
            .addPropertyChangeListener(
                DeviceWindowProperties.DeviceWindowTitle,
                new PropertyChangeListener() {
            public void propertyChange(PropertyChangeEvent ev) {
                String s = (String) ev.getNewValue();
                if (s != null) {
                    DebugViews.this.getMainWindow().setWindowTitle(s);
                }
            }
        });
        Point location =
            (Point) props.getValue(DeviceWindowProperties.DeviceWindowLocation);
        String s =
            (String) props.getValue(DeviceWindowProperties.DeviceWindowTitle);
        moveWindow(location);
        if (s != null) {
            DebugViews.this.getMainWindow().setWindowTitle(s);
        }
    }

    /**
     * Moves the main window depending on the location and size of the device
     * parameters passed.
     * @param deviceLocation the location of the device
     */
    private void moveWindow(Point deviceLocation) {
        if (deviceLocation != null) {
          ApplicationWindow mWindow = getMainWindow();
            mWindow.setWindowLocation(new Point(
                (int) (deviceLocation.getX() - getMainWindow().getFrame().getWidth()),
                (int) deviceLocation.getY()));
            // there seems to be a Java bug here: for some reason, when
            // the diagnostics window is iconified (minimized) and then hidden,
            // the icon on the taskbar appears again if setPosition() is invoked
            // (this happens when the SDK main window is moved, so the diagnostics
            // window will follow). The icon reappears in spite of the window being
            // hidden (setVisible(false)).
            // This is an unstable state and causes a grayed-out diagnostics window
            // to be displayed when the user presses on the diagnostics window icon.
            // The way around to fix this is in the following lines of code:
            // 1) Set the visible property to true.
            // 2) change the window state from ICONIFIED to NORMAL.
            // 3) set back the visible property to false.
            // It works!!!

            if (mWindow.getFrame().isVisible() == false &&
                mWindow.getFrame().getExtendedState() == java.awt.Frame.ICONIFIED) {
              mWindow.getFrame().setVisible(true);
              mWindow.getFrame().setExtendedState (java.awt.Frame.NORMAL);
              mWindow.getFrame().setVisible(false);
            }
        }
    }

    /**
     * Return the main application window.
     * @return the main ApplicationWindow.
     */
    public ApplicationWindow getMainWindow() {
        return applicationWindow;
    }

    /**
     * Adds a view to the diagnostics main window
     * @param viewFactory the view factory interface
     */
    public void addView(AbstractViewFactoryInterface viewFactory) {
        mainWindow.addView(viewFactory);
    }

    /**
     * Creates views from the property file in the old diagnostics format
     * @param p the properties
     */
    public void addViewsFromProperties(Properties p) {
        mainWindow.addViewsFromProperties(p);
    }
}
