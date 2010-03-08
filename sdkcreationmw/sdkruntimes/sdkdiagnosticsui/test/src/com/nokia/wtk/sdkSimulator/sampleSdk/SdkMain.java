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


package com.nokia.wtk.sdkSimulator.sampleSdk;

// java.util
import java.util.Properties;
import java.util.Map;
import java.util.HashMap;
import java.util.Iterator;
import java.util.zip.ZipException;
import java.util.zip.ZipEntry;
import java.util.jar.JarFile;

// java.io
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.File;
import java.io.InputStream;

// java.awt
import java.awt.Point;


// com.nokia.wtk.device
import com.nokia.wtk.device.DeviceWindowProperties;
import com.nokia.wtk.device.DeviceFile;
import com.nokia.wtk.device.DeviceMemory;
import com.nokia.wtk.device.DeviceBluetoothSend;
import com.nokia.wtk.device.DeviceBluetoothReceive;
import com.nokia.wtk.device.DeviceContentRequest;
import com.nokia.wtk.device.DeviceContentReceived;
import com.nokia.wtk.device.DeviceSocket;
import com.nokia.wtk.device.DeviceMessageFragment;
import com.nokia.wtk.device.DeviceMessage;
import com.nokia.wtk.device.DeviceSampleData;

// com.nokia.wtk.sdkSimulator.diagnosticsAdapter
import com.nokia.wtk.sdkSimulator.diagnosticsAdapter.AdapterManager;
import com.nokia.wtk.sdkSimulator.diagnosticsAdapter.SdkInterface;
import com.nokia.wtk.sdkSimulator.diagnosticsAdapter.PropertyListener;
import com.nokia.wtk.sdkSimulator.diagnosticsAdapter.BluetoothAdapter;
import com.nokia.wtk.sdkSimulator.diagnosticsAdapter.BrowserHistoryAdapter;
import com.nokia.wtk.sdkSimulator.diagnosticsAdapter.HTTPAdapter;
import com.nokia.wtk.sdkSimulator.diagnosticsAdapter.FileSystemAdapter;
import com.nokia.wtk.sdkSimulator.diagnosticsAdapter.J2MEAdapter;
import com.nokia.wtk.sdkSimulator.diagnosticsAdapter.SampleAdapter;
import com.nokia.wtk.sdkSimulator.diagnosticsAdapter.SMSAdapter;
import com.nokia.wtk.sdkSimulator.diagnosticsAdapter.SocketAdapter;

// com.nokia.wtk.diagnosticsTest.data
import com.nokia.wtk.sdkSimulator.data.BrowserResponse;
import com.nokia.wtk.sdkSimulator.data.PushData;


/**
 * <p>Title: SdkMain</p>
 * <p>Description: A sample SDK class to test the SDK diagnostics component
 * <p>Company: Nokia</p>
 * @version 1.0
 */
public class SdkMain implements SdkInterface {

  /**
   * main method. Starting point of the Diagnostics test program.
   */
  public static void main( String[] args ) {
    sdkInstance = new SdkMain();
    boolean result = sdkInstance.init();
    if (result == true) {
      sdkInstance.runTest();
    }
    else {
      System.out.println("error initializing Diagnostics");
    }
    System.out.println("exiting...");
    System.exit(0);
  }

  public SdkInterface getSdkInstance() {
    return sdkInstance;
  }

  /**
   * registers the specified service name with the specified
   * service object. Also creates a test object to generate
   * fake events related to that service
   * @param serviceName The name of the service
   * @param server The object that implements the service
   */
  public void registerServer(String serviceName, Object serviceObject) {
    synchronized (servers) {
      servers.put (serviceName, serviceObject);
      //debug
//      System.err.println("Registered service object: " + serviceName);
      // create an object that generates events for the sample view
      if (serviceName.equals(SAMPLE) && sampleEventGenerator == null) {
        sampleEventGenerator = new SampleEventGenerator();
      }
      // create an object that generates events for the MIDP view
      else if ((serviceName.equals(FILE_SYSTEM) ||
                serviceName.equals(J2ME)) &&
               midpEventGenerator == null) {
        midpEventGenerator = new MIDPEventGenerator();
      }
      // create an object that generates events for the Traffic view
      else if ((serviceName.equals(HTTP_TRAFFIC) ||
                serviceName.equals(SOCKET_TRAFFIC) ||
                serviceName.equals(BLUETOOTH_TRAFFIC)) &&
               trafficEventGenerator == null) {
        trafficEventGenerator = new TrafficEventGenerator();
      }
    }
  }

