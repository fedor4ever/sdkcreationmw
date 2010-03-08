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






package com.nokia.wtk.diagnostics.browser;

import java.util.Hashtable;
import com.nokia.wtk.util.ArrayUtils;
import com.nokia.wtk.device.DeviceContentReceived;

/**
 * A wrapper for DeviceContentReceived that allows attribute tagging.
 *
 */
public class AttributedContent implements DeviceContentReceived {

    /**
     * Creates a new AttributedContent object.
     * @param content   a content object to check (may be <code>null</code>).
     */
    AttributedContent(DeviceContentReceived content) {
        this.content = content;
        this.attributes = new Hashtable();
    }

    /** @return         <code>true</code> if the actual content object is missing. */
    public boolean isEmpty() { return (this.content == null); }


    /** Method implementation delegated to actual content object. */
    public String getRequestLocation() {
        return (this.content != null) ? this.content.getRequestLocation()
                                      : null;
    }

    /** Method implementation delegated to actual content object. */
    public int getRequestStatus(StringBuffer msgbuf) {
        return (this.content != null) ? this.content.getRequestStatus(msgbuf)
                                      : 0;
    }

    /** Method implementation delegated to actual content object. */
    public String getLocation() {
        return (this.content != null) ? this.content.getLocation()
                                      : null;
    }

    /** Method implementation delegated to actual content object. */
    public String getType() {
        return (this.content != null) ? this.content.getType()
                                      : null;
    }

    /** Method implementation delegated to actual content object. */
    public String getEncoding() {
        return (this.content != null) ? this.content.getEncoding()
                                      : null;
    }

    /**
     * Method implementation delegated to actual content object
     * unless additional data chunks have been appended.
     */
    public byte[] getData() {
        if (data != null) {
            return data;
        } else {
            if (content != null) {
                byte[] bytes = content.getData();
                if (bytes != null) {
                    return bytes;
                }
            }
        }
        return NO_DATA;
    }

    /** Method implementation delegated to actual content object. */
    public String[][] getHeaders() {
        return (this.content != null) ? this.content.getHeaders()
                                      : null;
    }

    /** Method implementation delegated to actual content object. */
    public long getTransactionId() {
        return (this.content != null) ? this.content.getTransactionId()
                                      : DeviceContentReceived.INVALID_TRANS_ID;
    }

    /** Returns the whole content  object. */
    public DeviceContentReceived getContent() {
        return this.content;
    }

    /**
     * Gets an attribute value for this unit of content.
     * @param name      the attribute name.
     * @return          the attribute value, or <code>null</code> if not defined.
     */
    Object getAttribute(String name) {
        if (name != null) {
            Object value = this.attributes.get(name);
            if (value != null) {
                return value;
            } else if (content instanceof AttributedContent) {
                return ((AttributedContent)content).getAttribute(name);
            }
        }
        return null;
    }

    /**
     * Sets an attribute for this unit of content.
     * @param name      the attribute name.
     * @param value     the attribute value.
     */
    void setAttribute(String name, Object value) {
        if (name != null) {
            if (value != null) {
                this.attributes.put(name, value);
            } else if (this.attributes.containsKey(name)) {
                this.attributes.remove(name);
            }
        }
    }

    /**
     * Adds a chunk of data (in case of streaming).
     * @param chunk     the bytes to add.
     */
     void addDataChunk(byte[] chunk) {
        if (chunk != null && chunk.length > 0) {
            if (data == null && content != null) data = content.getData();
            if (data == null) {
                data = ArrayUtils.clone(chunk);
            } else {
                data = ArrayUtils.add(data, chunk);
            }
            setAttribute(AGGREGATED_DATA, Boolean.TRUE);
        }
   }

    /**
     * Checks for a match with the actual content reference.
     * @param content   a content object to compare with.
     * @return          <code>true</code> if the actual content object is the same.
     */
    boolean isEqual(DeviceContentReceived content) {
        return (this.content == content);
    }

    private DeviceContentReceived content;
    private Hashtable attributes;
    private byte[]  data;

    static final String AGGREGATED_DATA = "aggregated_data";
    private static final byte [] NO_DATA = new byte[0];

}   // End of AttributedContent
