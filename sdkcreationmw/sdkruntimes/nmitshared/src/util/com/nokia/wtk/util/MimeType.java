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
 * Definitions for MIME-related strings and constants.
 *
 * Definitions for URL-related strings and constants.
 *
 * 0.2 File scheme added by Mika
 * 0.3 moved under util by Mika
 */

public interface MimeType {

    public static final String ALL_ALL = "*/*";
    public static final String TEXT = "text";
    public static final String TEXT_CSS = "text/css";
    public static final String TEXT_PLAIN = "text/plain";
    public static final String TEXT_XML = "text/xml";
    public static final String TEXT_XHTML = "application/xhtml+xml";
    public static final String TEXT_HTML = "text/html";
    public static final String TEXT_SUFFIX = ".txt";
    public static final String TEXT_PSUFFIX = "txt";
    public static final String HTML_SUFFIX = ".html";
    public static final String HTML2_SUFFIX = ".htm";
    public static final String HTML_PSUFFIX = "html";
    public static final String HTML2_PSUFFIX = "htm";
    public static final String XML_SUFFIX = ".xml";
    public static final String XML_PSUFFIX = "xml";
    public static final String XHTML_SUFFIX = ".xhtml";
    public static final String XHTML_PSUFFIX = "xhtml";
    public static final String CSS_SUFFIX = ".css";
    public static final String CSS_PSUFFIX = "css";

    // Use if we don't know the content type
    public static final String APP_OCTET_STREAM = "application/octet-stream";
    public static final String UNKNOWN = APP_OCTET_STREAM;

    public static final String APP_SMIL = "application/smil";
    public static final String SMIL_SUFFIX = ".smil";
    public static final String SMIL_PSUFFIX = "smil";
    public static final String SMIL_SUFFIX_2 = ".smi";
    public static final String SMIL_PSUFFIX_2 = "smi";

    public static final String WAP_TEXT_ALL = "text/vnd.wap.*";
    public static final String WAP_APP_ALL = "application/vnd.wap.*";

    public static final String WML_SOURCE = "text/vnd.wap.wml";
    public static final String WML_SOURCE_SUFFIX  = ".wml";
    public static final String WML_SOURCE_PSUFFIX = "wml";

    public static final String WML2_SOURCE = "application/vnd.wap.wml+xml";
    public static final String WML2_SOURCE_SUFFIX  = ".wml";
    public static final String WML2_SOURCE_ALT_SUFFIX  = ".wml2";
    public static final String WML2_SOURCE_PSUFFIX = "wml2";

    // x-vCard is official name according to imc.org, x-vcard is official
    // name according to WAE 1.1. Ok, we'll do both.
    public static final String VcARD_SOURCE = "text/x-vcard";
    public static final String VCARD_SOURCE = "text/x-vCard";
    public static final String VCARD_SOURCE_SUFFIX  = ".vcf";
    public static final String VCARD_SOURCE_PSUFFIX = "vcf";

    // x-vcalendar is official name according to imc.org, x-vcal is official
    // name according to WAE 1.1. Ok, we'll do both.
    public static final String VCAL_SOURCE = "text/x-vcal";
    public static final String VCALENDAR_SOURCE = "text/x-vCalendar";
    public static final String VcALENDAR_SOURCE = "text/x-vcalendar";
    public static final String VCAL_SOURCE_SUFFIX  = ".vcs";
    public static final String VCAL_SOURCE_PSUFFIX = "vcs";


        // There is no MimeType for the hex file type
    public static final String WML_HEX_SUFFIX  = ".wmlh";
    public static final String WML_HEX_PSUFFIX = "wmlh";

    public static final String WML_BINARY = "application/vnd.wap.wmlc";
    public static final String WML_BINARY_SUFFIX = ".wmlc";
    public static final String WML_BINARY_PSUFFIX = "wmlc";

    public static final String WBXML_BINARY = "application/vnd.wap.wbxml";
    public static final String WBXML_BINARY_SUFFIX = ".wbxml";
    public static final String WBXML_BINARY_PSUFFIX = "wbxml";

    public static final String WML_SCRIPT_SOURCE = "text/vnd.wap.wmlscript";
    public static final String WML_SCRIPT_SOURCE_SUFFIX = ".wmls";
    public static final String WML_SCRIPT_SOURCE_PSUFFIX = "wmls";

    public static final String WML_SCRIPT_BINARY = "application/vnd.wap.wmlscriptc";
    public static final String WML_SCRIPT_BINARY_SUFFIX = ".wmlsc";
    public static final String WML_SCRIPT_BINARY_PSUFFIX = "wmlsc";

