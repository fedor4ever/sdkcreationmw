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

/* java.beans */
import java.beans.PropertyChangeListener;

/**
 * A property interface
 *
 */
public interface Property {

    /**
     * Returns the name of this property. A property name cannot contain
     * the . (dot) character.
     *
     * @return The programmatic name of the property
     */
    public String getName();

    /**
     * Returns the type of the property value.
     * @return the type of the property value
     */
    public Class getType();

    /**
     * Returns the default value of this property.
     * @return The default value.
     */
    public Object getDefaultValue();

    /**
     * Determines whether a property may be included in an archive.
     * This does not imply or override any implicit requirements for archiveability.
     * @return <code>true</code> if the property may be included in an archive.
     */
    public boolean isArchiveable();

    /**
     * Returns <code>true</code> if this property is readonly. This
     * prevents PropertySet from setting value of this property. It's
     * also assumed that a read-only property never fires property change
     * events
     */
    public boolean isReadOnly();

    /**
     * Reads the value of a property.
     * @return the property value or <code>null</code> if not defined
     */
    public Object getValue();

    /**
     * Writes the value of a property.
     *
     * @param value the value of the property
     *
     * @throws      UnsupportedOperationException
     *              If the value of this property cannot be set
     * @throws      ClassCastException
     *              If the type of the object does not match the expectation
     * @throws      IllegalArgumentExceptions
     *              IF the type is right but the value is not acceptable
     */
    public void setValue(Object value);

    /**
     * Returns the text representation of the property value. If property
     * is not defined, returns <code>null</code>
     *
     * @return  the text representation of the property value.
     */
    public String getValueAsText();

    /**
     * Sets the value of this property.
     * @param value the text representation of the property value to set.
     *
     * @throws      UnsupportedOperationException
     *              If this method is not suported by the property
     * @throws      IllegalArgumentException
     *              If the text cannot be converted into the object
     */
    public void setValueAsText(String value);

    /**
     * Adds property change listener to this property.
     * The property must notify the listeners every time after its value has
     * changed. This property <strong>must be</strong> the source of such
     * property change events.
     */
    public void addPropertyChangeListener(PropertyChangeListener l);

    /**
     * Removes property change listener from this property
     */
    public void removePropertyChangeListener(PropertyChangeListener l);
}
