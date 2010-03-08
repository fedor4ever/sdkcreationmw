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
 * Description:  A class to define application-wide services like filing
 * and displaying dialogs. Abstract, must be implemented by the starting
 * parts of an application. Almost all calls for access to a subsystem
 * are specified to allow retuning null in case a subsystme like a FilingManager
 * or HelpSystem is not appropriate for this class. Classes using these calls
 * should make sure to check return values before dispatching methods to
 * these objects.
 * Company:      Nokia
 * @version 1.0
 */

/* java.util */
import java.awt.Window;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.Map;
import java.util.Properties;
import java.util.Vector;

import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JMenuBar;
import javax.swing.KeyStroke;
import javax.swing.SwingUtilities;

import com.nokia.wtk.util.Build;
import com.nokia.wtk.util.MessageWriters;
import com.nokia.wtk.util.PropertyEditors;
import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.Trace;

abstract public class AppEnvironment implements Resource, MessageWriters {

    static {
        // Make sure all astandard icons and stylers are ready
        ActionsUtilities.init();
        addResources(
            "resources/AppEnvironment.properties",
            AppEnvironment.class);
    }

    /**
     * Central variable for implementation. Extending classes should register
     * the single implementation to use application-wide right here.
     */
    static protected AppEnvironment appEnvironmentImplementation = null;

    /**
     * PropertySet to keep application-wide properties like text and icons used
     * by many subsystems.
     * @see getAppResources
     */
    static private Properties appProperties;

    static protected String filename;

    /**
     * Vector of pairs {Class, Properties }.
     * Position 0 is the main property set.
     * @see #addResources
     * @see #getOtherResources
     */
    static private Vector properties;

    /**
     * Property set used for user properties
     */
    static protected PropertySet userProperties;

    /**
     * Add a set of properties to the central map of properties. Some properties
     * added may be ignored or superceded by properties in the main property set
     * of the object implementing AppEnvironment in order to create different
     * versions of the Toolkit easily. After adding their properties here, packages
     * should get their properties by using getString, getURL, and getIcon
     * from this package.
     */
    public static void addResources(Properties set, Class theClass) {
        Object[] toAdd = { theClass, set };
        getOtherResources().add(0, toAdd);
        // Assumption: resources are added just before they get called in
        // constructing stuff. Therefore putting them at the beginning makes
        // sense
    }
    /**
     * Add a set of properties to the central map of properties. Some properties
     * added may be ignored or superceded by properties in order to create different
     * versions of the Toolkit easily. After adding their properties here, packages
     * should get their properties by using getString, getURL, and getIcon
     * from this package.
     * @param class the class adding the properties. Necessary for error messages
     * @param classLoaderResource the name of the resource to be fetched by the
     * ClassLoader that contains the properties. Will be used first to localize
     * the resource, if null the object returned by getAppEnvironment will be used.
     * Will be used as class object for Class.getResource(key) call
     */
    public static void addResources(
        String classLoaderResource,
        Class theClass) {
        // Handle resources
        Properties props = new Properties();

        try {
            if (theClass != null) {
                props.load(theClass.getResourceAsStream(classLoaderResource));
            } else {
                props.load(
                    getAppEnvironment().getClass().getResourceAsStream(
                        classLoaderResource));
            }
        } catch (Exception io) {
            MessageWriters.DebugMsg.println(// message can be in English,
            theClass.getName()
                + " properties load failure: "
                + io.getMessage());
        }
        addResources(props, theClass);
    }

    static public AppEnvironment getAppEnvironment() {
        return appEnvironmentImplementation;
    }

    /**
     *  // static properties necessary for this class
            in = AppEnvironment.class.
                    getResourceAsStream("resources/AppEnvironment.properties");
            if (in != null) {
                try {
                    Properties staticProperties = new Properties();
                    staticProperties.load(in);
                    appProperties.putAll(staticProperties);
                } catch (IOException io) {
                    io.printStackTrace();
                }
            }
     */
    /**
     * Gets the Properties exposed by the implementing AppEnvironment class,
     * based on the protected variable resourceFilename.
     * @return the PropertySet containing those exposed properties.
     */
    protected static Properties getAppResources() {
        if (appProperties == null) {
            // Create the Toolkit preferences:
            appProperties = new Properties();

        }
        return appProperties;
    }

    /**
     * Get a resource boolean value from the 'properties' file of the implementing
     * class.
     * @param key the name of the resource
     * @param defaultValue the default value to return if the resource is non-existent
     */
    static public boolean getBoolean(String key, boolean defaultValue) {
        String retVal =
            getString(
                key,
                defaultValue
                    ? Boolean.TRUE.toString()
                    : Boolean.FALSE.toString());
        return Boolean.valueOf(retVal).booleanValue();

    }

