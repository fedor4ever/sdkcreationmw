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
 * This listener is notified of structural changes in the property set (as
 * opposed to the value changes of the individual properties it contains).
 * The set of events include addition and removal of properties or property
 * sets anywhere with the property set hierarchy, as well as "bulk change"
 * notification which signals the end of a "bulk" update, such as load from
 * a file for example.
 *
 *
 * @see Property
 * @see PropertySet
 */
public interface PropertySetListener {

    /**
     * Invoked after a property set has been added to the property set
     * hierarchy.
     *
     * @param ps the source of the event
     * @param path the full path of the new property set, relative to ps
     */
    public void propertySetAdded(PropertySet ps, String path);

    /**
     * Invoked after a child property set has been removed from the property
     * set hierarchy.
     *
     * @param ps the source of the event
     * @param child the property set that has been removed
     * @param path the full path of the removed property set, relative to ps
     */
    public void propertySetRemoved(PropertySet ps, PropertySet child, String path);

    /**
     * Invoked after a property has been added to the property set hierarchy.
     *
     * @param ps the source of the event
     * @param path the full path of the new property, relative to ps
     */
    public void propertyAdded(PropertySet ps, String path);

    /**
     * Invoked after a property has been removed from the property set
     * hierarchy.
     *
     * @param ps the source of the event
     * @param p the property that has been removed
     * @param path the full path of the removed property, relative to ps
     */
    public void propertyRemoved(PropertySet ps, Property p, String path);

    /**
     * Invoked after a property within the property set hierarchy has changed.
     * In case of "bulk" updates, only one such notification will be fired
     * after the update is complete.
     *
     * @param ps the source of the event
     */
    public void propertySetChanged(PropertySet ps);
}
