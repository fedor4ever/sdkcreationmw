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



package com.nokia.epdt.plugins.s60.platsec;

/**
 *
 * SecEpocIniConstants
 */
public interface SecEpocIniConstants {
  // Platform Security keys
    public static String PLATFORM_SECURITY = "PLATFORM_SECURITY";
    public static String PLATSEC_DIAGNOSTICS = "PlatSecDiagnostics";
    public static String PLATSEC_ENFORCE_SYS_BIN = "PlatSecEnforceSysBin";
    public static String PLATSEC_PROCESS_ISOLATION = "PlatSecProcessIsolation";
    public static String PLATSEC_ENFORCEMENT = "PlatSecEnforcement";
    public static String PLAT_SEC_DISABLED_CAPS = "PlatSecDisabledCaps";
    // Capabilities
    public static String LOCAL_SERVICES = "LocalServices";
    public static String READ_USER_DATA = "ReadUserData";
    public static String WRITE_USER_DATA = "WriteUserData";
    public static String USER_ENVIRONMENT = "UserEnvironment";
    public static String NETWORK_SERVICES = "NetworkServices";
    public static String LOCATION = "Location";
    public static String MULTIMEDIA_DD = "MultiMediaDD";
    public static String READ_DEVICE_DATA = "ReadDeviceData";
    public static String WRITE_DEVICE_DATA = "WriteDeviceData";
    public static String POWER_MGMT = "PowerMgmt";
    public static String TRUSTED_UI = "TrustedUI";
    public static String PROT_SERV = "ProtServ";
    public static String NETWORK_CONTROL = "NetworkControl";
    public static String SW_EVENT = "SwEvent";
    public static String DRM = "DRM";
    public static String SURROUNDINGS_DD = "SurroundingsDD";
    public static String TCB = "TCB";
    public static String COMM_DD = "CommDD";
    public static String DISK_ADMIN = "DiskAdmin";
    public static String ALL_FILES = "AllFiles";
    public static String[] CAPABILITIES = { LOCAL_SERVICES, READ_USER_DATA, WRITE_USER_DATA, USER_ENVIRONMENT, 
			      NETWORK_SERVICES, LOCATION, MULTIMEDIA_DD, READ_DEVICE_DATA, 
			      WRITE_DEVICE_DATA, POWER_MGMT, TRUSTED_UI, PROT_SERV,
			      NETWORK_CONTROL, SW_EVENT, DRM, SURROUNDINGS_DD,
			      TCB, COMM_DD, DISK_ADMIN, ALL_FILES };
    public static String NO_CAPABILITIES = "NONE";
  
}
