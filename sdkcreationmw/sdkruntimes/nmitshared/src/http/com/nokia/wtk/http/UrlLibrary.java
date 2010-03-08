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

import java.net.InetAddress;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLDecoder;
import java.util.Map;
import java.util.HashMap;
import java.util.StringTokenizer;
import com.nokia.wtk.util.MessageWriters;
import com.nokia.wtk.util.MimeType;

/**
 * URL manipulation functions
 */
public class UrlLibrary implements MimeType {

    private static Map mimeTable = null;        // File name to MimeType
    private static Map reverseMimeTable = null; // MimeType to file suffix

    /**
     * Return a URL and possible convert relative URL to absolute.
     * Report any errors to user.
     * @param urlString relative URL string to convert.
     * @param base a base URL against which to expand a relative URL.
     */
    public static URL getAbsoluteUrl(String urlString, URL base) {
        URL url = null;

        if (urlString != null) {
            try {
                // URL may already be in absolute form:
                url = new URL(urlString);
            } catch (MalformedURLException ex) {
                // Assume it is a relative URL and expand it:
                if (base != null) {
                    try {
                        String newUrlString = UrlLibrary.expandRelativeURL(urlString, base);
                        url = new URL(newUrlString);
                    } catch (MalformedURLException ex2) {
                        url = null;
                        MessageWriters.ErrorMsg.println("MalformedURLException: " + urlString);
                    }
                } else {
                    url = null;
                    MessageWriters.ErrorMsg.println("MalformedURLException: " + urlString);
                }
            }
        } else {
            MessageWriters.ErrorMsg.println("WMLBrowserImpl.go does not contain url");
        }
        return url;
    }

    /**
     * Get the relative URL that represents the shortest path from a given base URL.
     * @param urlStr the absolute URL string to (possibly) compress
     * @return the compressed relative URL or the original URL 'urlStr' as appropriate
     */
    public static String compressRelativeURL(String urlStr, URL base) {
        if (base != null) {
            String baseStr = base.toExternalForm();
            if (urlStr == null) {
                return baseStr;
            }
            if (urlStr.indexOf(baseStr) == 0) {
                // Simple concatenated suffix case: (e.g. #cardname)
                urlStr = urlStr.substring(baseStr.length());
            } else  {
                //TODO
            }
        }
        return urlStr;
    }

    /**
     * Expand a relative URL string to an absolute URL string, using a given base URL.
     * @param urlStr the URL string to (possibly) expand
     * @param base the base URL from to expand
     * @return the expanded URL or the original URL 'urlStr' as appropriate
     */
    public static String expandRelativeURL(String urlStr, URL base) {
        int i = 0;


        if (base != null) {
            String baseDirectory = getDirectory(base);
            String baseFile = getFileName(base);
            String baseStr = base.toExternalForm();

            if (urlStr == null) {
                return baseStr;
            }

            // Make ./filename to filename
            if (urlStr.startsWith("./")) {
                urlStr = urlStr.substring(2);
            }

            // is reference part
            if (urlStr.startsWith("#")) {
                int hash = baseStr.indexOf("#");
                if (hash > 0) {
                    baseStr = baseStr.substring(0, hash);
                }
                urlStr = baseStr + urlStr;
            } // is query part
            else if (urlStr.startsWith("?") || urlStr.startsWith(";")) {
                // This relative URL starts from query
                    // try to cut everything strating from name. See RFC2096 C.1.  Normal Examples
                    baseStr = getDirectory(base);
                urlStr = baseStr + urlStr;
            }
            else {
                // it has file part, so we can try to remove ..:s
                if (urlStr.startsWith("//")) {
                    // This relative URL starts from root
                    String protocol = base.getProtocol();
                    urlStr = protocol + ":" + urlStr;
                }
                else if (urlStr.startsWith("/")) {
                    // This relative URL starts new directory
                    String protocol = base.getProtocol();
                    if (protocol.equalsIgnoreCase(MimeType.URL_FILE_SCHEME)) {
                        urlStr = MimeType.URL_FILE_SCHEME + MimeType.URL_SCHEME_SEP + urlStr;
                    } else if (protocol.equalsIgnoreCase(MimeType.URL_HTTP_SCHEME)) {
                        String host = base.getHost();
                        int port = base.getPort();
                        if (port == -1) {
                            urlStr = MimeType.URL_HTTP_SCHEME + MimeType.URL_SCHEME_SEP + host + urlStr;
                        } else {
                            urlStr = MimeType.URL_HTTP_SCHEME + MimeType.URL_SCHEME_SEP + host + ":" + port + urlStr;
                        }
                    }
                }
                else {
                    int end = -1;
                    if (urlStr.indexOf('#') != -1)
                        end = urlStr.indexOf('#');
                    if (urlStr.indexOf('?') != -1)
                        end = urlStr.indexOf('?');
                    if (urlStr.indexOf(';') != -1)
                        end = urlStr.indexOf(';');


                    int separator = urlStr.indexOf(MimeType.URL_SCHEME_SEP2);
                    if (separator == -1 || (end != -1 && separator > end)) {
                        urlStr = baseDirectory + urlStr;
                    }
                }

                if ((i=urlStr.indexOf("..")) != -1){
                    urlStr = removeDotDots(urlStr);
                }

                if ((i=urlStr.indexOf("/.")) != -1){
                    urlStr = removeSingleDots(urlStr);
                }
            }
        }

        return urlStr;
    }

