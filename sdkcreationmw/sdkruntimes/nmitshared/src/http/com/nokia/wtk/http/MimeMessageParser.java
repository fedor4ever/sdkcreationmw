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

import java.io.InputStream;
import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.text.ParseException;
import java.util.Vector;
import java.util.Enumeration;

/**
 * The MimeMessageParser is a static class that provides methods for:
 * <ul>
 * <li>parsing textual MIME messages into a HeaderList</li>
 * <li>separating headers and content from each other</li>
 * <ul>
 *
 */
public class MimeMessageParser {

    /**
     * Parses the given input stream of headers into given list.
     * This is a static method. Does not prevent duplicates.
     *
     * @param headers a string of headers each ending with CRLF
     * @param list the list to receive the parsed headers
     */
    public static void parseHeaders(String headers, HeaderList headerList)
	throws ParseException {
	// The latest header added to list.
	Header latestHeader = null;
    	// Create the list if not yet done.
    	int startPos = 0;
    	// Find the end of the first header line.
    	int nlPos = headers.indexOf('\n');
    	while (nlPos > 0) {
    	    // Exclude '\r' from the header value.
    	    int endPos = (headers.charAt(nlPos-1) == '\r') ? nlPos-1 : nlPos;
    	    if (endPos <= startPos) {
		// Empty line marking the end of headers was encountered.
		break;
    	    }
    	    // Get the header line with CR/LF stripped.
    	    String headerLine = headers.substring(startPos, endPos);

	    // Check if this line continues the value of the latest header.
	    if (!hasLeadingSpace(headerLine)) {
		// No, doesn't continue.
		// Find the colon separating the header name and value.
		int colonPos = headerLine.indexOf(':');
		if (colonPos > 0) {
		    // Separate the header to name and value,
		    // stripping white space.
		    String name = headerLine.substring(0, colonPos).trim();
		    String value = (colonPos+1 < headerLine.length()) ?
			headerLine.substring(colonPos+1, headerLine.length()) :
			"";
		    // Create new header and add it to list.
		    latestHeader = headerList.append(new Header(name, value));
		}
		else {
		    // No colon, invalid header line was encountered.
		    throw new ParseException(
			"Missing colon while parsing a header on the line "+
			"starting with \""+
			headerLine.substring(0, 
					     Math.min(headerLine.length(),70))
			+"\"",
			startPos);
		}
	    }
	    else if (latestHeader != null) {
		    // Yes, this line continues the value of the latest header.
		    // Append the current line to it.
		    String tmp = latestHeader.getValue() + " " + 
			headerLine.trim();
		    latestHeader.setValue(tmp);
	    }
	    else {
		// The first header line started with space or tab.
		throw new ParseException(
		    "The first line started with space or tab", startPos);
	    }
	    // Go to the beginning of the next line.
	    startPos = nlPos+1;
	    nlPos = headers.indexOf('\n', startPos);
    	} // end of while
    }

    /**
      * Reads all headers from the given input stream and returns
      * them as a string. The input stream may also contain content
      * data. The end of the headers in the stream is marked by an
      * empty line containing only LF or CRLF. The content data 
      * (if any) follows it.
      *
      * @param in an input containing the headers, possibly followed
      * by content data
      * @return a string containing the headers in MIME format
      */
    public static String getHeaderString(InputStream in) throws IOException {
	ByteArrayOutputStream out = getHeaderStream(in);
	return out.toString();
    }

    /**
      * Reads all headers from the given input stream and returns
      * them as a byte array. The input stream may also contain content
      * data. The end of the headers in the stream is marked by an
      * empty line containing only LF or CRLF. The content data 
      * (if any) follows it.
      *
      * @param in an input containing the headers, possibly followed
      * by content data
      * @return a byte array containing the headers in MIME format
      */
    public static byte[] getHeaderBytes(InputStream in) throws IOException {
	ByteArrayOutputStream out = getHeaderStream(in);
	return out.toByteArray();
    }

    /**
      * Reads content data from the given input stream. The input
      * stream <b>must</b> contain the MIME headers followed by the
      * content data.
      *
      * @param an input stream containing headers, followed by
      * content data
      * @return a byte array containing the content data
      */
    public static byte[] getContentBytes(InputStream in) throws IOException {
	// Get an input stream positioned at the beginning of the content.
	InputStream input = getContentStream(in);
	// Create a byte array output stream where to store the content.
    	ByteArrayOutputStream out =
	    new ByteArrayOutputStream(Math.min(in.available(), 2048));
	int ch;
	// Copy from input stream to output stream.
	while ((ch = input.read()) != -1) {
	    out.write(ch);
	}
	// Return the content byte array.
	return out.toByteArray();
    }

