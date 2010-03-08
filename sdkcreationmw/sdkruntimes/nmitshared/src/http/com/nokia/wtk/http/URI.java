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


/**
 * URI class that is ported from Rainbow. 
 * Code is very c-like, because we want to sync bugs in the future.
 */

package com.nokia.wtk.http;

import java.util.StringTokenizer;

public class URI {

    public static final char  WAE_URL_ESCAPE_CHAR  = '%';
    public static final char  WAE_URL_FRAG_CHAR = '#';

    /* reserved characters -- as per rfc2396 */
    public static final char  WAE_URL_PARAM_CHAR =  ';';
    public static final char  WAE_URL_PATH_CHAR  =  '/';
    public static final char  WAE_URL_QUERY_CHAR =  '?';
    public static final char  WAE_URL_COLON_CHAR =  ':';
    public static final char  WAE_URL_AT_CHAR   =   '@';
    public static final char  WAE_URL_AMP_CHAR  =   '&';
    public static final char  WAE_URL_EQUAL_CHAR =  '=';
    public static final char  WAE_URL_PLUS_CHAR  =  '+';
    public static final char  WAE_URL_DOLLAR_CHAR=  '$';
    public static final char  WAE_URL_COMMA_CHAR =  ',';

    /* mark characters -- as per definition in rfc2396 */
    public static final char  WAE_URL_DASH_CHAR  =  '-';
    public static final char  WAE_URL_UNDER_CHAR =  '_';
    public static final char  WAE_URL_DOT_CHAR   =  '.';
    public static final char  WAE_URL_BANG_CHAR  =  '!';
    public static final char  WAE_URL_TILDE_CHAR =  '~';
    public static final char  WAE_URL_STAR_CHAR  =  '*';
    public static final char  WAE_URL_APOST_CHAR =  '\'';
    public static final char  WAE_URL_PAREN_CHAR =  '(';
    public static final char  WAE_URL_THESIS_CHAR=  ')';

    public static final int WAE_URL_HTTP_PORT = 80;

    //  Global Types
    public static final int NW_SCHEMA_INVALID = 100;
    public static final int NW_SCHEMA_UNKNOWN = 101; 
    public static final int NW_SCHEMA_HTTPS   = 102;
    public static final int NW_SCHEMA_FTP     = 103; 
    public static final int NW_SCHEMA_GOPHER  = 104;
    public static final int NW_SCHEMA_MAILTO  = 105;
    public static final int NW_SCHEMA_NEWS    = 106;
    public static final int NW_SCHEMA_NNTP    = 107;
    public static final int NW_SCHEMA_TELNET  = 108;
    public static final int NW_SCHEMA_WAIS    = 109;
    public static final int NW_SCHEMA_FILE    = 110;
    public static final int NW_SCHEMA_HTTP    = 111;
    public static final int NW_SCHEMA_PROSPERO= 112;

    private String origUri = null;

    // URI/URL parts
    private String scheme = null;
    private int schemeStart = -1;
    private int schemeEnd = -1;
    private String opaque = null;
    private String authority = null;
    private int authorityStart = -1;
    private int authorityEnd = -1;
    private String userInfo = null;
    private String host = null;
    private int hostStart = -1;
    private int hostEnd = -1;
    private String port = null;
    private int portStart = -1;
    private int portEnd = -1;
    private String path = null;
    private int pathStart = -1;
    private int pathEnd = -1;
    private String query = null;
    private int queryStart = -1;
    private int queryEnd = -1;
    private String fragment = null;
    private int fragStart = -1;
    private int fragEnd = -1;
    private boolean isIPAddress = false;

    /*String that separates a scheme from a url.*/
    public static final String WAE_URL_NETLOC_SEP = "//";
    public static final String WAE_URL_PATH_SEP   = "/";
    public static final String WAE_URL_QUERY_SEP  = "?";
    public static final String WAE_URL_FRAG_SEP   = "#";
    public static final String WAE_URL_COLON_SEP  = ":";

    public static final String JAR_FILE_SCHEME = "jar:file";
    public static final String FILE_SCHEME = "file";
    
    /**
     * Creates new URI class from uri string.
     * It can be absolute or relative URL.
     */
    public URI(String uri) throws Exception {
	origUri = uri.trim();
	urlGetRfc2396Parts();
    }

    /**
     * Builds new URI from parts.
     * Will automatically append port, query and fragment separator.
     */
    public URI(String scheme, String host, String port, String path, 
	       String query, String fragment) throws Exception {
	StringBuffer sb = new StringBuffer();
	if (scheme != null) {
	    sb.append(scheme);
	}
	if (scheme != null && (scheme.equals(JAR_FILE_SCHEME) || scheme.equals(FILE_SCHEME))) {
	  sb.append(":");
	}
	else {
	  sb.append("://");
	}
	if (host != null) {
	    sb.append(host);
	}
	if (port != null) {
	    sb.append(":");
	    sb.append(port);
	}
	if (path != null) {
	    sb.append(path);
	}
	if (query != null) {
	    sb.append("?");
	    sb.append(query);
	}
	if (fragment != null) {
	    sb.append("#");
	    sb.append(fragment);
	}
	origUri = sb.toString();
	//System.out.println("this is new " + origUri);
	urlGetRfc2396Parts();
    }

