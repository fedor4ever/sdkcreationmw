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



package com.nokia.wtk.http;

import java.util.Date;
import java.util.Enumeration;
import java.util.Vector;
import java.text.ParseException;

/**
 * Header class implements a request/response header, which
 * consists of the header name and value.
 *
 */
public class Header {

    /**
     * Predefined constant specifying the character used to
     * separate multiple header values.
     */
    static final String VALUE_SEPARATOR = ",";

    /**
     * Predefined constant specifying the character used to
     * separate parameters of an header value from each other.
     */
    static final String PARAM_SEPARATOR = ";";

    /**
     * A string containing the name of this header.
     */
    String m_name = null;

    /**
     * A string containing the value of this header.
     */
    String m_value = null;

    /**
     * Constructs a header with the specified name and value.
     *
     * @param name name of the header
     * @param value value of the header
     */
    public Header(String name, String value) {
        m_name = name;
        setValue(value);
    }

    /**
      * Constructs a header with the specified name and integer value.
      *
      * @param name name of the header
      * @param value value of the header as an int
      */
    public Header(String name, int value) {
        m_name = name;
        setIntValue(value);
    }

    /**
      * Constructs a header with the specified name and Date value.
      *
      * @param name name of the header
      * @param value value of the header as a Date
      */
    public Header(String name, Date value) {
        m_name = name;
        setDateValue(value);
    }

    /**
      * Constructs a header that is a copy of the given header.
      *
      * @param hdr the header to copy
      */
    public Header(Header hdr) {
        // In Java it's okay for two headers to share their name and value
        // since String objects are constant and can't be deleted as long
        // as there is a reference to them.
        m_name = hdr.getName();
        m_value = hdr.getValue();
    }

    /**
      * Gets the name of the header.
      *
      * @return the name of the header
      */
    public String getName() {
        return m_name;
    }

    /**
      * Gets the value of the header as a string.
      * Note that if the header contains multiple values,
      * the returned string contains them all, separated
      * by commas.
      *
      * @return a string containing the value of the header
      * @see #getIntValue
      * @see #getDateValue
      * @see #getValues
      */
    public String getValue() {
        return m_value;
    }

    /**
      * Get the value of the header as an int.
      *
      * @return the value of the header as an int
      * @exception NumberFormatException if the value can't be
      * coverted to integer
      * @see #getValue
      * @see #getDateValue
      * @see #getValues
      */
    public int getIntValue() throws NumberFormatException {
        return Integer.parseInt(m_value);
    }

    /**
      * Get the value of the header as a Date.
      *
      * @return the value of the header as a Date
      * @exception ParseException if the value can't be
      * converted to Date
      * @see #getValue
      * @see #getIntValue
      * @see #getValues
      */
    public Date getDateValue() throws ParseException {
        InternetDateFormat dateFormatter = new InternetDateFormat();
        return dateFormatter.parse(m_value);
    }

    /**
      * Sets the value of the header.  Ensures not null.
      *
      * @param new value value of the header as a string
      * @see #setIntValue
      * @see #setDateValue
      * @see #setValues
      * @see #prependValue
      * @see #appendValue
      */
    public void setValue(String value) {
        if (value == null) value = "";
        m_value = value.trim();
    }

    /**
      * Sets the value of the header from an int.
      *
      * @param value new value of the header as an int
      * @see #setValue
      * @see #setDateValue
      * @see #setValues
      * @see #prependValue
      * @see #appendValue
      */
    public void setIntValue(int value) {
        m_value = String.valueOf(value);
    }

    /**
      * Sets the value of the header from a Date.
      *
      * @param value new value of the header as a Date.
      * @see #setValue
      * @see #setIntValue
      * @see #setValues
      * @see #prependValue
      * @see #appendValue
      */
    public void setDateValue(Date value) {
        InternetDateFormat dateFormatter = new InternetDateFormat();
        m_value = dateFormatter.format(value);
    }

    /**
      * Inserts the given value to the beginning of the
      * current value, using comma as the separator.
      *
      * @param value a string containing the value to insert
      * @return a string containing the new value of this header
      * @see #appendValue
      */
    public String prependValue(String value) {
        String tmp = value + "," + m_value;
        setValue(tmp);
        return m_value;
    }

    /**
      * Adds the given value to the end of the current value,
      * using comma as the separator.
      *
      * @param value a string containing the value to add
      * @return a string containing the new value of this header
      * @see #prependValue
      */
    public String appendValue(String value) {
        setValue(m_value + "," + value);
        return m_value;
    }

    /**
      * Gets all values of this header as a vector of strings.
      * The values are supposed to be separated by commas
      * (like stated in the HTTP specification). Can't be used
      * with date values since they contain a comma by themselves.
      * But there shouldn't be date headers with multiple values
      * anyway.
      *
      * @return a vector containing values as String objects
      * @see #getValue
      */
    public Vector getValues() {
        return MimeMessageParser.tokenizeHeaderValues(m_value);
    }

    /**
      * Sets the header value from an enumeration containing
      * multiple values as String objects. This just concatenates
      * the values into one string and separates them with commas
      * (conforms to the HTTP specification).
      *
      * @param values an enumeration containing string values
      * @return a string containing the resulting value
      * @see #setValue
      */
    public String setValues(Enumeration values) {
        StringBuffer sb = new StringBuffer();
        while (values.hasMoreElements()) {
            sb.append(((String) values.nextElement()).trim());
            if (values.hasMoreElements()) {
                sb.append(VALUE_SEPARATOR);
            }
        }
        return m_value = sb.toString();
    }

    public Vector getHeaderValues() {
        // Get the header values
        Vector stringValues = getValues();
        // Create a vector of header value objects.
        Vector valueVector = new Vector(stringValues.size());
        // Parse the string values into header value objects.
        for (int i = 0; i < stringValues.size(); i++) {
            // Construct a new header value object.
            valueVector.addElement(
                new HeaderValue((String) stringValues.elementAt(i), ';'));
        }
        return valueVector;
    }

    /**
     * Returns the type of the header value having maximum
     * quality factor. This may be a media type (like "text/html")
     * or character set (like "iso-8859-1").
     *
     * @return a string containing the preferred type
     */
    public String getPreferredType() {
        // Get the values of the header.
        Vector values = getHeaderValues();
        // Find the value which has the biggest quality factor.
        float maxFactor = 0.0f;
        String preferredType = null;
        for (int i = 0; i < values.size(); i++) {
            HeaderValue hv = (HeaderValue) values.elementAt(i);
            Float qf = hv.getQualityFactor();
            if (qf == null || qf.floatValue() == 1.0f) {
                // Found a value with the quality factor of 1.0.
                return hv.getType();
            }
            else if (qf.floatValue() > maxFactor) {
                // Found a factor greater than all previous factors.
                maxFactor = qf.floatValue();
                preferredType = hv.getType();
            }
        }
        // Return the type or null if the header value has no parameters.
        return preferredType;
    }

    /**
     * Tests equality of this header and another object. Overrides
     * the equals method in class Object.
     *
     * @param obj the object to test for equality
     * @return true if the given object is this header itself, or
     * another header object with the same name and value as this.
     * Otherwise returns false.
     */
    public boolean equals(Object obj) {
        if (obj == (Object) this) {
            // this object itself
            return true;
        }
        if (!(obj instanceof Header)) {
            // null or not instance of Header
            return false;
        }
        // Another Header object
        Header hdr = (Header) obj;
        return (m_name.equalsIgnoreCase(hdr.getName()) &&
                m_value.equalsIgnoreCase(hdr.getValue()));
    }

    public String toString() {
        return m_name + ": " + m_value;
    }
}