    static public String getExpiry() {
        return getString(appVersionExpiryDate);
    }

    static public String getBuildDate() {
        return getString(appBuildDate);
    }

    /**
     * Get a resource icon value from the property sets.
     * @param key the name of the resource
     */
    public static ImageIcon getIcon(String key) {
        URL url = null;
        // first check locally to the implementing class, i.e. the file set in
        // resourceFileName
        String val = getAppResources().getProperty(key);
        if (val != null) {
            // So we have a key defined in local implementing class. Find the
            // resource relative to the local implementing class, then
            url = getAppEnvironment().getClass().getResource(val);
        } else {
            // The key was not defined by the implementing class. Maybe it is
            // in one of the participating classes. Look at the properties
            // added by AppEnvironment.addResource() calls
            Object[] pair = getStringFromOtherProperties(key);
            if (pair != null) {
                // A property set (pair[1]) and its owning class (pair[0]) have
                // been found that defines the key. Get the URL resource local
                // to the class associated with the property set
                url = ((Class) pair[0]).getResource((String) pair[1]);
            }
        }
        // Not found in local property set, not found in all other added property
        // sets, just try it randomly in the system, then
        // if (url == null) {  // be more generic
        //    url = ClassLoader.getSystemResource(val);
        // }
        if (url == null) {
            // give up, use hardcoded default
            url =
                getAppEnvironment().getClass().getResource(
                    "resources/Help16.gif");
        }
        return (url != null) ? new ImageIcon(url) : null;
    }

    static public int getInt(String key) {
        String intVal = getString(key);
        return Integer.parseInt(intVal);
    }

    static public int getInt(String key, int defVal) {
        String intVal = getString(key);
        if (intVal == null) {
            return defVal;
        }
        try {
            return Integer.parseInt(intVal);
        } catch (Exception e) {}
        return defVal;
    }

    /**
     * Get a keystroke value from the 'properties' file.
     * @param key the name of the resource
     */
    public static KeyStroke getKeyStroke(String key) {
        String s = getString(key);
        return ((s != null) ? PropertyEditors.decodeKeyStroke(s) : null);
    }

    /**
     * Gets the set of resources to be exposed by this class.
     * @return the PropertySet containing those exposed properties.
     */
    protected static Vector getOtherResources() {
        if (properties == null) {
            // Create the Toolkit preferences:
            properties = new Vector();
        }
        return properties;
    }

    /**
     * Get a resource string value from the 'properties' file of the implementing
     * class
     * @param key the name of the resource
     * @return the String value of the property
     */
    public static String getString(String key) {
        String ret = getAppResources().getProperty(key);
        if (ret == null) {
            Object[] pair = getStringFromOtherProperties(key);
            if (pair != null) {
                return (String) pair[1];
            }
        }
        return ret;
    }
    /**
     * Get a resource string value from the 'properties' file of the implementing
     * class
     * @param key the name of the resource
     * @param defaultVakue the value returned if no value specified
     * @return the String value of the property
     */
    public static String getString(String key, String defaultValue) {
        String ret = getString(key);
        return (ret == null ? defaultValue : ret);
    }

    /**
     * Check vector of Class/Properties tuples for appearance of a value
     * for the key.
     * @param the key to look for
     * @returns a pair { class, returnValue } or null if nothing found.
     */
    static private Object[] getStringFromOtherProperties(String key) {
        Enumeration e = getOtherResources().elements();
        String ret = null;
        while (e.hasMoreElements()) {
            Object[] tuple = (Object[]) e.nextElement();
            ret = ((Properties) tuple[1]).getProperty(key);
            if (ret != null) {
                Object[] et = { tuple[0], ret };
                return et;
            }
        }
        return null;
    }

    static public String getTitle() {
        return getString(appTitle);
    }

    /**
     * Returns the PropertySet for all user properties and preferences.
     * Implementing classes shoulkd initialize this early.
     */
    public static PropertySet getUserProperties() {
        if (userProperties == null) {
            userProperties = new PropertySet();
        }
        return userProperties;

    }
    static public String getVersion() {
        return getString(appVersion);
    }

    private boolean aboutDialogIsRunning = false;
    protected EditorEnvironment editorEnv; // where to ask for an editor
    protected Vector exitRequests;

    protected FilingManager filingManager;
    protected FocusSystem focusSystem;
    protected Map instances; // A collection of MenuBuilder instances
    protected ProgressUI progressUI;

    private SplashScreen splashScreen;

