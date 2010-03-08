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



package com.nokia.epdt.core.config;

import java.util.Properties;

/**
 * 
 * Configurator
 */
public interface Configurator {
	public void register(Object user);
	public void unregister(Object user);
	public boolean dispatch(Object user);
	public void addConfiguratorListener(ConfiguratorListener l);
	public void removeConfiguratorListener(ConfiguratorListener l);
	public void setProperty(String key, String value);
	public void setProperties(Properties p);
	public String getProperty(String key);
	public Properties getProperties(String key);
}
