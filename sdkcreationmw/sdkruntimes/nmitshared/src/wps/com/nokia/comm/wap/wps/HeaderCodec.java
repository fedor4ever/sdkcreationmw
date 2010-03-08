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



package com.nokia.comm.wap.wps;

/**
 * Java Interface to HC (Header Codec) package.
 *
 * @version 1.0, 03/15/99
 *
 */

public class HeaderCodec {
	static {
	  System.loadLibrary("headercodecJNI");
	}
	private static final int DEFAULT_ENCODE_BUF_LEN = 4096;
	private static final int DEFAULT_DECODE_BUF_LEN = 8192;

	private static int enc_buf_len = DEFAULT_ENCODE_BUF_LEN;
	private static int dec_buf_len = DEFAULT_DECODE_BUF_LEN;

	/*
	 * Option Flags (see below)
	 */
	public static int OPT_NONE			= 0;
	public static int OPT_CONTENT_TYPE_FIRST	= 1;
	/*
	 * Buffers used by JNI interface.  Java method allocates arrays,
	 * JNI fills in m_headers_out array and sets m_buffers_out.
	 */
	public byte[] m_headers_in;
	public byte[] m_headers_out;
	public int m_buf_out_len;
	/*
	 * Error code map.  JNI Interface translates HC errors to index
	 * into this array on returns from native routines.
	 */
	private static String cEncErrorMessages[] = {
		"Unknown Error",
		"No Error",
		"Illegal Newline",
		"No Field Separator",
		"Bad Q value",
		"Bad Date",
		"Bad Month",
		"Invalid Date",
		"Unknown Method",
		"Unknown Content Disposition",
		"Bad Content Range",
		"Bad Range",
		"Bad Warning",
		"Internal Buffer Too Small",
		"Destination Buffer Too Small"
	};
	private static String cDecErrorMessages[] = {
		"Unknown Error",
		"No Error",
		"Invalid Field Code",
		"Illegal Codepage Shift",
		"Invalid Content Encoding",
		"Invalid Accept Range",
		"Invalid Connection",
		"Invalid Transfer Encoding",
		"Invalid Q Code",
		"Invalid Time Code",
		"Unknown Lang Code",
		"Unknown Cnt Type Code",
		"Unknown Charset Code",
		"Unknown Method Code",
		"Unknown Cache-Control Code",
		"Unknown Content Disp",
		"Unknown Range Code",
		"Integer Expected",
		"Length Expected",
		"Incorrect Value Length",
		"Empty Text",
		"Data Overflow",
		"Incorrect MD5 Length",
		"Internal Buffer Too Small",
		"Destination Buffer Too Small"
	};

	private static String encErrString(int status) {
		String rtnMsg;
		if (status <= 0 || status > 14) {
			rtnMsg = new String("Unknown Error (" + Integer.toString(status) + ")");
		} else {
			rtnMsg = cEncErrorMessages[status];
		}
		return rtnMsg;
	}
	private static String decErrString(int status) {
		String rtnMsg;
		if (status <= 0 || status > 24) {
			rtnMsg = new String("Unknown Error (" + Integer.toString(status) + ")");
		} else {
			rtnMsg = cDecErrorMessages[status];
		}
		return rtnMsg;
	}

	/**
 	 * Constructor
 	 */
	public HeaderCodec() {
		m_headers_out = new byte[enc_buf_len];
	}