    /**
     * Returns scheme part of the URI or null if not defined.
     */
    public String getScheme() {
	return scheme;
    }

    /**
     * Returns true if the scheme is "http"
     */
    public boolean isHttpScheme() {
	if (scheme != null && scheme.equals("http")) {
	    return true;
	}
	return false;
    }

    /**
     * Returns true if the scheme is "https"
     */
    public boolean isHttpsScheme() {
	if (scheme != null && scheme.equals("https")) {
	    return true;
	}
	return false;
    }

    /**
     * Returns true if the scheme is "file"
     */
    public boolean isFileScheme() {
	if (scheme != null && scheme.equals(FILE_SCHEME)) {
	    return true;
	}
	return false;
    }

    /**
     * Returns true if the scheme is "jar:file"
     */
    public boolean isJarFileScheme() {
	if (scheme != null && scheme.equals(JAR_FILE_SCHEME)) {
	    return true;
	}
	return false;
    }

    /**
     * Returns opaque part of the URI or null if not defined.
     */
    public String getOpaque() {
	return opaque;
    }

    /**
     * Returns authority part of the URI or null if not defined.
     */
    public String getAuthority() {
	return authority;
    }

    /**
     * Returns user info part of the URI or null if not defined.
     */
    public String getUserInfo() {
	return userInfo;
    }

    /**
     * Returns host part of the URI or null if not defined.
     */
    public String getHost() {
	return host;
    }

    /**
     * Returns port part of the URI or null if not defined.
     */
    public String getPort() {
	return port;
    }

    /**
     * Returns port part of the URI in integer format or -1 if not defined.
     */
    public int getPortInt() {
	if (port != null) {
	    return Integer.parseInt(port);
	}
	return -1;
    }

    /**
     * Returns parameters from the last segment see WMLScript Libraries 1.2
     */
    public String getParameters() {
	if (path == null)
	    return null;

	// find last path component
	int i = path.lastIndexOf(WAE_URL_PATH_CHAR);
	if (i == -1)
	    i = 0;
	// find the begining of the path
	int e = path.indexOf(WAE_URL_PARAM_CHAR, i);
	if (e == -1 || ((e+1) == path.length())) {
	    // no parameters
	    return null;
	}
	return path.substring(e+1);
    }

    /**
     * Returns query part of the URI or null if not defined.
     */
    public String getQuery() {
	return query;
    }

    /**
     * Returns path part of the URI with Parameters or null if not defined.
     */
    public String getPath() {
	return path;
    }

    /**
     * Returns path part of the URI with Parameters or null if not defined.
     */
    public String getPathNoParameters() {
	if (path == null)
	    return null;
	
	String tmpPath = path;
	int paramIndex = -1;
	while((paramIndex = tmpPath.indexOf(WAE_URL_PARAM_CHAR)) != -1) {
	    StringBuffer sb = new StringBuffer(tmpPath);
	    int endOfParam = tmpPath.indexOf(WAE_URL_PATH_CHAR, paramIndex);
	    if (endOfParam == -1) {
		// We are in the end of list
		endOfParam = tmpPath.length();
	    }
	    sb.delete(paramIndex, endOfParam);
	    tmpPath = sb.toString();
	}
	return tmpPath;
    }

    /**
     * Returns fragment part of the URI or null if not defined.
     */
    public String getFragment() {
	return fragment;
    }

    /**
     * Returns true if the address in the numeric IP address.
     */
    public boolean isIPAdress() {
	return isIPAddress;
    }

    /**
     * This function returns the base of a fully qualified URL (e.g.
     * it rips off the card name.
     */
    public String getBase() {
	if (fragment != null && fragStart != -1) {
	    // we need to cut fragment 
	    return origUri.substring(0, fragStart);
	}
	return origUri;
    }

    public static boolean isValidUri(String str) {
	try {
	    URI u = new URI(str); 
	} catch(Exception ex) {
	    return false;
	}
	return true;
    }

    /**
     * Returns true if the escape sequence is valid.
     * @param s is an string that contains escape sequence e.g. %ab
     */
    private static boolean isLegalEscapedChars(String s)
    {
	if (s == null || s.length() != 3)
	    return false;
	
	if (s.charAt(0) == WAE_URL_ESCAPE_CHAR) {
	    /* Found escape char.  Check for 2 following hex digits. */
	    if (isHexDigit(s.charAt(1)) && isHexDigit(s.charAt(2))) {
		/* Legal escape char sequence. */
		return true;
	    }
	}
	return false;
    }
    
    /**
     * Returns true is character represents hex number character
     */
    private static boolean isHexDigit(char c) {
	if (Character.isDigit(c))
	    return true;
	
	c = Character.toLowerCase(c);
	if (c == 'a' || c == 'b' || c == 'c' || c == 'd' || 
	   c == 'e' || c == 'f') {
	    return true;
	}

	return false;
    }
    
