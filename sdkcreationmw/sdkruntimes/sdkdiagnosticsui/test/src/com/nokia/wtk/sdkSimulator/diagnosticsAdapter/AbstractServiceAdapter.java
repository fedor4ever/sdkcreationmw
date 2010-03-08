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


package com.nokia.wtk.sdkSimulator.diagnosticsAdapter;

// com.lang.reflect
import java.lang.reflect.Method;

// java.util
import java.util.Iterator;
import java.util.List;
import java.util.LinkedList;
import java.util.Vector;

/**
 * <p>Title: AbstractServiceAdapter </p>
 * <p>Description: An abstract class that implements methods common to all
 *    service adapter objects </p>
 * <p>Company: Nokia</p>
 * @version 1.0
 */

public abstract class AbstractServiceAdapter {

  /**
   * Constructor
   *@param sdk a reference to the SDK object
   *@param name the name of the service adapter object
   */
  public AbstractServiceAdapter(SdkInterface sampleSdk, String serviceName) {
    this.sampleSdk = sampleSdk;
    this.serviceName = serviceName;
    listeners = new Vector();
  }

/**
 * Adds the specified listener object
 *@param listener the listener object to add
 *@return success or failure
 */
  public void addListener( Object listener ) {
    // if this is the first external listener in the list,
    // first register yourself with the SDK to receive
    // events and info
    if (listeners.isEmpty()) {
      registerWithSdk();
    }
    listeners.addElement( listener );
  }

/**
 * Removes the specified listener object
 *@param listener the listener object to remove
 *@return success or failure
 */
  public boolean removeListener( Object listener ) {
    boolean result = listeners.removeElement( listener );
    // if this is the last and only external listener in the list,
    // unregister yourself with the SDK (optional).
    if (result == true) {
      if (listeners.isEmpty()) {
          unregisterWithSdk();
      }
    }
    return result;
  }

/**
 * Returns the list of listeners
 * @return the list of listeners in an Iterator object
 */
  public Iterator listeners() {
    return listeners.iterator();
  }

/**
 * Returns the number of listeners
 * @return number of listeners
 */
  public int numberOfListeners() {
    return listeners.size();
  }

  /**
  * registers this object with the SDK to receive events/information.
  * This method must be implemented in the derived classes, since each
  * derived object is interested in a specific type of events/information
  * from the SDK.
  * Note: this method can be invoked by the derived class either at constructor
  * time (always receive events from SDK), or only when the first external
  * listener registers with the adapter object.In this implementation,
  * we register only when the first external listener registers.
  */
  protected abstract void registerWithSdk();

  /**
  * unregisters this object with the SDK.
  * This method must be implemented in the derived classes, since each
  * derived object is interested in a specifik type of events/information
  * from the SDK. The implementation of the method by the derived class
  * may be empty, meaning the object never unregisters.
  */
  protected abstract void unregisterWithSdk();


  /**
   * Specify an object and a method to call when the queue next becomes empty.
   * This method does not modify the behavior of the background processing
   * thread directly, instead it requests this behavior change. Until a new
   * callback is added via addCallback the thread will continue to process
   * the contents of the queue as usual. This method must be called before
   * the callback completion notification is requested for is added. There is
   * no guarantee that the specified method will be called immediately after the
   * added callback is processed. It will not be called until there are no
   * more callbacks on the queue to process.
   * @param obj the Object to invoke the specified method on.
   * @param method the Method to invoke when the queue becomes empty.
   */
  public static synchronized void callWhenComplete( Object obj, Method method ) {
    callWhenComplete = REQUESTED;
    callObj = obj;
    callMethod = method;
  }

  /**
   * Returns an instance of the Method class based on the name provided. The
   * name will be the methodName specified when addCallback is called. In this
   * way the subclass can completely control the selection of the appropriate
   * method to callback.
   * @param name a Method identifying name.
   * @return a Method instance.
   */
  protected abstract Method methodForName( String name );

  /**
   * Adds a callback to the queue. This should be called by concrete
   * implementations. Each callback received will be placed on the queue to be
   * later dispatched to the registered listeners. The methodName specified
   * must uniquely identify the listener interface method to call.
   * @param methodName a name which can be used to uniquely identify a plugin
   * interface method. @link methodForName
   * @param arguments an array of Objects representing the arguments passed to
   * the callback, these will be passed on to the plugin interface callback.
   */
  protected void addCallback( String methodName, Object [] arguments ) {
    synchronized ( callbackQueue ) {
      synchronized ( AbstractServiceAdapter.class ) {
        if ( callWhenComplete == REQUESTED ) {
          callWhenComplete = PENDING;
        }
      }
      callbackQueue.add( new CallbackDescription( this,
        ((List)listeners.clone() ).iterator(), methodName, arguments ) );
      callbackQueue.notifyAll();
    }
  }

