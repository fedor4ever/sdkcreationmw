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


/*
 * Created on Aug 9, 2004
 */
package com.nokia.wtk.sdkSimulator.diagnosticsAdapter;

// java.awt
import java.awt.Point;

// java.io
import java.io.File;
import java.io.IOException;
import java.io.Serializable;
import java.io.InputStream;
import java.io.FileInputStream;

// java.util
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Properties;

// com.nokia.wtk.app
import com.nokia.wtk.app.AppEnvironment;

// com.nokia.wtk.device
import com.nokia.wtk.device.Device;
import com.nokia.wtk.device.DeviceWindowProperties;

// com.nokia.wtk.diagnostics.debug
import com.nokia.wtk.diagnostics.debug.DebugViews;

// com.nokia.wtk.util
import com.nokia.wtk.util.Property;
import com.nokia.wtk.util.PropertySet;


/**
 * <p>Title: AdapterManager</p>
 * <p>Description: A local implementation of Device that mediates between
 *  the diagnostics component and specific SDK service providers.</p>
 * <p>Company: Nokia</p>
 * @version 1.0
 */
public class AdapterManager implements Device, PropertyListener {
  /**
   * Constructor.
   * @param deviceName the base name of the device this AdapterManager will
   * interact with.
   * @param sampleSdk a reference to the SDK object
   * @param supportedServices the list of services supported by the SDK
   */

  public AdapterManager( String deviceName,
                         SdkInterface sampleSdk,
                         Properties supportedServices) {
    this.deviceName = deviceName;
    this.sampleSdk = sampleSdk;
    this.supportedServices = supportedServices;

    // add shutdown thread to write all the properties and their new value
    // to disk
    Runtime.getRuntime().addShutdownHook( new Thread( new Runnable()
    {
      public void run() {
        if ( diagnosticsConfig != null && configFile != null) {
          diagnosticsConfig.save(configFile, null);
        }
      }
    } ) );

    // load the diagnostics configuration from disk.
    loadConfiguration();

    // construct SDK service objects
    constructServiceObjects();
  }


  /**
   * kicks off Diagnostics initialization
   * @param windowProperties a Map of properties and their initial values
   *
   * @return true if initialization was successful, false otherwise.
   */
  public boolean initializeDiagnostics(Map windowProperties) {

    // create the Diagnostics first object, passing this
    // AdapterManager instance
    diagnosticsWindow =
      new DebugViews(this, getDiagnosticsConfig());
    // set default close operation
    if ( diagnosticsWindow != null ) {
      diagnosticsWindow.getMainWindow().getFrame().setDefaultCloseOperation(
       javax.swing.WindowConstants.HIDE_ON_CLOSE );
    // add the window properties that we have received from the SDK (if any)

    try {
      diagnosticsViewsFile = new File (System.getProperty( USER_DIR ), DIAGNOSTICS_VIEWS_FILE_NAME );
      if (diagnosticsViewsFile != null) {
        InputStream is = new FileInputStream (diagnosticsViewsFile);

        // add the views from the DiagnosticsViews.properties file

         // get the .properties file from the jar
        Properties diagViewsProps = new Properties();

        // load the file into the propeties object
        if (is != null) {
            diagViewsProps.load(is);
            diagnosticsWindow.addViewsFromProperties(diagViewsProps);
        }
      }
    } catch (IOException ioe) {
            System.err.println
            ("initializeDiagnostics():failed to load properties file: " + DIAGNOSTICS_VIEWS_FILE_NAME);
            System.err.println( "\tIOException: " + ioe);
    }

      if (windowProperties != null) {
        Iterator iterator = windowProperties.keySet().iterator();
        while ( iterator.hasNext() ) {
          String property = (String)iterator.next();
          updatePropertySet( property, (Serializable)windowProperties.get( property ) );
        }
      }

      // propagate window properties to the GUI
      updatePropertiesInGui();

      // register to be notified about properties changes in the SDK
      sampleSdk.registerPropertyListener(this);
      return true;
    }
    else {
      return false;
    }
  }



