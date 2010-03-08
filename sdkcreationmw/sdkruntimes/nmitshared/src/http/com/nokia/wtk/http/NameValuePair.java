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

/**
 * Implements a generic container of name/value pairs.
 * Mainly used by the <a href="Header">Header</a> and
 * <a href="MimeMessageParser">MimeMessageParser</a>
 * classes to represent different levels of header
 * values.
 *
 */
public class NameValuePair {

    protected String m_name;
    protected String m_value;

    public NameValuePair(String name, String value) {
	m_name = name.trim();
	m_value = value.trim();
    }

    public NameValuePair(String str, char separator) {
	String name, value;
	int valuePos = str.indexOf(separator) + 1;
	if (valuePos > 0) {
	    m_name = str.substring(0, valuePos-1).trim();
	    m_value = (valuePos < str.length()) ?
		str.substring(valuePos).trim() : "";
	}
	else {
	    m_name = str.trim();
	    m_value = "";
	}
    }

    public String getName() {
	return m_name;
    }

    public String getValue() {
	return m_value;
    }

    public void setValue(String newValue) {
	m_value = newValue;
    }
}