    public static final String PUSH_SI_SOURCE = "text/vnd.wap.si";
    public static final String PUSH_SI_SOURCE_SUFFIX = ".si";
    public static final String PUSH_SI_SOURCE_PSUFFIX = "si";
    public static final String PUSH_SI_BINARY = "application/vnd.wap.sic";
    public static final String PUSH_SI_BINARY_SUFFIX = ".sic";
    public static final String PUSH_SI_BINARY_PSUFFIX = "sic";

    public static final String PUSH_SL_SOURCE = "text/vnd.wap.sl";
    public static final String PUSH_SL_SOURCE_SUFFIX = ".sl";
    public static final String PUSH_SL_SOURCE_PSUFFIX = "sl";
    public static final String PUSH_SL_BINARY = "application/vnd.wap.slc";
    public static final String PUSH_SL_BINARY_SUFFIX = ".slc";
    public static final String PUSH_SL_BINARY_PSUFFIX = "slc";

    public static final String CACHE_OP_BINARY = "application/vnd.wap.coc";
    public static final String CACHE_OP_BINARY_SUFFIX = ".coc";
    public static final String CACHE_OP_BINARY_PSUFFIX = "coc";
    public static final String CACHE_OP_SOURCE = "text/vnd.wap.co";
    public static final String CACHE_OP_SOURCE_SUFFIX = ".co";
    public static final String CACHE_OP_SOURCE_PSUFFIX = "co";

    public static final String X509_CERT = "application/x-x509-user-cert";
    public static final String X509_CERT_SUFFIX  = ".x509";
    public static final String X509_CERT_PSUFFIX = "x509";

    public static final String WTLS_CERT = "application/vnd.wap.wtls-ca-certificate";
    public static final String WTLS_CERT_SUFFIX  = ".wtls";
    public static final String WTLS_CERT_PSUFFIX = "wtls";

    public static final String SIGNED_CERTIFICATE = "application/vnd.wap.signed-certificate";
    public static final String HASHED_CERTIFICATE = "application/vnd.wap.hashed-certificate";
    public static final String USER_CERTIFICATE = "application/vnd.wap.cert-response";

    public static final String NAV_DOC_SOURCE = "text/vnd.wap.navigation-xml";
    public static final String NAV_DOC_BINARY = "application/vnd.wap.navigation-wbxml";

    public static final String URL_ENCODED = "application/x-www-form-urlencoded";

    public static final String MULTIPART = "multipart";
    public static final String MULTIPART_ALL = "multipart/*";
    public static final String MULTIPART_MIXED = "multipart/mixed";
    public static final String MULTIPART_RELATED = "multipart/related";
    public static final String MULTIPART_ALTERNATIVE = "multipart/alternative";
    public static final String MULTIPART_FORM_DATA = "multipart/form-data";

    public static final String MULTIPART_ALL_WSP = "application/vnd.wap.multipart.*";
    public static final String MULTIPART_MIXED_WSP = "application/vnd.wap.multipart.mixed";
    public static final String MULTIPART_RELATED_WSP = "application/vnd.wap.multipart.related";
    public static final String MULTIPART_ALTERNATIVE_WSP = "application/vnd.wap.multipart.alternative";
    public static final String MULTIPART_FORM_WSP = "application/vnd.wap.multipart.form-data";
    //
    // There is no accepted suffix for Multipart-formatted files, so we'll make one up
    // to encompass all subtypes, and the header inside will tell which one it is.
    //
    public static final String MULTIPART_SUFFIX = ".mp";	// HTTP MIME form
    public static final String MULTIPART_PSUFFIX = "mp";	// HTTP MIME form
    public static final String MULTIPART_WSP_SUFFIX = ".mpc";	// WAP-encoded form
    public static final String MULTIPART_WSP_PSUFFIX = "mpc";	// WAP-encoded form

    public static final String MMS_MESSAGE = "application/vnd.wap.mms-message";
    public static final String MMS_MESSAGE_SUFFIX  = ".mms";
    public static final String MMS_MESSAGE_PSUFFIX = "mms";

    public static final String IMAGE_GIF = "image/gif";
    public static final String IMAGE_GIF_SUFFIX = ".gif";
    public static final String IMAGE_GIF_PSUFFIX = "gif";

    public static final String IMAGE_JPG = "image/jpeg";
    public static final String IMAGE_JPG_SUFFIX = ".jpg";
    public static final String IMAGE_JPG_PSUFFIX = "jpg";
    public static final String IMAGE_JPEG_SUFFIX = ".jpeg";
    public static final String IMAGE_JPEG_PSUFFIX = "jpeg";