  /**
   * Called to begin a session with a new device, this is legacy, the device
   * starts the diagnostics window, so there is no need for it to call this.
   * @throws RuntimeException
   */
  public void openDevice() throws java.lang.RuntimeException {
    // There is no reason for debug views to call this, the device controls it.
    throw new java.lang.UnsupportedOperationException(
      "Method openDevice() not supported." );
  }

  /**
   * Called to end a session with a device, this is legacy, the device starts
   * the diagnostics window, so there is no need for it to call this.
   */
  public void closeDevice() {
    // There is no reason for debug views to call this, the device controls it.
    throw new java.lang.UnsupportedOperationException(
      "Method closeDevice() not supported." );
  }

  /**
   * Returns an instance of a service support object, given an interface.
   * @param type the Class of the device service requested.
   * @return an instance of the service requested or null if not supported.
   */
  public Object getInstanceOf( Class type ) {

    // return null if services is null or empty
    if (services == null || services.isEmpty()) {
      return null;
    }
    Iterator it = services.values().iterator();
    Object obj = null;
    if (it != null) {
      while (it.hasNext()) {
        obj = it.next();
        //check if the service object implements the requested interface
        Class[] interfaces = obj.getClass().getInterfaces();
        if (interfaces != null && interfaces.length > 0) {
          for (int i=0; i<interfaces.length; i++) {
            if (interfaces[i] == type) {
              //debug
//              System.err.println ("returning service object of type: " + type.getName());
              return obj;
            }
          }
        }
      }
      // interface not found, return null
      return null;
    }
    else {
      // something went wrong...
      return null;
    }
  }

  /**
   * Handles registration of host services (listeners)
   * @param type the Class of the service (listener interface)
   * @param listener the instance of the listener or null to unset.
   * @return true if the listener is supported, false if not or unset.
   */
  public boolean setInstanceOf( Class type, Object listener ) {
    if (services != null && services.containsKey(type)) {
      AbstractServiceAdapter serviceObj =
        (AbstractServiceAdapter) services.get(type);
      if (serviceObj != null) {
        serviceObj.addListener(listener);
        //debug
//        System.err.println ("added external listener of type: " + type.getName());
        return true;
      }
    }
    return false;
  }

  /**
   * Called when a property value changes.
   * @param property the name of the property that has changed.
   * @param value the new value of the property.
   */
  public void propertyChanged( String property, Serializable value ) {
    // if the Diagnostics window is already created,
    // update the property value directly in the diagnostics window
    // object
    // ** note: we should have a better mechanism here to notify
    // diagnostics object about property changes
    if (diagnosticsWindow != null) {
      if(property.equals( DeviceWindowProperties.DeviceWindowTitle) ) {
         String title = "Diagnostics - " + (String)value;
        diagnosticsWindow.getMainWindow().getFrame().setTitle(title);
        value = title;
      }
      else if(property.equals( DeviceWindowProperties.DeviceWindowVisible)) {
        boolean visible = ((Boolean)value).booleanValue();
        diagnosticsWindow.getMainWindow().getFrame().setVisible(visible);
        if (visible == true) {
          diagnosticsWindow.getMainWindow().getFrame().toFront();
        }
      }
      else if(property.equals(DeviceWindowProperties.DeviceWindowLocation)) {
        Point newLocation;
        if (value != null) {
          newLocation = (Point)value;
        }
        else {
          Point currentLocation =
            diagnosticsWindow.getMainWindow().getFrame().getLocation();
          newLocation = new Point (((int)currentLocation.getX() + 50),
                                   ((int)currentLocation.getY() + 50));
          value = newLocation;
        }
        diagnosticsWindow.getMainWindow().getFrame().setLocation(newLocation);
      }
    }

    // always set the new value in the Property object
    updatePropertySet(property, value);
  }


  /**
   * Updates the value of a property in the property set.
   * If the property does not exists, it creates it
   * @param property the name of the property that has changed.
   * @param value the new value of the property.
   */
  private void updatePropertySet( String property, Serializable value ) {
    Property prop = getDiagnosticsConfig().getProperty( property );
    if ( prop != null ) {
      prop.setValue( value );
    } else {
      prop = getDiagnosticsConfig().addProperty( property, value.getClass(), true );
      if ( prop != null ) {
        prop.setValue( value );
      }
    }
  }

