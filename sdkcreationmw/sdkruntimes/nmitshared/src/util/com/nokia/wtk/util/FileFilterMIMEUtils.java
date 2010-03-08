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



package com.nokia.wtk.util;

/**
 * Methods for managing file filters
 */
public class FileFilterMIMEUtils {

    /**
     * Return the appropriate FileFilter based on the MIME type.
     * @param mimeType The MIME type a filters is being sought for.
     * @return null if no appropriate filter was found, the filter otherwise.
     */
    public static FileFilter getFileFilter(String mimeType) {
        if (mimeType == null) {
            return null;
        }

        // get rid of leading/trailing spaces
        mimeType = mimeType.trim();

        if (mimeType.equals(MimeType.TEXT)) {
            return TEXT;
        } else if (mimeType.equals(MimeType.TEXT_HTML)) {
            return HTML;
        } else if (mimeType.equals(MimeType.TEXT_XHTML)) {
            return XHTML;
        } else if (mimeType.equals(MimeType.TEXT_CSS)) {
            return CSS;
        } else if (mimeType.equals(MimeType.WML_SOURCE)) {
            return WML_SOURCE;
        } else if (mimeType.equals(MimeType.WML_SCRIPT_SOURCE)) {
            return WMLS_SOURCE;
        } else if (mimeType.equals(MimeType.WML_BINARY)) {
            return WML_BINARY;
        } else if (mimeType.equals(MimeType.WML_SCRIPT_BINARY)) {
            return WMLS_BINARY;
        } else if (mimeType.equals(MimeType.X509_CERT)) {
            return X509_CERT;
        } else if (mimeType.equals(MimeType.WTLS_CERT)) {
            return WTLS_CERT;
        } else if (mimeType.equals(MimeType.IMAGE_WBMP)) {
            return WBMP_IMAGE;
        } else if (mimeType.equals(MimeType.IMAGE_GIF) ||
                mimeType.equals(MimeType.IMAGE_JPG) ||
                mimeType.equals(MimeType.IMAGE_PNG)) {
            return WAP_IMPORTIMAGE;
        } else if (mimeType.equals(MimeType.VcARD_SOURCE) ||
                mimeType.equals(MimeType.VCARD_SOURCE) ||
                mimeType.equals(MimeType.VCAL_SOURCE) ||
                mimeType.equals(MimeType.VCALENDAR_SOURCE) ||
                mimeType.equals(MimeType.VcALENDAR_SOURCE)) {
            return VCARD_OR_VCAL_SOURCE;
        } else if (mimeType.equals(MimeType.PUSH_SI_SOURCE)) {
            return SI_SOURCE_FILTER;
        } else if (mimeType.equals(MimeType.PUSH_SI_BINARY)) {
            return SI_BINARY_FILTER;
        } else if (mimeType.equals(MimeType.PUSH_SL_SOURCE)) {
            return SL_SOURCE_FILTER;
        } else if (mimeType.equals(MimeType.PUSH_SL_BINARY)) {
            return SL_BINARY_FILTER;
        } else if (mimeType.equals(MimeType.CACHE_OP_SOURCE)) {
            return CO_SOURCE_FILTER;
        } else if (mimeType.equals(MimeType.CACHE_OP_BINARY)) {
            return CO_BINARY_FILTER;
        } else if (mimeType.equals(MimeType.MMS_MESSAGE)) {
            return MMS_MESSAGE_FILTER;
        } else if (mimeType.equals(MimeType.APP_SMIL)) {
            return SMIL_FILTER;
        } else if (mimeType.equals(MimeType.DRM_MESSAGE)) {
            return DRM_MESSAGE_FILTER;
        } else if (mimeType.equals(MimeType.DRM_DOWNLOAD_DESC)) {
            return DD_FILTER;
        } else if (mimeType.equals(MimeType.DRM_CONTENT)) {
            return DCF_FILTER;
        } else if (mimeType.equals(MimeType.DRM_RIGHTS)) {
            return DRM_RIGHTS_FILTER;
        } else if (mimeType.equals(MimeType.DRM_RIGHTS_BIN)) {
            return DRM_RIGHTS_BIN_FILTER;
        } else if (mimeType.equals(MimeType.MULTIPART) ||
                mimeType.equals(MimeType.MULTIPART_ALL) ||
                mimeType.equals(MimeType.MULTIPART_MIXED) ||
                mimeType.equals(MimeType.MULTIPART_RELATED) ||
                mimeType.equals(MimeType.MULTIPART_ALTERNATIVE) ||
                mimeType.equals(MimeType.MULTIPART_FORM_DATA)) {
            return MULTIPART_FILTER;
         } else if (mimeType.equals(MimeType.MULTIPART_ALL_WSP) ||
                mimeType.equals(MimeType.MULTIPART_MIXED_WSP) ||
                mimeType.equals(MimeType.MULTIPART_RELATED_WSP) ||
                mimeType.equals(MimeType.MULTIPART_ALTERNATIVE_WSP) ||
                mimeType.equals(MimeType.MULTIPART_FORM_WSP)) {
            return MULTIPART_WSP_FILTER;
         }
         return null;
    }

