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

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.text.ParseException;
import java.util.Enumeration;
import java.util.Vector;

/**
  * The <code>HeaderList</code> class implements a list of request/response 
  * headers. The list never contains two headers with the same 
  * name (except Set-Cookie, which must retain separate headers to
  * maintain its semantics). If the list already contains a header with the same
  * name as the one being added, the new header is joined
  * with the old one. How and whether the join is done, 
  * depends on the method:
  * <ul>
  * <li>prepend inserts the new value before the old value</li>
  * <li>append adds the new value after the old value</li>
  * <li>replace never joins.</li>
  * </ul>
  *
  */
public class HeaderList implements Cloneable
{
    /**
      * A vector that contains all the header objects.
      */
    protected Vector m_vect = new Vector(5);

    /**
     * Constructs an empty header list.
     */
    public HeaderList() {
    }

    /**
     * Constructs a list of headers. The headers are given as a string
     * with each header ending at LF or CRLF. If the headers contain
     * a line with only LF or CRLF on it, it is assumed to mark the
     * end of headers.
     *
     * @param headers a string containing the headers
     * @exception java.text.ParseException if the headers can't be
     * parsed into a list because of invalid format
     */
    public HeaderList(String headers) throws ParseException {
    	MimeMessageParser.parseHeaders(headers, this);
    }
    /**
     * Constructs a list of headers. The headers are given as an
     * input stream where each header ends at LF or CRLF. If the
     * headers contain a line with only LF or CRLF on it, it is 
     * assumed to mark the end of headers.
     *
     * @param in an input stream from where to read the headers
     * @exception java.text.ParseException if the headers can't be
     * parsed into a list because of invalid format
     * @exception java.io.IOException if an I/O error occurs while
     * reading the headers from the input stream
     */
    public HeaderList(InputStream in) throws ParseException, IOException {
    	setHeaders(in);
    }

    /**
     * Sets the headers of this list from a string of headers.
     * The headers in the string should each end at LF or CRLF.
     *
     * @param headers a string containing the headers
     * @exception java.text.ParseException if the headers can't be
     * parsed into a list because of invalid format
     */
    public void setHeaders(String headers) throws ParseException {
    	MimeMessageParser.parseHeaders(headers, this);
    }

    /**
     * Sets the headers of this list from an input stream.
     * The headers in the stream should each end at LF or CRLF.
     *
     * @param in an input stream from which to read the headers
     * @exception java.text.ParseException if the headers can't be
     * parsed into a list because of invalid format
     * @exception IOException if an I/O error occurs
     */
    public void setHeaders(InputStream in) throws ParseException, IOException {
    	MimeMessageParser.parseHeaders(
	    MimeMessageParser.getHeaderString(in), this);
    }

    /**
     * Returns a header list object that is a copy of this list.
     */
    public Object clone() {
    	HeaderList headerList = new HeaderList();
    	headerList.m_vect = (Vector) m_vect.clone();
    	return headerList;
    }

    /**
     * Returns an enumeration of the headers in this list.
     *
     * @return an enumeration of headers
     */
    public Enumeration elements() {
    	return m_vect.elements();
    }

    /**
     * Returns the number of headers in this list.
     *
     * @return an integer specifying the number of headers in this list
     */
    public int size() {
    	return m_vect.size();
    }

    /**
     * Gets the first header in the list.
     *
     * @return the first header of the list or null if none
     */
    public Header getFirst() {
    	return (Header) m_vect.firstElement();
    }

    /**
     * Gets the last header in the list.
     *
     * @return the first header of the list or null if none
     */
    public Header getLast() {
    	return (Header) m_vect.lastElement();
    }

    /**
     * Gets the value of a header with the specified name.
     * This is a convenience method, which a shortcut for calling
     * first getHeader and then getValue on it. <b>Note:</b> if
     * the header has multiple values, the returned string
     * contains them all.
     *
     * @param name a string specifying the name of the header to find
     * @return a string containing the value of the header or null
     * if not found
     */
    public String getHeaderValue(String name) {
        Header header = getHeader(name);
        return (header != null) ? header.getValue() : null;
    }

    /**
     * Gets all values from a header with the specified name.
     *
     * @param name a string stating the name of the header
     * @return a vector of strings containing the header values
     * or null if not found
     */
    public Vector getHeaderValues(String name) {
	Header header = getHeader(name);
	return (header != null) ? header.getValues() : null;
    }

