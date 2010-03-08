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
 * The top-level interface for all mobile internet device simulators.
 * This interface must be implemented by all device simulators,
 * if they are be recognized as device simulator plugins by their
 * hosting application.  Device implementations must also provide a
 * default constructor to allow instantiation by a hosting application.
 *
 */
public interface Device {

    /**
     * Initializes this device after instantiation.
     * Callers must be prepared for any possible runtime exception
     * that may result from calling this method.  Such an exception
     * shall be regarded as an unsuccessful attempt to open the device
     * and will result in the device being removed from memory
     * with no further method calls made to the device.
     * @exception       java.lang.RuntimeException
     *                  if an error occured while attempting to open
     *                  this device.
     */
    public void openDevice() throws RuntimeException;

    /**
     * Terminates this device.
     * This should shut down any spawned threads or processes
     * and will be called just prior to discarding the device
     * from memory.
     */
    public void closeDevice();

    /**
     * Obtains a specified service that is supported by this device.
     * This method allows such services that are implemented by this
     * device (or one of its delegates) to be made available to the
     * hosting application.
     * <p>
     * Multiple calls must not result in multiple instances, thus
     * allowing this method to also be used to test for the existance
     * of a service.
     *
     * @param type      the type of service to obtain.
     * @return          the service implementation or <code>null</code>
     *                  if not supported by this device.
     */
    public Object getInstanceOf(Class type);

    /**
     * Registers a service instance for use by this device.
     * This method allows such services that are implemented by the
     * hosting application to be made available to this device (or
     * one of its delegates).  If the service instance provided is
     * <code>null</code>, then the service will be unregistered
     * with this device.
     *
     * @param type      the type of interface to set.
     * @param instance  the interface implementation to register
     *                  or <code>null</code> if unregistering.
     * @return          <code>true</code> if the interface is supported
     *                  by this device.  Otherwise, the interface is not
     *                  supported by this device.
     */
    public boolean setInstanceOf(Class type, Object instance);


    //
    // Properties that are implemented only by a host application.
    // Device implementations must not replace or alter them.
    //
    
    /** The directory containing the device jarfile and related resources. (Type = File) */
    public static final String DeviceDirectory_Property = "device.directory";
    
}   // End of Device