    /**
     * Encode post data. Don't use. Dangerous !!!
     */
    public static String encodePostData(String postData) {
        StringBuffer buf = new StringBuffer();
        if (postData != null) {
            // Encode query
            StringTokenizer st = new StringTokenizer(postData, "&");
            while (st.hasMoreTokens()) {
              String token = st.nextToken();
              int i = token.indexOf('=');
                  if (i < 0) {
                    // no '=' mark
                    buf.append(encodeString(token));
                  } else {
                    // encode left side
                    buf.append(encodeString(token.substring(0, i)));
                    buf.append("=");
                    // encode rigth side
                    buf.append(encodeString(token.substring(i + 1)));
                  }

              // add '&' if needed
              if (st.hasMoreTokens()) {
                buf.append("&");
              }
            }
        }
        return buf.toString();
    }

    /**
     * Translates a string into x-www-form-urlencoded format.
     * @param str string to encode
     */
    public static String encodeString(String str) {
        if (str == null) {
            return null;
        }
        int i, len;
        char c;
        StringBuffer sb = new StringBuffer();

        if ((len = str.length()) == 0) return str;
        try{
            str = URI.escapeString(str);
        }
        catch(Exception e){
            // Error in Escaping
            return null;
        }
        return str;
    }

    /**
     * Translates x-www-form-urlencoded format into string
     * @param s string to decode
     */
    public static String decodeString(String s) throws Exception {
        for (int i=0; i < s.length();i++) {
            char c = s.charAt(i);
            //System.out.println("c is " + (int)c);
            if (c < 0 || c > 128) {
                // Spec says we have to exception here
                throw new Exception("Unsupported characted");
            }
        }
        s = URLDecoder.decode(s);
        return s;
    }

    /**
     * Checks if URL is valid or not
     * supports relative URLs as well.
     * @param candidate is a candidate for a URL
     */
    public static boolean isValid(String candidate) {
        if (candidate == null)
            return false;

        candidate = candidate.trim();

        if (candidate.indexOf("::") != -1)
            return false;

        try{
            URL url = new URL(candidate);
            return checkUrl(candidate);
        }
        catch(MalformedURLException e){

            if (candidate.indexOf("://") > 0)
                return false;

            // Might be relative URL
            if (candidate.startsWith("//")) {
                // relative URL starting with host
                candidate = "http:" + candidate;
            } else if (candidate.startsWith("/")) {
                // relative URL starting with path to root
                candidate = "http://hostname" + candidate;
            } else if (candidate.startsWith(";") || candidate.startsWith("#")) {
                // relative URL starting with path to root
                candidate = "http://hostname/path" + candidate;
            } else if (candidate.indexOf(":/") != -1) {
                // it contains schema, check that it is before #
                int ref = candidate.indexOf("#");
                int schema = candidate.indexOf(":/");
                if (ref != -1 && schema < ref) {
                    return false;
                } else {
                    candidate = "http://hostname/path/" + candidate;
                }
                candidate = "http://hostname/path" + candidate;
            } else {
                // relative URL starting with filename or directory name
                candidate = "http://hostname/path/" + candidate;
            }
            try{
                URL url = new URL(candidate);
                // check if URL contains illegal characters
                return checkUrl(candidate);
            }
            catch(MalformedURLException e2){
                // We must give up
                return false;
            }
        }

    }

    /**
     * This function includes all kinds of addition URL checks.
     */
    private static boolean checkUrl(String candidate) {
        if (checkEncodingUrl(candidate) == false) {
            return false;
        }

        if (candidate.startsWith("http")) {
            if (candidate.indexOf("\\") != -1) {
                return false;
            }
            if (candidate.indexOf(" ") != -1) {
                return false;
            }
            if (candidate.indexOf("\t") != -1) {
                return false;
            }
            if (candidate.indexOf('"') != -1) {
                return false;
            }
            if (candidate.indexOf("`") != -1) {
                return false;
            }
            if (candidate.indexOf('^') != -1) {
                return false;
            }
            if (candidate.indexOf('{') != -1) {
                return false;
            }
            if (candidate.indexOf('}') != -1) {
                return false;
            }
            if (candidate.indexOf('[') != -1) {
                return false;
            }
            if (candidate.indexOf(']') != -1) {
                return false;
            }
            if (candidate.indexOf('<') != -1) {
                return false;
            }
            if (candidate.indexOf('>') != -1) {
                return false;
            }
            if (candidate.indexOf('|') != -1) {
                return false;
            }
        }

        return true;
    }


    /**
     * This function checks that URL is encoded in the right way
     * @returns true if URL is valid
     */
    private static boolean checkEncodingUrl(String candidate) {
        int loc = 0;

        // Idea is to check that if you have % then you have to have to digits after that
        while((loc = candidate.indexOf("%", loc)) != -1) {
            try {
                String encoding = candidate.substring(loc + 1, loc + 3);
                char d1 = encoding.charAt(0);
                char d2 = encoding.charAt(1);
                if (isHexCharacter(d1) && isHexCharacter(d2)) {
                    loc++;
                    continue;
                } else {
                    return false;
                }
            } catch(IndexOutOfBoundsException iex) {
                return false;
            }
        }
        return true;
    }

    /**
     * Checks if the character is hex
     */
    private static boolean isHexCharacter(char c) {
        if (Character.isDigit(c))
            return true;
        char l = Character.toLowerCase(c);
        if (l >= 'a' && l <= 'f') {
            return true;
        }
        return false;
    }

    /**
     * Get query string e.g.
     * "http://www.nokia.con/pathname/file.wml;32;64?a=10&b=20"
     * returns the string "a=10&b=20"
     */
    public static String getQuery(URL url) {
        if (url == null) {
            return null;
        }
        String u = url.toExternalForm();
            int ind = u.indexOf('?');
            String ref = ind < 0 ? null: u.substring(ind + 1);

            // Take # part off if possible
        if (ref != null) {
                ind = ref.indexOf('#');
                if (ind >= 0) {
                    ref = ref.substring(0, ind);
                }
            }
        return ref;
    }

