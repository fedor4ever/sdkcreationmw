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

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.StringWriter;
import java.util.Enumeration;

/**
 * A class to handle multipart MIME messages.
 *
 * This class handles multipart input streams, as defined by the RFC 1521. 
 * It provides a sequential interface to all MIME parts, and for each part
 * it delivers a suitable InputStream for getting its body.
 *
 * @see RFC 1521.
 */

public class MultipartInputStream extends InputStream {
    InputStream in         = null;
    byte        boundary[] = null ;
    byte        buffer[]   = null ;
    boolean     partEnd    = false ;
    boolean     fileEnd    = false ;
    int		msg_count  = 0;

    // Read boundary bytes of input in buffer
    // Return true if enough bytes available, false otherwise.

    private final boolean readBoundaryBytes() 
	throws IOException
    {
	int pos = 0;
	while ( pos < buffer.length ) {
	    int got = in.read(buffer, pos, buffer.length-pos);
	    if ( got < 0 ) 
		return false;
	    pos += got;
	}
	return true;
    }

    // Skip to next input boundary, set stream at begining of content:
    // Returns true if boundary was found, false otherwise.

    protected boolean skipToBoundary() 
	throws IOException
    {
	debugOut("In stb");
	int ch = in.read() ;
      skip:
	while ( ch != -1 ) {
	    if ( ch != '-' ) {
		ch = in.read() ;
		continue ;
	    }
	    if ((ch = in.read()) != '-') {
		continue ;
	    }
	    debugOut("stb: found --");
	    in.mark(boundary.length) ;
	    if ( ! readBoundaryBytes() ) {
		in.reset();
		ch = in.read();
	    	debugOut("stb: couldn't readBoundaryBytes");
		continue skip;
	    }
	    for (int i = 0 ; i < boundary.length ; i++) {
		if ( buffer[i] != boundary[i] ) {
		    in.reset() ;
		    ch = in.read() ;
	    	    debugOut("stb: non-matching boundary");
		    continue skip ;
		}
	    }
	    // FIXME: should we check for a properly syntaxed part, which
	    // means that we should expect '\r\n'. For now, we just skip
	    // as much as we can.
	    if ( (ch = in.read()) == '\r' ) {
		ch = in.read() ;
	    } 
	    in.mark(4);
	    if( in.read() == '-' )   {      // check fileEnd!
		if( in.read() == '-' && in.read() == '\r' && in.read() == '\n' )   {
		    fileEnd = true ;
	    	    debugOut("stb: read to end of file ");
		    return false ;
		}
	    }
	    in.reset();
	    debugOut("stb: returning TRUE");
	    return true ;
	}
	fileEnd = true ;
	debugOut("stb: returning FALSE");
	return false ;
    }

    /**
     * Read one byte of data from the current part.
     * @return A byte of data, or <strong>-1</strong> if end of file or boundary.
     * @exception IOException If some IO error occured.
     */

    public int read()
	throws IOException 
    {
	int ch ;
	if ( partEnd )
	    return -1 ;
	switch (ch = in.read()) {
	  case '\r':
	      // check for a boundary 
	      in.mark(boundary.length+3) ;
	      int c1 = in.read() ;
	      int c2 = in.read() ;
	      int c3 = in.read() ;
	      if ((c1 == '\n') && (c2 == '-') && (c3 == '-')) {
		  debugOut("In read:boundary?");
		  if ( ! readBoundaryBytes() ) {
		      in.reset();
		      return ch;
		  }
		  for (int i = 0 ; i < boundary.length ; i++) {
		      if ( buffer[i] != boundary[i] ) {
			  in.reset() ;
			  return ch ;
		      }
		  }
		  partEnd = true ;
		  if ( (ch = in.read()) == '\r' ) {
		      in.read() ;
		  } else if (ch == '-') {
		      // FIXME, check the second hyphen
		      if (in.read() == '-')
			  fileEnd = true ;
		  } else {
		      fileEnd = (ch == -1);
		  }
		  return -1 ;
	      } else {
		  in.reset () ;
		  return ch ;
	      }
	      // not reached
	  case -1:
	      fileEnd = true ;
	      return -1 ;
	  default:
	      return ch ;
	}
    }

