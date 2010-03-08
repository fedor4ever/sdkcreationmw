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

/* java.rmi */
import java.rmi.RemoteException;
import java.rmi.NotBoundException;
import java.rmi.Remote;
import java.rmi.Naming;

/* java.rmi.registry */
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;

/* java.rmi.server */
import java.rmi.server.UnicastRemoteObject;

/* javax.swing */
import javax.swing.SwingUtilities;

/* java.io */
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.io.IOException;

/* java.util */
import java.util.Properties;
import java.util.Random;

/* com.nokia.wap.wtk.app */
import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.app.ApplicationWindow;
import com.nokia.wtk.app.LocalApp;
import com.nokia.wtk.app.RemoteApp;
import com.nokia.wtk.app.StubEnv;

/**
 * This object provides support for re-using single instance of an
 * application. Even though it's a remote object, we use RMI for 
 * communication between instances running on the same machine.
 *
 * An application requiring single instance semantics must implement
 * LocalApp interface.  It creates a RemoteSingleton, passing in
 * a reference to itself in the constructor.
 *
 * This class stores the port in a class-wide Properties file, 
 * keyed by the name of the client application, so it can be used
 * by multiple applications concurrently.  A simple file lock is
 * used to syncronize changes to the data file.
 *
 * See Unit Test at end for usage example.
 *
 */