    /**
     * Insert query string e.g.
     * "http://www.nokia.con/pathname/file.wml" + "?a=b" = "http://www.nokia.con/pathname/file.wml?a=b"
     */
    public static String insertQuery(String url, String query) {
        if (url == null || query == null) {
            return null;
        }
        boolean hasQuery = false;
        String ref = "";
            int ind = url.indexOf('?');
        if (ind != -1) {
            hasQuery = true;
        }

            ind = url.indexOf('#');
            if (ind >= 0) {
                ref = url.substring(ind);
                url = url.substring(0, ind);
            }

            if (hasQuery) {
                url = url + "&";
            }
            else {
                url = url + "?";
            }

            url += query + ref;

        return url;
    }

    /**
     * Get Reference string e.g.
     * "http://www.nokia.con/pathname/file.wml;32;64?a=10&b=20#place"
     * returns the string "place"
     * @param url
     */
    public static String getRef(String url) {
        if (url == null) {
            return null;
        }

            int ind = url.indexOf('#');
            if (ind >= 0) {
                return url.substring(ind);
        }
        return null;
    }

    /**
     * Get domain name e.g.
     * "http://www.nokia.con/pathname/file.wml;32;64?a=10&b=20#place"
     * returns nokia.com
     */
    public static String getDomainName(URL url) {
        if (url == null) {
            return null;
        }
        String host = url.getHost();
        int i = host.indexOf('.');
        if (i >= 0 && host.length() > i) {
            return host.substring(i+1);
        } else {
            return null;
        }
    }

    /**
     * Returns a string that contains the semi-colon(;) separated parameter list e.g.
     * "http://www.nokia.con/pathname/file.wml;32;64?a=10&b=20"
     * returns the string ";32;64"
     */
    public static String getParams(URL url) {
        if (url == null) {
            return null;
        }
        String u = url.toExternalForm();
            int s = u.indexOf(';');

            // No list
            if (s == -1)
                return null;

            int e = u.indexOf('?', s);

            // cut at ?
            if (e > 0 && e > s)
                return u.substring(s+1, e);

            e = u.indexOf('#', s);
            if (e > 0 && e > s)
                return u.substring(s+1, e);

            // return the rest
            return u.substring(s+1);
    }

    /**
     * Returns URL without reference (#-part is ripped off)
     */
    public static String getBase(URL url) {
        if (url == null) {
            return null;
        }
        String u = url.toExternalForm();
            return getBase(u);
    }

    public static String getBase(String url) {
        if (url == null) {
            return null;
        }
            int ind = url.indexOf('#');
            String ref = ind < 0 ? url: url.substring(0, ind);

            return ref;
    }

    /**
     * Return URL with directory part e.g.
     * 	"http://www.nokia.com/sdk/examples/file.wml"
         *  returns "http://www.nokia.com/sdk/examples/"
     *
     */
    public static String getDirectory(URL url) {
        if (url == null) {
            return null;
        }
        String u = getPathUrl(url);

        // check if path exist, if not add "/" at the end of domain
        try {
          url = new URL(u);
          if (url.getPath() == "") {
            u = u + "/";
          }
        }catch(Exception e){
          return null;
        }


            // Now remove the file name ie. everything after last /
            int ind = u.lastIndexOf('/');
            String ref = ind < 0 ? null: u.substring(0, ind + 1);
            return ref;
    }

    /**
     * Return URL with path part e.g.
     * 	"http://www.nokia.com/sdk/examples/file.wml;a;b#abc"
         *  returns "http://www.nokia.com/sdk/examples/file.wml"
     *
     */
    public static String getPathUrl(URL u) {
        if (u == null) {
            return null;
        }
        String url = u.toExternalForm();

            // try to cut ;-part off
            int ind = url.indexOf(';');
            if (ind >= 0) {
                url = url.substring(0, ind);
        } else {
                // try to cut #-part off
                ind = url.indexOf('?');
                if (ind >= 0) {
                    url = url.substring(0, ind);
            } else {

                    // try to cut ?-part off
                    ind = url.indexOf('#');
                    if (ind >= 0) {
                        url = url.substring(0, ind);
                    }
                }
            }

            return url;
    }

    /**
     * Return path part from URL e.g.
     * 	"http://www.nokia.com/sdk/examples/file.wml;a;b#abc"
         *  returns "/sdk/examples/file.wml"
     *
     */
    public static String getPath(String url) {
        if (url == null) {
            return null;
        }

        try {
            URL foo = new URL(url);
            url = foo.getFile();
        } catch(Exception e) {
            // It's relative
        }

            // try to cut ;-part off
            int ind = url.indexOf(';');
            if (ind >= 0) {
                url = url.substring(0, ind);
        } else {

                // try to cut #-part off
                ind = url.indexOf('?');
                if (ind >= 0) {
                    url = url.substring(0, ind);
            } else {

                    // try to cut ?-part off
                    ind = url.indexOf('#');
                    if (ind >= 0) {
                        url = url.substring(0, ind);
                    }
                }
            }

            return url;
    }

    /**
     * Returns the current domain name with the leading .
     * e.g. .americas.nokia.com
     */
    public static String getDomainName() {
        try {
                    InetAddress addr  = InetAddress.getLocalHost();
                    InetAddress addr2 = InetAddress.getByName(addr.getHostAddress());
            String fullHost = addr2.getHostName();
            if (fullHost == null) {
                return null;
            }
            int dot = fullHost.indexOf('.');
            if (dot == -1) {
                return null;
            }
            return fullHost.substring(dot);
        } catch(Exception ex) {
            return null;
        }
    }

