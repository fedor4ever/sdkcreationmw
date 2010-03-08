/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * $Id: XRpcDataStruct.java,v 1.15 2005/08/26 22:45:26 slava Exp $
 */

// ========================================================================
//  Name        : XRpcDataStruct.java
//  Part of     : 
//  Description : 
//  Version     : 
//
//  Copyright (c) 2000 - 2005 Nokia Corporation.
//  This material, including documentation and any related
//  computer programs, is protected by copyright controlled by
//  Nokia Corporation. All rights are reserved. Copying,
//  including reproducing, storing, adapting or translating, any
//  or all of this material requires the prior written consent of
//  Nokia Corporation. This material also contains confidential
//  information which may not be disclosed to others without the
//  prior written consent of Nokia Corporation.
// ========================================================================


package com.nokia.xml.rpc;

/**
 * Represents an XRPC data structure
 *
 */
public
class       XRpcDataStruct
extends     XRpcBasicDataContainer
implements  XRpcMutableDataElement {

    /**
     * Creates empty <code>XRpcDataStruct</code>
     */
    public XRpcDataStruct() {
    }

    /**
     * Creates empty <code>XRpcDataStruct</code> of specified type
     *
     * @param type the type attribute of the structure
     */
    public XRpcDataStruct(String type) {
        setAttribute(XRpcAttr.TYPE, type);
    }

    /**
     * Creates empty <code>XRpcDataStruct</code> containing the specified data.
     *
     * @param data the data to add to the structure
     */
    public XRpcDataStruct(XRpcDataElement[] data) {
        addElements(data);
    }

    /**
     * Creates empty <code>XRpcDataStruct</code> containing the specified data
     *
     * @param data the data to add to the structure
     * @param type the type attribute of the structure
     */
    public XRpcDataStruct(XRpcDataElement[] data, String type) {
        this(type);
        addElements(data);
    }

    /**
     * Creates empty <code>XRpcDataStruct</code> containing the specified data
     *
     * @param data the data to add to the structure
     * @param type the type attribute of the structure
     * @param name the name attribute of the structure
     */
    public XRpcDataStruct(XRpcDataElement[] data, String type, String name) {
        this(data, type);
        setName(name);
    }

    /**
     * Returns {@link XRpcDataType#STRUCT}
     *
     * @return {@link XRpcDataType#STRUCT}
     */
    public XRpcDataType getDataType() {
        return XRpcDataType.STRUCT;
    }

    /**
     * Returns the type of the structure
     *
     * @return the type of the structure
     */
    public String getStructType() {
        return getAttributeValue(XRpcAttr.TYPE);
    }

    /**
     * Adds field to the structure.
     *
     * @param field the data field to add
     * @param name the name of the field
     */
    public void addField(XRpcMutableDataElement field, String name) {
        if (field == null) field = new XRpcNull();
        field.setAttribute(XRpcAttr.NAME, name);
        addElement(field);
    }

    /**
     * Adds a field of type <code>byte</code>.
     *
     * @param value the value of the field to add
     * @param name  the name of the field
     * @return      the created field
     */
    public XRpcByte addByteField(byte value, String name) {
        XRpcByte field = new XRpcByte(value);
        addField(field, name);
        return field;
    }

    /**
     * Adds a field of type <code>short</code>.
     *
     * @param value the value of the field to add
     * @param name  the name of the field
     * @return      the created field
     */
    public XRpcShort addShortField(short value, String name) {
        XRpcShort field = new XRpcShort(value);
        addField(field, name);
        return field;
    }

    /**
     * Adds a field of type <code>int</code>.
     *
     * @param value the value of the field to add
     * @param name  the name of the field
     * @return      the created field
     */
    public XRpcInt addIntField(int value, String name) {
        XRpcInt field = new XRpcInt(value);
        addField(field, name);
        return field;
    }

    /**
     * Adds a field of type <code>long</code>.
     *
     * @param value the value of the field to add
     * @param name  the name of the field
     * @return      the created field
     */
    public XRpcLong addLongField(long value, String name) {
        XRpcLong field = new XRpcLong(value);
        addField(field, name);
        return field;
    }

    /**
     * Adds a field of type <code>long</code>.
     *
     * @param value the value of the field to add
     * @param name  the name of the field
     * @return      the created field
     */
    public XRpcBoolean addBooleanField(boolean value, String name) {
        XRpcBoolean field = new XRpcBoolean(value);
        addField(field, name);
        return field;
    }

    /**
     * Adds a field of type <code>string</code>.
     *
     * @param value the value of the field to add
     * @param name  the name of the field
     * @return      the created field
     */
    public XRpcString addStringField(String value, String name) {
        XRpcString field = null;
        if (value != null) field = new XRpcString(value);
        addField(field, name);
        return field;
    }

    /**
     * Adds a field of type <code>string</code>.
     *
     * @param value the value of the field to add
     * @param name  the name of the field
     * @return      the created field
     */
    public XRpcBinaryData addBinaryField(byte [] value, String name) {
        XRpcBinaryData field = null;
        if (value!= null) field = new XRpcBinaryData(value);
        addField(field, name);
        return field;
    }

    /**
     * Adds an array field to the structure.
     *
     * @param data the array of data to add
     * @param name the name of the array element
     * @return      the created field
     */
    public XRpcDataArray addArrayField(XRpcDataElement [] data, String name) {
        XRpcDataArray field = null;
        if (data != null) field = new XRpcDataArray(data);
        addField(field, name);
        return field;
    }

    /**
     * Returns the named field, or <code>null</code> if no such field
     * in this container.
     *
     * @param name the name of the field to get
     * @return  the field of the structure that has the specified name,
     *          <code>null</code> if not found
     */
    public XRpcDataElement getField(String name) {
        int n = getElementCount();
        for (int i=0; i<n; i++) {
            XRpcDataElement data = getDataElement(i);
            if (name.equals(data.getAttributeValue(XRpcAttr.NAME))) {
                return data;
            }
        }
        return null;
    }

    /**
     * Returns the named field of specified type, or <code>null</code>
     * if no field in this container match the criteria.
     *
     * @param name the name of the field to get
     * @param type the data type of the field to get
     * @return  the field of the structure that has the specified name and
     *          type, <code>null</code> if not found
     */
    public XRpcDataElement getField(String name, XRpcDataType type) {
        int n = getElementCount();
        for (int i=0; i<n; i++) {
            XRpcDataElement data = getDataElement(i);
            if (name.equals(data.getAttributeValue(XRpcAttr.NAME)) &&
                data.getDataType() == type) {
                return data;
            }
        }
        return null;
    }

    /**
     * Returns the named field of type <code>byte</code>, or <code>null</code>
     * if no field in this container match the criteria.
     *
     * @param name the name of the field to get
     * @return  the field of type <code>byte</code> that has the specified
     *          name, or <code>null</code> if not found
     */
    public XRpcByte getByteField(String name) {
        return (XRpcByte)getField(name, XRpcDataType.BYTE);
    }

    /**
     * Returns the named field of type <code>short</code>, or <code>null</code>
     * if no field in this container match the criteria.
     *
     * @param name the name of the field to get
     * @return  the field of type <code>short</code> that has the specified
     *          name, or <code>null</code> if not found
     */
    public XRpcShort getShortField(String name) {
        return (XRpcShort)getField(name, XRpcDataType.SHORT);
    }

    /**
     * Returns the named field of type <code>int</code>, or <code>null</code>
     * if no field in this container match the criteria.
     *
     * @param name the name of the field to get
     * @return  the field of type <code>int</code> that has the specified
     *          name, or <code>null</code> if not found
     */
    public XRpcInt getIntField(String name) {
        return (XRpcInt)getField(name, XRpcDataType.INT);
    }

    /**
     * Returns the named field of type <code>long</code>, or <code>null</code>
     * if no field in this container match the criteria.
     *
     * @param name the name of the field to get
     * @return  the field of type <code>long</code> that has the specified
     *          name, or <code>null</code> if not found
     */
    public XRpcLong getLongField(String name) {
        return (XRpcLong)getField(name, XRpcDataType.LONG);
    }

    /**
     * Returns the named field of type <code>boolean</code>, or
     * <code>null</code> if no field in this container match the criteria.
     *
     * @param name the name of the field to get
     * @return  the field of type <code>boolean</code> that has the specified
     *          name, or <code>null</code> if not found
     */
    public XRpcBoolean getBooleanField(String name) {
        return (XRpcBoolean)getField(name, XRpcDataType.BOOLEAN);
    }

    /**
     * Returns the value of the named field of type <code>boolean</code>, or
     * <code>null</code> if no field in this container match the criteria.
     *
     * @param name the name of the field to get
     * @return  the field of type <code>boolean</code> that has the specified
     *          name, or <code>null</code> if not found
     */
    public Boolean getBooleanData(String name) {
        XRpcBoolean b = getBooleanField(name);
        return ((b == null) ? null : b.getBoolean());
    }

    /**
     * Returns the value of the named field of type <code>boolean</code>, or
     * <code>def</code> if no field in this container match the criteria.
     *
     * @param name the name of the field to get
     * @param def the default value to return
     * @return  the field of type <code>boolean</code> that has the specified
     *          name, or <code>def</code> if not found
     */
    public boolean getBooleanData(String name, boolean def) {
        XRpcBoolean b = getBooleanField(name);
        return ((b == null) ? def : b.getBoolean().booleanValue());
    }

    /**
     * Returns the named field of type <code>string</code>, or
     * <code>null</code> if no field in this container match the criteria.
     *
     * @param name the name of the field to get
     * @return  the field of type <code>string</code> that has the specified
     *          name, or <code>null</code> if not found
     */
    public XRpcString getStringField(String name) {
        return (XRpcString)getField(name, XRpcDataType.STRING);
    }

    /**
     * Returns the value of the named field of type <code>string</code>,
     * or <code>null</code> if no field in this container match the criteria.
     *
     * @param name the name of the field to get
     * @return  the value of the named field of type <code>string</code>,
     *          or <code>null</code> if no such field is found.
     */
    public String getStringData(String name) {
        XRpcString str = getStringField(name);
        return ((str == null) ? null : str.getString());
    }

    /**
     * Returns the named field of type <code>base64</code>, or <code>null</code>
     * if no field in this container match the criteria.
     *
     * @param name the name of the field to get
     * @return  the field of type <code>base64</code> that has the specified
     *          name, or <code>null</code> if not found
     */
    public XRpcBinaryData getBinaryField(String name) {
        return (XRpcBinaryData)getField(name, XRpcDataType.BINARY);
    }

    /**
     * Returns the named field of type <code>array</code>, or <code>null</code>
     * if no field in this container match the criteria.
     *
     * @param name the name of the field to get
     * @return  the field of type <code>array</code> that has the specified
     *          name, or <code>null</code> if not found
     */
    public XRpcDataArray getArrayField(String name) {
        return (XRpcDataArray)getField(name, XRpcDataType.ARRAY);
    }

    /**
     * Returns the named field of type <code>struct</code>, or
     * <code>null</code> if no field in this container match the criteria.
     *
     * @param name the name of the field to get
     * @return  the named field of type <code>struct</code>, or
     *          <code>null</code> if no such field is found
     */
    public XRpcDataStruct getStructField(String name) {
        return (XRpcDataStruct)getField(name, XRpcDataType.STRUCT);
    }

    /**
     * Returns the named field of type <code>struct</code> of specified type,
     * or <code>null</code> if no field in this container match the criteria.
     *
     * @param name the name of the field to get
     * @param type the expected type of the structure
     * @return  the named field of type <code>struct</code> of specified type,
     *          or <code>null</code> if no such field is found
     */
    public XRpcDataStruct getStructField(String name, String type) {
        XRpcDataStruct struct = getStructField(name);
        if (struct != null) {
            if (type == null ||
                type.equals(struct.getAttributeValue(XRpcAttr.TYPE))) {
                return struct;
            }
        }
        return null;
    }

    /**
     * Returns the value of the named field of type <code>base64</code>, or
     * <code>null</code> if no field in this container match the criteria.
     *
     * @param name the name of the field to get
     * @return  the value of the named field of type <code>base64</code>,
     *          or <code>null</code> if no such field is found
     */
    public byte [] getBinaryData(String name) {
        XRpcBinaryData data = getBinaryField(name);
        return ((data == null) ? null : data.toByteArray());
    }
}