    /**
     * Returns true if all escape sequences is valid in the string
     * @param s is an string that contains escape sequences
     */
    public static boolean isLegalEscapedString(String s)
    {
	int i = 0;

	if (s == null)
	    return true;

	while(i < s.length()) {
	    if (s.charAt(i) == WAE_URL_ESCAPE_CHAR) {
		String candidate = null;
		try {
		    candidate = s.substring(i, i+3);
		} catch(IndexOutOfBoundsException ex) {
		    // hex sequence is too short
		    return false;
		}
		if (!isLegalEscapedChars(candidate))
		    return false;
	
		/* Legal escape char sequence.  Increment
		   and continue checking. */
		i = i + 3;
	    }
	    else {
		i++;   // Not escape.  Increment and continue checking.
	    }
	}
	return true;
    }

    /**
     * Returns true if alpha as specified in RFC2396
     */
    private static boolean isAlpha(char c) {
	c = Character.toLowerCase(c);
	if (c >= 'a' && c <= 'z') {
	    return true;
	}
	return false;
    }

    /**
     * See RFC 2396
     */
    private static boolean isUricChar(char c)
    {
	if (isAlpha(c)          ||
	    isHexDigit(c)       ||
	    (c == WAE_URL_ESCAPE_CHAR) ||  /* % */
	    (c == WAE_URL_PARAM_CHAR)  ||  /* ; */
	    (c == WAE_URL_PATH_CHAR)   ||  /* / */
	    (c == WAE_URL_QUERY_CHAR)  ||  /* ? */
	    (c == WAE_URL_COLON_CHAR)  ||  /* : */
	    (c == WAE_URL_AT_CHAR)     ||  /* @ */
	    (c == WAE_URL_AMP_CHAR)    ||  /* & */
	    (c == WAE_URL_EQUAL_CHAR)  ||  /* = */
	    (c == WAE_URL_PLUS_CHAR)   ||  /* + */
	    (c == WAE_URL_DOLLAR_CHAR) ||  /* $ */
	    (c == WAE_URL_COMMA_CHAR)  ||  /* , */
	    (c == WAE_URL_DASH_CHAR)   ||  /* - */
	    (c == WAE_URL_UNDER_CHAR)  ||  /* _ */
	    (c == WAE_URL_DOT_CHAR)    ||  /* . */
	    (c == WAE_URL_BANG_CHAR)   ||  /* ! */
	    (c == WAE_URL_TILDE_CHAR)  ||  /* ~ */
	    (c == WAE_URL_STAR_CHAR)   ||  /* * */
	    (c == WAE_URL_APOST_CHAR)  ||  /* ' */
	    (c == WAE_URL_PAREN_CHAR)  ||  /* ( */
	    (c == WAE_URL_THESIS_CHAR) )   /* ) */
	    {
		return true;
	    } else {
		return false;
	    }
    }