  /**
   * registers the specified listener as a property listener
   * the property listeners listens to events related to the
   * Diagnostics window, such as window title, window visibility,
   * window position.
   * @param listener The property listener object
   */
  public void registerPropertyListener(PropertyListener listener) {
    propertyListener = listener;
  }

  /**
   * returns the root directory of the SDK Java file system
   * @return the DeviceFile object that represents the root directory
   */
  public DeviceFile getFileSystemRoot() {
    DeviceFile root = null;
    if (midpEventGenerator != null) {
      root = midpEventGenerator.getRootDir();
    }
    return root;
  }

  /**
   * private constructor
   */
  private SdkMain () {
  }

  /**
   * performs initialization
   */
  private boolean init () {

    Properties supportedServices = loadServiceProperties();
    if (supportedServices == null || supportedServices.isEmpty()) {
      return false;
    }

    // create a new map to hold service names and associated service objects
    servers = new HashMap();

    // create a Map of SDK properties used by Diagnostics
    windowProperties = new HashMap();
    // add initial SDK properties: window title, window location,
    // window visibility
    windowProperties.put (DeviceWindowProperties.DeviceWindowTitle,
                          SAMPLE_NAME);
    windowProperties.put (DeviceWindowProperties.DeviceWindowLocation,
                          new Point (590, 60));
    windowProperties.put (DeviceWindowProperties.DeviceWindowVisible,
                          Boolean.TRUE);


    // create the AdapterManager objects, passing the SDK name and object
    adapter = new AdapterManager(SAMPLE_NAME, (SdkInterface)this, supportedServices);

    // ask the AdapterManager to create and initialize Diagnostics
    // pass the map of window properties
    return (adapter.initializeDiagnostics(windowProperties));
  }


  /**
   * Load the Service.properties file for the current emulator. This file contains
   * all the services supported by the SDK. Supported services have a value = 'true'
   * @return a Properties object containing the properties found in the file.
   */
   private Properties loadServiceProperties() {
     // debug
//     System.err.println ("loading configured SDK services...");
     Properties supportedServices = new Properties();
     try {
       JarFile file = new JarFile( new File( System.getProperty( "user.dir" ),
                                             JAR_NAME) );
       if (file == null) {
         System.err.println("jar file:" + JAR_NAME + " not found!");
       }
       else {
         ZipEntry entry = file.getEntry( SERVICES_FILE_NAME );
         if (entry == null) {
           System.err.println ("zipped file: " + SERVICES_FILE_NAME + " not found in jar");
         }
         else {
           InputStream stream = file.getInputStream(entry);
           supportedServices.load( stream );
           // debug
//           System.err.println ("configured SDK services loaded");
         }
       }
     } catch ( ZipException e ) {
       System.err.println( "ZipException extracting " + SERVICES_FILE_NAME + "from jar file: " + JAR_NAME +"\n" + e.getMessage() );
     } catch ( IOException e ) {
       System.err.println( "IOException: " + e );
     } catch ( SecurityException e ) {
       System.err.println( "SecurityException: " + e );
     }
     return supportedServices;
   }

