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



package com.nokia.wtk.util.gui;

/* java.net */
import java.net.URL;

/* java.util */
import java.util.Hashtable;

/* javax.swing */
import javax.swing.Icon;
import javax.swing.ImageIcon;

/* com.nokia.wtk.http */
import com.nokia.wtk.util.MimeType;

/**
 * Manager for file type icons.
 * The naming convention for the icon files is [type][size].gif
 * Includes an Icon cache.
 *
 */
public class FileIcon {

    public static final int SIZE_SMALL = 1;
    public static final int SIZE_MEDIUM = 2;
    public static final int SIZE_LARGE = 3;

    // Mime to file prefix map
    private static Hashtable gIconMap;
    // Icons we have loaded
    private static Hashtable gIconCache;

    // List of icon types (filename prefixes) we have

    private static String AUDIO_ICON = "audio";
    private static String COMPILED_ICON = "compiled_document";
    private static String CSS_ICON = "css";
    private static String DOCUMENT_ICON = "document";
    private static String MMS_ICON = "mms";
    private static String MULTI_PUSH_ICON = "multipart_push";
    private static String SCRIPT_ICON = "script";
    private static String SMIL_ICON = "smil";
    private static String VIDEO_ICON = "video";
    private static String WAP_PUSH_ICON = "wap_push";
    private static String WBMP_ICON = "wbmp";
    private static String WML_ICON = "wml";
    private static String WMLS_ICON = "wmls";
    private static String XHTML_ICON = "xhtml";
    private static String XML_ICON = "xml-generic";
    private static String TEXT_ICON = "text";
    private static String DCF_ICON = "dcf";
    private static String UNKNOWN_ICON = "unknown";
    private static String BLANK_ICON = "blank";

    private static final String ICON_LOCATION = "resources/icons/";
    private static final String RESOURCE_PREFIX = "fileicon.";

    /**
     * Get the standard Icon for a file type (defaults to SMALL size)
     * @param type Mime type
     */
    public static ImageIcon getFileIcon(String type) {
        return getFileIcon(type, SIZE_SMALL);
    }

    /**
     * Get the standard Icon for a file type.  If can't find it,
     * will return UNKNOWN_ICON.
     * @param type Mime type
     */
    public static ImageIcon getFileIcon(String type, int sizeType) {
        String prefix = getIconPrefix(type, false);
        String sz = null;
        switch (sizeType) {
          case SIZE_SMALL:
              sz = "16";
              break;
          case SIZE_MEDIUM:
              sz = "32";
              break;
          case SIZE_LARGE:
          default:
              sz = "48";
        }
        String name = ICON_LOCATION + prefix + sz + ".gif";

        // Check cache first
        ImageIcon ic = (ImageIcon)gIconCache.get(name);
        if (ic != null) {
            return ic;
        }
        return loadIcon(name);
    }

    /**
     * Does this file type have a registered icon?
     * @param ctype Mime type
     * @returns true if in our table, false if not.
     */
    public static boolean isRegistered(String ctype) {
        if (ctype == null) return false;
        String p = getIconPrefix(ctype, true);
        if (p != null) {
            return true;
        }
        return false;
    }

    /**
     * Actually load the image and create an ImageIcon from the resource key.
     * Add into our local cache.
     */
    private static ImageIcon loadIcon(String name) {
        URL url = FileIcon.class.getResource(name);
        if (url != null) {
            ImageIcon ii = new ImageIcon(url);
            gIconCache.put(name, ii);
            return ii;
        }
        return null;
    }