    /**
     * This function parses URL into part as per appendix A, rfc2396, URI-reference
     */
    private void urlGetRfc2396Parts() throws Exception
    {
	int cursor = 0;
	int pStart = 0;
	int pEnd = 0;

	if ((origUri == null) || (origUri.length() == 0)) {
	    throw new Exception("No input URI defined"); 
	}

	// Handle Jar URLs separately
	if (origUri.startsWith(JAR_FILE_SCHEME + ":")) {
	    uriGetJarFileParts();
	    return;
	}

	/* do simple check to make sure all chars are valid in a uri */
	int i = 0;
	while (i < origUri.length()) {
	    if (!isUricChar(origUri.charAt(i)) &&
		(origUri.charAt(i) != WAE_URL_FRAG_CHAR)) {
		throw new Exception("Invalid Characters in URI");
	    }
	    i++;
	}

	if (!isLegalEscapedString(origUri)) {
	    throw new Exception("URI contains Invalid Escaping");
	}

	// Clean URI/URL parts
	scheme = null;
	schemeStart = -1;
	schemeEnd = -1;
	opaque = null;
	authority = null;
	authorityStart = -1;
	authorityEnd = -1;
	userInfo = null;
	host = null;
	hostStart = -1;
	hostEnd = -1;
	port = null; 
	portStart = -1;
	portEnd = -1;
	path = null;
	pathStart = -1;
	pathEnd = -1;
	query = null;
	queryStart = -1;
	queryEnd = -1;
	fragment = null;
	fragStart = -1;
	fragEnd = -1;
	isIPAddress = false;

	fragStart  = origUri.indexOf(WAE_URL_FRAG_CHAR);
	queryStart = origUri.indexOf(WAE_URL_QUERY_CHAR);
	pEnd = origUri.length();

	/* can only have one frag */
	if ( (fragStart != -1) &&
	     (origUri.indexOf(WAE_URL_FRAG_CHAR, fragStart+1) != -1) ) {
	    throw new Exception("URI contains two fragments");
	}

	if (fragStart != -1) {
	    fragment = origUri.substring(fragStart+1);
	    pEnd = fragStart;
	    fragEnd = origUri.length();
	}

	/* fragment can contain query char, but not the other way 'round */
	if ((queryStart != -1) && (fragStart != -1) &&
	    (queryStart >= fragStart)) {
	    queryStart = -1;
	}
	else if (queryStart != -1) {
	    query = origUri.substring(queryStart+1, pEnd);
	    queryEnd = pEnd;
	    pEnd = queryStart;
	}

	/* we've stripped off frag and query, so current pEnd will be end of */
	/* path if a path exists. */
	pathEnd = pEnd;

	/* see if we have a scheme */
	schemeStart = schemeEnd = origUri.indexOf(WAE_URL_COLON_CHAR);
	if (schemeStart != -1) {
	    if (schemeStart > pEnd) {
		/* here the first colon shows up in a query or frag so it doesn't */
		/* represent the end of a scheme string */
		schemeStart = -1;
	    }
	    else { 
		/* now check for valid scheme string */
		if (!isAlpha(origUri.charAt(0))) {
		    schemeStart = -1;
		} else {
		    cursor = 0;
		    while (cursor < schemeStart) {
			char c = origUri.charAt(cursor);
			if (!isAlpha(c) && 
			    !isHexDigit(c) &&
			    (c != WAE_URL_PLUS_CHAR) &&
			    (c != WAE_URL_DASH_CHAR) &&
			    (c != WAE_URL_DOT_CHAR)) {
			    schemeStart = -1;
			}
			cursor++;
		    }
		}
	    }
	    if (schemeStart != -1) {
		schemeStart = pStart;
		cursor++; /* go one past colon character */
		scheme = origUri.substring(schemeStart, schemeEnd);
		pStart = cursor;
	    } else {
		cursor = pStart;
	    }
	}
    
	/* if we've got a scheme, handle possible opaque_part first */
	if ((schemeStart != -1) && (origUri.charAt(cursor) != WAE_URL_PATH_CHAR)) {
	    /* only restriction on opaque_part is >= 1 non-frag char, and doesn't */
	    /* start with a slash */
	    queryStart = -1;
	    query = null;
	    if (cursor != origUri.length()) {
		opaque = origUri.substring(cursor);
	    }
	    
	    if (opaque != null && opaque.length() > 0) {
		return;
	    } else {
		throw new Exception("Opaque data is not valid");
	    }
	}
	
	/* we've got scheme followed by net_path or abs_path, or no scheme */
	/* and net_path, abs_path or rel_path. net_path & abs_path start with '/' */
	else {
	    pathStart = -1;
	    if (origUri.charAt(cursor) != WAE_URL_PATH_CHAR) { /* must have rel_path */
		if (scheme != null) {
		    throw new Exception("Internal Parsing Conflict");
		}
		pathStart = origUri.indexOf(WAE_URL_PATH_CHAR, cursor);
		if ((pathStart == -1) || (pathStart > pEnd)) {
		    pathStart = pEnd;
		}
		while (cursor < pathStart) {
		    char c = origUri.charAt(cursor);
		    /* we know is_uric_char(*pCh) and it's not frag, query or path char */
		    /* only other char not possible in rel_segment is colon */
		    if (c == WAE_URL_COLON_CHAR) {
			throw new Exception("Colon character in relative URL part");
		    }
		    cursor++;
		}
		/* rel_segment must have at least one char */
		if (cursor > pStart) {
		    pathStart = pStart;
		    pathEnd = pEnd;
		} else {
		    path = null;
		    /* we've got a relativeURI but no path, and we've already taken */
		    /* care of any query and fragment, so just return */
		    return;
		}
	    }

	    /* will be (net_path | abs_path) [ ? query ] */
	    if (((cursor+1) < origUri.length()) && (origUri.charAt(cursor+1) == WAE_URL_PATH_CHAR) 
		&& (pathStart == -1)) {
		/* handle net_path case */
		cursor += 2;    /* point at next char after "//" */
		authorityStart = cursor;
		pathStart = origUri.indexOf(WAE_URL_PATH_CHAR, cursor);
		if ((pathStart == -1) || (pathStart > pEnd)) {
		    pathStart = -1;
		    authorityEnd = pEnd;
		}
		else {
		    authorityEnd = pathStart;
		}
      
		authority = origUri.substring(authorityStart, authorityEnd);

		/* net_path = "//" authority [abs_path] so look for "//" authority */
		/* authority = server | reg_name.  Server can be empty but reg_name */
		/* has at least one character, so we're really just looking to see */
		/* if this parses properly as [userinfo @] hostport */
		
		/* if we have an @ char, see if we have userinfo @ hostport */
		/* if userinfo is valid pCh will point one past the @ character */
		/* otherwise will not change.  userinfo can be null, or any valid */
		/* char other than / @ ? so we don't need to explicitly check -- */
		/* we've already found first occurrence of those 3 chars. */
		int pAtChar = origUri.indexOf(WAE_URL_AT_CHAR, cursor);
		
		if ((pAtChar != -1) && (pAtChar > authorityEnd)) {
		    pAtChar = -1;
		}
		else if (pAtChar != -1) {
		    userInfo = origUri.substring(cursor, pAtChar);
		    cursor = pAtChar + 1;
		}
		else {
		    userInfo = null;
		}
      
		/* now look for hostport -- if we have : check for valid port */
		portStart = origUri.indexOf(WAE_URL_COLON_CHAR, cursor);
		/* port is *digit, which means it might be empty */
		if ((portStart != -1) && (portStart < authorityEnd)) {
		    pStart = portStart;
		    portStart++;
		    portEnd = portStart;
		    while ( (portEnd < authorityEnd) &&
			    Character.isDigit(origUri.charAt(portEnd)) ) {
			portEnd++;
		    }

		    /* now if we ate all the digits, and we're not at a path, */
		    /* query, frag, or end of uri then we can't make a valid */
		    /* uri interpreting the current substring as */
		    /* : port [abs_path] [? query] [# fragment] so back off */
		    if (portEnd != authorityEnd) {
			portStart = -1;
			userInfo = null;
			hostStart = -1;
			//We cannot make a valid URI. so throw exception.
			throw new Exception("Cannot form a valid URI");
		    }
		}
		else {
		    portStart = authorityEnd;
		}

		if (portStart != -1 && portEnd != -1) {
		    port = origUri.substring(portStart, portEnd);
		}

		/* portStart is overloaded -- -1 means we found ":" but not a valid */
		/* port #, portStart == authorityEnd means there is no ":" */
		if (portStart != -1) {
		    int segArray[] = new int[4];
		    int nDots = 0;
		    int nonDigits = 0;

		    /* now look for hostname or IPv4address */
		    hostStart = cursor;
		    if (portStart == authorityEnd) {
			hostEnd = portStart;
		    } else {
			hostEnd = portStart -1;
			if (origUri.charAt(hostEnd) != WAE_URL_COLON_CHAR) {
			    throw new Exception("Internal error in host part");
			}
		    }
		    cursor = portStart - 1;
		    while (cursor >= hostStart) {
			if (origUri.charAt(cursor) == WAE_URL_DOT_CHAR) {
			    if (nDots < 4) {
				segArray[nDots] = cursor;
			    }
			    nDots++;
			}
			else if (!Character.isDigit(origUri.charAt(cursor))) {
			    nonDigits++;
			}
			cursor--;
		    }
        
		    /* since portStart was overloaded, set to NULL if no valid port */
		    if (portStart == authorityEnd) {
			portStart = -1;
			port = null;
		    }
        
		    /* this may be IPv4address -- so make sure at least one */
		    /* digit between each pair of dots and a digit at the */
		    /* beginning and end. */
		    if ((nDots == 3) && (nonDigits == 0)) {
			if ( ((segArray[1] + 1) != segArray[0]) &&
			     ((segArray[2] + 1) != segArray[1]) &&
			     Character.isDigit(origUri.charAt(hostStart)) &&
			     Character.isDigit(origUri.charAt(hostEnd - 1)) ) {
			    isIPAddress = true;
			    //host = origUri.substring(hostStart, hostEnd);
			}
		    }
		    else { 
			/* better have *(domainlabel ".") toplabel ["."] */
			/* domain is less restrictive than top, so if we set domain */
			/* false we will also set top false, so to simplify test at */
			/* end, make sure all substrings in host are validDomain. */
			boolean validDomain = true;
			boolean validTop = false;
          
			cursor = hostStart;
			while (cursor < hostEnd) {
			    if (Character.isDigit(origUri.charAt(cursor))) {
				validDomain = true;
			    }
			    else if (isAlpha(origUri.charAt(cursor))) {
				validTop = true;
			    }

			    cursor++;
			    while ( (cursor < hostEnd) && (origUri.charAt(cursor) != WAE_URL_DOT_CHAR) ) {
				if (!isAlpha(origUri.charAt(cursor)) &&
				    !Character.isDigit(origUri.charAt(cursor)) ) {
				    /* if we're at the end of the host, or if the next */
				    /* char is a dot we need alphanum for valid label, but */
				    /* if we're in the interior, we can also accept dash */
				    if ( ((cursor == (hostEnd - 1)) ||
					  (origUri.charAt(cursor + 1) == WAE_URL_DOT_CHAR)) ||
					 (origUri.charAt(cursor) != WAE_URL_DASH_CHAR) ) {
					validTop = false;
					validDomain = false;
					//System.out.println("c " + cursor + " " + hostEnd + " " + origUri.charAt(cursor + 1));
				    }
				}
				cursor++;
			    }
			    /* we're at dot or hostEnd, bump by one */
			    cursor++;
			}
			if (!validDomain || !validTop) {
			    hostStart = -1; 
			    userInfo = null;
			    pAtChar = -1;
			}
		    }
		}  
	    }
	    else {
		/* we've got abs_path only, no auth */
		if (pathStart == -1) {
		    pathStart = cursor;
		}
	    }
	}

	if (pathStart != -1) {
	    path = origUri.substring(pathStart, pathEnd);
	}

	if (hostStart != -1) {
	    host = origUri.substring(hostStart, hostEnd);
	}

	if (portStart != -1) {
	    port = origUri.substring(portStart, portEnd);
	}

	return;
    }

