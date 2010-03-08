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



package com.nokia.wtk.util;

/**
 * An abstract adapter class for receiving property set events.
 * The methods in this class are empty. This class exists as
 * convenience for creating listener objects.
 *
 * @see Property
 * @see PropertySet
 * @see PropertySetListener
 */
public abstract class PropertySetAdapter implements PropertySetListener {

    /**
     * Invoked when a child property set is added
     * @param ps the property set listener is attached to
     * @param path the full path of the new property set, relative to ps
     */
    public void propertySetAdded(PropertySet ps, String path) {}

    /**
     * Invoked when a child property set is removed
     * @param ps the property set listener is attached to
     * @param child the property set that has been removed
     * @param path the full path of the removed property set, relative to ps
     */
    public void propertySetRemoved(PropertySet ps, PropertySet child, String path) {}

    /**
     * Invoked when a property is added
     * @param ps the property set listener is attached to
     * @param path the full path of the new property, relative to ps
     */
    public void propertyAdded(PropertySet ps, String path) {}

    /**
     * Invoked when a property is removed
     * @param ps the property set listener is attached to
     * @param p the property that has been removed
     * @param path the full path of the removed property, relative to ps
     */
    public void propertyRemoved(PropertySet ps, Property p, String path) {}

    /**
     * Invoked when something within a property set hierarhy has changed
     * @param ps the property set listener is attached to
     */
    public void propertySetChanged(PropertySet ps) {}
}