  /**
   * runs the diagnostics test in an infinite loop. Each time
   * a list of available events is displayed, each identified
   * by a number. The user selects a number, and the corresponding
   * event is generated and sent to Diagnostics.
   * Only events associated with supported services are offered.
   * The loop ends when the user enters '-1'.
   */
  private void runTest () {
    System.out.println ("\n*****************  Test  ********************");
    String input = getEventToSend();
    while (!(input == null || input.equals("-1"))) {
      // property change events
      if (input.equals("1")) {
        propertyListener.propertyChanged (DeviceWindowProperties.DeviceWindowVisible,
                                          Boolean.TRUE);
      }
      else if (input.equals("2")) {
        propertyListener.propertyChanged (DeviceWindowProperties.DeviceWindowVisible,
                                          Boolean.FALSE);
      }
      else if (input.equals("3")) {
        propertyListener.propertyChanged (DeviceWindowProperties.DeviceWindowLocation,
                                          (Point)null);
      }
      else if (input.equals("4")) {
        propertyListener.propertyChanged (DeviceWindowProperties.DeviceWindowTitle,
                                          SAMPLE_NAME + " changed " + String.valueOf(titleChangeCount));
        titleChangeCount ++;
      }

      // other events
      else if (input.equals("5")) {
        generateFileAdded(servers.get(FILE_SYSTEM));
      }
      else if (input.equals("6")) {
        generateTcpSend(servers.get(SOCKET_TRAFFIC));
      }
      else if (input.equals("7")) {
        generateTcpReceive(servers.get(SOCKET_TRAFFIC));
      }
      else if (input.equals("8")) {
        generateUdpSend(servers.get(SOCKET_TRAFFIC));
      }
      else if (input.equals("9")) {
        generateUdpReceive(servers.get(SOCKET_TRAFFIC));
      }
      else if (input.equals("10")) {
        generateSmsSend(servers.get(SMS_TRAFFIC));
      }
      else if (input.equals("11")) {
        generateSmsReceive(servers.get(SMS_TRAFFIC));
      }
      else if (input.equals("12")) {
        generateMemoryChanged(servers.get(J2ME));
      }
      else if (input.equals("13")) {
        generateBluetoothSend(servers.get(BLUETOOTH_TRAFFIC));
      }
      else if (input.equals("14")) {
        generateBluetoothReceive(servers.get(BLUETOOTH_TRAFFIC));
      }
      else if (input.equals("15")) {
        generateHttpSend(servers.get(HTTP_TRAFFIC));
      }
      else if (input.equals("16")) {
        generateHttpReceive(
          servers.get(HTTP_TRAFFIC), servers.get(BROWSER_HISTORY));
      }
      else if (input.equals("17")) {
        generateMmsPush(servers.get(HTTP_TRAFFIC));
      }
      else if (input.equals("18")) {
        generateMmsSend(servers.get(HTTP_TRAFFIC));
      }
      else if (input.equals("19")) {
        generateSampleEvent(servers.get(SAMPLE));
      }
      else {
        System.out.println ("invalid input");
      }
      input = getEventToSend();
    } // while
  }