    /**
     * Returns URL string without ..-marks.
     * Can be used to get minimal URL.
     * @param url to work
     * @return String - minimized URL
     */
    public static String removeDotDots(String url) {
        if (url == null) {
            return null;
        }

        String path = getPath(url);
        int begin = url.indexOf(path);

        int end = -1;
        if (url.indexOf('#') != -1)
            end = url.indexOf('#');
        if (url.indexOf('?') != -1)
            end = url.indexOf('?');

        StringBuffer buf = new StringBuffer(url);
        int ind = 0;
            // try to cut /..-part off
            while((ind = url.indexOf("/..")) != -1) {
                // Replace file path area only
                if (end != -1 && end < ind)
                    break;
            int o = url.lastIndexOf("/", ind-1);
            // don't go to host part
            if (begin != -1 && o < begin)
                break;
            buf.delete(o, ind+3);
            url = buf.toString();
            }
            // try to cut \..-part off
            while((ind = url.indexOf("\\..")) != -1) {
                // Replace file path area only
                if (end != -1 && end < ind)
                    break;
            int o = url.lastIndexOf("\\", ind-1);
            // don't go to host part
            if (begin != -1 && o < begin)
                break;
            buf.delete(o, ind+3);
            url = buf.toString();
            }

            return url;
    }

    /**
     * Returns URL string without /./-parts.
     * Can be used to get minimize the URL
     * @param url to work
     * @return String - minimized URL
     */
    public static String removeSingleDots(String url) {
        if (url == null) {
            return null;
        }
        String path = getPath(url);
        int begin = url.indexOf(path);

        int end = -1;
        if (url.indexOf('#') != -1)
            end = url.indexOf('#');
        if (url.indexOf('?') != -1)
            end = url.indexOf('?');

        StringBuffer buf = new StringBuffer(url);
        int ind = 0;
            // try to cut /./-part off
            while((ind = url.indexOf("/./")) != -1) {
                // Replace file path area only
                if (end != -1 && end < ind)
                    break;
            buf.delete(ind, ind+3);
            url = buf.toString();
            }

        if (url.endsWith("/.")) {
            int len = url.length();
            url = url.substring(0, len-1);
        }

        return url;
    }

    /**
     * Returns the file name e.g.
     * "http://www.nokia.com/sdk/examples/file.wml" returns "file.wml"
     */
    public static String getFileName(URL url) {
        if (url == null) {
            return null;
        }
        String u = url.toExternalForm();

            // try to cut ;-part off
            int ind = u.indexOf(';');
            if (ind >= 0) {
                u = u.substring(0, ind);
        } else {

                // try to cut #-part off
                ind = u.indexOf('#');
                if (ind >= 0) {
                    u = u.substring(0, ind);
            } else {

                    // try to cut ?-part off
                    ind = u.indexOf('?');
                    if (ind >= 0) {
                        u = u.substring(0, ind);
                    }
                }
            }

            ind = u.lastIndexOf('/');
            String ref = ind < 0 ? null: u.substring(ind + 1);
            return ref;
    }

    /**
     * Compares two domains for access control e.g.
     * orig www.wapforum.org match against access domain wapforum.org
     * but not match against access domain forum.org
     * See access element 11.3.1
     */
    public static boolean matchAccessDomain(String origDomain, String accessDomain) {
        if (origDomain == null || accessDomain == null)
            return false;
        // Remove Port Number, if present
        if (accessDomain.indexOf(":") != -1)
        {
            int endIndex = accessDomain.lastIndexOf(":");
            accessDomain = accessDomain.substring( 0, endIndex );
        }
        if (origDomain.equalsIgnoreCase(accessDomain))
            return true;
        int i = origDomain.indexOf('.');
        while(i > 0) {
            String d = origDomain.substring(i+1);
            if (d.equalsIgnoreCase(accessDomain))
                return true;
            i = origDomain.indexOf('.', i+1);
        }

        return false;
    }

    /**
     * Compares two access paths for access control e.g.
     * orig /cbb/cgi.pl match against access path /cbb
     * but not match against access path /test/hello.wml
     * String must start with /-symbol !!!
     * See access element 11.3.1
     */
    public static boolean matchAccessPath(String origPath, String accessPath) {
        if (accessPath == null || origPath == null || origPath.equals(accessPath) || accessPath.equals("/"))
            return true;
        if (!accessPath.endsWith("/")) {
            accessPath = accessPath + "/";
        }
        if (origPath.startsWith(accessPath))
            return true;

                // Before returning failure try one more thing.  If the origPath
                // begins with "//", then treat it as just one "/".
                if (origPath.startsWith("//") && origPath.startsWith(accessPath, 1))
                        return true;

                return false;
    }

    /**
     * Match URL against access domain and access path.
     * See access element 11.3.1
     * @param origURL referrer URL
     * @param accessDomain is access domain from WML page
     * @param accessPath is access path from WML page
     **/
    public static boolean matchAccess(URL origURL, String accessDomain, String accessPath) {
        if (origURL == null)
            return false;
        String protocol = origURL.getProtocol();
        if (protocol.equals(URL_FILE_SCHEME))
            return false;

        String origDomain = origURL.getHost();
        String origPath = origURL.getFile();

        if (!accessPath.startsWith("/")) {
            // it's relative url
            String tmp = origPath;

            if (tmp == null)
                tmp = "";

            // let's remove file name
                int ind = tmp.lastIndexOf('/');
                tmp = ind < 0 ? "": tmp.substring(0, ind + 1);

            if (!tmp.endsWith("/")) {
                tmp = tmp + "/";
            }
            accessPath = tmp + accessPath;
            // Let's remove relative urls
        }

        accessPath = removeDotDots(accessPath);
        accessPath = removeSingleDots(accessPath);

         //System.out.println("orig: '" + origPath + "' '" + accessPath + "'");
         //System.out.println("orig: '" + origDomain + "' '" + accessDomain + "'");
        boolean p = matchAccessPath(origPath, accessPath);
        boolean d = matchAccessDomain(origDomain, accessDomain);
         //System.out.println("match path " + p + " match domain " + d);

        if (p == true && d == true)
            return true;
        else
            return false;
    }