    /**
     * Returns a set of file filters compatible with the specified MIME type.
     */
    public
    static
    synchronized
    FileFilter [] getCompatibleFileFilters(String mimeType)
    {
        if (mimeType == null) {
            return null;
        } else if (mimeType.equals(MimeType.TEXT_XHTML) ||
                   mimeType.equals(MimeType.TEXT_HTML) ||
                   mimeType.equals(MimeType.TEXT_XML) ||
                   mimeType.equals(MimeType.WML2_SOURCE)) {
            return new FileFilter [] { XHTML, HTML, XML, HTM };
        }
        return null;
    }

    private final static String WML_FILETYPES[] = {
        MimeType.WML_SOURCE_PSUFFIX,
        MimeType.WML_BINARY_PSUFFIX,
        MimeType.WBXML_BINARY_PSUFFIX
    };

    private final static String HTML_FILETYPES[] = {
        MimeType.HTML_PSUFFIX,

    };
    private final static String HTM_FILETYPES[] = {
        MimeType.HTML2_PSUFFIX,
    };

    private final static String WAP_SOURCE_FILETYPES[] = {
        MimeType.WML_SOURCE_PSUFFIX,
        MimeType.WML_SCRIPT_SOURCE_PSUFFIX
    };

    private final static String MULTIPART_FILETYPES[] = {
        MimeType.MULTIPART_PSUFFIX,
        MimeType.MULTIPART_WSP_PSUFFIX
    };

    private final static String ALL_TOOLKIT_TYPES[] = {
        MimeType.WML_SOURCE_PSUFFIX,
        MimeType.WML_SCRIPT_SOURCE_PSUFFIX,
        MimeType.XML_PSUFFIX,
        MimeType.XHTML_PSUFFIX,
        MimeType.CSS_PSUFFIX,
        MimeType.IMAGE_WBMP_PSUFFIX,
        MimeType.IMAGE_GIF_PSUFFIX,
        MimeType.IMAGE_JPG_PSUFFIX,
        MimeType.PUSH_SI_SOURCE_PSUFFIX,
        MimeType.PUSH_SI_BINARY_PSUFFIX,
        MimeType.PUSH_SL_SOURCE_PSUFFIX,
        MimeType.PUSH_SL_BINARY_PSUFFIX,
        MimeType.CACHE_OP_SOURCE_PSUFFIX,
        MimeType.CACHE_OP_BINARY_PSUFFIX,
        MimeType.MULTIPART_PSUFFIX,
        MimeType.MULTIPART_WSP_PSUFFIX,
        MimeType.HTML_PSUFFIX,
        MimeType.HTML2_PSUFFIX,
        MimeType.MMS_MESSAGE_PSUFFIX,
        MimeType.SMIL_PSUFFIX,
        MimeType.SMIL_PSUFFIX_2,
        MimeType.DRM_CONTENT_PSUFFIX,
        MimeType.DRM_MESSAGE_PSUFFIX,
        MimeType.DRM_RIGHTS_PSUFFIX,
        MimeType.DRM_DOWNLOAD_DESC_PSUFFIX
    };

    private final static String ALL_LOADABLE_TYPES[] = {
        MimeType.WML_SOURCE_PSUFFIX,
        MimeType.WML_BINARY_PSUFFIX,
        MimeType.WBXML_BINARY_PSUFFIX,
        MimeType.XHTML_PSUFFIX,
        MimeType.HTML_PSUFFIX,
        MimeType.IMAGE_WBMP_PSUFFIX,
        MimeType.IMAGE_GIF_PSUFFIX,
        MimeType.PUSH_SI_SOURCE_PSUFFIX,
        MimeType.PUSH_SL_SOURCE_PSUFFIX,
        MimeType.CACHE_OP_SOURCE_PSUFFIX,
        MimeType.VCARD_SOURCE_PSUFFIX,
        MimeType.VCAL_SOURCE_PSUFFIX,
        MimeType.MULTIPART_PSUFFIX,
        MimeType.MULTIPART_WSP_PSUFFIX,
        MimeType.DRM_MESSAGE_PSUFFIX,
        MimeType.DRM_CONTENT_PSUFFIX,
        MimeType.DRM_RIGHTS_PSUFFIX,
        MimeType.DRM_DOWNLOAD_DESC_PSUFFIX,
        MimeType.MMS_MESSAGE_PSUFFIX
    };