  /**
   * displays the list of available events
   * and accepts the user selection
   *@return the event selected
   */
  private String getEventToSend () {

    if (propertyListener == null &&
        (servers == null || servers.isEmpty())) {
      System.err.println ("no event listeners registered");
      return "-1";
    }
    // prompt for input
    System.out.println ("\nEnter event number from the following list:\n");

    // property changed events
    if (propertyListener != null) {
      System.out.println ("1.  Window visible");
      System.out.println ("2.  Window hidden");
      System.out.println ("3.  Window moved");
      System.out.println ("4.  Title changed\n");
    }

    // other events
    synchronized (servers) {
      if (servers != null && !(servers.isEmpty())) {
        Iterator it = servers.keySet().iterator();
        while (it.hasNext()) {
          String name = (String)it.next();
          if (name.equalsIgnoreCase(HTTP_TRAFFIC)) {
            System.out.println ("15. HTTP send");
            System.out.println ("16. HTTP receive");
            System.out.println ("17. MMS push");
            System.out.println ("18. MMS send");
          }
          else if (name.equalsIgnoreCase(SMS_TRAFFIC)) {
            System.out.println ("10. SMS send");
            System.out.println ("11. SMS receive");
          }
          else if (name.equalsIgnoreCase(SOCKET_TRAFFIC)) {
            System.out.println ("6.  TCP send");
            System.out.println ("7.  TCP receive");
            System.out.println ("8.  UDP send");
            System.out.println ("9.  UDP receive");
          }
          else if (name.equalsIgnoreCase(BLUETOOTH_TRAFFIC)) {
            System.out.println ("13. Bluetooth send");
            System.out.println ("14. Bluetooth receive");
          }
          else if (name.equalsIgnoreCase(BROWSER_HISTORY)) {
          }
          else if (name.equalsIgnoreCase(J2ME)) {
            System.out.println ("12. KVM Memory changed");
          }
          else if (name.equalsIgnoreCase(FILE_SYSTEM)) {
            System.out.println ("5.  File added");
          }
          else if (name.equalsIgnoreCase(SAMPLE)) {
            System.out.println ("19. Sample event");
          }
        } // while
        System.out.println ("\n-1. Exit");
      } // if servers not empty
      else {
        System.out.println ("-1. Exit");
        System.out.println ("\n(no service objects registered)");
      }
    } // synchronized

    //accept user input
    String str = "";
    try {
      BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
      System.out.print("> ");
      str = in.readLine();
    } catch (IOException e) {
        System.err.println ("IOException caught: " + e.getMessage());
        str = "-1";
    }
    //return user input
    return str;
  }


  /**
   * generates and sends a contentRequested event
   * @param adapter The adapter object that receives the
   *  event from the SDK and will forward it to Diagnostics
   */
  private void generateHttpSend (Object adapter) {
  // debug
  // System.out.println ("\n\tIn generateHttpSend()");
    if (adapter instanceof HTTPAdapter) {
      if (trafficEventGenerator != null) {
        DeviceContentRequest httpRequest = trafficEventGenerator.sendHttp();
        if (httpRequest != null) {
          // send event
          ((HTTPAdapter)adapter).contentRequested(httpRequest);
          //debug
//          System.err.println ("sent contentRequested event to adapter");
        }
      }
      else {
        //debug
       System.err.println ("Event generator object is null, cannot send event");
      }
    }
    else {
      //debug
     System.err.println ("Invalid adapter, cannot send event");
    }
  }

  /**
   * generates and sends a contentReceived event
   * @param httpAdapter The adapter object that receives the contentReceived event
   *                    from the SDK and will forward it to Diagnostics
   * @param browserAdapter The adapter object that receives the historyPushed event
   *                    from the SDK and will forward it to Diagnostics
   */
  private void generateHttpReceive ( Object httpAdapter, Object browserAdapter) {
  // debug
  // System.out.println ("\n\tIn generateHttpReceive()");
    if (httpAdapter instanceof HTTPAdapter) {
      if (trafficEventGenerator != null) {
        BrowserResponse httpResponse = trafficEventGenerator.receiveHttp();
        if (httpResponse != null) {
          String url = httpResponse.getLocation();
          // send event
          ((HTTPAdapter)httpAdapter).
            contentReceived(httpResponse,
                            httpResponse.isCacheHit());
          //debug
//          System.err.println ("sent contentReceived event to adapter");

          //if there is a browserHistory listener, send a historyPushed event
          if (browserAdapter != null &&
            browserAdapter instanceof BrowserHistoryAdapter) {
            ((BrowserHistoryAdapter)browserAdapter).historyPushed(url);
            //debug
//            System.err.println ("sent historyPushed event to adapter");
          }
        }
      }
      else {
        //debug
        System.err.println ("Event generator object is null, cannot send event");
      }
    }
    else {
      //debug
      System.err.println ("Invalid adapter, cannot send event");
    }
  }