    /**
     * Tries to get Mime Type from the filename.
     * Function will match against the end of file name, so
     * filename must be pure file name. See also getFileName.
     * @param filename to resolved
     * @see MimeType
     */
    public static String getMimeType(String filename) {
        if (filename == null) {
            return null;
        } else {
            String mimeType = null;
            int dot = filename.lastIndexOf('.');
            if (dot >= 0) {
                if (mimeTable == null) createMimeTable();
                String ext = filename.substring(dot).toLowerCase();
                mimeType = (String)mimeTable.get(ext);
            }
            return ((mimeType == null) ? "" : mimeType);
        }
    }

    /**
     * Creates the hashtable that maps lowercase file extensions
     * (including dot) into appropriate Mime type.
     */
    private static synchronized void createMimeTable() {
        if (mimeTable == null) {
            mimeTable = new HashMap(37);

                           /* extension */           /* Mime type */
            // WAP
            mimeTable.put( WML_SOURCE_SUFFIX,        WML_SOURCE          );
            mimeTable.put( WML_BINARY_SUFFIX,        WML_BINARY          );
            mimeTable.put( WBXML_BINARY_SUFFIX,      WBXML_BINARY        );
            mimeTable.put( WML_SCRIPT_SOURCE_SUFFIX, WML_SCRIPT_SOURCE   );
            mimeTable.put( WML_SCRIPT_BINARY_SUFFIX, WML_SCRIPT_BINARY   );
            // XHTML
            mimeTable.put( XHTML_SUFFIX,             TEXT_XHTML          );
            mimeTable.put( WML2_SOURCE_ALT_SUFFIX,   WML2_SOURCE         );
            mimeTable.put( XML_SUFFIX,               TEXT_XML            );
            mimeTable.put( HTML_SUFFIX,              TEXT_HTML           );
            mimeTable.put( HTML2_SUFFIX,             TEXT_HTML           );
            mimeTable.put( TEXT_SUFFIX,              TEXT_PLAIN          );
            mimeTable.put( CSS_SUFFIX,               TEXT_CSS            );
            // push
            mimeTable.put( PUSH_SI_SOURCE_SUFFIX,    PUSH_SI_SOURCE      );
            mimeTable.put( PUSH_SI_BINARY_SUFFIX,    PUSH_SI_BINARY      );
            mimeTable.put( PUSH_SL_SOURCE_SUFFIX,    PUSH_SL_SOURCE      );
            mimeTable.put( PUSH_SL_BINARY_SUFFIX,    PUSH_SL_BINARY      );
            mimeTable.put( CACHE_OP_SOURCE_SUFFIX,   CACHE_OP_SOURCE     );
            mimeTable.put( CACHE_OP_BINARY_SUFFIX,   CACHE_OP_BINARY     );
            // multipart/mms
            mimeTable.put( MULTIPART_WSP_SUFFIX,     MULTIPART_MIXED_WSP );
            mimeTable.put( MULTIPART_SUFFIX,         MULTIPART_MIXED     );
            mimeTable.put( MMS_MESSAGE_SUFFIX,       MMS_MESSAGE         );
            mimeTable.put( SMIL_SUFFIX,              APP_SMIL            );
            mimeTable.put( SMIL_SUFFIX_2,            APP_SMIL            );
            // images
            mimeTable.put( IMAGE_GIF_SUFFIX,         IMAGE_GIF           );
            mimeTable.put( IMAGE_JPG_SUFFIX,         IMAGE_JPG           );
            mimeTable.put( IMAGE_JPEG_SUFFIX,        IMAGE_JPG           );
            mimeTable.put( IMAGE_PNG_SUFFIX,         IMAGE_PNG           );
            mimeTable.put( IMAGE_WBMP_SUFFIX,        IMAGE_WBMP          );
            mimeTable.put( IMAGE_BMP_SUFFIX,         IMAGE_BMP           );
            // audio
            mimeTable.put( MIDI_SUFFIX,              AUDIO_SP_MIDI       );
            mimeTable.put( MID_SUFFIX,               AUDIO_SP_MIDI       );
            mimeTable.put( AMR_SUFFIX,               AUDIO_AMR           );
            mimeTable.put( AMR_WB_SUFFIX,            AUDIO_AMR_WB        );
            mimeTable.put( WAV_SUFFIX,               AUDIO_WAV           );
            mimeTable.put( RINGTONE_SUFFIX,          APP_RINGTONE        );
            mimeTable.put( RINGTONE_SUFFIX2,         APP_RINGTONE        );
            mimeTable.put( MP3_SUFFIX,               AUDIO_MPEG          );
            // video
            mimeTable.put( AV_3GPP_SUFFIX,           VIDEO_3GPP          );
            // DRM
            mimeTable.put( DRM_MESSAGE_SUFFIX,       DRM_MESSAGE         );
            mimeTable.put( DRM_CONTENT_SUFFIX,       DRM_CONTENT         );
            mimeTable.put( DRM_RIGHTS_SUFFIX,        DRM_RIGHTS          );
            mimeTable.put( DRM_RIGHTS_BIN_SUFFIX,    DRM_RIGHTS_BIN      );
            mimeTable.put( DRM_DOWNLOAD_DESC_SUFFIX, DRM_DOWNLOAD_DESC   );
            // misc
            mimeTable.put( URL_RESPONSE_SUFFIX,      URL_RESPONSE        );
            mimeTable.put( HTTP_RESPONSE_SUFFIX,     HTTP_RESPONSE       );
            mimeTable.put( VCARD_SOURCE_SUFFIX,      VCARD_SOURCE        );
            mimeTable.put( VCAL_SOURCE_SUFFIX,       VCALENDAR_SOURCE    );
            mimeTable.put( SYMBIAN_INSTALL_SUFFIX,   APP_SYMBIAN_INSTALL );
            mimeTable.put( JAR_SUFFIX,               APP_JAVA_ARCHIVE    );
            mimeTable.put( APP_DESCRIPTOR_SUFFIX,    TEXT_APP_DESCRIPTOR );
            mimeTable.put( APP_NOK_S40THEME_SUFFIX,  APP_NOK_S40THEME    );
        }
    }

