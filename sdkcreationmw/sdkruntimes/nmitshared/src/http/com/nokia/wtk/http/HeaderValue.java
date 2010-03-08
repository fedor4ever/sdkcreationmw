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

import java.util.Vector;

/**
 * The HeaderValue class implements a name/value pair
 * container for header values. The pair consists of
 * the header value type and its associated parameters.
 *
 * For example, for the header "text/html;level=1;q=0.7"
 * the name value pair has the name "text/html" and
 * value "level=1;q=0.7". The type of the value can be
 * retrieved with the getName() method. The parameters
 * can be retrieved as a vector with the method
 * <a href="#getParams()">getParams</a>.
 *
 */
public class HeaderValue extends NameValuePair {

    /**
     * A vector that holds cached copy of the parsed parameters.
     */
    private Vector m_paramVector = null;

    /**
     * Constructs a HeaderValue object that will
     * have the specified type and parameters.
     */
    public HeaderValue(String type, String params) {
	super(type, params);
    }

    /**
     * Constructs a HeaderValue object by splitting the
     * specified type/parameters string into a name/value
     * pair using the specified separator character.
     */
    public HeaderValue(String str, char separator) {
	super(str, separator);
    }

    /**
     * Returns a string containing the type of the header value.
     * This may be a media type (like "text/html") or character
     * set (like "iso-8859-1") etc.
     *
     * @return a string
     */
    public String getType() {
	return getName();
    }

    /**
     * Sets the value (parameters) of the header value.
     * Does exactly same as <a href="#setParams">setParams</a>.
     *
     * @param newValue the parameters to set
     */
    public void setValue(String newValue) {
	// Set the params.
	super.setValue(newValue);
	// Discard the cached copy of the parsed parameters.
	m_paramVector = null;
    }

    /**
     * Returns as a vector of <a href="NameValuePair">NameValuePair</a>
     * objects the parameters of the header value. For example,
     * the name value pairs "level=1", "q=0.7".
     *
     * @return a vector of NameValuePair objects containing
     * the parameters of the header value
     */
    public Vector getParams() {
	if (m_paramVector == null) {
	    // We don't have cached copy of the parameters.
	    m_paramVector = MimeMessageParser.getHeaderValueParams(m_value);
	}
	return m_paramVector;
    }

    /**
     * Sets the parameters of the header value.
     * Does exactly same as <a href="#setValue">setValue</a>.
     *
     * @param params the parameters to set
     */
    public void setParams(String params) {
	setValue(params);
    }

    /**
     * Returns a string containing the value of the specified parameter.
     *
     * @param paramName the name of the parameter whose value to get
     * @return a string containing the value of the specified parameter
     * or <b>null</b> if the parameter was not found
     */
    public String getParamValue(String paramName) {
	Vector params = getParams();
	for (int i = 0; i < params.size(); i++) {
	    NameValuePair param = (NameValuePair) params.elementAt(i);
	    if (param.getName().equalsIgnoreCase(paramName)) {
		// Param was found, return its value.
		return param.getValue();
	    }
	}
	// Not found.
	return null;
    }

    /**
     * Return a Float containing the quality factor of the header value.
     *
     * @return a Float containing the quality factor (0..1)
     * or <b>null</b> if not found or invalid value
     */
    public Float getQualityFactor() {
	String strFactor = getParamValue("q");
	if (strFactor != null) {
	    try {
		return Float.valueOf(strFactor);
	    }
	    catch (NumberFormatException e) {
		// Propably invalid quality factor. Just ignore it.
		// Falls through
	    }
	}
	// Parameter not found or invalid quality factor value.
	return null;
    }
}