    /**
     * Parses a given path and extracts from it the filename
     * @param  basePath - the base path e.g. /browser/welcome.wml 
     * @param  relPath  - relative path e.g. test/welcome2.xml
     */

    public static String getResolvedPath(String basePath, String relPath)
    {
	boolean shortened;
	int pStart = -1;
	int pDest = -1;
	int pSegment = -1;
	int length = 0;
	StringBuffer path = new StringBuffer();

	if (relPath == null) {
	    return basePath;
	}

	if (basePath == null) {
	    return null;
	}

	path.append(basePath);

	/* RFC2396 section 5 step 6 */
  
	/*
	** "a) All but the last segment of the base URI's path component is copied
	** to the buffer.  In other words, any characters after the last
	** (right-most) slash charactger, if any, are excluded."  __NOTA BENE__ if
	** the base path is empty we add a single / per the WML Script Standard
	** Library Spec and proposed changes to RFC2396.
	*/
	pDest = basePath.lastIndexOf(WAE_URL_PATH_CHAR);
	if (pDest != -1) {
	    if (pDest < (basePath.length()-1)) { // We don't need to cut if this is last character
		path.delete(pDest+1, basePath.length());
	    }
	}
	else if (relPath != null) { /* slash doesn't exist, and we've got a rel path */
	    path = new StringBuffer();
	    path.append(WAE_URL_PATH_SEP);
	}

	if (relPath != null) {
	    /* b) the reference's path component is appended to the buffer */
	    path.append(relPath);
	}
  
	/*
	** c) All occurrences of "./", where "." is a complete path segment, are
	**    removed from the buffer string.
	** d) If the buffer string ends with "." as a complete path segment, that
	**    "." is removed.
	*/
	shortened = true;
	pDest = pStart = 0;
	while (shortened)
	    {
		String f = path.toString();
		shortened = false;
		pDest = f.indexOf("/./");
		if (pDest != -1)
		    {
			shortened = true;
			path.deleteCharAt(pDest);
			path.deleteCharAt(pDest);
		    }
	    }
  
	String fa = path.toString();
	if (fa.endsWith("/.")) {
	    path.deleteCharAt(fa.length()-1);
	}

	//System.out.println(path.toString());
	/*
	** e) all occurrences of "<segment>/../", where <segment> is a complete
	**    path segment not equal to "..", are removed from the buffer string.
	**    Removal of these path segments is performed iteratively, removing
	**    the leftmost matching pattern on each iteration, until no matching
	**    pattern remains.
	** f) If the buffer string ends with "<segment>/..", where <segment> is a
	**    complete path segment not equal to "..", that "<segment>/.." is removed.
	** g) If the resulting buffer string still begins with one or more complete
	**    path segments of "..", then the reference is considered to be in error.
	**    Implementations may handle this error by reaining these components in
	**    the resolved path (i.e., treating them as part of the final URI), by
	**    removing them from the resolved parth (i.e., discarding relative levels
	**    above the root), or by avoiding traversal of the reference.
	*/
	shortened = true;
	pDest = pStart;
	while (shortened)
	    {
		String f = path.toString();
		shortened = false;
		pDest = f.indexOf(WAE_URL_DOT_CHAR, pDest);
		if (pDest != -1) {
		    shortened = true;
		    if ( ((pDest == pStart) || (f.charAt(pDest - 1) == WAE_URL_PATH_CHAR)) &&
			 ( f.charAt(pDest + 1) == WAE_URL_DOT_CHAR) ) {
			if ( ((pDest == pStart) || ((pDest - 1) == pStart)) &&
			     (((pDest + 2) == f.length()) || (f.charAt(pDest + 2) == WAE_URL_PATH_CHAR)) ) {
			    // path = null;
			    return path.toString();
			}
			/* see if we're at end of string */
			if ((pDest + 2) == f.length()) {
			    pSegment = pDest - 2;
			    while ((pSegment >= pStart) && (f.charAt(pSegment) != WAE_URL_PATH_CHAR)) {
				pSegment--;
			    }
			    if (f.charAt(pSegment) == WAE_URL_PATH_CHAR) {
				path.delete(pSegment + 1, f.length());
			    }
			    else {
				path.delete(pSegment + 1, f.length());
			    }
			    shortened = false;
			}
			else if (f.charAt(pDest + 2) == WAE_URL_PATH_CHAR) {
			    pSegment = pDest - 2;
			    while ((pSegment > pStart) && (f.charAt(pSegment) != WAE_URL_PATH_CHAR)) {
				pSegment--;
			    }
			    /*
			     * Move pDest + 2 to pSegment.  
			     */
			    //System.out.println(pSegment + " " + pDest);
			    path.delete(pSegment, pDest + 2);
			    pDest = pSegment;
			}
		    } else {
			pDest++;
		    }
		}
	    }
  
	return path.toString();
    }