    /**
     * Tries to get file suffix for a Mime Type.
     * If none found, return null.
     *
     * @param Mime type to get file suffix for.
     * @see MimeType
     */
    public static String getFileSuffix(String mimeType) {
        if (mimeType == null) {
            return null;
        }
        String type = mimeType.toLowerCase();
        String suffix = null;
        if (reverseMimeTable == null)
            createReverseMimeTable();
        suffix = (String)reverseMimeTable.get(type);
        return suffix;
    }

    /**
     * Creates the hashtable that maps Mime type to lowercase file extensions.
     */
    private static synchronized void createReverseMimeTable() {
        if (reverseMimeTable == null) {
            reverseMimeTable = new HashMap(37);

                           /* Mime type */     /* extension */
            // WAP
            reverseMimeTable.put( WML_SOURCE,          WML_SOURCE_SUFFIX         );
            reverseMimeTable.put( WML_BINARY,          WML_BINARY_SUFFIX         );
            reverseMimeTable.put( WBXML_BINARY,        WBXML_BINARY_SUFFIX       );
            reverseMimeTable.put( WML_SCRIPT_SOURCE,   WML_SCRIPT_SOURCE_SUFFIX  );
            reverseMimeTable.put( WML_SCRIPT_BINARY,   WML_SCRIPT_BINARY_SUFFIX  );
            // XHTML
            reverseMimeTable.put( TEXT_XHTML,          XHTML_SUFFIX              );
            reverseMimeTable.put( WML2_SOURCE,         WML_SOURCE_SUFFIX         );
            reverseMimeTable.put( TEXT_XML,            XML_SUFFIX                );
            reverseMimeTable.put( TEXT_HTML,           HTML_SUFFIX               );
            reverseMimeTable.put( TEXT_HTML,           HTML2_SUFFIX              );
            reverseMimeTable.put( TEXT_PLAIN,          TEXT_SUFFIX               );
            reverseMimeTable.put( TEXT_CSS,            CSS_SUFFIX                );
            // push
            reverseMimeTable.put( PUSH_SI_SOURCE,      PUSH_SI_SOURCE_SUFFIX     );
            reverseMimeTable.put( PUSH_SI_BINARY,      PUSH_SI_BINARY_SUFFIX     );
            reverseMimeTable.put( PUSH_SL_SOURCE,      PUSH_SL_SOURCE_SUFFIX     );
            reverseMimeTable.put( PUSH_SL_BINARY,      PUSH_SL_BINARY_SUFFIX     );
            reverseMimeTable.put( CACHE_OP_SOURCE,     CACHE_OP_SOURCE_SUFFIX    );
            reverseMimeTable.put( CACHE_OP_BINARY,     CACHE_OP_BINARY_SUFFIX    );
            // multipart/mms
            reverseMimeTable.put( MULTIPART_MIXED_WSP, MULTIPART_WSP_SUFFIX      );
            reverseMimeTable.put( MULTIPART_RELATED_WSP, MULTIPART_WSP_SUFFIX      );
            reverseMimeTable.put( MULTIPART_ALTERNATIVE_WSP, MULTIPART_WSP_SUFFIX      );
            reverseMimeTable.put( MULTIPART_MIXED,     MULTIPART_SUFFIX          );
            reverseMimeTable.put( MULTIPART_RELATED,   MULTIPART_SUFFIX          );
            reverseMimeTable.put( MULTIPART_ALTERNATIVE,     MULTIPART_SUFFIX          );
            reverseMimeTable.put( MMS_MESSAGE,         MMS_MESSAGE_SUFFIX        );
            reverseMimeTable.put( APP_SMIL,            SMIL_SUFFIX               );
            // images
            reverseMimeTable.put( IMAGE_GIF,           IMAGE_GIF_SUFFIX          );
            reverseMimeTable.put( IMAGE_JPG,           IMAGE_JPG_SUFFIX          );
            reverseMimeTable.put( IMAGE_PNG,           IMAGE_PNG_SUFFIX          );
            reverseMimeTable.put( IMAGE_WBMP,          IMAGE_WBMP_SUFFIX         );
            reverseMimeTable.put( IMAGE_BMP,           IMAGE_BMP_SUFFIX          );
            // audio
            reverseMimeTable.put( AUDIO_SP_MIDI,       MIDI_SUFFIX               );
            reverseMimeTable.put( AUDIO_SP_MIDI,       MID_SUFFIX                );
            reverseMimeTable.put( AUDIO_AMR,           AMR_SUFFIX                );
            reverseMimeTable.put( AUDIO_AMR_WB,        AMR_WB_SUFFIX             );
            reverseMimeTable.put( AUDIO_WAV,           WAV_SUFFIX                );
            reverseMimeTable.put( APP_RINGTONE,        RINGTONE_SUFFIX2          );
            reverseMimeTable.put( AUDIO_MPEG,          MP3_SUFFIX                );
            reverseMimeTable.put( AUDIO_3GPP,          AV_3GPP_SUFFIX            );
            // video
            reverseMimeTable.put( VIDEO_3GPP,          AV_3GPP_SUFFIX            );
            // DRM
            reverseMimeTable.put( DRM_MESSAGE,         DRM_MESSAGE_SUFFIX        );
            reverseMimeTable.put( DRM_CONTENT,         DRM_CONTENT_SUFFIX        );
            reverseMimeTable.put( DRM_RIGHTS,          DRM_RIGHTS_SUFFIX         );
            reverseMimeTable.put( DRM_RIGHTS_BIN,      DRM_RIGHTS_BIN_SUFFIX     );
            reverseMimeTable.put( DRM_DOWNLOAD_DESC,   DRM_DOWNLOAD_DESC_SUFFIX  );
            // misc
            reverseMimeTable.put( URL_RESPONSE,        URL_RESPONSE_SUFFIX       );
            reverseMimeTable.put( HTTP_RESPONSE,       HTTP_RESPONSE_SUFFIX      );
            reverseMimeTable.put( VCARD_SOURCE,        VCARD_SOURCE_SUFFIX       );
            reverseMimeTable.put( VCALENDAR_SOURCE,    VCAL_SOURCE_SUFFIX        );
            reverseMimeTable.put( APP_SYMBIAN_INSTALL, SYMBIAN_INSTALL_SUFFIX    );
            reverseMimeTable.put( APP_JAVA_ARCHIVE,    JAR_SUFFIX                );
            reverseMimeTable.put( TEXT_APP_DESCRIPTOR, APP_DESCRIPTOR_SUFFIX     );
            reverseMimeTable.put( APP_NOK_S40THEME,    APP_NOK_S40THEME_SUFFIX   );
        }
    }

