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
 * $Id: XRpcDataArray.java,v 1.6 2005/08/26 22:45:26 slava Exp $
 */

// ========================================================================
//  Name        : XRpcDataArray.java
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
 * Represents an array
 *
 */
public
class       XRpcDataArray
extends     XRpcBasicDataContainer
implements  XRpcMutableDataElement {

    /**
     * Creats an empty array.
     */
    public XRpcDataArray() {
    }

    /**
     * Creates an array and fills it with data.
     *
     * @param data the elements of the array
     */
    public XRpcDataArray(XRpcDataElement[] data) {
        addData(data);
    }

    /**
     * Creates an array and fills it with data.
     *
     * @param data the elements of the array
     * @param name the name attribute of the array
     */
    public XRpcDataArray(XRpcDataElement[] data, String name) {
        this(data);
        setAttribute(XRpcAttr.NAME, name);
    }

    /**
     * Returns the data type of this element.
     *
     * @return {@link XRpcDataType#ARRAY}
     */
    public XRpcDataType getDataType() {
        return XRpcDataType.ARRAY;
    }

    /**
     * Sets the elements of the array.
     *
     * @param data the elements of the array
     */
    public void setData(XRpcDataElement[] data) {
        removeAllElements();
        addData(data);
    }

    /**
     * Adds elements to the array.
     *
     * @param data the elements to add
     */
    public void addData(XRpcDataElement[] data) {
        if (data != null && data.length > 0) {
            int n = data.length;
            for (int i=0; i<n; i++) {
                if (data[i] == null) {
                    addElement(new XRpcNull());
                } else {
                    addElement(data[i]);
                }
            }
        }
    }
}