    /**
     * Gets a header by name.
     *
     * @param name a string specifying the name of the header to find
     * @return a header matching the name or null if not found
     */
    public Header getHeader(String name) {
        int index = indexOf(name);
        return (index >= 0) ? (Header) m_vect.elementAt(index) : null;
    }

    /**
     * Gets a header by index. Index of the first header is 0.
     *
     * @param index an integer specifying the index of the header
     * @return a header specified by the index or null if
     * the index is out of range.
     */
    public Header getHeader(int index) {
    	return index < size() ? (Header) m_vect.elementAt(index) : null;
    }

    /**
     * Tells whether the list contains a header with the specified
     * name. There may be more than one matching header.
     *
     * @param a string stating the name of the header to find
     * @return true if found, false if not
     */
    public boolean hasHeader(String name) {
        return indexOf(name) >= 0;
    }

    /**
     * Tells whether the list contains the specified header.
     * Both the name and value of the header must match.
     *
     * @param header the header to find
     * @return true if found, false if not
     */
    public boolean hasHeader(Header header) {
        return indexOf(header) >= 0;
    }

    /**
     * Inserts the given header to the beginning of the list.
     * If the list already contains a header with the same
     * name, the value of this header is inserted to the
     * beginning of it.
     *
     * The exception to this is Set-Cookie, which MUST retain
     * separate header entries.
     *
     * @param newHeader the header to insert
     * @return The header object now in the list. If there
     * already was a header with the same name, it is returned.
     * otherwise the new header is returned.
     */
    public Header prepend(Header newHeader) {
	Header oldHeader = getHeader(newHeader.getName());
	if (oldHeader == null) {
	    m_vect.insertElementAt(newHeader, 0);
	    return newHeader;
	}
	if (newHeader.getName().equalsIgnoreCase("Set-Cookie"))
		m_vect.addElement(newHeader);
	else	
		oldHeader.prependValue(newHeader.getValue());
	return oldHeader;
    }

    /**
     * Tries to append the given header to the end of the list.
     * If the list already contains a header with the same
     * name, the value of this header is appended to it.
     *
     * The exception to this is Set-Cookie, which MUST retain
     * separate header entries.
     *
     * @param newHeader the header to append
     * @return The header object now in the list. If there
     * already was a header with the same name, it is returned.
     * otherwise the new header is returned.
     */
    public Header append(Header newHeader) {
	Header oldHeader = getHeader(newHeader.getName());
	if (oldHeader == null) {
	    m_vect.addElement(newHeader);
	    return newHeader;
	}
	if (newHeader.getName().equalsIgnoreCase("Set-Cookie"))
		m_vect.addElement(newHeader);
	else	
		oldHeader.appendValue(newHeader.getValue());
	return oldHeader;
    }

    /**
     * Replaces with the given header the first header that has
     * the same name, or if not found, appends the new header to
     * the end of the list.
     *
     * @param newHeader header to replace or append
     * @return replaced header or null if not found
     */
    public Header replace(Header newHeader) {
        int index = indexOf(newHeader.getName());
        if (index >= 0) {
            // Matching header was found, replace it.
            Header replacedHeader = (Header) m_vect.elementAt(index);
            m_vect.setElementAt(newHeader, index);
            return replacedHeader;
        }
        // Not replaced, just append the new header.
    	m_vect.addElement(newHeader);
    	return null;
    }

    /**
     * Removes the first header from the list and returns it.
     *
     * @return the removed header, null if the list was empty
     */
    public Header removeFirst() {
    	if (!m_vect.isEmpty()) {
	    Header header = (Header) m_vect.firstElement();
	    m_vect.removeElementAt(0);
	    return header;
	}
	return null;
    }

    /**
     * Removes the last header from the list and returns it.
     *
     * @return the removed header, null if the list was empty
     */
    public Header removeLast() {
    	if (!m_vect.isEmpty()) {
	    Header header = (Header) m_vect.lastElement();
	    m_vect.removeElementAt(m_vect.size()-1);
	    return header;
	}
	return null;
    }

    /**
     * Removes a header from the list. The contents of the <code>
     * header</code> are matched, not the object reference. Uses <code>
     * Header.equals</code> method for comparison, which means that
     * both the header name and header value must match those of the
     * <code>header</code> argument. Comparisons are case-insensitive.
     *
     * @see Header#equals
     * @param header a header object to remove
     * @return true if found, false if not
     */
    public boolean remove(Header header) {
    	return m_vect.removeElement(header);
    }