  /**
   * Returns the next CallbackDescription from the queue, blocking until there
   * is something to return.
   * @return the next CallbackDescription from the queue.
   */
  private static CallbackDescription getNextCallbackFromQueue() {
    synchronized ( callbackQueue ) {
      while ( callbackQueue.size() == 0 ) {
        synchronized ( AbstractServiceAdapter.class ) {
          if ( callWhenComplete == PENDING ) {
            try {
              callMethod.invoke( callObj, new Object[0] );
            } catch ( Exception e ) {
              // Ignore any exceptions, there is nothing we can do
            } finally {
              callWhenComplete = UNSET;
              callObj = null;
              callMethod = null;
            }
          }
        }
        try {
          callbackQueue.wait();
        } catch ( Exception e ) {
          System.err.println( "Exception in getNextCallbackFromQueue: " + e );
          e.printStackTrace();
        }
      }

      return (CallbackDescription)callbackQueue.remove( 0 );
    }
  }

  /**
   * Sends the next callback found on the callback queue. The CallbackDescription
   * contains the information necessary to invoke the callback it represents.
   */
  private static void sendNextCallback() {
    try {
      CallbackDescription callback = getNextCallbackFromQueue();
      Iterator iterator = callback.getListeners();
      Method method = callback.getServiceAdapter().methodForName(
        callback.getMethodName() );
      if ( method != null ) {
        while ( iterator.hasNext() ) {
          try {
            Object listener = iterator.next();
            method.invoke( listener, callback.getArguments() );
          } catch ( Exception e ) {
            System.err.println( "Exception in sendNextCallback: " + e );
            System.err.println( "Method was: " + method.getName() );
          }
        }
      }
    } catch ( Exception e ) {
      System.err.println( "Exception 2 in sendNextCallback: " + e );
    }
  }

  private static final int UNSET = 0;
  private static final int REQUESTED = 1;
  private static final int PENDING = 2;

  private static LinkedList callbackQueue;
  private static int callWhenComplete = UNSET;
  private static Object callObj = null;
  private static Method callMethod = null;
  private Vector listeners;

  /**
   * Start the message handling thread, there will only be once instance of
   * it and only one instance of the queue to ensure order preservation of the
   * callbacks.
   */
  static {
    callbackQueue = new LinkedList();
    ( new MessageThread() ).start();
  }

  /**
   * <p>Title: CallbackDescription</p>
   * <p>Description: inner class used in the queueing of callback messages.</p>
   * <p>Company: Nokia</p>
   * @version 1.0
   */
  private class CallbackDescription {
    /**
     * Constructs a new CallbackDescription instance.
     * @param adapter the instance of AbstractServiceAdapter this callback was
     * created by.
     * @param listeners the listeners the callback will be sent to.
     * @param methodName the method called.
     * @param arguments the arguments sent to the method.
     */
    public CallbackDescription( AbstractServiceAdapter adapter, Iterator listeners,
      String methodName, Object [] arguments ) {
      this.adapter = adapter;
      this.listeners = listeners;
      this.methodName = methodName;
      this.arguments = arguments;
    }

    /**
     * returns a reference to this service adapter object
     * @return the service adapter object
     */
    public AbstractServiceAdapter getServiceAdapter() {
      return adapter;
    }

    /**
     * returns the list of listeners who listen to events that this
     * object receives from the SK
     * @return the list of listeners
     */
    public Iterator getListeners() {
      return listeners;
    }

    public String getMethodName() {
      return methodName;
    }

    public Object [] getArguments() {
      return arguments;
    }

    protected AbstractServiceAdapter adapter;
    protected Iterator listeners;
    private String methodName;
    private Object [] arguments;
  }

  private static class MessageThread extends Thread {
    public MessageThread() {
      super( SDK_SERVICE_MESSAGE_THREAD );
      setPriority( Thread.MIN_PRIORITY );
      setDaemon( true );
    }

    public void run() {
      while ( true ) {
        sendNextCallback();
      }
    }

    private static final String SDK_SERVICE_MESSAGE_THREAD =
      "AbstractServiceAdapter MessageThread";
  }

  protected SdkInterface sampleSdk;
  protected String serviceName;

}
