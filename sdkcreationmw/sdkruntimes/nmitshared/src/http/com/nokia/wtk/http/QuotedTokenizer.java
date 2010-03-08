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

import java.util.Enumeration;
import java.util.NoSuchElementException;

/**
 * The QuotedTokenizer class implements an enumerator that
 * returns the components of the specified string as an
 * enumeration. This class is quite similar to Java's
 * StringTokenizer class, but QuotedTokenizer can handle
 * quoted strings. Another difference is that only one
 * separator character at a time is supported.
 *
 */
public class QuotedTokenizer implements Enumeration {

    protected String m_tokenString;
    protected char m_quote;
    protected char m_separator;
    protected int m_index = 0;
    protected int m_tokenBegin = 0;
    protected boolean m_quoted = false;

    /**
     * Constructs a quoted string tokenizer with the specified
     * quote and separator characters.
     *
     * @param tokenString the (possibly quoted) string to tokenize
     * @param quote the quote character
     * @param separator the separator character that separates tokens
     * from each other (ignored if in quotes)
     */
    public QuotedTokenizer(String tokenString, char quote, char separator) {
	m_tokenString = tokenString;
	m_quote = quote;
	m_separator = separator;
    }

    public boolean hasMoreElements() {
	return (m_tokenBegin < m_tokenString.length());
    }

    public Object nextElement() throws NoSuchElementException {
	for (; m_index < m_tokenString.length(); m_index++) {
	    int ch = m_tokenString.charAt(m_index);
	    if (!m_quoted) {
		if (ch == m_quote) {
		    // Quoted value begins.
		    m_quoted = true;
		}
		else if (ch == m_separator) {
		    // Value separator. Extract the preceding value.
		    if (m_tokenBegin < m_index) {
			// Extract the next token.
			String token =
			    m_tokenString.substring(m_tokenBegin, m_index);
			// Mark the beginning of the next value.
			m_tokenBegin = m_index+1;
			// Return the token.
			return token.trim();
		    }
		    // Mark the beginning of the next value.
		    m_tokenBegin = m_index+1;
		}
		else {
		    // Other characters are part of the value, just skip them.
		}
	    }
	    else if (ch == m_quote) {
		// End of quotation.
		m_quoted = false;
	    }
	}
	// Extract the last header value.
	if (m_tokenBegin < m_index) {
	    // Return the last token.
	    String token = m_tokenString.substring(m_tokenBegin, m_index);
	    m_tokenBegin = m_index;
	    return token.trim();
	}
	// No more elements.
	throw new NoSuchElementException();
    }

    /**
     * DEBUG main
     */
    public static void main(String args[]) {
	String values = ",text/html;q=0.8, text/plain,, application/gzip,";
	Enumeration e = new QuotedTokenizer(values, '"', ',');
	while (e.hasMoreElements()) {
	    String value = (String) e.nextElement();
	    System.out.println(value);
	}
    }

}