	/**
 	 * Encode string into WPS-encoded byte array (see WPS spec 8.4).
	 * Does not support user code pages. 
	 *
 	 * @Parameter headers
	 * String of HTTP headers, each separated with newline.  
	 * Can be terminated with NULL or two successive newlines.
	 *
	 * @Parameter flags
	 *	OPT_NONE is the default header handling.
	 * 	CONTENT_TYPE_FIRST specifies that the Content-Type field
	 *	is handled as normal headers (by default, the value of this
	 *	field is first without the header field code).
	 *	(see WSP Spec 8.2.3.2, 8.2.3.3, 8.2.4.1, and 8.5)
	 *
	 * @returns byte array of encoded headers.
	 *
	 * @exception any errors causes an exception to be thrown.
	 **/
	public byte[] encode(String httpHeaders, int flags) 
	   throws WpsException {
		if (httpHeaders == null) httpHeaders = "\n\n";
		//
		// Encoder gets weird if the headers are not terminated with 2 CRs
		//
		if (!httpHeaders.endsWith("\n\n")) {
		    if (httpHeaders.endsWith("\n")) {
			httpHeaders = httpHeaders + "\n";
		    } else {
			httpHeaders = httpHeaders + "\n\n";
		    }
		}
		m_headers_in = httpHeaders.getBytes();
		m_buf_out_len = 0;

		int st = cEncode( flags );
		if (st != 1) {
			throw new WpsException ( "Header encoder: " + encErrString(st));
		}
		byte[] encHeaders = new byte[m_buf_out_len];
		System.arraycopy(m_headers_out, 0, encHeaders, 0, m_buf_out_len);
		return encHeaders;
	}
	/**
 	 * Decodes byte array of WPS-encoded byte array (see WPS spec 8.4)
	 * into ASCII string of HTTP headers.  Does not support user code pages.
	 *
 	 * @Parameter headers
	 * Byte arrary of WSP headers.
	 *
	 * @Parameter flags:
	 *	OPT_NONE is the default header handling.
	 * 	OPT_CONTENT_TYPE_FIRST specifies that the Content-Type field
	 *	is handled as normal headers (by default, the value of this
	 *	field is first without the header field code).
	 *	(see WSP Spec 8.2.3.2, 8.2.3.3, 8.2.4.1, and 8.5)
	 *
	 * @returns string of HTTP headers.
	 *
	 * @exception any errors causes an exception to be thrown.
	 **/
	public String decode(byte[] encHeaders, int flags) 
	   throws WpsException {

		m_headers_in = encHeaders;
		m_buf_out_len = 0;

		int st = cDecode( flags );
		if (st != 1) {
			throw new WpsException ( "Header decoder: " + decErrString(st));
		}
		String httpHeaders = new String(m_headers_out);
		String rtn = httpHeaders.substring(0, m_buf_out_len);
		return rtn;
	}
	/*
	 * These methods allow caller to set buffer sizes to pass to HC interface
	 */
	public void setDecodeBufSize(int size) {
		if (size < 0)
			return;
		dec_buf_len = size;
	}
	public void setEncodeBufSize(int size) {
		if (size < 0)
			return;
		enc_buf_len = size;
	}
	private native int cEncode(int flags);
	private native int cDecode(int flags);
	/*
	 * Unit test
	 */
	public static void main(String args[]) {
		HeaderCodec hd = new HeaderCodec();

		byte[] encHdrs;
		String decHdrs = null;
		String testit = new String(
			"Content-Length: 45\n" +
			"Content-Language: en, da\n" +
			"Cache-Control: no-cache\n" +
			"Content-Type: text/html\n" +
			"Accept: text/*\n" +
			"Accept-Charset: utf-8\n" +
			"Host: localhost"
			);

		System.out.println("Main: Encoding..." + testit);
		try {
			encHdrs = hd.encode( testit, OPT_NONE );
			ByteBuffer bb = new ByteBuffer();
			bb.set( encHdrs );
			System.out.println("Encoded OK:");
			System.out.println( bb.toString() );
		}
		catch (WpsException e) {
			System.out.println("Encoding Failed! ");
			System.out.println(" : " + e.getMessage());
			return;
		}
		System.out.println("Main: Decoding...");
		try {
			decHdrs = hd.decode( encHdrs, OPT_NONE );
			System.out.println("Decoded OK:");
			System.out.println( decHdrs );
		}
		catch (WpsException e) {
			System.out.println("Decoding Failed!");
			System.out.println(" : " + e.getMessage());
		}
	}
}