    public static final String IMAGE_PNG = "image/png";
    public static final String IMAGE_PNG_SUFFIX = ".png";
    public static final String IMAGE_PNG_PSUFFIX = "png";

    public static final String IMAGE_WBMP = "image/vnd.wap.wbmp";
    public static final String IMAGE_WBMP_SUFFIX = ".wbmp";
    public static final String IMAGE_WBMP_PSUFFIX = "wbmp";

    public static final String IMAGE_BMP = "image/bmp";
    public static final String IMAGE_BMP_SUFFIX = ".bmp";
    public static final String IMAGE_BMP_PSUFFIX = "bmp";

    // AUDIO

    // All the sundry MIDI content-types and two most popular extensions
    public static final String AUDIO_MIDI = "audio/midi";
    public static final String AUDIO_MID = "audio/mid";
    public static final String AUDIO_SP_MIDI = "audio/sp-midi";
    public static final String AUDIO_X_MIDI = "audio/x-midi";
    public static final String MIDI_SUFFIX = ".midi";
    public static final String MIDI_PSUFFIX = "midi";
    public static final String MID_SUFFIX = ".mid";
    public static final String MID_PSUFFIX = "mid";

    public static final String AUDIO_AMR = "audio/amr";
    public static final String AUDIO_XAMR = "audio/x-amr";
    public static final String AMR_SUFFIX = ".amr";
    public static final String AMR_PSUFFIX = "amr";

    // See RFC 3267
    public static final String AUDIO_AMR_WB = "audio/amr-wb";
    public static final String AMR_WB_SUFFIX = ".awb";
    public static final String AMR_WB_PSUFFIX = "awb";

    public static final String AUDIO_WAV = "audio/wav";
    public static final String WAV_SUFFIX = ".wav";
    public static final String WAV_PSUFFIX = "wav";

    public static final String AUDIO_MPEG = "audio/mpeg";
    public static final String MP3_SUFFIX = ".mp3";
    public static final String MP3_PSUFFIX = "mp3";

    public static final String AUDIO_3GPP = "audio/3gpp";
    public static final String VIDEO_3GPP = "video/3gpp";
    public static final String AV_3GPP_SUFFIX = ".3gp";
    public static final String AV_3GPP_PSUFFIX = "3gp";

    public static final String APP_RINGTONE = "application/vnd.nokia.ringing-tone";
    public static final String RINGTONE_SUFFIX = ".rng";
    public static final String RINGTONE_PSUFFIX = "rng";
    public static final String RINGTONE_SUFFIX2 = ".nrt";
    public static final String RINGTONE_PSUFFIX2 = "nrt";

    // Symbian OS Installation Package
    public static final String APP_SYMBIAN_INSTALL = "application/vnd.symbian.install";
    public static final String SYMBIAN_INSTALL_SUFFIX = ".sis";
    public static final String SYMBIAN_INSTALL_PSUFFIX = "sis";

    // UI Themes for S40 phones
    public static final String APP_NOK_S40THEME = "application/vnd.nok-s40theme";
    public static final String APP_NOK_S40THEME_SUFFIX = ".nth";
    public static final String APP_NOK_S40THEME_PSUFFIX = "nth";

    // Java-related content types
    public static final String APP_JAVA_ARCHIVE = "application/java-archive";
    public static final String JAR_SUFFIX = ".jar";
    public static final String JAR_PSUFFIX = "jar";

    public static final String TEXT_APP_DESCRIPTOR = "text/vnd.sun.j2me.app-descriptor";
    public static final String APP_DESCRIPTOR_SUFFIX = ".jad";
    public static final String APP_DESCRIPTOR_PSUFFIX = "jad";

    // OMA DRM
    //
    // DRM Rights Object (text and WBXML encoded versions.  See OMA-Download-DRMREL)
    public static final String DRM_RIGHTS = "application/vnd.oma.drm.rights+xml";
    public static final String DRM_RIGHTS_SUFFIX = ".dr";
    public static final String DRM_RIGHTS_PSUFFIX = "dr";
    public static final String DRM_RIGHTS_BIN = "application/vnd.oma.drm.rights+wbxml";
    public static final String DRM_RIGHTS_BIN_SUFFIX = ".drc";
    public static final String DRM_RIGHTS_BIN_PSUFFIX = "drc";
    // DRM Content Format (encripted media object.  See OMA-Download-DRMCF)
    public static final String DRM_CONTENT = "application/vnd.oma.drm.content";
    public static final String DRM_CONTENT_SUFFIX = ".dcf";
    public static final String DRM_CONTENT_PSUFFIX = "dcf";
    // DRM Message (MIME format.  See OMA-Download-DRM)
    public static final String DRM_MESSAGE = "application/vnd.oma.drm.message";
    public static final String DRM_MESSAGE_SUFFIX = ".dm";
    public static final String DRM_MESSAGE_PSUFFIX = "dm";
    // OMA Download Descriptor (See OMA-Download-OTA)
    public static final String DRM_DOWNLOAD_DESC = "application/vnd.oma.dd+xml";
    public static final String DRM_DOWNLOAD_DESC_SUFFIX = ".dd";
    public static final String DRM_DOWNLOAD_DESC_PSUFFIX = "dd";