  /**
   * Returns the Diagnostics configuration that this AdapterManager loaded
   * in a ProperySet object
   * @return a PropertySet containing the diagnostics window configuration
   */
  public PropertySet getDiagnosticsConfig() {
    if ( diagnosticsConfig == null ) {
      loadConfiguration();
    }
    return diagnosticsConfig;
  }

  public File getConfigFile() {
      return configFile;
  }


  /**
   * Called to set the initial property values in the GUI.
   */
  private void updatePropertiesInGui() {
    // ** note!!!: we should have a better mechanism here to notify
    // diagnostics object about property changes
    if (diagnosticsWindow != null) {
      Property prop =
        getDiagnosticsConfig().getProperty(DeviceWindowProperties.DeviceWindowVisible);
      if ( prop != null ) {
        boolean visible = ((Boolean)prop.getValue()).booleanValue();
        diagnosticsWindow.getMainWindow().getFrame().setVisible(visible);
        if (visible == true) {
          diagnosticsWindow.getMainWindow().getFrame().toFront();
        }
      }
      prop =
        getDiagnosticsConfig().getProperty(DeviceWindowProperties.DeviceWindowLocation);
      if ( prop != null ) {
        Point location = (Point)prop.getValue();
        diagnosticsWindow.getMainWindow().getFrame().setLocation(location);
      }
      prop =
        getDiagnosticsConfig().getProperty(DeviceWindowProperties.DeviceWindowTitle);
      if ( prop != null ) {
        String value = (String)prop.getValue();
        String title = "Diagnostics - " + (String)value;
        diagnosticsWindow.getMainWindow().getFrame().setTitle(title);
      }
    }
  }

  /**
   * Registers this AdapterManager as a PropertyListener to receive propertyChanged
   * events from the native phone software.
   */
  private void registerPropertyListener() {
    sampleSdk.registerPropertyListener(this);
  }

  /**
   * Locate and load diagnostics configuration files in the current operating
   * directory as specified in the user.dir property of Java.
   */
  private void loadConfiguration() {
    //debug
//    System.err.println ("Loading diagnostics configuration ...");
    try {
      configFile = new File (System.getProperty( USER_DIR ), CONFIG_FILE_NAME );
      if (configFile != null) {
        diagnosticsConfig = new PropertySet();
        if (configFile.exists()) {
          diagnosticsConfig.load( configFile, true );
          //debug
//          System.err.println ("diagnostics configuration loaded");
        }
        else {
          //debug
//          System.err.println ("configuration file does not exists, new one will be created");
        }
      }
      else {
        System.err.println( "Error loading configuration\nexiting...");
        System.exit( 1 );
      }

    } catch ( Exception e ) {
      System.err.println("Exception caught while loading configuration file " + e + "\nexiting...");
      System.exit( 1 );
    }
  }

