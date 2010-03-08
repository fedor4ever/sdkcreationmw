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





package com.nokia.wtk.diagnostics.gui;

/* java.io */
import java.io.File;
import java.io.IOException;
import java.net.MalformedURLException;

/* java.net */
import java.net.URL;

/* com.nokia.wtk.diagnostics.browser */
import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.diagnostics.browser.AttributedContent;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.IOUtils;
import com.nokia.wtk.util.MessageWriters;

/**
 * <p>Title: DocViewer</p>
 * <p>Description: A class to encapsulate opening and viewing documentation.
 * Objects of this class are encapsulations of document locations (URL, file
 * paths) that have methods to attempt to open these documents in external
 * viewers using JAVA or Native support constructs.
 * or URLs not on the device.</p>
 * <p>Company: Nokia Corporation</p>
 * @version 1.0
 */
class DocViewer implements MessageWriters {

    // The variable containing the path, URL, or location fragment
    private String document_;
    private Browser browser_;

    /** Constructs a doc viewer
     * @param doc The document file or URL of the document to show.
     * @param b the browser instance, may be <code>null</code>
     */
    public DocViewer(String doc, Browser b) {
        document_ = doc;
        browser_ = b;
    }

    /**
     * Show a local (i.e. on harddisk) document in the default browser.
     * Currently used to view local PDFs. Uses only JAVA code.
     *
     * Invokes the default viewer for a document.
     * This is system dependent.  For example, in Windows the registry
     * will contain the associations required to invoke the proper viewer,
     * based on the argument passed in.  In Unix, a hard-coded association
     * is made to invoke the Netscape browser.
     */
    void showReferenceDocument() {

        String document = testDocumentString();
        if (document== null) {
            System.err.println("Problem!");
        }
        String os = System.getProperty("os.name");

        if ((os != null) && os.startsWith("Windows")) {
            String cmd = "rundll32 url.dll,FileProtocolHandler " + document;


            try { Runtime.getRuntime().exec(cmd); }
            catch (IOException x) {
                System.err.println("Could not invoke document viewer, cmd=\"" + cmd + "\"");
            }
        } else {    // Presume UNIX and Netscape
            String cmd = "netscape -remote openURL(" + document + ")";


            try {
                // Attempt to use browser that is already running:
                // Wait for exit code and if 0, command worked,
                // otherwise start a new browser instance.
                if (Runtime.getRuntime().exec(cmd).waitFor() != 0) {
                    // Command failed, start up a new browser
                    cmd = "netscape " + document;
                    Runtime.getRuntime().exec(cmd);
                }
            }
            catch (InterruptedException x) {}
            catch (IOException x) {
                System.err.println("Could not invoke browser, cmd=\"" + cmd + "\"");
            }
        }
    }

    /**
     * Transforms a document fragment path into a URL
     * @return null if the documkent_ could not be converted to URL. If document_
     * is a file URL, the string will be replaced with the proper file separator
     */
    private String testDocumentString() {

        String document;

        URL url = createURLFromUserInput(document_, browser_);
        if (url != null) {
            document = url.toExternalForm();
            final String fileUrlPrefix = "file:/";
            if (document.startsWith(fileUrlPrefix)) {
                document = document_.substring(fileUrlPrefix.length());
                if (document.startsWith("/")) {
                    // Must be UNC name; put a leading slash back:
                    document = '/' + document_;
                }
                // Convert slashes to system-dependent separators:
                document = document_.replace('/', File.separatorChar);
            }
            return document;
        }
        return null;
    }

    /**
     * Utility method to filter user input intended to be a URL request.
     * @param text the user input
     */
    //TODO: Move this method to URLlibrary/URLutil class maybe?
    public static URL createURLFromUserInput(String text, Browser browser) {
        URL url = null;

        if ((text != null) && (text.length() > 0)) {
            try {
                //TODO: Expand heuristics to try for more possibilities?
                if (text.startsWith("#") && browser != null) {
                    // Possible WML Card reference:
                    AttributedContent cont = browser.getBrowserHistory().
                                                     getCurrentDocument();
                    if (cont != null) {
                        String deckURL = cont.getLocation();
                        int lastHashPos = deckURL.lastIndexOf('#');
                        if (lastHashPos >= 0) {
                            deckURL = deckURL.substring(0, lastHashPos);
                        }
                        url = createURLFromUserInput(deckURL + text, browser);
                    }
                } else {    // Possible file reference:
                    // Convert slashes to system-dependent separators:
                    String filename = text.replace('/', File.separatorChar);
                    String reference = null;
                    int lastHashPos = filename.lastIndexOf('#');
                    if (lastHashPos >= 0) {
                        // Temporarily save and remove any card reference:
                        reference = filename.substring(lastHashPos);
                        filename = filename.substring(0, lastHashPos);
                    }
                    // See if it's a regular file:
                    File f = new File(filename);
                    if (f.exists()) {
                        url = new URL( IOUtils.encodeFileURL( f ) );
                    } else {
                        // See if it's a relative file:
                        f = new File(System.getProperty("user.dir") +
                                            File.separatorChar + filename);
                        if (f.exists()) {
                            url = new URL( IOUtils.encodeFileURL( f ) );
                        }
                    }
                    if ((url != null) && (reference != null)) {
                        // Reattach the card reference:
                        url = new URL(url.toExternalForm() + reference);
                    }
                }
                if (url == null) {  // Assume HTTP
                    // Add presumed HTTP schema prefix if missing:
                    if (text.indexOf(':') < 0) {
                        text = "http://" + text;
                    }
                    url = new URL(text);
                }
//SYSOUT.println("createURLFromUserInput(\"" + text + "\") ==> " + url);
            }
            catch (MalformedURLException ex) {}
        }

        return url;
    }


    /**
     * Checks if this is a "schema" request. A schema request does not need
     * to be a valid URL.
     *
     * @param url a URL to check
     */
    static boolean isSchemaRequest(String url) {
        if ((url != null) && (url.length() > 0)) {
            int colon = url.indexOf(':');
            if (colon > 1) {    // filter out drive letters, like d:\foo.wml
                String schema = url.substring(0, colon);
                return (schema.equalsIgnoreCase("file") ||
                        schema.equalsIgnoreCase("http"));
            }
        }
        return false;
    }


}