    /**
     * Returns true is two URLS have same deck
     * @param target URL to go
     * @param source URL
     */
    public static boolean isSameDeck(URL target, URL source) {
        if (target == null || source == null) {
            return false;
        }

        String base1 = getBase(target);
        String base2 = getBase(source);
        if (base1.equals(base2)) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * Debugging
     */
    public static void main(String args[]) {
/*
       try {
        URL u1 = new URL("http://www.nokia.com/sdk/examples/file.wml");
        URL u1p = new URL("http://www.nokia.com:888/sdk/examples/file.wml");
        URL u1f = new URL("file://c:/sdk/examples/file.wml");
        URL u2 = new URL("http://www.nokia.com/sdk/examples/file.wml?a@=b&b=f");
        URL u3 = new URL("http://www.nokia.com/sdk/examples/file.wml#re@f");
        URL u4 = new URL("http://www.nokia.com/file.wml");
        URL u5 = new URL("http://www.nokia.com/sdk/examples/file.wml;e=r;df?r=wedf&tr=ty#ref");
        URL u6 = new URL("http://karhu/file.wml");
        URL u7 = new URL("http://karhu/./file.wml");
        URL u8 = new URL("http://karhu/././file.wml");
        URL u9 = new URL("http://karhu/test/./file.wml");
        URL u9b = new URL("http://karhu/test/file.wml");
        URL u10 = new URL("http://karhu/.");
        URL u10b = new URL("http://karhu/");
        URL u11 = new URL("http://testsuite.nokia-boston.com#card1");
        System.out.println("External form = " + u2.toExternalForm());
        System.out.println("URLs");
        System.out.println(u1.toString());
        System.out.println(u2.toString());
        System.out.println(u3.toString());
        System.out.println(u4.toString());
        System.out.println(u5.toString());

        System.out.println("ref for " + u1.toString() + " = " + getRef(u1.toString()));
        System.out.println("ref for " + u2.toString() + " = " + getRef(u2.toString()));
        System.out.println("ref for " + u3.toString() + " = " + getRef(u3.toString()));
        System.out.println("ref for " + u4.toString() + " = " + getRef(u4.toString()));
        System.out.println("ref for " + u5.toString() + " = " + getRef(u5.toString()));

        System.out.println("Get Directory " + u1.toString() + " = " + getDirectory(u1));
        System.out.println("Get Directory " + u2.toString() + " = " + getDirectory(u2));
        System.out.println("Get Directory " + u3.toString() + " = " + getDirectory(u3));
        System.out.println("Get Directory " + u4.toString() + " = " + getDirectory(u4));
        System.out.println("Get Directory " + u5.toString() + " = " + getDirectory(u5));

        System.out.println("D1. " + matchAccessDomain("www.wapforum.org", "wapforum.org"));
        System.out.println("D2. " + matchAccessDomain("www.wapforum.org", "forum.org"));
        System.out.println("D3. " + matchAccessDomain("forum", "forum.org"));

        System.out.println("P1. " + matchAccessPath("/cbb/bonds.cgi", "/cbb"));
        System.out.println("P2. " + matchAccessPath("/internal/cbb/bonds.wml", "/cbb"));
        System.out.println("P3. " + matchAccessPath("/cbb", "/cbb/demos"));
        System.out.println("P4. " + matchAccessPath("/cbb/ab/c", "../.."));

        System.out.println("Encode Post data");
        System.out.println(encodePostData("a=b&a%=b&/"));
        System.out.println(encodePostData("a=b&a=b"));
        System.out.println(encodeString("a=b&a%=b&/"));
        System.out.println(encodeString("a=b&a=b"));

        System.out.println("Domain name");
        System.out.println(getDomainName(u1));
        System.out.println(getDomainName(u6));

        InetAddress ia = InetAddress.getLocalHost();
        System.out.println("Host name: " + ia.getHostName());

        System.out.println("Domain name: " + getDomainName());
        System.out.println("ER0 Base " + u11.toString() + " rel: default.asp result: " +
           expandRelativeURL("default.asp", u11));
        System.out.println("ER1 Base " + u1.toString() + " rel: ../test1.wml result: " +
           expandRelativeURL("../test1.wml", u1));
        System.out.println("ER2 Base " + u1.toString() + " rel: test2.wml result: " +
           expandRelativeURL("test2.wml", u1));
        System.out.println("ER3 Base " + u1.toString() + " rel: /dir/test3.wml result: " +
           expandRelativeURL("/dir/test3.wml", u1));
        System.out.println("ER4 Base " + u1f.toString() + " rel: ../test1.wml result: " +
           expandRelativeURL("../test1.wml", u1f));
        System.out.println("ER5 Base " + u1p.toString() + " rel: /test2.wml result: " +
           expandRelativeURL("/test2.wml", u1p));
        System.out.println("ER6 Base " + u1.toString() + " rel: //host5/dir/test3.wml result: " +
           expandRelativeURL("//host5/dir/test3.wml", u1));
        System.out.println("ER7 Base " + u2.toString() + " rel: ?test=ok result: " +
           expandRelativeURL("?test=ok", u2));
        System.out.println("ER8 Base " + u3.toString() + " rel: ?test=ok result: " +
           expandRelativeURL("?test=ok", u3));
        System.out.println("ER9 Base " + u1.toString() + " rel: t.wmls#f('http://test') result: " +
           expandRelativeURL("t.wmls#f('http://test')", u1));
        System.out.println("ER10 Base " + u4.toString() + " rel: ../../test.wml result: " +
           expandRelativeURL("../../test.wml", u4));

        System.out.println("IQ 1 Base " + u1.toString() + " query a=b " + insertQuery(u1.toString(), "a=b"));
        System.out.println("IQ 2 Base " + u2.toString() + " query a=b&b=h " + insertQuery(u2.toString(), "a=b&b=h"));
        System.out.println("IQ 3 Base " + u5.toString() + " query a=b&b=h " + insertQuery(u5.toString(), "a=b&b=h"));

        System.out.println("IV1 http://host/file " + isValid("http://host/file"));
        System.out.println("IV2 http://host " + isValid("http://host"));
        System.out.println("IV3 http:/host " + isValid("http:/host"));
        System.out.println("IV4 ../host " + isValid("../host"));
        System.out.println("IV5 http://host/file#test#test " + isValid("http://host/file#test#test"));
        System.out.println("IV6 http://host/file;sdf?sd=er " + isValid("http://host/file;sdf?sd=er"));
        System.out.println("IV7 experimental://www.hut.fi/#test " + isValid("experimental://www.hut.fi/#test"));
        System.out.println("IV8 experimental?://www.dd.fi/test#test " + isValid("experimental?://www.dd.fi/test#test"));
        System.out.println("Below should be false");
        System.out.println(isValid("http://anil.com/sc\"ript#function()"));
        System.out.println(isValid("#function(\")"));
        System.out.println(isValid("http://anil.com/sc^ript#function()"));
        System.out.println(isValid("#function(^)"));
        System.out.println(isValid("http://anil.com/sc`ript#function()"));
        System.out.println(isValid("#function(`)"));
        System.out.println(isValid("http://anil.com/sc{ript#function()"));
        System.out.println(isValid("#function({)"));
        System.out.println(isValid("http://anil.com/sc|ript#function()"));
        System.out.println("encode1: " + encodeString("this is test"));
        System.out.println("decode1: " + decodeString("this%20is%20test"));
        try{decodeString("this%20is\u03a0end");}catch(Exception e) { System.out.println("decode2: ok"); };

        // Remove dot test cases
        System.out.println("Remove SingleDot1 " + removeSingleDots(u7.toString()).equals(u6.toString()));
        System.out.println("Remove SingleDot2 " + removeSingleDots(u8.toString()).equals(u6.toString()));
        System.out.println("Remove SingleDot3 " + removeSingleDots(u9.toString()).equals(u9b.toString()));
        System.out.println("Remove SingleDot4 " + removeSingleDots(u10.toString()).equals(u10b.toString()) + removeSingleDots(u10.toString()));

        // Check Encoding
        System.out.println("Encoding checks");
        System.out.println("Encoding check 1 " + checkEncodingUrl("aa%20ee%99"));
        System.out.println("Encoding check 2 " + !checkEncodingUrl("aa%20ee%9"));
        System.out.println("Encoding check 3 " + checkEncodingUrl("aa%FFee%99"));
        System.out.println("Encoding check 4 " + checkEncodingUrl("%aa%0fee"));
        System.out.println("Encoding check 5 " + !checkEncodingUrl("%aa%0gee"));
        System.out.println("Encoding check 6 " + !checkEncodingUrl("%aa%wgee"));
        System.out.println("Encoding check 7 " + checkEncodingUrl("aadfgwgee"));

        System.out.println(isValid("http://anil.com:::80/"));
        //Thread.sleep(50000);
       } catch(Exception e) {
        System.out.println(e.toString());
       }

       System.out.println("File suffix for " + WML_SOURCE + " is " + getFileSuffix(WML_SOURCE));
*/
    }
}
