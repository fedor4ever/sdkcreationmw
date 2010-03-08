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

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

/**
 *
 * AbstractConfigurator
 */
public abstract class AbstractConfigurator implements Configurator {
	protected boolean dispatch = false;
	protected List listenerList = new ArrayList();
	
	public void addConfiguratorListener(ConfiguratorListener l) {
		if(!listenerList.contains(l))
			listenerList.add(l);
	}
	
	public void removeConfiguratorListener(ConfiguratorListener l) {
		if(listenerList.contains(l))
			listenerList.remove(l);		
	}
	
	public void notifyPropertiesChanged(String key) {
		Iterator i = listenerList.iterator();
		while(i.hasNext()) {
			((ConfiguratorListener)i.next()).propertiesSetChanged(key);
		}
	}
	
	public void notifyPropertyChanged(String key, String value) {
		Iterator i = listenerList.iterator();
		while(i.hasNext()) {
			((ConfiguratorListener)i.next()).propertyChanged(key, value);
		}		
	}
}