  /**
   * Construct the service objects used to communicate events and information
   * from the SDK to Diagnostics. Also, build a Map that associates each listener
   * interface to a service support object.
   */
  private void constructServiceObjects() {
    // construct service objects and map of listeners-services

    services = new HashMap();
    //debug
//    System.err.println ("creating service support objects (aka adpaters)...");
    for ( int i = 0; i < serviceNames.length; i++ ) {
      String value = supportedServices.getProperty( serviceNames[i] );
      // if value == 'true' (service supported)
      if ( value != null && Boolean.valueOf( value ).booleanValue() ) {

        // HTTP traffic (Browser) service
        if ( serviceNames[i].equalsIgnoreCase( HTTP_TRAFFIC ) ) {
          //debug
//          System.err.println("creating sdkHTTPService object...");
          SdkHTTPService sdkHttpService =
            new SdkHTTPService(sampleSdk, serviceNames[i]);
          services.put( com.nokia.wtk.device.DeviceHTTPContentListener.class,
                        sdkHttpService);
          //debug
//          System.err.println("\tcreated");
        }
        // Browser history service
        else if ( serviceNames[i].equalsIgnoreCase( BROWSER_HISTORY) ) {
          //debug
//          System.err.println("creating SdkBrowserHistoryService object...");
          SdkBrowserHistoryService sdkBrowserHistoryService =
            new SdkBrowserHistoryService(sampleSdk, serviceNames[i]);
          services.put( com.nokia.wtk.device.DeviceBrowserHistoryListener.class,
                        sdkBrowserHistoryService);
          //debug
//          System.err.println("\tcreated");
        }
        // SMS service
        else if ( serviceNames[i].equalsIgnoreCase( SMS_TRAFFIC ) ) {
          //debug
//          System.err.println("creating SdkSMSService object...");
          services.put( com.nokia.wtk.device.DeviceMessageListener.class,
                        new SdkSMSService(sampleSdk, serviceNames[i]));
          //debug
//          System.err.println("\tcreated");
        }
        // Socket (TCP/UDP traffic) service
        else if ( serviceNames[i].equalsIgnoreCase( SOCKET_TRAFFIC ) ) {
          //debug
//          System.err.println("creating SdkSocketService object...");
          services.put( com.nokia.wtk.device.DeviceSocketListener.class,
                        new SdkSocketService(sampleSdk, serviceNames[i]));
          //debug
//          System.err.println("\tcreated");
        }
        // Bluetooth service
        else if ( serviceNames[i].equalsIgnoreCase( BLUETOOTH_TRAFFIC ) ) {
          //debug
//          System.err.println("creating SdkBluetoothService object...");
          services.put( com.nokia.wtk.device.DeviceBluetoothListener.class,
                        new SdkBluetoothService(sampleSdk, serviceNames[i]));
          //debug
//          System.err.println("\tcreated");
        }
        // J2ME service
        else if ( serviceNames[i].equalsIgnoreCase( J2ME ) ) {
          //debug
//          System.err.println("creating SdkJ2MEService object...");
          services.put( com.nokia.wtk.device.DeviceJ2MEListener.class,
                        new SdkJ2MEService(sampleSdk, serviceNames[i]));
          //debug
//          System.err.println("\tcreated");
        }
        // File system service
        else if ( serviceNames[i].equalsIgnoreCase( FILE_SYSTEM ) ) {
          //debug
//          System.err.println("creating SdkFileSystemService object...");
          services.put( com.nokia.wtk.device.DeviceFileSystemListener.class,
                        new SdkFileSystemService(sampleSdk, serviceNames[i]));
          //debug
//          System.err.println("\tcreated");
        }
        // Sample service
        else if ( serviceNames[i].equalsIgnoreCase( SAMPLE ) ) {
          //debug
//          System.err.println("creating SdkSampleService object...");
          services.put( com.nokia.wtk.device.DeviceSampleListener.class,
                        new SdkSampleService(sampleSdk, serviceNames[i]));
          //debug
//          System.err.println("\tcreated");
        }
      } // if service supported
    } // end for loop
    //debug
//    System.err.println ("service support objects created\n");


}

  private static final String USER_DIR =           "user.dir";
  private static final String CONFIG_FILE_NAME =   "diagnostics.config";
  private static final String JAR_FILE_EXTENSION = ".jar";
  private static final String HTTP_TRAFFIC =       "HttpTraffic";
  private static final String BROWSER_HISTORY=     "BrowserHistory";
  private static final String SMS_TRAFFIC =        "SmsTraffic";
  private static final String SOCKET_TRAFFIC =     "SocketTraffic";
  private static final String BLUETOOTH_TRAFFIC =  "BluetoothTraffic";
  private static final String J2ME =               "J2ME";
  private static final String FILE_SYSTEM =        "FileSystem";
  private static final String SAMPLE =             "Sample";
  private static final String DIAGNOSTICS_VIEWS_FILE_NAME = "DiagnosticsViews.properties";

/**
 * array of SDK service names
 */
  private static final String [] serviceNames = {
    HTTP_TRAFFIC,
    BROWSER_HISTORY,
    SMS_TRAFFIC,
    SOCKET_TRAFFIC,
    BLUETOOTH_TRAFFIC,
    J2ME,
    FILE_SYSTEM,
    SAMPLE
  };

  private static File configFile = null;
  private static File diagnosticsViewsFile = null;
  private static PropertySet diagnosticsConfig = null;
  private Map services = null;
  private String deviceName = null;
  private SdkInterface sampleSdk = null;
  private DebugViews diagnosticsWindow = null;
  private Properties supportedServices = null;
}