    // Open file menu definitions:
    public final static FileFilter ALL_FOR_EDIT = new FileFilter(
                                                    ALL_TOOLKIT_TYPES,
                                                    "All editable types"
                                                );
    // Open file menu definitions:
    public final static FileFilter ALL_FOR_LOAD = new FileFilter(
                                                    ALL_LOADABLE_TYPES,
                                                    "All loadable types"
                                                );
    public final static FileFilter TEXT = new FileFilter(
                                                    MimeType.TEXT_SUFFIX,
                                                    "Text"
                                                );
    public final static FileFilter HTML = new FileFilter(
                                                    HTML_FILETYPES,
                                                    "HTML"
                                                );
    public final static FileFilter HTM = new FileFilter(
                                                    HTM_FILETYPES,
                                                    "HTM"
                                                );
    public final static FileFilter XML = new FileFilter(
                                                    MimeType.XML_PSUFFIX,
                                                    "XML"
                                                );
    public final static FileFilter XHTML = new FileFilter(
                                                    MimeType.XHTML_PSUFFIX,
                                                    "XHTML"
                                                );
    public final static FileFilter CSS = new FileFilter(
                                                    MimeType.CSS_PSUFFIX,
                                                    "CSS"
                                                );
    public final static FileFilter WML_FILES = new FileFilter(
                                                    WML_FILETYPES,
                                                    "WML types"
                                                );

    public final static FileFilter WAP_SOURCES = new FileFilter(
                                                    WAP_SOURCE_FILETYPES,
                                                    "WAP sources"
                                                );

    public final static FileFilter WML_SOURCE = new FileFilter(
                                                    MimeType.WML_SOURCE_PSUFFIX,
                                                    "WML"
                                                );

    public final static FileFilter WML_BINARY = new FileFilter(
                                                    MimeType.WML_BINARY_PSUFFIX,
                                                    "Encoded WML"
                                                );

    public final static FileFilter WMLS_SOURCE = new FileFilter(
                                                    MimeType.WML_SCRIPT_SOURCE_PSUFFIX,
                                                    "WML Script"
                                                );

    public final static FileFilter WMLS_BINARY = new FileFilter(
                                                    MimeType.WML_SCRIPT_BINARY_PSUFFIX,
                                                    "Encoded WML Script"
                                                );

    public final static FileFilter WML_HEXBINARY = new FileFilter(
                                                    MimeType.WML_HEX_PSUFFIX,
                                                    "WML ByteCode Hex"
                                                );

    public final static FileFilter WBMP_IMAGE = new FileFilter(
                                                    MimeType.IMAGE_WBMP_PSUFFIX,
                                                    "WBMP image"
                                                );

    public final static FileFilter WAP_IMPORTIMAGE = new FileFilter(
                                                    new String[] {
                                                        MimeType.IMAGE_GIF_PSUFFIX,
                                                        MimeType.IMAGE_JPG_PSUFFIX,
                                                        MimeType.IMAGE_PNG_PSUFFIX
                                                    },
                                                    "Images"
                                                );

    public final static FileFilter VCARD_OR_VCAL_SOURCE = new FileFilter(
                                                    new String[] {
                                                        MimeType.VCARD_SOURCE_PSUFFIX,
                                                        MimeType.VCAL_SOURCE_PSUFFIX
                                                    },
                                                    "Business cards, calendar dates"
                                                );

    public final static FileFilter X509_CERT = new FileFilter(
                                                    MimeType.X509_CERT_PSUFFIX,
                                                    "X509 certificate"
                                                );

    public final static FileFilter WTLS_CERT = new FileFilter(
                                                    MimeType.WTLS_CERT_PSUFFIX,
                                                    "WTLS certificate"
                                                );


    public final static FileFilter SERVICE_INDICATION_FILTER = new FileFilter(
                                                  new String[] {
                                                    MimeType.PUSH_SI_SOURCE_PSUFFIX,
                                                    MimeType.PUSH_SI_BINARY_PSUFFIX },
                                                    "Service Indication"
                                                );

    public final static FileFilter SI_SOURCE_FILTER = new FileFilter(
                                                    MimeType.PUSH_SI_SOURCE_PSUFFIX,
                                                    "SI"
                                                );