  /**
   * generates and sends a contentPushed event for MMS push
   * @param adapter The adapter object that receives the
   *  event from the SDK and will forward it to Diagnostics
   */
  private void generateMmsPush(Object adapter) {
  // debug
  // System.out.println ("\n\tIn generateMmsPush()");
    if (adapter instanceof HTTPAdapter) {
      if (trafficEventGenerator != null) {
        PushData mmsData = trafficEventGenerator.pushMms();
        if (mmsData != null) {
          // send event
          ((HTTPAdapter)adapter).contentPushed(mmsData);
          //debug
//          System.err.println ("sent contentPushed event to adapter");
        }
      }
      else {
        //debug
        System.err.println ("Event generator object is null, cannot send event");
      }
    }
    else {
      //debug
      System.err.println ("Invalid adapter, cannot send event");
    }
  }

  /**
   * generates and sends a contentPushed event for MMS send
   * @param adapter The adapter object that receives the
   *  event from the SDK and will forward it to Diagnostics
   */
  private void generateMmsSend(Object adapter) {
  // debug
  // System.out.println ("\n\tIn generateMmsSend()");
    if (adapter instanceof HTTPAdapter) {
      if (trafficEventGenerator != null) {
        PushData mmsData = trafficEventGenerator.sendMms();
        if (mmsData != null) {
          // send event
          ((HTTPAdapter)adapter).contentPushed(mmsData);
          //debug
//          System.err.println ("sent contentPushed event to adapter");
        }
      }
      else {
        //debug
        System.err.println ("Event generator object is null, cannot send event");
      }
    }
    else {
      //debug
      System.err.println ("Invalid adapter, cannot send event");
    }
  }


  /**
   * generates and sends a SMS messageSent event
   * @param adapter The adapter object that receives the
   *  event from the SDK and will forward it to Diagnostics
   */
  private void generateSmsSend (Object adapter) {
  // debug
  // System.out.println ("\n\tIn generateSmsSend()");
    if (adapter instanceof SMSAdapter) {
      if (trafficEventGenerator != null) {
        DeviceMessage smsMsg = trafficEventGenerator.sendSMSMessage();
        if (smsMsg != null) {
          // send event
          ((SMSAdapter)adapter).messageSent(smsMsg);
          //debug
//          System.err.println ("sent messageSent event to adapter");
        }
      }
      else {
        //debug
        System.err.println ("Event generator object is null, cannot send event");
      }
    }
    else {
      //debug
      System.err.println ("Invalid adapter, cannot send event");
    }
  }

  /**
   * generates and sends a SMS messageReceived event
   * @param adapter The adapter object that receives the
   *  event from the SDK and will forward it to Diagnostics
   */
  private void generateSmsReceive (Object adapter) {
  // debug
  // System.out.println ("\n\tIn generateSmsReceive()");
    if (adapter instanceof SMSAdapter) {
      if (trafficEventGenerator != null) {
        DeviceMessage smsMsg = trafficEventGenerator.receiveSMSMessage();
        if (smsMsg != null) {
          // send event
          ((SMSAdapter)adapter).messageReceived(smsMsg);
          //debug
 //         System.err.println ("sent messageReceived event to adapter");
        }
      }
      else {
        //debug
        System.err.println ("Event generator object is null, cannot send event");
      }
    }
    else {
      //debug
      System.err.println ("Invalid adapter, cannot send event");
    }
  }

  /**
   * generates and sends a bluetoothSent event
   * @param adapter The adapter object that receives the
   *  event from the SDK and will forward it to Diagnostics
   */
  private void generateBluetoothSend (Object adapter) {
  // debug
  // System.out.println ("\n\tIn generateBluetoothSend()");
    if (adapter instanceof BluetoothAdapter) {
      if (trafficEventGenerator != null) {
        DeviceBluetoothSend btSend = trafficEventGenerator.sendBluetooth();
        if (btSend != null) {
          // send event
          ((BluetoothAdapter)adapter).bluetoothSent(btSend);
          //debug
//          System.err.println ("sent bluetoothSent event to adapter");
        }
      }
      else {
        //debug
        System.err.println ("Event generator object is null, cannot send event");
      }
    }
    else {
      //debug
      System.err.println ("Invalid adapter, cannot send event");
    }
  }