public class RemoteSingleton extends UnicastRemoteObject 
        implements RemoteApp {

    // Lock and data files
    private static File LOCK_FILE = new File("rsi.lock");
    private static File INFO_FILE = new File("rsi.info");

    // Typically available port numbers
    private static final int MIN_PORT = 1100;
    private static final int MAX_PORT = 5000;

    // Where we store the port number
    private static Properties gProperties = null;

    // There's no reason to create a new instance every
    // time we have to call toFront().
    //
    private Runnable mWindowToFront = null;

    // local app to call args with
    private LocalApp mLocalApp = null;

    // String identifier for this application
    private String mAppId = "unknownApp";

    // Listener for other remote instances
    private RemoteSingleton mRemoteListener = null;

    /**
     * Work item that brings an application window to front. 
     * Used if there are no arguments.
     */
    private class WindowToFront implements Runnable {
        private AppEnvironment mEnv = null;
        /*
         * Constructor
         * @param env App to bring to front
         */
        public WindowToFront(AppEnvironment env) {
            mEnv = env;
        }

        public void run() {
            if (mEnv == null) {
                // Log.debug("RMI: no app to bring to front!");
                return;
            } 
            // Log.debug("RMI: bringing app window to front");
            ApplicationWindow appWindow = mEnv.getMainWindow();
            if (appWindow == null) {
                return;
            }
            appWindow.setWindowIconified(false);
            appWindow.toFront();
        }
    }

    /**
     * Work item that performs a command line on the local app sent
     * from a remote instance.  Used when there are arguments to handle.
     */
    private class RemoteCommand implements Runnable {
        private String [] mArgs;
        private LocalApp mApp;

        /*
         * Constructor
         * @param args args to call
         * @param app LocalApp to run
         */
        RemoteCommand(String[] args, LocalApp app) {
            mArgs = args;
            mApp = app;
        }

        public void run() {
            // Log.debug("RMI: remotely passed args " + concatArgs(mArgs));
            if (mApp != null) {
                mApp.handleArguments(mArgs);
            } else {
                // Log.debug("RMI: no local app!");
            }
        }
    }

    /**
     * Creates <code>RemoteSingleton</code>
     *
     * @param env AppEnvironment to bring forward
     * @param klass Client class. Name used to uniquely identify the client app.
     * @param locApp Application object to call when executing commands.
     */
    public RemoteSingleton(AppEnvironment env, Class klass, LocalApp locApp) throws RemoteException {
        if (env != null) {
            mWindowToFront = new WindowToFront(env);
        }
        mAppId = klass.getName();
        mLocalApp = locApp;
    }

    /**
     * Remote request to perform command.
     *
     * @param args the arguments (command line)
     * @see com.nokia.wtk.app.RemoteApp
     */
    public void performCommand(String [] args) {
        //
        // make sure that GUI code is always invoked on GUI thread to avoid
        // need for synchronization. There is no reason to wait (i.e.
        // invokeAndWait()) until it's actually done. Besides, if we do
        // invokeLater(), we don't even have to check if we are already
        // in the context of event dispatch thread or not (invokeAndWait()
        // requires checking)
        //
        // We use the static instance for commands with an empty command line.
        //
        // Log.debug("in performCommand: " + concatArgs(args));
        Runnable workItem = null;
        if (args == null || args.length == 0) {
            workItem = mWindowToFront;
        } else {
            workItem = new RemoteCommand(args, mLocalApp);
        }
        SwingUtilities.invokeLater(workItem);
    }

    /**
     * Attempts to forward the command line arguments to already running
     * instance. If unsuccessful, registers a remote object
     * to receive such requests from other instances.
     *
     * @return true if we succesfully connect to other instance.
     */
    public boolean useOtherInstance(String[] args) {
        //
        // try to connect to another running instance 
        //
        int port = getAppPort(mAppId);
        if (port > 0) {
            RemoteApp remote = connectToInstance(port);
            if (remote == null) {
                deleteAppPort(mAppId);
            } else {
                try {
                    remote.performCommand(args);
                    return true;
                } catch (Exception x) {
                    // Log.error("Can't connect to other instance");
                    deleteAppPort(mAppId);
                }
            }
        }
        //
        // We have failed to connect to another running instance.
        // Try to register our own RMI listener.
        // Limit max number of attempts to some reasonably large number.
        //
        Random random = new Random();
        for (int i = 0; i < 100; i++) {
            port = MIN_PORT + random.nextInt(MAX_PORT - MIN_PORT);
            try {
                Registry r = LocateRegistry.createRegistry(port);
                if (mRemoteListener == null) {
                    mRemoteListener = new RemoteSingleton(null, this.getClass(), mLocalApp);
                }

                String name = getBindName(port);
                Naming.bind(name, mRemoteListener);
                // Log.debug("Bound to RMI port: " + port);

                // save port number
                saveAppPort(mAppId, port);
                return false;

            } catch (RemoteException x) {
                // ignore
                // x.printStackTrace();
            } catch (Exception x) {
                // dump
                x.printStackTrace();
            }
        }
        mRemoteListener = null;
        return false;
    }

    /**
     * Connect to a running instance as registered with the RMI
     * registry at the specified port on the local machine.
     *
     * @param port the "private" RMI registry port
     */
    private RemoteApp connectToInstance(int port) {
        String name = getBindName(port);
        try {
            Remote remote = Naming.lookup(name);
            // Log.debug("Connected to " + name);
            return (RemoteApp)remote;

        } catch (NotBoundException x) {
            // ignore
        } catch (IOException x) {
            // ignore
        } catch (Exception x) {
            // dump
            x.printStackTrace();
        }
        // Log.debug("Cannot connect to " + name);
        return null;
    }

    /**
     * Return name used to bind via RMI to remote object.
     * @param port Port number encoded in name
     * @see java.rmi.Naming
     */
    private String getBindName(int port) {
        String name = "rmi://127.0.0.1:" + port + "/" + mAppId;
        return name;
    }

    /********************************************************
     *  P R O P E R T Y    S E C T I O N
     ********************************************************/

    /**
     * Return name used to identify the RMI port used for this app in
     * the properties file.
     */
    private static String getPortPropertyName(String appName) {
        return appName + ".port";
    }

    /**
     * Save the value of RMI port used by the application with this key.
     * @param appKey String key that identifies this application uniquely.
     * @param port Port number to save
     */
    private static boolean saveAppPort(String appKey, int port) {
        getLock();
        String key = getPortPropertyName(appKey);
        getProperties().setProperty(key, Integer.toString(port));
        saveProperties();
        releaseLock();
        return true;
    }

    /**
     * Get value of application port identified by supplied key.
     *
     * @param appKey String key that identifies this application uniquely.
     * @return value of application port or -1 on any failure.
     */
    private static int getAppPort(String appKey) {
        String key = getPortPropertyName(appKey);
        int portNumber = -1;
        String p = getProperties().getProperty(key);
        if (p == null || p.length() == 0) {
            return -1;
        }
        try {
            portNumber = Integer.parseInt(p);
        } catch (NumberFormatException ex) {
            // Log.debug("Bad remote port number: " + p);
        }
        return portNumber;
    }

    /**
     * Delete value of application port
     * @param appKey String key that identifies this application uniquely.
     */
    private static boolean deleteAppPort(String appKey) {
        getLock();
        String key = getPortPropertyName(appKey);
        getProperties().remove(key);
        saveProperties();
        releaseLock();
        return true;
    }

    /**
     * Acquire a lock on the data file by creating a lockfile.
     * @return true if succesful, false on any error.
     */
    private static boolean getLock() {
        //
        // Try 5 times at 1 second intervals, then give up.
        // Note: Depends on 1.4 'createFile' method guarenteed atomic.
        //
        for (int i = 0; i < 5; i++) {
            try {
                LOCK_FILE.createNewFile();
                LOCK_FILE.deleteOnExit();
                return true;
            } catch (IOException ex) {
            }
            try {
               Thread.sleep(1000);
            } catch (InterruptedException ex) {
               return false;
            }
        }
        return false;
    }

    /**
     * Release lock on the data file.
     */
    private static boolean releaseLock() {
        boolean ok = LOCK_FILE.delete();
        return ok;
    }

    /**
     * Get properties for this class. Load from file on first reference.
     * @return Properties for this class (possibly empty)
     */
    private static Properties getProperties() {
        if (gProperties == null) {
            gProperties = new Properties();
            try {
                 FileInputStream in = new FileInputStream(INFO_FILE);
                 gProperties.load(in);
            } catch (IOException io) {
                 // Log.debug("Can't read RemoteSingleton properties file!");
            }
        }
        return gProperties;
    }

    /**
     * Save properties to file.  If never used, do nothing.
     */
    private static void saveProperties() {
        if (gProperties == null) {
            return;
        }
        try {
            FileOutputStream o = new FileOutputStream(INFO_FILE);
            gProperties.store(o, "RemoteSingleton Properties");
        } catch (IOException io) {
            // Log.debug("Can't save RemoteSingleton properties!");
        }
    }

    /**
     * For debugging convenience.  Convert an array of
     * Strings to a single space-separated string.
     */
    private static String concatArgs(String[] args) {
       if (args != null && args.length > 0) {
           StringBuffer sb = new StringBuffer();
           for (int i = 0; i < args.length; i++) {
               sb.append(args[i] + " ");
           }
           return sb.toString();
       }
       return "";
    }

    /**********************************************************
     * UNIT TEST
     *    
     * A fake LocalApp with a singleton object.
     * To test, run:
     *    RemoteSingleton a b c
     * Then from another console:
     *    RemoteSingleton d e f
     * First instance should display second's arguments.
     *
     **********************************************************/

    public static void main(String[] args) {
        // Log.initDefaults();
        TestApp test = new TestApp();
        test.doArgs(args);
    }

    private static class TestApp implements LocalApp {
        RemoteSingleton mRs = null;

        /**
         * Fake app constructor
         */
        public TestApp() {
            try {
                mRs = new RemoteSingleton(new StubEnv(), TestApp.class, TestApp.this);
            } catch (RemoteException ex) {
                System.out.println("Can't create RemoteSingleton: " + ex.getMessage());
                System.exit(1);
            }
        }

        /**
         * <code>handleArguments</code> executes commands locally.
         * This method will also be called on behalf of remote instances.
         *
         * @see com.nokia.wtk.app.LocalApp
         */
        public void handleArguments(String[] args) {
            String end = concatArgs(args);
            System.out.println("Performing test with args " + end);
        }

        /*
         * An example of using RemoteSingleton to implement single instance semantics.
         */
        public void doArgs(String[] args) {
            if (mRs.useOtherInstance(args)) {
                System.out.println("Used other instance");
                System.exit(0);
            }
            this.handleArguments(args);
        }
    }
}