    /**
     * Consult the MIMEType-to-Icon-filename map
     * @param key Mime type String
     * @param need an exact match?
     * @return file name prefix (no size indicator)
     */
    private static String getIconPrefix(String key, boolean needExact) {
        if (gIconMap == null) {
            gIconMap = new Hashtable();
            gIconCache = new Hashtable();

            // Specific types
            gIconMap.put(MimeType.TEXT_PLAIN,           TEXT_ICON);
            gIconMap.put(MimeType.TEXT_CSS,             CSS_ICON);
            gIconMap.put(MimeType.TEXT_XHTML,           XHTML_ICON);
            gIconMap.put(MimeType.TEXT_HTML,            XHTML_ICON);
            gIconMap.put(MimeType.WML_SOURCE,           WML_ICON);
            gIconMap.put(MimeType.WML_SCRIPT_SOURCE,    WMLS_ICON);
            gIconMap.put(MimeType.IMAGE_WBMP,           WBMP_ICON);
            gIconMap.put(MimeType.APP_SMIL,             SMIL_ICON);
            gIconMap.put(MimeType.PUSH_SI_SOURCE,       WAP_PUSH_ICON);
            gIconMap.put(MimeType.PUSH_SI_BINARY,       WAP_PUSH_ICON);
            gIconMap.put(MimeType.PUSH_SL_SOURCE,       WAP_PUSH_ICON);
            gIconMap.put(MimeType.PUSH_SL_BINARY,       WAP_PUSH_ICON);
            gIconMap.put(MimeType.CACHE_OP_SOURCE,      WAP_PUSH_ICON);
            gIconMap.put(MimeType.CACHE_OP_BINARY,      WAP_PUSH_ICON);
            gIconMap.put(MimeType.MMS_MESSAGE,          MMS_ICON);
            gIconMap.put(MimeType.MULTIPART_MIXED_WSP,  MULTI_PUSH_ICON);
            gIconMap.put(MimeType.MULTIPART_RELATED_WSP,MULTI_PUSH_ICON);
            gIconMap.put(MimeType.MULTIPART_ALTERNATIVE_WSP,MULTI_PUSH_ICON);
            gIconMap.put(MimeType.MULTIPART_MIXED,      MULTI_PUSH_ICON);
            gIconMap.put(MimeType.MULTIPART_RELATED,    MULTI_PUSH_ICON);
            gIconMap.put(MimeType.MULTIPART_ALTERNATIVE,MULTI_PUSH_ICON);
            // Audio
            gIconMap.put(MimeType.AUDIO_MIDI,           AUDIO_ICON);
            gIconMap.put(MimeType.AUDIO_SP_MIDI,        AUDIO_ICON);
            gIconMap.put(MimeType.AUDIO_AMR,            AUDIO_ICON);
            gIconMap.put(MimeType.AUDIO_AMR_WB,         AUDIO_ICON);
            gIconMap.put(MimeType.AUDIO_XAMR,           AUDIO_ICON);
            gIconMap.put(MimeType.AUDIO_WAV,            AUDIO_ICON);
            gIconMap.put(MimeType.AUDIO_MPEG,           AUDIO_ICON);
            gIconMap.put(MimeType.AUDIO_3GPP,           AUDIO_ICON);
            gIconMap.put(MimeType.APP_RINGTONE,         AUDIO_ICON);
            // DRM
            gIconMap.put(MimeType.DRM_CONTENT,          DCF_ICON);
            gIconMap.put(MimeType.DRM_MESSAGE,          DCF_ICON);
            // Video
            gIconMap.put(MimeType.VIDEO_3GPP,           VIDEO_ICON);
            // Comiled formst
            gIconMap.put(MimeType.WML_BINARY,           COMPILED_ICON);
            gIconMap.put(MimeType.WML_SCRIPT_BINARY,    COMPILED_ICON);
            // Misc
            gIconMap.put(MimeType.HTTP_RESPONSE,        BLANK_ICON);
        }
        String p = (String)gIconMap.get(key);

        // If no exact match, try some generic ones
        if (p == null) {
            if (needExact) {
                return null;
            }
            if (key.endsWith("+xml")) {
                // Generic XML
                p = XML_ICON;
            } else if (key.startsWith("text")) {
                // Generic text
                p = TEXT_ICON;
            } else {
                // Don't know
                p = UNKNOWN_ICON;
            }
        }
        return p;
    }
}