  /**
   * generates and sends a bluetoothReceived event
   * @param adapter The adapter object that receives the
   *  event from the SDK and will forward it to Diagnostics
   */
  private void generateBluetoothReceive (Object adapter) {
  // debug
  // System.out.println ("\n\tIn generateBluetoothReceive()");
    if (adapter instanceof BluetoothAdapter) {
      if (trafficEventGenerator != null) {
        DeviceBluetoothReceive btReceive = trafficEventGenerator.receiveBluetooth();
        if (btReceive != null) {
          // send event
          ((BluetoothAdapter)adapter).bluetoothReceived(btReceive);
          //debug
//          System.err.println ("sent bluetoothSent event to adapter");
        }
      }
      else {
        //debug
        System.err.println ("Event generator object is null, cannot send event");
      }
    }
    else {
      //debug
      System.err.println ("Invalid adapter, cannot send event");
    }
  }

  /**
   * generates and sends a TCP messageSent event
   * @param adapter The adapter object that receives the
   *  event from the SDK and will forward it to Diagnostics
   */
  private void generateTcpSend (Object adapter) {
  // debug
  // System.out.println ("\n\tIn generateTcpSend()");
    if (adapter instanceof SocketAdapter) {
      if (trafficEventGenerator != null) {
        DeviceSocket socket = trafficEventGenerator.send_receiveTcp();
        if (socket != null) {
          // send event
          ((SocketAdapter)adapter).
            messageSent(socket,
                        trafficEventGenerator.getTcpSendPayload());
          //debug
//          System.err.println ("sent messageSent(TCP) event to adapter");
        }
      }
      else {
        //debug
        System.err.println ("Event generator object is null, cannot send event");
      }
    }
    else {
      //debug
      System.err.println ("Invalid adapter, cannot send event");
    }
  }

  /**
   * generates and sends a TCP messageReceived event
   * @param adapter The adapter object that receives the
   *  event from the SDK and will forward it to Diagnostics
   */
  private void generateTcpReceive (Object adapter) {
  // debug
  // System.out.println ("\n\tIn generateTcpReceive()");
    if (adapter instanceof SocketAdapter) {
      if (trafficEventGenerator != null) {
        DeviceSocket socket = trafficEventGenerator.send_receiveTcp();
        if (socket != null) {
          // send event
          ((SocketAdapter)adapter).
            messageReceived(socket,
                        trafficEventGenerator.getTcpReceivePayload());
          //debug
//          System.err.println ("sent messageReceived(TCP) event to adapter");
        }
      }
      else {
        //debug
        System.err.println ("Event generator object is null, cannot send event");
      }
    }
    else {
      //debug
      System.err.println ("Invalid adapter, cannot send event");
    }
  }

  /**
   * generates and sends a UDP messageSent event
   * @param adapter The adapter object that receives the
   *  event from the SDK and will forward it to Diagnostics
   */
  private void generateUdpSend (Object adapter) {
  // debug
  // System.out.println ("\n\tIn generateUdpSend()");
    if (adapter instanceof SocketAdapter) {
      if (trafficEventGenerator != null) {
        DeviceSocket socket = trafficEventGenerator.send_receiveUdp();
        if (socket != null) {
          // send event
          ((SocketAdapter)adapter).
            messageSent(socket,
                        trafficEventGenerator.getUdpSendPayload());
          //debug
//          System.err.println ("sent messageSent(UDP) event to adapter");
        }
      }
      else {
        //debug
        System.err.println ("Event generator object is null, cannot send event");
      }
    }
    else {
      //debug
      System.err.println ("Invalid adapter, cannot send event");
    }
  }