    //
    // URL_RESPONSE is a pseudo-MIME-type that allows us to store an entire WAP response,
    // including headers, to a file.
    //
    public static final String URL_RESPONSE = "application/vnd.wap.url-response";
    public static final String URL_RESPONSE_SUFFIX = ".url";
    public static final String URL_RESPONSE_PSUFFIX = "url";
    //
    // HTTP_RESPONSE is a pseudo-MIME-type that allows us to store an entire HTTP response,
    // including status line and headers, to a file.
    //
    public static final String HTTP_RESPONSE = "application/vnd.nokia.http-response";
    public static final String HTTP_RESPONSE_SUFFIX = ".http";
    public static final String HTTP_RESPONSE_PSUFFIX = "http";

    public static final String URL_HTTP_SCHEME  = "http";
    public static final String URL_JAR_SCHEME  = "jar";
    public static final String URL_HTTPS_SCHEME = "https";
    public static final String URL_FILE_SCHEME  = "file";
    public static final String URL_WTAI_SCHEME  = "wtai";
    public static final String URL_SCHEME_SEP2  = ":/";
    public static final String URL_SCHEME_SEP = "://";

    public static final String ACCEPT_HEADER = "Accept";
    public static final String ACCEPT_CHARSET_HEADER = "Accept-Charset";
    public static final String ACCEPT_ENCODING_HEADER = "Accept-Encoding";
    public static final String ACCEPT_LANGUAGE_HEADER = "Accept-Language";
    public static final String CACHE_CONTROL_HEADER = "Cache-Control";
    public static final String CONTENT_TYPE_HEADER = "Content-Type";
    public static final String CONTENT_ENCODING_HEADER = "Content-Encoding";
    public static final String CONTENT_LANGUAGE_HEADER = "Content-Language";
    public static final String CONTENT_LENGTH_HEADER = "Content-Length";
    public static final String CONTENT_LOCATION_HEADER = "Content-Location";
    public static final String CONTENT_ID_HEADER = "Content-ID";
    public static final String REFERER_HEADER = "Referer";
    public static final String PROFILE_HEADER = "Profile";
    public static final String PROFILE_DIFF_HEADER = "Profile-Diff";
    public static final String PROFILE_WARNING_HEADER = "Profile-Warning";
    public static final String USER_AGENT_HEADER = "User-Agent";
    //
    // Push message headers
    //
    public static final String APPLICATION_ID_HEADER= "X-Wap-Application-Id";
    public static final String CONTENT_URI_HEADER= "X-Wap-Content-URI";
    public static final String INITIATOR_URI_HEADER= "X-Wap-Initiator-URI";

    //
    // Location Info headers
    //
    public static final String POSITIONING_METHOD_HEADER = "X-NOKIA-WAP-LOC-LOCATIONINFO-METHOD";
    public static final String LATITUDE_HEADER= "X-NOKIA-WAP-LOC-LAT";
    public static final String LONGITUDE_HEADER= "X-NOKIA-WAP-LOC-LONG";
    public static final String GSM_HEADER= "X-NOKIA-WAP-LOC-GSM-INFORMATION";
    public static final String LL_FORMAT_HEADER= "X-NOKIA-WAP-LOC-LL-FORMAT";
    public static final String PN_FORMAT_HEADER= "X-NOKIA-WAP-LOC-PN-FORMAT";
    public static final String ACTUAL_LL_FORMAT_HEADER = "X-NOKIA-WAP-LOC-ACTUAL-LL";
    public static final String ACCURACY_HEADER = "X-NOKIA-WAP-LOC-ACCURACY";
    public static final String ERROR_HEADER = "X-NOKIA-WAP-LOC-ERROR";

    //
    // Location Info headers
    //
    public static final String MSISDN_BEARER_HEADER = "X-Nokia-BEARER";
    public static final String MSISDN_CMODE_HEADER= "X-Nokia-CONNECTION_MODE";
}