    /**
     *  Returns URI that is made from base and relative URL
     */
    public static URI relToAbs(URI base, URI relative) throws Exception
    {

	/*
	 * Step1 -- Parse URL into its components
	 */
	if (base == null) {
	    return null;
	}
    
	if (relative == null) {
	    return base;
	}

	/*
	 * Step 2
	 */

	/* If the path component is empty and the scheme, authority, and query components are 
	 * undefined, then it is a reference to the current document and we are done.  
	 *
	 * Otherwise, the reference URI's query and fragment components are defined as found 
	 * (or not found) within the URI reference and not inherited from the base URI.
	 */

	if (relative.getScheme() == null && relative.getAuthority() == null && 
	    relative.getPath() == null &&  relative.getQuery() == null) {
	    return base;
	}


	/*Step 3*/
  
	/* If the scheme component is defined, indicating that the reference starts with 
	 * a scheme name, then the reference is interpreted as an absolute URI 
	 * and we are done.  
	 * 
	 * Otherwise, the reference URI's scheme is inherited from the base URI's scheme 
	 * component.
	 */

	if (relative.getScheme() != null) {
	    return relative;
	}

	/*Step 4*/

	/* If the authority component is defined, then the reference is a network-path
	 *
	 * Otherwise, the reference URI's authority is inherited from the base URI's 
	 * authority component, which will also be undefined if the URI scheme does not  
	 * use an authority component.
	 * Note:  referee is a network path
	 */
	if (relative.getAuthority() != null) {
	    return new URI(base.getScheme(), relative.getHost(), relative.getPort(), relative.getPath(), 
			   relative.getQuery(), relative.getFragment());
	}

	/*Step5*/
	/* If the path component begins with a slash character, then the reference is an absolute
	 * path
	 * Note: the reference is an absolute path
	 */

	if ((relative.getPath() != null) && (relative.getPath().startsWith("/"))) {
	    return new URI(base.getScheme(), base.getHost(), base.getPort(), 
			   relative.getPath(), relative.getQuery(), relative.getFragment());
	}

	/* 
	 * Otherwise resolving a relative path reference 
	 */
	String result = getResolvedPath(base.getPath(), relative.getPath());
	return new URI(base.getScheme(), base.getHost(), base.getPort(), 
		       result, relative.getQuery(), relative.getFragment());
    }