  /**
   * generates and sends a UDP messageReceived event
   * @param adapter The adapter object that receives the
   *  event from the SDK and will forward it to Diagnostics
   */
  private void generateUdpReceive (Object adapter) {
  // debug
  // System.out.println ("\n\tIn generateUdpReceive()");
    if (adapter instanceof SocketAdapter) {
      if (trafficEventGenerator != null) {
        DeviceSocket socket = trafficEventGenerator.send_receiveUdp();
        if (socket != null) {
          // send event
          ((SocketAdapter)adapter).
            messageReceived(socket,
                        trafficEventGenerator.getUdpReceivePayload());
          //debug
//          System.err.println ("sent messageReceived(UDP) event to adapter");
        }
      }
      else {
        //debug
        System.err.println ("Event generator object is null, cannot send event");
      }
    }
    else {
      //debug
      System.err.println ("Invalid adapter, cannot send event");
    }
  }

  /**
   * generates and sends a memoryChanged event
   * @param adapter The adapter object that receives the
   *  event from the SDK and will forward it to Diagnostics
   */
  private void generateMemoryChanged (Object adapter) {
  // debug
  // System.out.println ("\n\tIn generateMemoryChanged()");
    if (adapter instanceof J2MEAdapter) {
      if (midpEventGenerator != null) {
        DeviceMemory memory = midpEventGenerator.changeMemory();
        if (memory != null) {
          // send event
          ((J2MEAdapter)adapter).KVMMemoryChanged(memory);
          //debug
  //        System.err.println ("sent KVMMemoryChanged event to adapter");
        }
      }
      else {
        System.err.println ("event object was null...");
      }
    }
    else {
      //debug
      System.err.println ("Invalid adapter, cannot send event");
    }
  }

  /**
   * generates and sends a fileAdded event
   * @param adapter The adapter object that receives the
   *  event from the SDK and will forward it to Diagnostics
   */
  private void generateFileAdded (Object adapter) {
  // debug
  // System.out.println ("\n\tIn generateFileAdded()");
    if (midpEventGenerator != null) {
      DeviceFile newFile = midpEventGenerator.addFile();
      if (newFile != null) {
        // send fileAdded event
        ((FileSystemAdapter)adapter).fileAdded(newFile);
        // send fileUpdated events for directories
        ((FileSystemAdapter)adapter).
          fileUpdated(midpEventGenerator.getApplDir());
        ((FileSystemAdapter)adapter).
          fileUpdated(midpEventGenerator.getRootDir());
      }
      else {
        System.err.println ("event object was null...");
      }
    }
  }

  /**
   * generates and sends a sampleEventGenerated event
   * @param adapter The adapter object that receives the
   *  event from the SDK and will forward it to Diagnostics
   */
  private void generateSampleEvent (Object adapter) {
  // debug
  // System.out.println ("\n\tIn generateSampleEvent()");
    if (sampleEventGenerator != null) {
      DeviceSampleData sample = sampleEventGenerator.getNextSample();
      if (sample != null) {
        // send event
        ((SampleAdapter)adapter).
          sampleEventGenerated(sample);
      }
      else {
        System.err.println ("event object was null...");
      }
    }
  }


  private static SdkMain sdkInstance  = null;
  private Properties services = null;
  private Map servers = null;
  private Map windowProperties = null;
  private static AdapterManager adapter = null;
  private static PropertyListener propertyListener = null;
  private static SampleEventGenerator sampleEventGenerator = null;
  private static MIDPEventGenerator midpEventGenerator = null;
  private static TrafficEventGenerator trafficEventGenerator = null;
  private int pointX = 100;
  private int pointY = 10;
  private int titleChangeCount = 1;

  private static final String JAR_NAME =          "sdkSimulator.jar";
  private static final String SERVICES_FILE_NAME = "resources/services.properties";

  private static final String SAMPLE_NAME=        "Sample SDK";
  private static final String HTTP_TRAFFIC =      "HttpTraffic";
  private static final String BROWSER_HISTORY =   "BrowserHistory";
  private static final String SMS_TRAFFIC =       "SmsTraffic";
  private static final String SOCKET_TRAFFIC =    "SocketTraffic";
  private static final String BLUETOOTH_TRAFFIC = "BluetoothTraffic";
  private static final String J2ME =              "J2ME";
  private static final String FILE_SYSTEM =       "FileSystem";
  private static final String SAMPLE =            "Sample";




}