    /**
     * Removes a header specified by the name. Removes only
     * the first matching header. If all headers with the
     * specified name are to be removed, use <a href="#removeAll">
     * removeAll</a> instead.
     *
     * @see #removeAll(String)
     * @param name a string stating the name of the header to remove
     * @return the removed header or <b>null</b> if not found
     */
    public Header remove(String name) {
        int index = indexOf(name);
        if (index >= 0) {
            Header removedHeader = (Header) m_vect.elementAt(index);
            m_vect.removeElementAt(index);
            return removedHeader;
        }
    	return null;
    }

    /**
     * Removes all headers matching the given name.
     *
     * @param name a string stating the name of the header(s) to remove
     * @return the number of headers removed
     */
    public int removeAll(String name) {
    	int count = 0;
    	Vector list = m_vect;
    	for (int i = size()-1; i >= 0; i--) {
	    if (name.equalsIgnoreCase(((Header)list.elementAt(i)).getName())) {
		list.removeElementAt(i);
		count++;
	    }
	}
	return count;
    }

    /**
     * Removes all headers from this header list.
     */
    public void clear() {
    	m_vect.removeAllElements();
    }

    /**
     * Dumps the contents of the list to given output stream.
     *
     * @param out an output stream where to write
     */
    public void dump(OutputStream out) throws IOException {
	PrintWriter writer = new PrintWriter(out);
    	Enumeration enumVar = elements();
    	while (enumVar.hasMoreElements()) {
	    Header header = (Header) enumVar.nextElement();
	    writer.println(header.getName()+": "+header.getValue());
    	}
	writer.flush();
    }

    /**
     * Internal utility method for finding a header by its name
     * AND value.
     *
     * @param hdr a header containing the name and value to match
     * @return an integer specifying the array index of the found
     * header or <strong>-1</strong> if not found
     */
    int indexOf(Header hdr) {
        return m_vect.indexOf(hdr);
    }

    /**
     * Internal utility method for finding a header by its name.
     *
     * @param name a string containing the header name to match
     * @return an integer specifying the array index of the found
     * header or <strong>-1</strong> if not found
     */
    int indexOf(String name) {
	int size = m_vect.size();
	for (int i = 0; i < size; i++) {
	    Header hdr = (Header) m_vect.elementAt(i);
	    if (name.equalsIgnoreCase(hdr.getName())) {
		return i;
	    }
	}
	return -1;
    }

    // DEBUG main
    public static void main(String argv[])
    {
        try {
	    HeaderList list = new HeaderList(System.in);
	    list.dump(System.out);
        }
	catch (ParseException e) {
	    System.out.print("At offset "+e.getErrorOffset()+" ");
	    e.printStackTrace();
	}
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Return list in a human readable form.
     */
    public String toString() {
        StringBuffer sb = new StringBuffer();
        Enumeration e = elements();
        while(e.hasMoreElements()) {
            Header h = (Header)e.nextElement();
            sb.append(h.toString() + "\n");
        }
        return sb.toString();
    }

    /**
     * Create a new header list with quoted-printable encoded header values.
     * Intended to be called before sending headers out.
     */
    public HeaderList encodeQP() {
        HeaderList hl = new HeaderList();
	int size = m_vect.size();
	for (int i = 0; i < size; i++) {
	    Header h = (Header) m_vect.elementAt(i);
            String val = HeaderCharEncoding.encodeQP(h.getValue());
	    Header encH = new Header(h.getName(), val);
            hl.replace(encH);
	}
        return hl;
    }

    /**
     * Create a new header list with Base64-encoded header values.
     * Intended to be called before sending headers out.
     */
    public HeaderList encodeBase64() {
        HeaderList hl = new HeaderList();
	int size = m_vect.size();
	for (int i = 0; i < size; i++) {
	    Header h = (Header) m_vect.elementAt(i);
            String val = HeaderCharEncoding.encodeBase64(h.getValue());
	    Header encH = new Header(h.getName(), val);
            hl.replace(encH);
	}
        return hl;
    }

    /**
     * Create a new header list with decoded header values.
     * Intended to be called after reading headers in.
     */
    public HeaderList decode() {
        HeaderList hl = new HeaderList();
	int size = m_vect.size();
	for (int i = 0; i < size; i++) {
	    Header h = (Header) m_vect.elementAt(i);
            String val = h.getValue();
            if (val != null) {
                val = HeaderCharEncoding.decode(val.getBytes());
            }
	    Header encH = new Header(h.getName(), val);
            hl.replace(encH);
	}
        return hl;
    }
}