    public final static FileFilter SI_BINARY_FILTER = new FileFilter(
                                                    MimeType.PUSH_SI_BINARY_PSUFFIX,
                                                    "Encoded SI"
                                                );

    public final static FileFilter SERVICE_LOADING_FILTER = new FileFilter(
                                                  new String[] {
                                                      MimeType.PUSH_SL_SOURCE_PSUFFIX,
                                                      MimeType.PUSH_SL_BINARY_PSUFFIX },
                                                    "Service Loading"
                                                );

    public final static FileFilter SL_SOURCE_FILTER = new FileFilter(
                                                    MimeType.PUSH_SL_SOURCE_PSUFFIX,
                                                    "SL"
                                                );

    public final static FileFilter SL_BINARY_FILTER = new FileFilter(
                                                    MimeType.PUSH_SL_BINARY_PSUFFIX,
                                                    "Encoded SL"
                                                );

    public final static FileFilter CACHE_OP_FILTER = new FileFilter(
                                                  new String[] {
                                                    MimeType.CACHE_OP_SOURCE_PSUFFIX,
                                                    MimeType.CACHE_OP_BINARY_PSUFFIX },
                                                    "Cache Operation"
                                                );

    public final static FileFilter CO_SOURCE_FILTER = new FileFilter(
                                                    MimeType.CACHE_OP_SOURCE_PSUFFIX,
                                                    "CO"
                                                );

    public final static FileFilter CO_BINARY_FILTER = new FileFilter(
                                                    MimeType.CACHE_OP_BINARY_PSUFFIX,
                                                    "Encoded CO"
                                                );

    public final static FileFilter MMS_MESSAGE_FILTER = new FileFilter(
                                                    MimeType.MMS_MESSAGE_PSUFFIX,
                                                    "MMS Message"
                                                );

    public final static FileFilter SMIL_FILTER = new FileFilter(
                                                  new String[] {
                                                    MimeType.SMIL_PSUFFIX,
                                                    MimeType.SMIL_PSUFFIX_2 },
                                                    "SMIL"
                                                );

    public final static FileFilter MULTIPART_FILTER = new FileFilter(
                                                    MULTIPART_FILETYPES,
                                                    "Multipart"
                                                );

    public final static FileFilter MULTIPART_WSP_FILTER = new FileFilter(
                                                    MimeType.MULTIPART_WSP_PSUFFIX,
                                                    "WSP Multipart"
                                                );

    public final static FileFilter ALL_PUSH = new FileFilter(
                                                  new String[] {
                                                    MimeType.PUSH_SI_SOURCE_PSUFFIX,
                                                    MimeType.PUSH_SI_BINARY_PSUFFIX,
                                                    MimeType.PUSH_SL_SOURCE_PSUFFIX,
                                                    MimeType.PUSH_SL_BINARY_PSUFFIX,
                                                    MimeType.CACHE_OP_BINARY_PSUFFIX,
                                                    MimeType.CACHE_OP_SOURCE_PSUFFIX,
                                                    MimeType.MMS_MESSAGE_PSUFFIX
                                                  },
                                                    "Push"
                                                );

    public final static FileFilter DCF_FILTER = new FileFilter(
                                                    MimeType.DRM_CONTENT_PSUFFIX,
                                                    "DRM Content Format"
                                                );

    public final static FileFilter DRM_MESSAGE_FILTER = new FileFilter(
                                                    MimeType.DRM_MESSAGE_PSUFFIX,
                                                    "DRM Message"
                                                );

    public final static FileFilter DRM_RIGHTS_FILTER = new FileFilter(
                                                  new String[] {
                                                    MimeType.DRM_RIGHTS_PSUFFIX,
                                                  },
                                                    "DRM Rights"
                                                );
    public final static FileFilter DRM_RIGHTS_BIN_FILTER = new FileFilter(
                                                  new String[] {
                                                    MimeType.DRM_RIGHTS_BIN_PSUFFIX,
                                                  },
                                                    "DRM Rights (WBXML)"
                                                );
    public final static FileFilter DRM_FILTER = new FileFilter(
                                                  new String[] {
                                                    MimeType.DRM_CONTENT_PSUFFIX,
                                                    MimeType.DRM_MESSAGE_PSUFFIX,
                                                  },
                                                    "DRM"
                                                );

    public final static FileFilter DD_FILTER = new FileFilter(
                                                    MimeType.DRM_DOWNLOAD_DESC_PSUFFIX,
                                                    "Download Descriptor"
                                                );

}