    public void closeSplashScreen() {
        if (splashScreen == null) {
            return;
        }

        // Close the splash screen.
        //
        // NOTE: we must do it on AWT thread to avoid a deadlock which occurs
        // if user hits "close" button before splash screen has disappered.
        // In this case AWT invokes Toolkit.exitApplication() which blocks
        // on NativeSupport.lockLogonSession() waiting for the main thread
        // to finish initialization. When we call splashScreen.dispose()
        // from the main thread, it blocks on invokeAndWait() waiting for
        // AWT thread to process the request, thus causing deadlock.
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                splashScreen.dispose();
            }
        });
    }

    /**
     * Creates a menubar for a window component. Associated with this menu bar is
     * a MenuBuilder object that the user can access by using
     * (@code getMenuBuilder }.
     * @param target the target class with which to resolve resource references
     * @param menuResource the name of the resource which contains the menu definition
     * @param actions a set of the target actions with which to resolve command references
     * @param properties a set of the target properties with which to resolve property references
     */
    public JMenuBar createMenuBar(
        Class target,
        String menuResource,
        Map actions,
        PropertySet properties) {
        // Instantiation validates parameters:
        MenuBuilder mb = new MenuBuilder(target, actions, properties);
        JMenuBar menuB = mb.loadMenuBar(menuResource);
        if (menuB != null) {
            if (instances == null) {
                instances = new Hashtable(5);
            }
            if ((menuB != null) && (mb != null)) {
                instances.put(menuB, mb);
            }
        }
        return menuB;
    }

    /**
     * Releases a menubar, rendering the associated MenuBuilder object unusable.
     * Actions and items will be unbound, components released, Hashtables emptied.
     * Recommended to use when menu item is no longer needed.
     */
    public void dispose(JMenuBar menubar) {
        if (menubar == null) {
            return;
        }
        MenuBuilder mb = getMenuBuilder(menubar);
        if (mb == null) {
            return;
        }
        instances.remove(menubar);
        mb.dispose(menubar);
    }

    /**
     * Return the application window that has the focus. This call is useful
     * for ApplicationDialogs. Convenience function, will actually call the
     * FocusSystem object to answer this query.
     * @return the ApplicationWindow that has the focus. May return null
     * if there isn't an ApplicationWindow with the focus.
     */
    public ApplicationWindow getActiveWindow() {
        if (focusSystem != null) {
            return focusSystem.getActiveWindow();
        }
        return null;
    }

    /**
     * Returns the EditorEnvironment (an editor factory) to use by the application.
     * By default, it is null.  Override if this application supports content
     * editors (e.g., see EditorsUI).
     * @return an instance of EditorEnvironment OR NULL>
     */
    public EditorEnvironment getEditorEnvironment() {
        return editorEnv;
    }

    /**
     * Returns the filing manager in use by this application. By default will
     * return a standard DefaultFilingManager. Override if some other FilingManager
     * should be returned. If overridden, a FilingManager will not be created
     * in this class.
     * @return an instance of FilingManager OR NULL>
     */
    public FilingManager getFilingManager() {
        if (filingManager == null) {
            filingManager = new DefaultFilingManager();
        }
        return filingManager;
    }

    /**
     * Returns FocusSystem. Default implementation here is to return a fixed instance
     * of FocusSystem. Implementations may override this to return null or some
     * derived class of FocusSystem, in which case a FocusSystem will NOTr be
     * instantiated here.
     * @return an instance of HelpSystem OR NULL.
     */
    public FocusSystem getFocusSystem() {
        if (focusSystem == null) {
            focusSystem = new FocusSystem();
        }
        return focusSystem;
    }

    /**
     * Return the main application window.
     * @return the main ApplicationWindow. Could be null if there is no central
     * window!
     */
    abstract public ApplicationWindow getMainWindow();

    /**
      * Fetches a MenuBuilder instance by its identifying key.
      * @param key the identifying key for the MenuBuilder instance.
      *            This should be the root of a constructed menu component (e.g. JMenuBar)
      * @return the MenuBuilder instance or <code>null</code> if the key is not defined.
      */
    public MenuBuilder getMenuBuilder(Object key) {
        if ((key != null) && (instances != null)) {
            return (MenuBuilder) instances.get(key);
        } else {
            return null;
        }
    }

    /**
     * Returns ProgressUI. Default implementation here is to return a fixed instance
     * of ProgressUI. Implementations may override this to return null or some
     * some derived class of HelpSystem, in which
     * case a ProgressUI will NOT be instantiated here.
     * @return an instance of ProgressUI OR NULL.
     */
    public ProgressUI getProgressUI() {
        if (progressUI == null) {
            progressUI = new ProgressUI();
        }
        return progressUI;
    }

    /**
     * Invokes the "about" dialog.
     *
     * @return <code>true</code> if the About dialog has been invoked,
     *         <code>false</code> if it was already running
     */

    public boolean invokeAboutDialog() {
        if (aboutDialogIsRunning) {
            JFrame appFrame = getMainWindow().getFrame();
            Window[] owned = appFrame.getOwnedWindows();
            boolean found = false;

            if (owned != null && owned.length > 0) {

                // start from the end because visible windows are usually
                // at the end of the list
                for (int i = owned.length - 1; i >= 0 && !found; i--) {
                    if (owned[i].isVisible()) {
                        owned[i].toFront();
                        found = true;
                    }
                }
            }

            if (!found) {
                getMainWindow().toFront();
            }

            return false;
        } else {
            try {
                aboutDialogIsRunning = true;
                String buildDate = "";
                //
                // Uncomment the following buildDate code during development
                //
                // String buildDate = getBuildDate();
                // buildDate = (buildDate != null) && (buildDate.length() > 1)
                //             ? "<br><br></b><i>(Build date:" + buildDate + ")</i>"
                //             : "";
                String version =
                        "Version "
                        + getVersion()
                        + " "
                        + buildDate;

                ApplicationDialogs.invokeAboutDialog(
                    getString(appTitle),
                    getIcon(aboutDialogImage),
                    version,
                    getString(aboutDialogBodyText),
                    getString(appCopyrightNotice),
                    null);

                return true;
            } finally {
                aboutDialogIsRunning = false;
            }
        }
    }

    /**
     * Registers an application exit request.
     */
    public void registerExitRequest(ExitRequest exitRequest) {
        if (exitRequests == null) {
            exitRequests = new Vector();
        }
        if (!exitRequests.contains(exitRequest)) {
            exitRequests.add(exitRequest);
        }
    }

    /**
     * Request the application to exit. Calling this call will make the application
     * request an exit at every registered ExitRequest in a linear fashion. As soon
     * as one returns false, this call will return false and exit is to be aborted.
     * This call should never return true, because the system should have exited
     * by then. Applications needing to shut off devices or save prefernces should
     * encapsulate
     * that code as ExitRequests that will be called from here.
     * @return false if any registered exit requests of the requests added
     * returns false. Will never return true, system will be torn down
     * first.
     */
    public boolean requestExitApplication() {

        if (exitRequests != null) {
            for (Iterator e = exitRequests.iterator(); e.hasNext();) {
                ExitRequest req = (ExitRequest) e.next();
                if (!req.applicationExit()) {
                    return false;
                }
            }
        }

        // auf Wiedersehen
        System.exit(0);
        return true;
    }

    /**
     * Set the name for the resources file that will override all other
     * resources. By setting this file, the application-coder can override
     * any resource with any key anywhere in their own or shared libraries
     * that use the AppEnvironment system
     * @param theResourceFileName the path and name of the file, relative to the
     * class returned by getAppEnvironment(). Setting this name will remove
     * the previous properties that overrode all other properties. Setting to
     * null does nothing.
     */
    protected void setResourceFileName(String resourceFileName) {
        if (resourceFileName == null) {
            return;
        }
        InputStream in = null;
        in =
            getAppEnvironment().getClass().getResourceAsStream(
                resourceFileName);
        if (in != null) {
            try {
                Properties props = new Properties();
                props.load(in);
                appProperties = props;
            } catch (IOException io) {
                io.printStackTrace();
                Trace.Warning.printStackTrace(io);
            }
        }
    }

    /**
     * Enable or disable right-click menus. Convenience function to localize
     * code for sub-classing objects.
     */
    protected void setRightClickMenuEnabled(boolean enabled) {
        if (enabled) {
            RightClickMenuSystem.startRightClickMenuSystem();
        } else {
            RightClickMenuSystem.stopRightClickMenuSystem();
        }
    }

    /**
     * Show splash window. Async, will return while splash is still on screen.
     */
    public void showSplashScreen(int waitInMilliseconds) {
        // Show the splash screen. Wait until it shows on the screen
        final long[] start = new long[] { 0 };
        splashScreen = new SplashScreen(false);
        splashScreen.addWindowListener(new WindowAdapter() {
            public void windowOpened(WindowEvent e) {
                synchronized (start) {
                    start[0] = 1;
                    start.notify();
                }
            }
        });
        splashScreen.show();
        splashScreen.requestFocus();
        synchronized (start) {
            while (start[0] == 0) {
                try {
                    start.wait(waitInMilliseconds);
                } catch (Exception x) {
                    DebugMsg.println(x.getMessage());
                }
            }
        }
    }

    /**
     * Unregisters an application exit request.
     */
    public void unregisterExitRequest(ExitRequest exitRequest) {
        if (exitRequests == null) {
            return;
        }
        exitRequests.remove(exitRequest);
    }
}