    /**
     * Read n bytes of data from the current part.
     * @return the number of bytes data, read or <strong>-1</strong> 
     * if end of file.
     * @exception IOException If some IO error occured.
     */
    public int read (byte b[], int off, int len)
	throws IOException 
    {
	int got = 0 ;
	int ch ;

	while ( got < len ) {
	    if ((ch = read()) == -1) {
		return (got == 0) ? -1 : got ;
	    }
	    b[off+(got++)] = (byte) (ch & 0xFF) ;
	}
	return got ;
    }

    public long skip (long n) 
	throws IOException 
    {
	while ((--n >= 0) && (read() != -1))
	    ;
	return n ;
    }

    public int available ()
	throws IOException
    {
	return in.available();
    }

    /**
     * Switch to the next available part of data.
     * One can interrupt the current part, and use this method to switch
     * to next part before current part was totally read.
     * 
     * @return A boolean <strong>true</strong> if there next partis ready,
     *   or <strong>false</strong> if this was the last part.
     */

    public boolean nextInputStream() 
	throws IOException
    {
	if ( fileEnd ) {
	    return false ;
	}
	if ( ! partEnd ) { 
	    return skipToBoundary() ;
	} else {
	    partEnd = false ;
	    return true ;
	}
    }

    /**
     * Construct a new multipart input stream.
     * 
     * @param in The initial (multipart) input stream.
     * @param boundary The input stream MIME boundary.
     */

    public MultipartInputStream (InputStream in, byte boundary[]) {
	this.in       = (in.markSupported() 
			 ? in 
			 : new BufferedInputStream(in, boundary.length+4));
	this.boundary = boundary ;
	this.buffer   = new byte[boundary.length] ;
	this.partEnd  = false ;
	this.fileEnd  = false ;
    }

    /**
     * Get the boundary string from the content-type header in a header list.
     * This works with any multipart subtype.
     *
     * @return boundary string or null on any failure.
     */
    public static String getBoundaryString(HeaderList hl) {
    	String vals = hl.getHeaderValue("Content-Type");
    	if (vals == null) {
    		debugOut("No content type header");
    		return null;
    	} 
    	debugOut("Got content type header:" + vals);
    
    	NameValuePair nv = MimeMessageParser.splitHeaderValue( vals );
    	if (nv == null) {
    		debugOut("No content type header value");
    		return null;
    	}
    	String ct_param = nv.getValue();	
    	if (ct_param == null || !ct_param.startsWith("boundary")) {
    		debugOut("No boundary parameter found");
    		return null;
    	}
    	String boundary = null;
    	boolean got_it = false;
    	Enumeration qt = new QuotedTokenizer(ct_param, '"', '=');
    	while (qt.hasMoreElements()) {
    		String b = (String)qt.nextElement();
    		if (b.equals("boundary")) {
    			got_it = true;
    		} else {
		   if (got_it)
    			boundary = b;
		}
    	}
    	if (!got_it) {
    		debugOut("No boundary parsed");
		return null;
	}
    	debugOut("Found boundary: " + boundary);
	//
	// trim any quotation marks.
	//
    	if (boundary.charAt(0) == '"') {
    		int end = boundary.length() - 2;
    		boundary = boundary.substring(1, end);
    	}
    	return boundary;
    }

    /**
     * Parse the headers from the current multipart input stream
     */
    public String getMessageHeaders() {
    	// 
    	// Read from input stream, storing to StringWriter
    	// 
    	StringWriter out = null;
	try {
		out = new StringWriter(Math.min(this.in.available(), 1024));
	} catch (IOException inex) {
		return null;
	}
	int ch;
	boolean gotFirstChar = false;
	int lfCount = 0;
	try {
    	  while ((ch = this.in.read()) != -1) {
	    //
	    // Trim any starting CR/LF
	    //
	    if ((ch == '\n' || ch == '\r') && !gotFirstChar) {
		continue;
	    }
	    gotFirstChar = true;
	    if (ch == '\n') {
		if (++lfCount > 1) {
		    // 
		    // Empty line -> end of headers encountered.
		    // 
		    break;
		}
	    }
	    else if (ch != '\r') {
		// The line has at least one character other than CR or LF.
		lfCount = 0;
	    }
	    out.write(ch);
    	  }
	} catch (IOException rdex) {
		// return what we've got
	}
	if (!gotFirstChar)
		return null;
	String rtnStr = out.toString();
	while (rtnStr.endsWith("\n")) {
		int len = rtnStr.length();
		if (len < 2)
			break;
		rtnStr = rtnStr.substring(0, len - 1);
	}
	return rtnStr;
    }