    /**
     * This rutine parses jar:file: URIs
     */
    private void uriGetJarFileParts() throws Exception {
	int pStart = 0;
	int pEnd = 0;

	if (origUri == null) {
	    throw new Exception("URI not defined");
	}

	if (!isLegalEscapedString(origUri)) {
	    throw new Exception("URI contains Invalid Escaping");
	}

	// Clean URI/URL parts
	scheme = null;
	schemeStart = -1;
	schemeEnd = -1;
	opaque = null;
	authority = null;
	authorityStart = -1;
	authorityEnd = -1;
	userInfo = null;
	host = null;
	hostStart = -1;
	hostEnd = -1;
	port = null; 
	portStart = -1;
	portEnd = -1;
	path = null;
	pathStart = -1;
	pathEnd = -1;
	query = null;
	queryStart = -1;
	queryEnd = -1;
	fragment = null;
	fragStart = -1;
	fragEnd = -1;
	isIPAddress = false;

	fragStart  = origUri.indexOf(WAE_URL_FRAG_CHAR);
	queryStart = origUri.indexOf(WAE_URL_QUERY_CHAR);
	pEnd = origUri.length();

	/* can only have one frag */
	if ( (fragStart != -1) &&
	     (origUri.indexOf(WAE_URL_FRAG_CHAR, fragStart+1) != -1) ) {
	    throw new Exception("URI contains two fragments");
	}

	if (fragStart != -1) {
	    fragment = origUri.substring(fragStart+1);
	    pEnd = fragStart;
	    fragEnd = origUri.length();
	}

	/* fragment can contain query char, but not the other way 'round */
	if ((queryStart != -1) && (fragStart != -1) &&
	    (queryStart >= fragStart)) {
	    queryStart = -1;
	}
	else if (queryStart != -1) {
	    query = origUri.substring(queryStart+1, pEnd);
	    queryEnd = pEnd;
	    pEnd = queryStart;
	}

	/* we've stripped off frag and query, so current pEnd will be end of */
	/* path if a path exists. */
	pathEnd = pEnd;

	scheme = JAR_FILE_SCHEME;
	schemeStart = 0;
	schemeEnd = JAR_FILE_SCHEME.length();

	if (origUri.length() > (schemeEnd+1)) {
	   pathStart = schemeEnd + 1;
	   path = origUri.substring(pathStart, pathEnd);
	}
    }

    private static boolean shouldEscape(char ch)
    {
	if ( ((ch >= 0x00) && (ch <= 0x1F)) || (ch == 0x7F) ) return true; /*control characters*/

	if ( (ch == 0x20) ) return true; /*space*/
  
	if ( (ch == ';') ||
	     (ch == '/') ||
	     (ch == '?') ||
	     (ch == ':') ||
	     (ch == '@') ||
	     (ch == '&') ||
	     (ch == '=') ||
	     (ch == '+') ||
	     (ch == '$') ||
	     (ch == ',') ) return true; /*Reserved*/

	if ( (ch == '{') ||
	     (ch == '}') ||
	     (ch == '|') ||
	     (ch == '\\') ||
	     (ch == '^') ||
	     (ch == '[') ||
	     (ch == ']') ||
	     (ch == '_') ||     // some browsers have problems with _
	     (ch == '\'') ) return true; /* unwise */


	if ( (ch == '<') ||
	     (ch == '>') ||
	     (ch == '#') ||
	     (ch == '%') ||
	     (ch == '"') ) return true; /* Delims */

	if ( (ch >= 0x8F) && (ch <= 0xFF) ) return true; /*Non-US-ASCII*/

	return false;
    }

    /**  
     * This function will encode a string into x-www-form-urlencoded format.
     */
    public static String escapeString( String src ) throws Exception
    {
	int posInSrc = 0;
	int posInDest = 0;
	StringBuffer sb = new StringBuffer();
	boolean allASCII=true;

	if (src == null) {
	    return null;
	}

	/* Walk through the source and escape any characters into sb */
	posInDest = 0;
	for ( posInSrc = 0; posInSrc < src.length(); posInSrc++ ) {
	    if (src.charAt(posInSrc) > 127) {
		allASCII = false;
	    }
    
	    /* MJA */
	    if (src.charAt(posInSrc) >= 0xFF) { /*if string contains characters > 0xFF an invalid value is returned*/
		throw new Exception("Illegal Character in String");
	    }
      
	    String escaped = escapeChar(src.charAt(posInSrc));
	    sb.append(escaped);
	}

	return sb.toString();
    }