    /**
      * Reads content data from the given input stream. The input
      * stream <b>must</b> contain the MIME headers followed by the
      * content data.
      *
      * @param an input stream containing headers, followed by
      * the content data
      * @return a buffered input stream positioned at the beginning
      * of the content data
      */
    public static BufferedInputStream getContentStream(InputStream in)
	throws IOException {
	// Create a buffered input stream if necessary.
	BufferedInputStream input = (in instanceof BufferedInputStream) ?
	    (BufferedInputStream) in : new BufferedInputStream(in);
    	// Read from input stream, storing to output stream.
	int ch;
	int lfCount = 0;
    	while ((ch = input.read()) != -1) {
	    if (ch == '\n') {
		if (++lfCount > 1) {
		    // Empty line -> end of headers encountered.
		    break;
		}
	    }
	    else if (ch != '\r') {
		// The line has at least one character other than CR or LF.
		lfCount = 0;
	    }
    	}
	// Input stream is now positioned right at the beginning of the
	// content data.
	return input;
    }


    /**
      * Reads all headers from the given input stream and copies
      * them to a byte array output stream. The input stream may 
      * also contain content data. The end of the headers in the 
      * stream is marked by an empty line containing only LF or 
      * CRLF. The content data (if any) follows it.
      *
      * @param in an input containing the headers, possibly followed
      * by content data
      * @return a byte array output stream containing the headers
      * and positioned right after the last header
      */
    public static ByteArrayOutputStream getHeaderStream(InputStream in)
	throws IOException {
        // Create a byte array output stream to store the headers.
	// The initial size of the output stream is limited to 1024
	// bytes, since input streams containing more bytes than that
	// propably contain also content data (which is not read).
    	ByteArrayOutputStream out =
	    new ByteArrayOutputStream(Math.min(in.available(), 1024));
	// Create a buffered input stream if necessary.
	InputStream input = (in instanceof BufferedInputStream) ?
	    in : new BufferedInputStream(in);
    	// Read from input stream, storing to output stream.
	int ch;
	int lfCount = 0;
    	while ((ch = input.read()) != -1) {
	    out.write(ch);
	    if (ch == '\n') {
		if (++lfCount > 1) {
		    // Empty line -> end of headers encountered.
		    break;
		}
	    }
	    else if (ch != '\r') {
		// The line has at least one character other than CR or LF.
		lfCount = 0;
	    }
    	}
	return out;
    }

    /**
     * A utility method that checks whether the given header
     * line starts with a leading space (in which case it
     * usually continues a header value from the previous line).
     *
     * @param a string containing the header line to check
     * @return a boolean value of true if the given line
     * starts with a space or tab
     */
    protected static boolean hasLeadingSpace(String s) {
	return (s.charAt(0) == ' ' || s.charAt(0) == '\t');
    }


    /**
     * Returns a vector of strings containing the individual
     * header values of the given comma separated header value 
     * string. For example, a header value string
     * "text/html;level=1;q=0.8, text/plain" would produce an
     * enumeration containing two strings: "text/html;level=1;q=0.8"
     * and "text/plain".
     *
     * @param headerValues a string containing the header values to
     * tokenize
     * @return an enumeration of header value strings
     */
    public static Vector tokenizeHeaderValues(String headerValues) {
	Vector values = new Vector(5);
	Enumeration e = new QuotedTokenizer(headerValues, '"', ',');
	while (e.hasMoreElements()) {
	    values.addElement(e.nextElement());
	}
	return values;
    }

    /**
     * Splits into a name/value pair a header value containing
     * type and parameters. For example, a header value string
     * "text/html;level=1;q=0.8" would produce a name/value pair
     * having the name "text/html" and value "level=1;q=0.8".
     *
     * @param headerValue the header value to parse
     * @return the header value parsed into a name/value pair
     */
    public static NameValuePair splitHeaderValue(String headerValue) {
	return new NameValuePair(headerValue, ';');
    }

    /**
     * Parses the given string of header value parameters into
     * a vector of name/value pairs. For example, a parameter
     * string containing "level=1;q=0.8" would produce a vector
     * containing two name value pairs:<br>
     * <ol>
     * <li>name=level, value=1</li>
     * <li>name=q, value=0.8</li>
     * </ol>
     *
     * @param valueParams a string containing the value parameters
     * @return an enumeration that returns all the  
     */
    public static Vector getHeaderValueParams(String valueParams) {
	// Create a tokenizer to enumerate the parameters.
        Enumeration enumParams = new QuotedTokenizer(valueParams, '"', ';');
	// Create a new vector of name/value pairs.
	Vector nvPairs = new Vector(5);
	// Tokenize the params into name/value pairs.
	while (enumParams.hasMoreElements()) {
	    // Get the next parameter.
	    String param = (String) enumParams.nextElement();
	    // Split the parameter into a name/value pair and
	    // add it to nvPairs vector.
	    nvPairs.addElement(new NameValuePair(param, '='));
	}
	return nvPairs;
    }

    /**
     * DEBUG main
     */
    public static void main(String args[]) {
	String paramString = "level=1;q=0.6";
	Vector params = MimeMessageParser.getHeaderValueParams(paramString);
	for (int i = 0; i < params.size(); i++) {
	    NameValuePair nvPair = (NameValuePair) params.elementAt(i);
	    System.out.println("name: " + nvPair.getName()+
			       " value: " + nvPair.getValue());
	}
    }
}