    /**
     * Advance the input stream in this multipart message, and 
     * parse the message.
     *
     * @return URLresponse object or null on any failure.
     */
    public URLresponse parseNextMessage() {

	while (true) {
		try {
		    boolean found_next = this.nextInputStream();
		    if (!found_next) {
		    	if (this.fileEnd) {
		   		debugOut("EndOfFile");
				return null;
			} else
				continue;
		    }
		} catch (IOException ex) {
			debugOut("Exception: " + ex.toString());
			return null;
		}
		int total = 0;
		int rtn = 0;
		String hdr_str = this.getMessageHeaders();
		if (hdr_str == null) {
			debugOut("Can't find message headers");
			continue;
		}
		debugOut("pnm: got part headers: " + hdr_str);
		HeaderList hl_part = null;
		try {
			hl_part = new HeaderList(hdr_str);
		} catch (Exception ex) {
			debugOut("Can't parse message headers");
			continue;
		}
                //
                // Decode any quoted-printable or Base64 values
                //
                hl_part = hl_part.decode();
		//
		// At this point, we at least have some headers (and a message)
		//
		URLresponse resp = new URLresponse();
		resp.setHeaders( hl_part );
		String content_str = null;
		int avail = 0;
		try {
			avail = this.available();
		} catch (Exception ex) {
		}
		//
		// Copy rest of input stream to a byte stream and attach to the reply.
		//
    		ByteArrayOutputStream content =
		    new ByteArrayOutputStream(Math.min(avail, 2048));
		try {
			int ch;
			while ((ch = this.read()) != -1) {
			    content.write(ch);
			}
		} catch (Exception ex) {
			
		}	
		resp.setData( content.toByteArray() );

		this.msg_count++;
		debugOut ( "******* REPL " + msg_count + " ******");
		debugOut ( "CONTENT-TYPE:\n" + resp.getMimeType());
		debugOut ( "HEADERS:\n" + hl_part.toString() );
		debugOut ( "CONTENT:\n" + new String(content.toByteArray()));
		debugOut ( "******* END REPL " + msg_count + " ******");

		return resp;
	}

    }

    /**
     * Debug facility.
     */
    private static boolean debug_on = false;
    public static void setDebug(boolean isDebugOn) { debug_on = isDebugOn; }
    private static void debugOut(String s) {
	if (debug_on)
		System.out.println("MultipartInpuStream." + s);
    }

    /**
     * Unit Test.  Expects a multipart MIME file to read on argument line.
     */
    public static void main(String[] args) {

	URLresponse mpf = null;
	if (args.length < 1) {
		System.out.println("Usage: MimeInputStream <multipart-file>");
		System.exit(1);
	}
	setDebug(true);
	try {
		mpf = URLresponse.readFile(args[0]);
	} catch (Exception e) {
		System.out.println("ERROR reading " + args[0] + ": " + e.getMessage());
		System.exit(1);
	}	
	String boundary = getBoundaryString(mpf.getHeaders());
	if (boundary == null) {
		System.out.println("ERROR: No boundary string found!");
		System.exit(1);
	}
	//
	// Initialize new Multipart stream and loop through all messages in content.
	//
	MultipartInputStream mp_in = new MultipartInputStream( mpf.getDataStream(), boundary.getBytes() );

	URLresponse part = null;
	do {
		part = mp_in.parseNextMessage();
		if (part != null) {
			System.out.println("--- Msg " + mp_in.msg_count + ": " + part.getMimeType());
			System.out.println( new String(part.getData()) );
			System.out.println( "\n");
		}
	} while (part != null);
	System.out.println("--- Parsed " + mp_in.msg_count + " Messages");
    }

}