    /**
     * Escapes character
     */
    private static String escapeChar(char c) {
	StringBuffer sb = new StringBuffer();
	final String HEXDIGITS[] = {"0", "1", "2", "3","4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F" };
	if ( shouldEscape(c) ) {
	    sb.append(WAE_URL_ESCAPE_CHAR);
	    sb.append(HEXDIGITS[((int)c) >> 4]);
	    sb.append(HEXDIGITS[((int)c) & 15]);

	    return sb.toString();
	}
	else {
	    /*Apply one to one escaping to space.*/
	    if ( c == ' ' ) {
		sb.append("%20"); 
	    }
	    else {
		sb.append(c);
	    }
	}
	return sb.toString();
    }

    /**
     * Returns the length of original URI string that was given
     */
    public int length() {
	if (origUri != null) {
	    return origUri.length();
	}
	return -1;
    }

    public boolean equals(URI u) {
	if (u == null) {
	    return false;
	}
	if (u.toString().equals(origUri)) {
	    return true;
	}
	return false;
    }

    public boolean equals(String u) {
	if (u == null) {
	    return false;
	}
	if (u.equals(origUri)) {
	    return true;
	}
	return false;
    }

    /**
     * Returns the original URI string that was given
     */
    public String toString() {
	return origUri;
    }

    /**
     * Returns the relative URI of the referer url with respect to the target url
     */
    public static String getRelativeUri(String refererUri, String targetUri) {
	    if (refererUri == null || targetUri == null) {
	      return null;
	    }
	    
	    if (!URI.isValidUri(refererUri) || !URI.isValidUri(targetUri)) {
	      return null;
	    }	    
	    
	    try {
        // Get tokens with "/" as delimiter
        StringTokenizer referer = new StringTokenizer(refererUri, "/");
        StringTokenizer target = new StringTokenizer(targetUri, "/");

        String targetToken = null;
        String refererToken = null;

        int tokenCount = 1;
          
        int length = (referer.countTokens() < target.countTokens())? referer.countTokens():target.countTokens();        
          
        // Compare each token of the Uri        
        while ( tokenCount <= length ) {
            
          targetToken = target.nextToken();
          refererToken = referer.nextToken();
            
          // If token is scheme or domain, compare them
          if (tokenCount < 2) {            
              
            if (!refererToken.equalsIgnoreCase(targetToken)) {// scheme OR domain does not match            
              return refererUri;
            }
          }
            
          // If token is not scheme or domain
          else {
              
            if (!refererToken.equalsIgnoreCase(targetToken)) {// they do not match              
              
              // include "/" in not the file element 
              if (referer.countTokens() > 0) {
                refererToken = "/" + refererToken;
              }            
              while( referer.hasMoreElements() ) {
                // get the remaning String and concatenate
                refererToken = refererToken + referer.nextToken("");
              }
              break;
            }
          }
          tokenCount++;
        }

        return refererToken;
      } catch(Exception exp) {
        return null;
      }
	    
    }


    /**
     * Return debug dump from the class
     */
    public String debugDump() {
	StringBuffer s = new StringBuffer();
	s.append("Scheme=");
	s.append(scheme);
	s.append(", schemeStart=");
	s.append(schemeStart);
	s.append(", schemeEnd=");
	s.append(schemeEnd);
	s.append(", Opaque=");
	s.append(opaque);
	s.append(", Authority=");
	s.append(authority);
	s.append(", authorityStart=");
	s.append(authorityStart);
	s.append(", authorityEnd=");
	s.append(authorityEnd);
	s.append(", UserInfo=");
	s.append(userInfo);
	s.append(", Host=");
	s.append(host);
	s.append(", hostStart=");
	s.append(hostStart);
	s.append(", hostEnd=");
	s.append(hostEnd);
	s.append(", Port=");
	s.append(port);
	s.append(", portStart=");
	s.append(portStart);
	s.append(", portEnd=");
	s.append(portEnd);
	s.append(", Query=");
	s.append(query);
	s.append(", queryStart=");
	s.append(queryStart);
	s.append(", queryEnd=");
	s.append(queryEnd);
	s.append(", Path=");
	s.append(path);
	s.append(", pathStart=");
	s.append(pathStart);
	s.append(", pathEnd=");
	s.append(pathEnd);
	s.append(", Fragment=");
	s.append(fragment);
	s.append(", fragStart=");
	s.append(fragStart);
	s.append(", fragEnd=");
	s.append(fragEnd);
	s.append(", isIPAddress=");
	s.append(isIPAddress);

	return s.toString();
    }  
    
    
    public static void main(String srgs[]) {
      try { 
        System.out.println(URI.getRelativeUri("http://waplabdc/br5owser/areddy/anil.asp", 
                                      "http://waplabdc/browser/areddy/mika.asp"));
      } catch(Exception ex) {
      }
    }
}
