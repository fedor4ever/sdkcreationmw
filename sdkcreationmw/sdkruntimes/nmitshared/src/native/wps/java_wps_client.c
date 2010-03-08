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



/*
 * WAP Server Protocol Stack interface for Java. 
 * Implements the native methods called from JNI
 *
 */

#include "java_wps_client.h"

#define DECODED_HDR_MAX_LENGTH 10240		/* maximum length of decoded HTTP headers */
#define DATA_MAX_LENGTH (128 * 1024)		/* maximum length of cmode buffer */
#define ADDRESS_MAX_LENGTH 1024		/* maximum length of remote address field */
#define URI_MAX_LENGTH 1024		/* maximum length of a URL */
#define BODY_MAX_LENGTH (128 * 1024)	/* maximum length cless buffer */
#define HEADERS_MAX_LENGTH 10240	/* maximum length (encoded) Headers */

/* WSP get PDU method ranges. */
#define WSP_GET_METHOD_START  0x40
#define WSP_GET_METHOD_END    0x5f

#define DO_HDR_ENCODING 1
static BOOL useCodec = TRUE;

JavaVM *jvm;

/*jclass bearer_class;
jmethodID bearer_open_method;
jmethodID bearer_close_method;
jmethodID bearer_close_all_method;
jmethodID bearer_receive_buffer_method;
jmethodID bearer_send_buffer_method;
jmethodID bearer_number_of_open_ports_method;
jmethodID bearer_init_method;
jmethodID bearer_shutdown_method;
jmethodID bearer_get_default_method;
jmethodID bearer_get_local_host_name_method;
jfieldID bearer_id_field;

jclass bearermessage_class;
jmethodID bearermessage_constructor;
jfieldID bearermessage_buf_field;
jfieldID bearermessage_lport_field;
jfieldID bearermessage_buflen_field;
jfieldID bearermessage_laddr_field;
jfieldID bearermessage_laddrlen_field;
jfieldID bearermessage_rport_field;
jfieldID bearermessage_raddr_field;
jfieldID bearermessage_raddrlen_field;
jfieldID bearermessage_type_field;*/

jclass class_bytebuffer;
jmethodID method_bytebuffer_constructor;
jfieldID field_bytebuffer_buf;
jfieldID field_bytebuffer_len;
jfieldID field_bytebuffer_max_len;

extern int num_bearers;

static jclass class_unit_invoke_packet;
static jfieldID field_unit_invoke_packet_conn;
static jfieldID field_unit_invoke_packet_method;
static jfieldID field_unit_invoke_packet_uri;
static jfieldID field_unit_invoke_packet_body;
static jfieldID field_unit_invoke_packet_headers;
static jfieldID field_unit_invoke_packet_trans_id;

static jclass class_unit_result_packet;
static jfieldID field_unit_result_packet_conn;
static jfieldID field_unit_result_packet_body;
static jfieldID field_unit_result_packet_headers;
static jfieldID field_unit_result_packet_status;
static jfieldID field_unit_result_packet_trans_id;

static jclass class_unit_push_packet;
static jfieldID field_unit_push_packet_conn;
static jfieldID field_unit_push_packet_body;
static jfieldID field_unit_push_packet_headers;
static jfieldID field_unit_push_packet_push_id;

static jclass class_connection;
static jfieldID field_connection_id;
static jfieldID field_connection_bearer_id;
static jfieldID field_connection_raddr;
static jfieldID field_connection_laddr;
static jfieldID field_connection_rport;
static jfieldID field_connection_lport;
static jfieldID field_connection_layer_cfg;

static jclass class_transaction;
static jmethodID method_transaction_constructor;
static jfieldID field_transaction_id;
static jfieldID field_transaction_state;

static jclass class_exception;
static jmethodID method_exception_constructor;
static jfieldID field_exception_id;

static jclass class_session;
static jmethodID method_session_constructor;
static jfieldID field_session_id;
static jfieldID field_session_state;
static jfieldID field_session_nrofevents;
static jfieldID field_session_load_id;

static jclass class_event;
static jfieldID field_event_type;
static jfieldID field_event_session;
static jfieldID field_event_transaction;
static jfieldID field_event_exception;

/* 
 * Local Functions
 */
static char* hexBaseCharacters  = "0123456789abcdef";

#include <ctype.h>

#define LINE_LEN 80
#define MAX_CHARS 19
#define HEX_START 1
#define CHAR_START (19 * 3 + 2)

/* 
 * Function   : printBinaryData
 *
 * Prints binary debug data.
 *
 * Parameters : - 
 * Returns    : nothing
 */
static void printBinaryData(
    const BYTE* data,
    UINT32      dataLen)
{ 
	UINT32 i, index;
	char line[LINE_LEN + 1];
    
	index = 0;
	while (1) {
		for (i = 0; i < LINE_LEN - 1; i++) {
			line[i] = ' ';
		}
		for (i = 0; i < MAX_CHARS && index < dataLen; i++, index++) {
			char c = data[index];

			line[HEX_START + i*3] = hexBaseCharacters[(c >> 4) & 15];
			line[HEX_START + i*3 + 1] = hexBaseCharacters[c & 15];
			if (c >= 32 && c <= 126) {
				line[CHAR_START + i] = c;
			} else {
				line[CHAR_START + i] = '_';
			}
		}            
		line[LINE_LEN - 1] = '\0';
		JNI_DEBUG_ARG1("%s\n", line);
		if (index == dataLen) {
			break;
		}
	}
}


/* 
 * Function   : headerCodecMessage
 *
 * Converts the header codec status value into textual representation.
 *
 * Parameters : - status: error status
 * Returns    : ptr to error string
 */
#ifdef JNI_DEBUGGING
static const char* headerCodecMessage(WPS_STATUS status)
{
	const char* result;

	switch (status) {
	case WPS_CODEC_ERROR_INT_BUFFER_TOO_SMALL:
		result = "The codec's internal buffer is too small.";
		break;
	case WPS_CODEC_ERROR_DEST_BUFFER_TOO_SMALL:
		result = "The destination buffer passed to the header encoder/decoder is too small.";
		break;
	case WPS_HC_ERROR_HTTP_ILLEGAL_NEWLINE:
		result = "HTTP syntax error: illegal combination of newline characters (the HC accepts '\n' or '\r\n').";
		break;
	case WPS_HC_ERROR_HTTP_NO_FIELD_SEPARATOR:
		result = "HTTP syntax error: header field without field separator (':').";
		break;
	case WPS_HC_ERROR_HTTP_BAD_Q_VALUE:
		result = "HTTP error: failed to parse Q-value.";
		break;
	case WPS_HC_ERROR_HTTP_BAD_DATE:
		result = "HTTP error: failed to parse date value.";
		break;
	case WPS_HC_ERROR_HTTP_BAD_MONTH:
		result = "HTTP error: failed to parse month value.";
		break;
	case WPS_HC_ERROR_HTTP_INVALID_DATE:
		result = "HTTP error: invalid date value.";
		break;
	case WPS_HC_ERROR_HTTP_UNKNOWN_METHOD:
		result = "HTTP error: unknown method name.";
		break;
	case WPS_HC_ERROR_HTTP_UNKNOWN_CONTENT_DISP:
		result = "HTTP error: unknown Content-Disposition value.";
		break;
	case WPS_HC_ERROR_HTTP_BAD_CONTENT_RANGE:
		result = "HTTP error: failed to parse Content-Range value.";
		break;
	case WPS_HC_ERROR_HTTP_BAD_RANGE:
		result = "HTTP error: failed to parse Range value.";
		break;
	case WPS_HC_ERROR_HTTP_BAD_WARNING:
		result = "HTTP error: failed to parse Warning value.";
		break;
	case WPS_HC_ERROR_WSP_INVALID_FIELD_CODE:
		result = "WSP error: invalid header field code.";
		break;
	case WPS_HC_ERROR_WSP_ILLEGAL_CODEPAGE_SHIFT:
		result = "WSP error: attempt to shift to an unknown header code page.";
		break;
	case WPS_HC_ERROR_WSP_INVALID_CONTENT_ENC:
		result = "WSP error: invalid Content-Encoding value.";
		break;
	case WPS_HC_ERROR_WSP_INVALID_ACCEPT_RANGE:
		result = "WSP error: invalid Accept-Range value.";
		break;
	case WPS_HC_ERROR_WSP_INVALID_CONNECTION:
		result = "WSP error: invalid Connection value.";
		break;
	case WPS_HC_ERROR_WSP_INVALID_TRANSFER_ENC:
		result = "WSP error: invalid Transfer-Encoding value.";
		break;
	case WPS_HC_ERROR_WSP_INVALID_Q_CODE:
		result = "WSP error: invalid Q-value code.";
		break;
	case WPS_HC_ERROR_WSP_INVALID_TIME_CODE:
		result = "WSP error: invalid Time prefix in the Retry-After value.";
		break;
	case WPS_HC_ERROR_WSP_UNKNOWN_LANG_CODE:
		result = "WSP error: unknown language code.";
		break;
	case WPS_HC_ERROR_WSP_UNKNOWN_CNT_TYPE_CODE:
		result = "WSP error: unknown Content-Type code.";
		break;
	case WPS_HC_ERROR_WSP_UNKNOWN_CHARSET_CODE:
		result = "WSP error: unknown charset code.";
		break;
	case WPS_HC_ERROR_WSP_UNKNOWN_METHOD_CODE:
		result = "WSP error: unknown method code.";
		break;
	case WPS_HC_ERROR_WSP_UNKNOWN_CACHE_CTL_CODE:
		result = "WSP error: unknown Cache-Control code.";
		break;
	case WPS_HC_ERROR_WSP_UNKNOWN_CONTENT_DISP:
		result = "WSP error: unknown Content-Disposition value.";
		break;
	case WPS_HC_ERROR_WSP_UNKNOWN_RANGE_CODE:
		result = "WSP error: unknown Range specifier code.";
		break;
	case WPS_HC_ERROR_WSP_INTEGER_EXPECTED:
		result = "WSP error: text found where integer value was expected.";
		break;
	case WPS_HC_ERROR_WSP_LENGTH_EXPECTED:
		result = "WSP error: short-int or text found where length value was expected.";
		break;
	case WPS_HC_ERROR_WSP_INCORRECT_VALUE_LEN:
		result = "WSP error: Incorrect Value-length.";
		break;
	case WPS_HC_ERROR_WSP_EMPTY_TEXT:
		result = "WSP error: empty text string.";
		break;
	case WPS_HC_ERROR_WSP_DATA_OVERFLOW:
		result = "WSP error: data overflows the end of the WSP header.";
		break;
	case WPS_HC_ERROR_WSP_INCORRECT_MD5_LEN:
		result = "WSP error: Content-MD5 value has incorrect length (it must be exactly 16 octets).";
		break;
	default:
		result = "Unknown HTTP header codec error";
		break;
	}
	return result;
}

#endif /* JNI_DEBUGGING */

/* 
 * Function   : decodeHeader
 *
 *  Decodes a header field.
 *
 * Parameters : - 
 * Returns    : nothing
 */

static WPS_STATUS decodeHeader(
	BYTE* data,
	UINT32      dataLen,
	UINT32      bodyLen,
	BOOL        separateContenttype,
	BYTE**      decodedData,
	UINT32*     decodedDataLen)
{
	WPS_STATUS status = WPS_NO_ERROR;
	BITF       flags = 0;

	if (useCodec) {
		if (separateContenttype == FALSE) {
			flags |= WPS_HC_OPT_DONT_SEPARATE_CONTENTTYPE;
		}

		*decodedDataLen = DECODED_HDR_MAX_LENGTH;
		*decodedData = (BYTE *) malloc(DECODED_HDR_MAX_LENGTH);
		if (*decodedData == NULL) {
			status = WPS_ERROR_OUT_OF_MEMORY;
            
		} else if (dataLen == 0) {
			JNI_DEBUG("-- Empty headers--");
			*decodedDataLen  = 0;
			**decodedData = '\0';
		} else {
			JNI_DEBUG_ARG1("-- Decoding headers (len %d)--", dataLen);
			printBinaryData(data, dataLen);

			status = HC_DecodeHeader(
				data, dataLen, bodyLen, NULL, 0, flags,
				*decodedData, decodedDataLen);
			if (WPS_HC_STATUS_IS_ERROR(status)) {
				JNI_DEBUG_ARG1("-- error in decoding: %s",
						  headerCodecMessage(status));
				status = WPS_CODEC_STATUS_BASE;
			} else if (WPS_HC_STATUS_IS_WARNING(status)) {
				JNI_DEBUG_ARG1("-- warning in decoding: %s",
						  headerCodecMessage(status));
				status = WPS_NO_ERROR;
			}

			if (status == WPS_NO_ERROR) {
				JNI_DEBUG_ARG1("-- into\n%s--\n", *decodedData);
			}
		}
	} else {
		JNI_DEBUG("-- Native decoding not used");
		*decodedData = data;
		*decodedDataLen = dataLen;
	}

	return status;
}

/* 
 * Function   : decodeHeaderFree
 *
 * Free data allocated by decoder.
 *
 * Parameters : - data: decoder data to free
 * Returns    : nothing
 */

static void decodeHeaderFree(BYTE** data)
{
	if (useCodec) {
		if (*data != NULL) {
    			free(*data);
    			*data = NULL;
		}
	}
}


/* 
 * Function   : encodeHeader
 *
 * Encodes a header field.
 *
 * Parameters : - 
 * Returns    : nothing
 */
static WPS_STATUS encodeHeader(
	BYTE* data,
	UINT32      dataLen,
	BOOL        separateContenttype,
	BYTE**      encodedData,
	UINT32*     encodedDataLen)
{
	WPS_STATUS status = WPS_NO_ERROR;
	BITF       flags = 0;
	BYTE*      nullTerminatedData;

	if (useCodec) {
		if (separateContenttype == FALSE) {
			flags |= WPS_HC_OPT_DONT_SEPARATE_CONTENTTYPE;
		}
		*encodedDataLen = dataLen;
		*encodedData = (BYTE *) malloc(dataLen);
		nullTerminatedData = (BYTE *) malloc(dataLen + 1);
		if (*encodedData == NULL || nullTerminatedData == NULL) {
			status = WPS_ERROR_OUT_OF_MEMORY;
           	 
		} else if (dataLen == 0) {
			JNI_DEBUG("-- Empty headers");
			*encodedDataLen = 0;
		} else {
			UINT32 i;
			BYTE *frcp = data;
			BYTE *tocp = nullTerminatedData;
			for (i = 0; i < dataLen; i++, frcp++, tocp++) {
				*tocp = *frcp;
			}
			/* memcpy(nullTerminatedData, data, dataLen); */
			nullTerminatedData[dataLen] = '\0';

			JNI_DEBUG_ARG1("-- Encoding headers\n%s\n", nullTerminatedData);

			status = HC_EncodeHeader( nullTerminatedData, NULL, 0, flags,
				*encodedData, encodedDataLen);
			if (WPS_HC_STATUS_IS_ERROR(status)) {
				JNI_DEBUG_ARG1("-- error in encoding: %s",
			  		headerCodecMessage(status));
				status = WPS_CODEC_STATUS_BASE;
			} else if (WPS_HC_STATUS_IS_WARNING(status)) {
				JNI_DEBUG_ARG1("-- warning in encoding: %s",
			     		headerCodecMessage(status));
				status = WPS_NO_ERROR;
			} 
			if (status == WPS_NO_ERROR) {
				JNI_DEBUG_ARG1("-- into (len %d)\n", 
			     		(int) *encodedDataLen);
				printBinaryData(*encodedData, *encodedDataLen);
			}
			free(nullTerminatedData);
		}
	} else {
		*encodedData = data;
		*encodedDataLen = dataLen;
	}
	return status;
}

/* 
 * Function   : encodeHeaderFree
 *
 * Free data allocated by encoder.
 *
 * Parameters : - data: encoder data to free
 * Returns    : nothing
 */

static void encodeHeaderFree(BYTE** data)
{
	if (useCodec) {
		if (*data != NULL) {
			free(*data);
			*data = NULL;
		}
	}
}

/*
 * Translate Connection Mode status into index into
 * cmExceptioMessage array in Connection class.
 */
int ConnectionApiStatus(WPS_STATUS s) {
	switch((int) s) {
	case WPS_NO_ERROR:
		return 1;
	case WPS_ERROR_UNKNOWN_LAYER_CONFIGURATION:
		return 2;
	case WPS_ERROR_BEARER_ERROR:
		return 3;
	case WPS_ERROR_BEARER_MISMATCH:
		return 4;
	case WPS_ERROR_LAYER_CFG_MISMATCH:
		return 5;
	case WPS_ERROR_PORT_ALREADY_BOUND:
		return 6;
	case WPS_ERROR_CANNOT_OPEN_PORT:
		return 7;
	case WPS_ERROR_UNKNOWN_BEARER:
		return 8;
	case WPS_ERROR_NO_DEFAULT_BEARER:
		return 9;
	case WPS_ERROR_TOO_MANY_CONNECTIONS:
		return 10;
	case WPS_ERROR_NULL_POINTER:
		return 11;
	case WPS_ERROR_STACK_NOT_RUNNING:
		return 12;
	case WPS_ERROR_BUFFER_TOO_SMALL:
		return 13;
	}
	return 0;
}

/*
 * Translate Session Layer status into Java index into error string array
 */
int WSPConnStatus(WPS_STATUS s) {
	switch((int) s) {
	case WSP_NO_ERROR:
		return 1;
	case WSP_ERROR_TOO_SMALL_BUFFER:
		return 2;
	case WSP_ERROR_INVALID_SESSION:
		return 3;
	case WSP_ERROR_INVALID_TRANSACTION: 
		return 4;
	case WSP_ERROR_PARAMETER_ERROR:
		return 5;
	case WSP_ERROR_SESSION_NOT_CONNECTED: 
		return 6;
	case WSP_ERROR_DATA_NOT_AVAIL: 
		return 7;
	case WSP_ERROR_NOT_SUPPORTED:
		return 8;
	case WSP_ERROR_INVALID_STATE: 
		return 9;
	case WSP_ERROR_TIMEOUT: 
		return 10;
	case WSP_ERROR_SESSION_CLOSED: 
		return 11;
	case WSP_ERROR_NO_MORE_SESSIONS: 
		return 12;
	case WSP_ERROR_NO_MORE_METHODS: 
		return 13;
	}
	return 0;
}

/*
 * Translate Session Layer status into Java index into error string array
 */
char *WSPConnStatusString(WPS_STATUS s) {
	static char tmpstr[16];

	switch((int) s) {
	case WSP_NO_ERROR: return "WSP_NO_ERROR";
	case WSP_ERROR_TOO_SMALL_BUFFER: return "WSP_ERROR_TOO_SMALL_BUFFER";
	case WSP_ERROR_INVALID_SESSION: return "WSP_ERROR_INVALID_SESSION";
	case WSP_ERROR_INVALID_TRANSACTION: return "WSP_ERROR_INVALID_TRANSACTION"; 
	case WSP_ERROR_PARAMETER_ERROR: return "WSP_ERROR_PARAMETER_ERROR";
	case WSP_ERROR_SESSION_NOT_CONNECTED: return "WSP_ERROR_SESSION_NOT_CONNECTED"; 
	case WSP_ERROR_DATA_NOT_AVAIL: return "WSP_ERROR_DATA_NOT_AVAIL"; 
	case WSP_ERROR_NOT_SUPPORTED: return "WSP_ERROR_NOT_SUPPORTED";
	case WSP_ERROR_INVALID_STATE: return "WSP_ERROR_INVALID_STATE"; 
	case WSP_ERROR_TIMEOUT: return "WSP_ERROR_TIMEOUT"; 
	case WSP_ERROR_SESSION_CLOSED: return "WSP_ERROR_SESSION_CLOSED"; 
	case WSP_ERROR_NO_MORE_SESSIONS: return "WSP_ERROR_NO_MORE_SESSIONS"; 
	case WSP_ERROR_NO_MORE_METHODS: return "WSP_ERROR_NO_MORE_METHODS"; 
	}
	sprintf(tmpstr, "<%d>", s);
	return tmpstr;
}

/*
 * Translate ABORT reason into string
 */
char *WSPAbortReasonString(BYTE s) {
	static char tmpstr[16];

	switch(s) {
	 /* Reason codes defined in WTP */
	 case WTP_REASON_CODE_UNKNOWN:	return "Unknown";
	 case WTP_REASON_CODE_PROTOERR:	return "WTP Protocol error";
	 case WTP_REASON_CODE_INVALIDTID:	return "Invalid TID";
	 case WTP_REASON_CODE_NOTIMPLEMENTEDCL2:	return "Not implemented CL2";
	 case WTP_REASON_CODE_NOTIMPLEMENTEDSAR:	return "Not implemented SAR";
	 case WTP_REASON_CODE_NOTIMPLEMENTEDUACK:	return "Not implemented ACK";
	 case WTP_REASON_CODE_WTPVERSIONZERO:	return "WTP Version zero";
	 case WTP_REASON_CODE_CAPTEMPEXCEEDED:	return "Cap temp exceeded";
	 case WTP_REASON_CODE_NORESPONSE:	return "No respone";
	 case WTP_REASON_CODE_MESSAGETOOLARGE:	return "Message too large";

	 /* Reason codes defined in WSP */
	 case WSP_REASON_CODE_PROTOERR:	return "WSP Protocol error";
	 case WSP_REASON_CODE_DISCONNECT:	return "Disconnect";
	 case WSP_REASON_CODE_SUSPEND:	return "Suspend";
	 case WSP_REASON_CODE_RESUME:	return "Resume";
	 case WSP_REASON_CODE_CONGESTION:	return "Congestion";
	 case WSP_REASON_CODE_CONNECTERR:	return "Connection Error";
	 case WSP_REASON_CODE_MRUEXCEEDED:	return "MRU Exceeded";
	 case WSP_REASON_CODE_MOREXCEEDED:	return "MOR Exceeded";
	 case WSP_REASON_CODE_PEERREQ:	return "Peer Request";
	 case WSP_REASON_CODE_NETERR:	return "Net error";
	 case WSP_REASON_CODE_USERREQ:	return "User Request";
	 case WSP_REASON_CODE_USERRFS:	return "User RFS";
	 case WSP_REASON_CODE_USERPND:	return "User PND";
	 case WSP_REASON_CODE_USERDCR:	return "User DCR";
	 case WSP_REASON_CODE_USERDCU:	return "User DCU";
	}
	sprintf(tmpstr, "<%d>", s);
	return tmpstr;
}

/*
 * Translate Transaction Layer status into index into 
 * ExceptionMessage array in Transaction class.
 */
int WSPTransStatus(WPS_STATUS s) {
	switch((int) s) {
	case WSP_NO_ERROR:
		return 1;
	case WSP_ERROR_TOO_SMALL_BUFFER:
		return 2;
	case WSP_ERROR_INVALID_SESSION:
		return 3;
	case WSP_ERROR_INVALID_TRANSACTION: 
		return 4;
	case WSP_ERROR_PARAMETER_ERROR:
		return 5;
	case WSP_ERROR_SESSION_NOT_CONNECTED: 
		return 6;
	case WSP_ERROR_DATA_NOT_AVAIL: 
		return 7;
	case WSP_ERROR_NOT_SUPPORTED:
		return 8;
	case WSP_ERROR_INVALID_STATE:
		return 9;
	case WSP_ERROR_TIMEOUT: 
		return 10;
	case WSP_ERROR_SESSION_CLOSED: 
		return 11;
	}
	return 0;
}

/*
 * Translate status returns from connectionless operations to arrays
 * into clExceptionMessage array in Connection class.
 */
int WSPClessStatus(WPS_STATUS s) {
	switch((int) s) {
	case WSP_NO_ERROR:
		return 1;
	case WSP_ERROR_INVALID_PARAMETER:
		return 2;
	case WSP_ERROR_TIMEOUT:
		return 3;
	case WSP_ERROR_TOO_SMALL_BODY:
		return 4;
	case WSP_ERROR_TOO_SMALL_HEADER:
		return 5;
	case WSP_ERROR_TOO_SMALL_URI:
		return 6;
	case WSP_ERROR_UNKNOWN_ERROR:
		return 7;
	}
	return 0;
}


/*
 * Class:     comm_wap_wps_Stack
 * Method:    init
 * Signature: ()I
 *
 * This method returns zero if it's successful, -1 in case JNI error, 
 * and positive error code in case of stack initialization error.
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Stack_cInit(
	JNIEnv *env, jobject obj)  {

    WPS_STATUS status;
    jclass tmp_class;
  
    JNI_DEBUG("Java interface to Nokia Wireless Protocol Stack initializing")

    if (((*env) -> GetJavaVM(env, &jvm)) != 0) {
        JNI_DEBUG("  ERROR getJavaVM")
        return -1;
    }

    // -----------------------
    //  INITIALIZE UnitInvokePacket
    // -----------------------
    JNI_DEBUG("Class initializing UnitInvokePacket")

    if ((tmp_class = (*env) -> FindClass(env, "com/nokia/comm/wap/wps/UnitInvokePacket")) == 0) {
        JNI_DEBUG("ERROR: Class not found")
        return -1;
    }
    if ((class_unit_invoke_packet = (*env) -> NewGlobalRef(env, tmp_class)) == 0) {
        JNI_DEBUG("ERROR: Cannot create global reference")
        return -1;
    }

    JNI_DEBUG("  Checking object fields")
    field_unit_invoke_packet_conn     = (*env) -> GetFieldID(env, class_unit_invoke_packet, "conn", "Lcom/nokia/comm/wap/wps/Connection;");
    field_unit_invoke_packet_method   = (*env) -> GetFieldID(env, class_unit_invoke_packet, "method", "I");
    field_unit_invoke_packet_uri      = (*env) -> GetFieldID(env, class_unit_invoke_packet, "uri", "[B");
    field_unit_invoke_packet_body     = (*env) -> GetFieldID(env, class_unit_invoke_packet, "body", "[B");
    field_unit_invoke_packet_headers  = (*env) -> GetFieldID(env, class_unit_invoke_packet, "headers", "[B");
    field_unit_invoke_packet_trans_id = (*env) -> GetFieldID(env, class_unit_invoke_packet, "transId", "I");

    // -----------------------
    //  INITIALIZE UnitResultPacket
    // -----------------------
    JNI_DEBUG("Class initializing UnitResultPacket")
        
    if ((tmp_class = (*env) -> FindClass(env, "com/nokia/comm/wap/wps/UnitResultPacket")) == 0) {
        JNI_DEBUG("ERROR: Class not found")
        return -1;
    }
    if ((class_unit_result_packet = (*env) -> NewGlobalRef(env, tmp_class)) == 0) {
        JNI_DEBUG("ERROR: Cannot create global reference")
        return -1;
    }

    JNI_DEBUG("  Checking object fields")
    field_unit_result_packet_conn = (*env) -> GetFieldID(env, class_unit_result_packet, "conn", "Lcom/nokia/comm/wap/wps/Connection;");
    field_unit_result_packet_body = (*env) -> GetFieldID(env, class_unit_result_packet, "body", "[B");
    field_unit_result_packet_headers = (*env) -> GetFieldID(env, class_unit_result_packet, "headers", "[B");
    field_unit_result_packet_status = (*env) -> GetFieldID(env, class_unit_result_packet, "status", "I");
    field_unit_result_packet_trans_id = (*env) -> GetFieldID(env, class_unit_result_packet, "transId", "I");

    // -----------------------
    //  INITIALIZE UnitPushPacket
    // -----------------------
    JNI_DEBUG("Class initializing UnitPushPacket")

    if ((tmp_class = (*env) -> FindClass(env, "com/nokia/comm/wap/wps/UnitPushPacket")) == 0) {
        JNI_DEBUG("ERROR: Class not found")
        return -1;
    }
    if ((class_unit_push_packet = (*env) -> NewGlobalRef(env, tmp_class)) == 0) {
        JNI_DEBUG("ERROR: Cannot create global reference")
            return -1;
    }

    JNI_DEBUG("  Checking object fields")
    field_unit_push_packet_conn = (*env) -> GetFieldID(env, class_unit_push_packet, "conn", "Lcom/nokia/comm/wap/wps/Connection;");
    field_unit_push_packet_body = (*env) -> GetFieldID(env, class_unit_push_packet, "body", "[B");
    field_unit_push_packet_headers = (*env) -> GetFieldID(env, class_unit_push_packet, "headers", "[B");
    field_unit_push_packet_push_id = (*env) -> GetFieldID(env, class_unit_push_packet, "pushId", "I");

    // -----------------------
    //  INITIALIZE Connection
    // -----------------------
    JNI_DEBUG("Class initializing Connection")

    if ((tmp_class = (*env) -> FindClass(env, "com/nokia/comm/wap/wps/Connection")) == 0) {
        JNI_DEBUG("ERROR: Class not found")
        return -1;
    }
    if ((class_connection = (*env) -> NewGlobalRef(env, tmp_class)) == 0) {
        JNI_DEBUG("ERROR: Cannot create global reference")
        return -1;
    }

    JNI_DEBUG("  Checking object fields")
    field_connection_id = (*env) -> GetFieldID(env, class_connection, "id", "I");
    field_connection_bearer_id = (*env) -> GetFieldID(env, class_connection, "bearerId", "I");
    field_connection_laddr = (*env) -> GetFieldID(env, class_connection, "lAddr", "[B");
    field_connection_lport = (*env) -> GetFieldID(env, class_connection, "lPort", "I");
    field_connection_raddr = (*env) -> GetFieldID(env, class_connection, "rAddr", "[B");
    field_connection_rport = (*env) -> GetFieldID(env, class_connection, "rPort", "I");
    field_connection_layer_cfg = (*env) -> GetFieldID(env, class_connection, "layerCfg", "I");

    // -----------------------
    //  INITIALIZE Session
    // -----------------------
    JNI_DEBUG("Class initializing Session")

    if ((tmp_class = (*env) -> FindClass(env, "com/nokia/comm/wap/wps/Session")) == 0) {
        JNI_DEBUG("ERROR: Class not found")
        return -1;
    }
    if ((class_session = (*env) -> NewGlobalRef(env, tmp_class)) == 0) {
        JNI_DEBUG("ERROR: Cannot create global reference")
        return -1;
    }

    JNI_DEBUG("  Checking object fields")
    method_session_constructor = (*env) -> GetMethodID(env, class_session, "<init>", "()V");
    field_session_id    = (*env) -> GetFieldID(env, class_session, "id", "I");
    field_session_state = (*env) -> GetFieldID(env, class_session, "state", "I");
    field_session_nrofevents = (*env) -> GetFieldID(env, class_session, "nrOfEvents", "I");
    field_session_load_id = (*env) -> GetFieldID(env, class_session, "loadID", "I");

    // -----------------------
    //  INITIALIZE Transaction
    // -----------------------
    JNI_DEBUG("Class initializing Transaction")

    if ((tmp_class = (*env) -> FindClass(env, "com/nokia/comm/wap/wps/Transaction")) == 0) {
        JNI_DEBUG("ERROR: Class not found")
        return -1;
    }
    if ((class_transaction = (*env) -> NewGlobalRef(env, tmp_class)) == 0) {
        JNI_DEBUG("ERROR: Cannot create global reference")
        return -1;
    }

    JNI_DEBUG("  Checking object fields")
    method_transaction_constructor = (*env) -> GetMethodID(env, class_transaction, "<init>", "()V");
    field_transaction_id    = (*env) -> GetFieldID(env, class_transaction, "id", "I");
    field_transaction_state = (*env) -> GetFieldID(env, class_transaction, "state", "I");

    // -----------------------
    //  INITIALIZE Exception
    // -----------------------
    JNI_DEBUG("Class initializing Exception")

    if ((tmp_class = (*env) -> FindClass(env, "com/nokia/comm/wap/wps/ExceptionEvent")) == 0) {
        JNI_DEBUG("ERROR: Class not found")
        return -1;
    }
    if ((class_exception = (*env) -> NewGlobalRef(env, tmp_class)) == 0) {
        JNI_DEBUG("ERROR: Cannot create global reference")
        return -1;
    }

    JNI_DEBUG("  Checking object fields")
    method_exception_constructor = (*env) -> GetMethodID(env, class_exception, "<init>", "()V");
    field_exception_id    = (*env) -> GetFieldID(env, class_exception, "id", "I");

    // -----------------------
    //  INITIALIZE Event
    // -----------------------
    JNI_DEBUG("Class initializing Event")

    if ((tmp_class = (*env) -> FindClass(env, "com/nokia/comm/wap/wps/Event")) == 0) {
        JNI_DEBUG("ERROR: Class not found")
        return -1;
    }
    if ((class_event = (*env) -> NewGlobalRef(env, tmp_class)) == 0) {
        JNI_DEBUG("ERROR: Cannot create global reference")
        return -1;
    }

    JNI_DEBUG("  Checking object fields")
    field_event_type        = (*env) -> GetFieldID(env, class_event, "type", "I");
    field_event_session     = (*env) -> GetFieldID(env, class_event, "session", "Lcom/nokia/comm/wap/wps/Session;");
    field_event_transaction = (*env) -> GetFieldID(env, class_event, "transaction", "Lcom/nokia/comm/wap/wps/Transaction;");
    field_event_exception   = (*env) -> GetFieldID(env, class_event, "exception", "Lcom/nokia/comm/wap/wps/ExceptionEvent;");

    // -----------------------
    //  INITIALIZE ByteBuffer
    // -----------------------
    JNI_DEBUG("Class initializing ByteBuffer")
        
    if ((tmp_class = (*env) -> FindClass(env, "com/nokia/comm/wap/wps/ByteBuffer")) == 0) {
        JNI_DEBUG("  ERROR FindClass com.nokia.comm.wap.wps.ByteBuffer")
        return -1;
    }
    if ((class_bytebuffer = (*env) -> NewGlobalRef(env, tmp_class)) == 0) {
        JNI_DEBUG("  ERROR NewGlobalRef com.nokia.comm.wap.wps.ByteBuffer")
        return -1;
    }

    JNI_DEBUG("  Checking object fields")
    method_bytebuffer_constructor = (*env) -> GetMethodID(env, class_bytebuffer, "<init>", "()V");
    field_bytebuffer_buf          = (*env) -> GetFieldID(env, class_bytebuffer, "buf", "[B");
    field_bytebuffer_len          = (*env) -> GetFieldID(env, class_bytebuffer, "len", "I");
    field_bytebuffer_max_len      = (*env) -> GetFieldID(env, class_bytebuffer, "maxLen", "I");

    /* NOTE: BEARER_INIT_HERE is defined in the makefile */
/*#ifdef BEARER_INIT_HERE
    // -------------------
    //  INITIALIZE Bearer
    // -------------------
    JNI_DEBUG("Class initializing Bearer")

    if ((tmp_class = (*env) -> FindClass(env, "com/nokia/comm/wap/wps/Bearer")) == 0) {
        JNI_DEBUG("  ERROR FindClass com.nokia.comm.wap.wps.Bearer")
        return -1;
    }
    if ((bearer_class = (*env) -> NewGlobalRef(env, tmp_class)) == 0) {
        JNI_DEBUG("  ERROR NewGlobalRef com.nokia.comm.wap.wps.Bearer")
        return -1;
    }

    JNI_DEBUG("  Checking object fields")
    bearer_open_method                 = (*env) -> GetMethodID(env, bearer_class, "open", "(IZ)I");
    bearer_close_method                = (*env) -> GetMethodID(env, bearer_class, "close", "(I)I");
    bearer_close_all_method            = (*env) -> GetMethodID(env, bearer_class, "closeAll", "()I");
    bearer_receive_buffer_method       = (*env) -> GetMethodID(env, bearer_class, "receiveBuffer", "(Lcom/nokia/comm/wap/wps/BearerMessage;)I");
    bearer_send_buffer_method          = (*env) -> GetMethodID(env, bearer_class, "sendBuffer", "(Lcom/nokia/comm/wap/wps/BearerMessage;)I");
    bearer_number_of_open_ports_method = (*env) -> GetMethodID(env, bearer_class, "numberOfOpenPorts", "()I");
    bearer_init_method                 = (*env) -> GetMethodID(env, bearer_class, "init", "()I");
    bearer_get_default_method          = (*env) -> GetMethodID(env, bearer_class, "getDefault", "(I)I");
    bearer_get_local_host_name_method  = (*env) -> GetMethodID(env, bearer_class, "getLocalHostName", "(Lcom/nokia/comm/wap/wps/ByteBuffer;)I");
    bearer_id_field                    = (*env) -> GetFieldID(env, bearer_class, "id", "I");

    // --------------------------
    //  INITIALIZE BearerMessage
    // --------------------------
    JNI_DEBUG("Class initializing BearerMessage")
    if ((tmp_class = (*env) -> FindClass(env, "com/nokia/comm/wap/wps/BearerMessage")) == 0) {
        JNI_DEBUG("  ERROR FindClass com.nokia.comm.wap.wps.BearerMessage")
        return -1;
    }
    if ((bearermessage_class = (*env) -> NewGlobalRef(env, tmp_class)) == 0) {
        JNI_DEBUG("  ERROR NewGlobalRef com.nokia.comm.wap.wps.BearerMessage")
        return -1;
    }

    JNI_DEBUG("  Checking object fields")
    bearermessage_constructor = (*env) -> GetMethodID(env, bearermessage_class, "<init>", "()V");
    bearermessage_buf_field   = (*env) -> GetFieldID(env, bearermessage_class, "buf", "Lcom/nokia/comm/wap/wps/ByteBuffer;");
    bearermessage_lport_field = (*env) -> GetFieldID(env, bearermessage_class, "lPort", "I");
    bearermessage_laddr_field = (*env) -> GetFieldID(env, bearermessage_class, "lAddr", "Lcom/nokia/comm/wap/wps/ByteBuffer;");
    bearermessage_rport_field = (*env) -> GetFieldID(env, bearermessage_class, "rPort", "I");
    bearermessage_raddr_field = (*env) -> GetFieldID(env, bearermessage_class, "rAddr", "Lcom/nokia/comm/wap/wps/ByteBuffer;");
    bearermessage_type_field  = (*env) -> GetFieldID(env, bearermessage_class, "type", "I");

#else /* BEARER_INIT_HERE */
  //  JavaBearer_Init( env );
//#endif /* !BEARER_INIT_HERE */

    JNI_DEBUG("DLL initializing")

    status = WPS_Startup();
    if (status == WPS_NO_ERROR) {
        WSL_LOG_Init();
        JNI_DEBUG("Java interface to Nokia Wireless Protocol Stack initialization completed")
    } else {
        JNI_DEBUG("Java interface to Nokia Wireless Protocol Stack initialization FAILED")
    }
    return status;
}

/*
 * Class:     comm_wap_wps_Stack
 * Method:    cFinish
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Stack_cFinish(
	JNIEnv *env, jobject obj) 
{
	JNI_DEBUG("Java interface to Nokia Wireless Protocol Stack shutdown")

	JNI_DEBUG("Removing global references")

	(*env) -> DeleteGlobalRef(env, class_unit_invoke_packet);
	(*env) -> DeleteGlobalRef(env, class_unit_result_packet);
	(*env) -> DeleteGlobalRef(env, class_unit_push_packet);
	(*env) -> DeleteGlobalRef(env, class_connection);
	(*env) -> DeleteGlobalRef(env, class_session);
	(*env) -> DeleteGlobalRef(env, class_transaction);
	(*env) -> DeleteGlobalRef(env, class_event);
	(*env) -> DeleteGlobalRef(env, class_exception);
	//(*env) -> DeleteGlobalRef(env, bearer_class);
	//(*env) -> DeleteGlobalRef(env, bearermessage_class);
	(*env) -> DeleteGlobalRef(env, class_bytebuffer);

	JNI_DEBUG("DLL shutdown")

	if ( WPS_Shutdown( WPS_SHUTDOWN_ACTIVITY_NONE ) != WPS_NO_ERROR){
		JNI_DEBUG("Stack shutdown error")
	}

	JNI_DEBUG("Java interface to Nokia Wireless Protocol Stack shutdown completed")

	return 1;
}

/*
 * Class:     com_nokia_comm_wap_wps_Stack
 * Method:    cSetLog
 * Signature: ([B)I
 */
JNIEXPORT void JNICALL Java_com_nokia_comm_wap_wps_Stack_cSetLog(
	JNIEnv *env, 
	jobject obj, 
	jbyteArray buf) 
{
	jbyte		*buf_ptr;
	jboolean	is_copy;
	UINT32          buf_len;
	
	JNI_DEBUG("Call   -> wslSetLog");
	buf_len = (UINT32) (*env) -> GetArrayLength(env, buf);
	buf_ptr = (*env) -> GetByteArrayElements(env, buf, &is_copy);

	WSL_LOG_SetLog((UCHAR *) buf_ptr);
	(*env) -> ReleaseByteArrayElements(env, buf, buf_ptr, 0);
	
	JNI_DEBUG("  Return -> wslSetLog");
}

/*
 * Class:     com_nokia_comm_wap_wps_Stack
 * Method:    cSetDebugLog
 * Signature: ([B)I
 */
JNIEXPORT void JNICALL Java_com_nokia_comm_wap_wps_Stack_cSetDebugLog(
	JNIEnv *env, 
	jobject obj, 
	jbyteArray buf) 
{
	jbyte		*buf_ptr;
	jboolean	is_copy;
	UINT32          buf_len;
	
	JNI_DEBUG("Call   -> wslSetDebugLog");
        buf_len = (UINT32) (*env) -> GetArrayLength(env, buf);
	buf_ptr = (*env) -> GetByteArrayElements(env, buf, &is_copy);

	WSL_LOG_SetDebugLog((UCHAR *) buf_ptr);
	(*env) -> ReleaseByteArrayElements(env, buf, buf_ptr, 0);
	
	JNI_DEBUG("  Return -> wslSetDebugLog");
}

/*
 * Class:     com_nokia_comm_wap_wps_Stack
 * Method:    cSetUseCodec
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_nokia_comm_wap_wps_Stack_cSetUseCodec
(JNIEnv *env, jobject obj, jboolean value)
{
	if (value == 1) {
		JNI_DEBUG("Call   -> cSetUseCodec to TRUE");
	} else {
		JNI_DEBUG("Call   -> cSetUseCodec to FALSE");
	}
	useCodec = (BOOL) value;
}


/*
 * Set up Connection class
 */
WPS_STATUS JNI_SetConnection(
	JNIEnv *env, const WPS_HCONN conn, jobject connection) 
{
	WPS_STATUS  status;

	WPS_HOST	host;
	unsigned short host_len;
	WPS_PORT	port;

	jbyteArray  byte_array;
	jbyte		*byte_ptr;
	jboolean	is_copy;
	int			i, bearer_id;
	bearer_adapter_info *bearer_desc = NULL;
        void* bearer_ptr = &bearer_desc; // to avoid GCC warnings

	(*env)->SetIntField(env,connection,field_connection_id,(jint)conn);
	status = WPS_GetBearer(conn, bearer_ptr);

	if (status != WPS_NO_ERROR) {
		return status;
	}

	bearer_id = (bearer_desc -> id) - JNI_FIRST_BEARER_ID;
	(*env) -> SetIntField(env, connection, field_connection_bearer_id, (jint) bearer_id);

	host  = (WPS_HOST) malloc(ADDRESS_MAX_LENGTH);
	host_len = ADDRESS_MAX_LENGTH;
	
	status = WPS_GetRemoteAddress(
		conn,
		host, &host_len,
		&port);

	if (status != WPS_NO_ERROR) {
		return status;
	}

	byte_array = (*env) -> NewByteArray(env, host_len);
	byte_ptr = (*env) -> GetByteArrayElements(env, byte_array, &is_copy);
	for(i = 0; i < host_len; i++) {
		*(byte_ptr + i) = *(host + i);
	}

	(*env) -> ReleaseByteArrayElements(env, byte_array, byte_ptr, 0);

	(*env) -> SetObjectField(env, connection, field_connection_raddr, (jobject) byte_array);
	(*env) -> SetIntField(env, connection, field_connection_rport, (jint) port);

	
	host_len = ADDRESS_MAX_LENGTH;
	
	status = WPS_GetLocalAddress(
		conn,
		host, &host_len,
		&port);

	if (status != WPS_NO_ERROR) {
		return status;
	}

	byte_array = (*env) -> NewByteArray(env, host_len);
	byte_ptr = (*env) -> GetByteArrayElements(env, byte_array, &is_copy);
	for(i = 0; i < host_len; i++) {
		*(byte_ptr + i) = *(host + i);
	}

	free(host);
	(*env) -> ReleaseByteArrayElements(env, byte_array, byte_ptr, 0);

	(*env) -> SetObjectField(env, connection, field_connection_laddr, (jobject) byte_array);
	(*env) -> SetIntField(env, connection, field_connection_lport, (jint) port);

	return status;
}

/*
 * Class:     com_nokia_comm_wap_wps_Transaction
 * Method:    wspExecuteTransaction
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Transaction_wspExecuteTransaction(
	JNIEnv *env, jobject obj, jint transaction_id)
{
	WPS_STATUS status;

	JNI_DEBUG_ARG1("Call   -> wspExecuteTransaction TID:%d\n", (int)transaction_id)

	status = WSP_ExecuteTransaction((WSP_TransactionHandle) transaction_id);

	JNI_DEBUG_ARG1("  Return -> wspExecuteTransaction: %d\n", status)

	return WSPTransStatus(status);
}

/*
 * Class:     com_nokia_comm_wap_wps_Transaction
 * Method:    wspAbortTransaction
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Transaction_wspAbortTransaction(
	JNIEnv *env, jobject obj, jint transaction_id)
{
	WPS_STATUS status;

	JNI_DEBUG_ARG1("Call   -> wspAbortTransaction TID:%d\n", (int)transaction_id)

	status = WSP_AbortTransaction((WSP_TransactionHandle) transaction_id, 
		(BYTE)WTP_ERROR_ABORT_USER);

	JNI_DEBUG_ARG1("  Return -> wspAbortTransaction: %d\n", status)

	return WSPTransStatus(status);
}


/*
 * Class:     com_nokia_comm_wap_wps_Transaction
 * Method:    wspAcknowledgeTransaction
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Transaction_wspAcknowledgeTransaction(
	JNIEnv *env, jobject obj, jint transaction_id)
{
	WPS_STATUS status;

	JNI_DEBUG("Call   -> wspAcknowledgeTransaction")

	status = WSP_AcknowledgeTransaction((WSP_TransactionHandle) transaction_id);

	JNI_DEBUG("  Return -> wspAcknowledgeTransaction")

	return WSPTransStatus(status);
}

/*
 * Class:     com_nokia_comm_wap_wps_Transaction
 * Method:    wspCreateTransaction
 * Signature: (ILcom/nokia/comm/wap/wps/UnitInvokePacket;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Transaction_wspCreateTransaction(
	JNIEnv *env, jobject obj, jint sess_id, jobject p)
{
	WPS_STATUS    status;
	WSP_TransactionHandle trans_id;
	UINT8		method;
	UINT16		uri_len;
	UINT16		body_len;
	UINT16		headers_len;
#ifdef DO_HDR_ENCODING
	BYTE*		encoded_hdr;
	UINT32		encoded_hdr_len;
	BOOL		haveContentTypeHdr = FALSE;
#endif
	jbyteArray	uri_array;
	jbyteArray	body_array;
	jbyteArray	headers_array;
	jbyte		*uri_ptr = 0;
	jbyte		*body_ptr = 0;
	jbyte		*headers_ptr = 0;
	jboolean	is_copy;

	JNI_DEBUG("Call   -> wspCreateTransaction");

	method = (UINT8) (*env) -> GetIntField(env, p, field_unit_invoke_packet_method);

	uri_array = (jbyteArray) (*env) -> GetObjectField(env, p, field_unit_invoke_packet_uri);
	uri_len = (int) (*env) -> GetArrayLength(env, uri_array);
	uri_ptr = (*env) -> GetByteArrayElements(env, uri_array, &is_copy);

	body_array = (jbyteArray) (*env) -> GetObjectField(env, p, field_unit_invoke_packet_body);
	body_len = (int) (*env) -> GetArrayLength(env, body_array);
	if (body_len)
		body_ptr = (*env) -> GetByteArrayElements(env, body_array, &is_copy);

	headers_array = (jbyteArray) (*env) -> GetObjectField(env, p, field_unit_invoke_packet_headers);
	headers_len = (int) (*env) -> GetArrayLength(env, headers_array);
	if (headers_len) {
		headers_ptr = (*env) -> GetByteArrayElements(env, headers_array, &is_copy);
	}
	JNI_DEBUG_CMD(printf("	wspCrTrans-> Sess:%d Method:%x URL: %s (%d URL, %d Hdr, %d Body)\n", 
		(int)sess_id, method, uri_ptr, uri_len, headers_len, body_len);)

#ifdef DO_HDR_ENCODING
	/*
	 * Encode headers.  Use separate content-type header encoding on GETs, since request 
	 * has no content type, but not on POSTs.
	 */
	if (method == WSP_METHOD_POST)
		haveContentTypeHdr = TRUE;
	encoded_hdr = headers_ptr;
	encoded_hdr_len = headers_len;
	if (useCodec && headers_len != 0) {
		status = encodeHeader((BYTE*) headers_ptr, headers_len, haveContentTypeHdr, &encoded_hdr,
			&encoded_hdr_len);
		if (status != WPS_NO_ERROR) {
			encoded_hdr = headers_ptr;
			encoded_hdr_len = headers_len;
			JNI_DEBUG(" wspCrTrans -> Encoding Headers Failed!")
		} else {
			JNI_DEBUG_ARG1(" ->Encoded headers OK to %d bytes\n", encoded_hdr_len)
		}
	}
	status = WSP_CreateMethodTransaction( (WSP_SessionHandle) sess_id, method, uri_ptr, 
		uri_len, encoded_hdr, (UINT16)encoded_hdr_len, body_ptr, body_len, &trans_id);
#else
	status = WSP_CreateMethodTransaction((WSP_SessionHandle) sess_id, 
		method, uri_ptr, uri_len, headers_ptr, headers_len, body_ptr, body_len, &trans_id);
#endif /* DO_HDR_ENCODING */

	if (status == WPS_NO_ERROR) {
		(*env) -> SetIntField(env, obj, field_transaction_id, (jint) trans_id);
	}
#ifdef DO_HDR_ENCODING
	if (useCodec && headers_len != 0) {
		encodeHeaderFree(&encoded_hdr); 
	}
#endif /* DO_HDR_ENCODING */

	JNI_DEBUG_CMD(printf("  Return -> wspCreateTransaction: %d TID:%d\n", status, trans_id);)
	return WSPTransStatus(status);
}

/*
 * Class:     com_nokia_comm_wap_wps_Transaction
 * Method:    wspReleaseTransaction
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Transaction_wspReleaseTransaction(
	JNIEnv *env, jobject obj, jint transaction_id)
{
	WPS_STATUS status;

	JNI_DEBUG("Call   -> wspReleaseTransaction")

	status = WSP_ReleaseTransaction((WSP_TransactionHandle) transaction_id);

	JNI_DEBUG_ARG1("  Return -> wspReleaseTransaction: %d", status)

	return WSPTransStatus(status);
}

/*
 * Class:     com_nokia_comm_wap_wps_Transaction
 * Method:    wspGetTransactionData
 * Signature: (IILcom/nokia/comm/wap/wps/ByteBuffer;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Transaction_wspGetTransactionData(
	JNIEnv *env, jobject obj, jint transaction_id, jint selector, jobject buf_obj)
{
	WPS_STATUS status = WPS_NO_ERROR;
	BYTE*      data;
	Length     data_len = 0;
	UINT32     data_selector;
	Length     body_length = 1;		/* len of body for header decode behavior */
	BYTE       tmp_body_buffer[1];
	BOOL       decode = FALSE;		/* do we call decoder? */
	BOOL       separateContentType = FALSE;	/* default for any content without C-T hdrs */

	JNI_DEBUG_ARG1("Call   -> wspGetTransactionData (%d)\n",(int)selector);

	switch(selector) {
	   case 1:
		data_selector = WSP_TRANSACTION_PUSH_BODY;
		break;
	   case 2:
		data_selector = WSP_TRANSACTION_PUSH_HEADERS;
		status = WSP_GetTransactionData(
			(WSP_TransactionHandle) transaction_id, 
			tmp_body_buffer, &body_length, WSP_TRANSACTION_PUSH_HEADERS);
		decode = TRUE;
		separateContentType = TRUE;
		/*
		 * Reset status, which is checked at end of loop.
		 */
		if (status == WSP_ERROR_TOO_SMALL_BUFFER) {
			status = WSP_NO_ERROR;
		}
		break;
	   case 3:
		data_selector = WSP_TRANSACTION_RESULT_STATUS;
		data_len = 1;   
		break;
	   case 4:
		data_selector = WSP_TRANSACTION_RESULT_BODY;
		break;
	   case 5:
		data_selector = WSP_TRANSACTION_RESULT_HEADERS;
		/*
		 * Find out body length for decoder arg.
		 */
		status = WSP_GetTransactionData(
			(WSP_TransactionHandle) transaction_id, 
			tmp_body_buffer, &body_length, WSP_TRANSACTION_RESULT_HEADERS);
		JNI_DEBUG_ARG1(" -> wspGetTransactionData:ResHdrs is %d bytes\n", body_length);
		/*
		 * Reset status, which is checked at end of loop.
		 */
		if (status == WSP_ERROR_TOO_SMALL_BUFFER) {
			status = WSP_NO_ERROR;
		}
		decode = TRUE;
		separateContentType = TRUE;
		break;
	   case 6:
		data_selector = WSP_TRANSACTION_INVOKE_URI;
		break;
	   case 7:
		data_selector = WSP_TRANSACTION_INVOKE_METHOD;
		data_len = 1;   
		break;
	   case 8:
		data_selector = WSP_TRANSACTION_INVOKE_HEADERS;
		/* 
		 * Determine body size 
		 */
		status = WSP_GetTransactionData( (WSP_TransactionHandle) transaction_id, 
			tmp_body_buffer, &body_length, WSP_TRANSACTION_INVOKE_BODY);
		JNI_DEBUG_ARG1(" -> wspGetTransactionData:InvkHdrs is %d bytes\n", body_length);
		if (status == WPS_NO_ERROR) {
			/* 
			 * Determine method and thus content type coding 
			 */
			BYTE   method[1];
			Length size = 1;

			status = WSP_GetTransactionData(
	    				(WSP_TransactionHandle) transaction_id, 
	    				method, &size, WSP_TRANSACTION_INVOKE_METHOD);
			if (status == WPS_NO_ERROR) {
	    			if (*method >= WSP_GET_METHOD_START &&
				    *method <= WSP_GET_METHOD_END)
	    			{
					separateContentType = FALSE;
	    			} else {
					separateContentType = TRUE;
	    			}
			}
		}
		decode = TRUE;
		break;
	   case 9:
		data_selector = WSP_TRANSACTION_INVOKE_BODY;	/* for POSTs */
		break;
	   case 10:
		data_selector = WSP_TRANSACTION_ACK_HEADERS;
		decode = TRUE;
		separateContentType = FALSE;
		break;
	   case 11:
		data_selector = WSP_TRANSACTION_ABORT_REASON;
		body_length = 1;
		break;
	   default:
		JNI_DEBUG("  Return -> wspGetTransactionData error: invalid selector");
		return WSPTransStatus(WSP_ERROR_PARAMETER_ERROR);
	}
    
        if (data_len == 0) {
            /* 
	     * We don't know the length of the data beforehand, get the
             * actual data length and allocate only that sized block 
	     */
            status = WSP_GetTransactionData( (WSP_TransactionHandle) transaction_id, 
                NULL, &data_len, data_selector); 
            if (status == WSP_ERROR_TOO_SMALL_BUFFER) {
                status = WPS_NO_ERROR;
            }
        }

	if (status == WPS_NO_ERROR) {
		JNI_DEBUG_ARG1(" -> wspGetTransactionData getting %d bytes\n", data_len);
		if ((data = (BYTE *) malloc(data_len)) == NULL) {
			status = WPS_ERROR_OUT_OF_MEMORY;
		} else {            
			status = WSP_GetTransactionData(
				(WSP_TransactionHandle) transaction_id, data,
				&data_len, data_selector); 
			if (status == WPS_NO_ERROR) {
				if (decode == TRUE) {
	    				BYTE*  decoded_data;
	    				UINT32 decoded_data_len;
                   	 
	    				status = decodeHeader( data, data_len, 
						body_length, separateContentType,
						&decoded_data, &decoded_data_len);
	    				if (status == WPS_NO_ERROR) {
						JNI_SetByteBuffer32(env, decoded_data,
				    			decoded_data_len, buf_obj);
	    				}
	    				decodeHeaderFree(&decoded_data);
				} else {
	    				JNI_SetByteBuffer32(env, data, data_len, buf_obj);
				}
			}
			free(data);
		}
	}
	JNI_DEBUG_ARG1("  Return -> wspGetTransactionData (%d)\n",(int)status);
	if (data_selector == WSP_TRANSACTION_ABORT_REASON)
		JNI_DEBUG_ARG1("  Abort reason: (%s)\n", WSPAbortReasonString(*data));
	return WSPTransStatus(status);
}

/*
 * Class:     com_nokia_comm_wap_wps_Transaction
 * Method:    wspGetTransactionOwner
 * Signature: (ILcom/nokia/comm/wap/wps/Session;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Transaction_wspGetTransactionOwner(
	JNIEnv *env, jobject obj, jint transaction_id, jobject session_obj)
{
	WPS_STATUS status;
	WSP_SessionHandle session;

	JNI_DEBUG("Call   -> wspGetTransactionOwner")

	status = WSP_GetTransactionOwner((WSP_TransactionHandle) transaction_id, &session);

	if (status == WPS_NO_ERROR) {
		(*env) -> SetIntField(env, session_obj, field_session_id, (jint) session);
	}

	JNI_DEBUG("  Return -> wspGetTransactionOwner")

	return WSPTransStatus(status);
}

/*
 * Class:     com_nokia_comm_wap_wps_Transaction
 * Method:    wspGetTransactionState
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Transaction_wspGetTransactionState(
	JNIEnv *env, jobject obj, jint transaction_id)
{
	WPS_STATUS    status;
	WSP_TransactionState state;

	JNI_DEBUG("Call   -> wspGetTransactionState")

	status = WSP_GetTransactionState((WSP_TransactionHandle) transaction_id, &state);

	if (status == WPS_NO_ERROR) {
		if (state == WSP_TRANSACTION_INIT) {
			(*env) -> SetIntField(env, obj, field_session_state, (jint) 1);
		}
		else if (state == WSP_TRANSACTION_REQUESTING) {
			(*env) -> SetIntField(env, obj, field_session_state, (jint) 2);
		}
		else if (state == WSP_TRANSACTION_REPLYING) {
			(*env) -> SetIntField(env, obj, field_session_state, (jint) 3);
		}
		else if (state == WSP_TRANSACTION_DONE) {
			(*env) -> SetIntField(env, obj, field_session_state, (jint) 4);
		}
		else if (state == WSP_TRANSACTION_ABORTED) {
			(*env) -> SetIntField(env, obj, field_session_state, (jint) 5);
		}
		else if (state == WSP_TRANSACTION_PUSHING) {
			(*env) -> SetIntField(env, obj, field_session_state, (jint) 6);
		}
		else {
			(*env) -> SetIntField(env, obj, field_session_state, (jint) 0);
		}
	}
	JNI_DEBUG("  Return -> wspGetTransactionState")

	return WSPTransStatus(status);
}

/*
 * Class:     com_nokia_comm_wap_wps_Session
 * Method:    wspCreateSession
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Session_wspCreateSession(
	JNIEnv *env, jobject obj, jint max_messages)
{
	WPS_STATUS    status;
	WSP_SessionHandle session;
	
	JNI_DEBUG("Call   -> wspCreateSession")

	status = WSP_CreateSession((UINT32) max_messages, &session );

	if (status == WPS_NO_ERROR) {
		(*env) -> SetIntField(env, obj, field_session_id, (jint) session);
	}

	JNI_DEBUG_ARG1("  Return -> wspCreateSession ID:%d", session)

	return WSPConnStatus(status);
}

/*
 * Class:     com_nokia_comm_wap_wps_Session
 * Method:    wspConnect
 * Signature: (ILcom/nokia/comm/wap/wps/ByteBuffer;Lcom/nokia/comm/wap/wps/ByteBuffer;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Session_wspConnect(
	JNIEnv *env, jobject obj, jint conn_id, jobject cap_obj, jobject hdr_obj)
{
	WPS_STATUS    status;
	WSP_SessionHandle sess_id;

	jbyteArray  hdr_array, cap_array;
	jbyte		*hdr_ptr = NULL;
	jbyte		*cap_ptr = NULL;
	jboolean	is_copy;
	UINT16 hdr_len = 0;
	UINT16 cap_len = 0;
	
	sess_id = (WSP_SessionHandle)(*env) -> GetIntField(env, obj, field_session_id);

	JNI_DEBUG_ARG1("Call  -> wspConnect (sessID %d)", sess_id)

	hdr_array = (jbyteArray) (*env) -> GetObjectField(env, hdr_obj, field_bytebuffer_buf);
	hdr_len = (UINT16) (*env) -> GetArrayLength(env, hdr_array);
	hdr_ptr = (*env) -> GetByteArrayElements(env, hdr_array, &is_copy);
	if (hdr_len == 0)
		hdr_ptr = NULL;

	cap_array = (jbyteArray) (*env) -> GetObjectField(env, cap_obj, field_bytebuffer_buf);
	cap_len = (UINT16) (*env) -> GetArrayLength(env, cap_array);
	cap_ptr = (*env) -> GetByteArrayElements(env, cap_array, &is_copy);
	if (cap_len == 0)
		cap_ptr = NULL;

	JNI_DEBUG_ARG1("wspConnect: hdrlen %d", hdr_len)
	JNI_DEBUG_ARG1("wspConnect: caplen %d", cap_len)
#ifdef NO_NEGOTIATION
	status = WSP_Connect(sess_id, (WPS_HCONN) conn_id, NULL, 0, NULL, 0);
#else
	status = WSP_Connect(sess_id, (WPS_HCONN) conn_id, 
		(BYTE *) cap_ptr, cap_len, 
		/*
		 * We don't do static headers yet
		 */
		NULL, 0); /* hdr_ptr, hdr_len); */
#endif
	if (hdr_ptr != NULL)
		(*env) -> ReleaseByteArrayElements(env, hdr_array, hdr_ptr, 0);
	if (cap_ptr != NULL)
		(*env) -> ReleaseByteArrayElements(env, cap_array, cap_ptr, 0);

	JNI_DEBUG_ARG1("  Return -> wspConnect: %s\n", WSPConnStatusString(status))

	return WSPConnStatus(status);
}

/*
 * Class:     com_nokia_comm_wap_wps_Session
 * Method:    wspDisconnect
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Session_wspDisconnect(
	JNIEnv *env, jobject obj, jint session_id)
{
	WPS_STATUS    status;

	JNI_DEBUG("Call   -> wspDisconnect")

	status = WSP_Disconnect((WSP_SessionHandle) session_id);

	JNI_DEBUG_ARG1("  Return -> wspDisconnect: %d\n", status)

	return WSPConnStatus(status);
}

/*
 * Class:     com_nokia_comm_wap_wps_Session
 * Method:    wspReleaseSession
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Session_wspReleaseSession(
	JNIEnv *env, jobject obj, jint session_id)
{
	WPS_STATUS    status;

	JNI_DEBUG("Call   -> wspReleaseSession")

	status = WSP_ReleaseSession((WSP_SessionHandle) session_id);

	JNI_DEBUG("  Return -> wspReleaseSession")

	return WSPConnStatus(status);
}

/*
 * Class:     com_nokia_comm_wap_wps_Session
 * Method:    wspSuspend
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Session_wspSuspend(
	JNIEnv *env, jobject obj, jint session_id)
{
	WPS_STATUS    status;

	JNI_DEBUG("Call   -> wspSuspend")

	status = WSP_SuspendSession((WSP_SessionHandle) session_id);

	JNI_DEBUG("  Return -> wspSuspend")

	return WSPConnStatus(status);
}

/*
 * Class:     com_nokia_comm_wap_wps_Session
 * Method:    wspResume
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Session_wspResume(
	JNIEnv *env, jobject obj, jint session_id, jint conn_id)
{
	WPS_STATUS    status;

	JNI_DEBUG("Call   -> wspResume")

	/*
	 * For now, we pass no new cap or static headers to the newly 
	 * resumed session.
	 */
	status = WSP_ResumeSession((WSP_SessionHandle) session_id, (WPS_HCONN) conn_id,
		NULL, 0, NULL, 0);

	JNI_DEBUG("  Return -> wspResume")

	return WSPConnStatus(status);
}

/*
 * Class:     com_nokia_comm_wap_wps_Session
 * Method:    wspSave
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Session_wspSave(
	JNIEnv *env, jobject obj, jint session_id)
{
	WPS_STATUS    status;
	UINT32 load_id;

	JNI_DEBUG("Call   -> wspSave")

	status = WSP_SaveSession((WSP_SessionHandle) session_id, &load_id);

	if (status == WPS_NO_ERROR) {
		(*env) -> SetIntField(env, obj, field_session_load_id, (jint) load_id);
	}

	JNI_DEBUG_ARG1("  Return -> wspSave: Load ID %d", load_id)
	
	return WSPConnStatus(status);
}

/*
 * Class:     com_nokia_comm_wap_wps_Session
 * Method:    wspGetConnection
 * Signature: (ILcom/nokia/comm/wap/wps/Connection;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Session_wspGetConnection(
	JNIEnv *env, jobject obj, jint session_id, jobject conn_obj)
{
	WPS_STATUS status;
	WPS_HCONN  conn;

	JNI_DEBUG("Call   -> wspGetConnection")

	status = WSP_GetConnection(session_id, &conn);

	if (status == WPS_NO_ERROR) {
		status = JNI_SetConnection(env, conn, conn_obj);
	}

	JNI_DEBUG("  Return -> wspGetConnection")

	return WSPConnStatus(status);
}

/*
 * Class:     com_nokia_comm_wap_wps_Session
 * Method:    wspGetEvent
 * Signature: (IILcom/nokia/comm/wap/wps/Event;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Session_wspGetEvent(
	JNIEnv *env, jobject obj, jint session_id, jint timeout, jobject event_obj)
{
	WPS_STATUS    status;
	WSP_Events    evt;
	WSP_ProtocolObjectHandle src;
	jobject src_obj;
	char dbgbuf[512];

	JNI_DEBUG("Call   -> wspGetEvent")

	status = WSP_GetEvent((UINT32) session_id, (UINT16) timeout, &evt, &src);

	if (status == WPS_NO_ERROR) {
		sprintf(dbgbuf, "	wspGetEvent: Got EVENT: %d\n", evt);
		JNI_DEBUG( dbgbuf )
		if (evt == WSP_EVENT_DISCONNECT_IND_S) {
			(*env) -> SetIntField(env, event_obj, field_event_type, (jint) 1);
			if ((src_obj = (*env) -> NewObject(env, class_session, method_session_constructor)) == 0) {
				JNI_DEBUG("  ERROR NewObject")
				return 0;
			}
			(*env) -> SetIntField(env, src_obj, field_session_id, (jint) src);
			(*env) -> SetObjectField(env, event_obj, field_event_session, (jobject) src_obj);
			JNI_DEBUG("  DisconnectEvent")
		}
		else if (evt == WSP_EVENT_SUSPEND_IND_S) {
			(*env) -> SetIntField(env, event_obj, field_event_type, (jint) 2);
			if ((src_obj = (*env) -> NewObject(env, class_session, method_session_constructor)) == 0) {
				JNI_DEBUG("  ERROR NewObject")
				return 0;
			}
			(*env) -> SetIntField(env, src_obj, field_session_id, (jint) src);
			(*env) -> SetObjectField(env, event_obj, field_event_session, (jobject) src_obj);
			JNI_DEBUG("  SuspendEvent")
		}
		else if (evt == WSP_EVENT_RESUME_CNF_S) {
			(*env) -> SetIntField(env, event_obj, field_event_type, (jint) 3);
			if ((src_obj = (*env) -> NewObject(env, class_session, method_session_constructor)) == 0) {
				JNI_DEBUG("  ERROR NewObject")
				return 0;
			}
			(*env) -> SetIntField(env, src_obj, field_session_id, (jint) src);
			(*env) -> SetObjectField(env, event_obj, field_event_session, (jobject) src_obj);
			JNI_DEBUG("  ResumeCNFEvent")
		}
		else if (evt == WSP_EVENT_CONNECT_CNF_S) {
			(*env) -> SetIntField(env, event_obj, field_event_type, (jint) 4);
			if ((src_obj = (*env) -> NewObject(env, class_session, method_session_constructor)) == 0) {
				JNI_DEBUG("  ERROR NewObject")
				return 0;
			}
			(*env) -> SetIntField(env, src_obj, field_session_id, (jint) src);
			(*env) -> SetObjectField(env, event_obj, field_event_session, (jobject) src_obj);
			JNI_DEBUG("  ResumeCNFEvent")
		}
		else if (evt == WSP_EVENT_REDIRECT_IND_S) {
			(*env) -> SetIntField(env, event_obj, field_event_type, (jint) 5);
			if ((src_obj = (*env) -> NewObject(env, class_session, method_session_constructor)) == 0) {
				JNI_DEBUG("  ERROR NewObject")
				return 0;
			}
			(*env) -> SetIntField(env, src_obj, field_session_id, (jint) src);
			(*env) -> SetObjectField(env, event_obj, field_event_session, (jobject) src_obj);
			JNI_DEBUG("  RedirectEvent")
		}
		else if (evt == WSP_EVENT_METHODINVOKE_CNF_T) {
			(*env) -> SetIntField(env, event_obj, field_event_type, (jint) 6);
			if ((src_obj = (*env) -> NewObject(env, class_transaction, method_transaction_constructor)) == 0) {
				JNI_DEBUG("  ERROR NewObject")
				return 0;
			}
			(*env) -> SetIntField(env, src_obj, field_transaction_id, (jint) src);
			(*env) -> SetObjectField(env, event_obj, field_event_transaction, (jobject) src_obj);
			JNI_DEBUG("  MethodInvCNFEvent")
		}
		else if (evt == WSP_EVENT_METHODRESULT_IND_T) {
			(*env) -> SetIntField(env, event_obj, field_event_type, (jint) 7);
			if ((src_obj = (*env) -> NewObject(env, class_transaction, method_transaction_constructor)) == 0) {
				JNI_DEBUG("  ERROR NewObject")
				return 0;
			}
			(*env) -> SetIntField(env, src_obj, field_transaction_id, (jint) src);
			(*env) -> SetObjectField(env, event_obj, field_event_transaction, (jobject) src_obj);
			JNI_DEBUG("  MethodInvResultEvent")
		}
		else if (evt == WSP_EVENT_ABORT_IND_T) {
			BYTE reason;
			Length reason_len = 1;
			WPS_STATUS    rstatus;
			(*env) -> SetIntField(env, event_obj, field_event_type, (jint) 8);
			if ((src_obj = (*env) -> NewObject(env, class_transaction, method_transaction_constructor)) == 0) {
				JNI_DEBUG("  ERROR NewObject")
				return 0;
			}
			(*env) -> SetIntField(env, src_obj, field_transaction_id, (jint) src);
			(*env) -> SetObjectField(env, event_obj, field_event_transaction, (jobject) src_obj);
			rstatus = WSP_GetTransactionData( (WSP_TransactionHandle) src, &reason,
				&reason_len, WSP_TRANSACTION_ABORT_REASON); 
			if (rstatus == WPS_NO_ERROR) {
				JNI_DEBUG_ARG1("  AbortEvent.  Reason: %s", WSPAbortReasonString(reason))
			} else {
				JNI_DEBUG("  AbortEvent")
			}
		}
		else if (evt == WSP_EVENT_PUSH_IND_T) {
			(*env) -> SetIntField(env, event_obj, field_event_type, (jint) 9);
			if ((src_obj = (*env) -> NewObject(env, class_transaction, method_transaction_constructor)) == 0) {
				JNI_DEBUG("  ERROR NewObject")
				return 0;
			}
			(*env) -> SetIntField(env, src_obj, field_transaction_id, (jint) src);
			(*env) -> SetObjectField(env, event_obj, field_event_transaction, (jobject) src_obj);
			JNI_DEBUG("  PushEvent")
		}
		else if (evt == WSP_EVENT_CONFIRMEDPUSH_IND_T) {
			(*env) -> SetIntField(env, event_obj, field_event_type, (jint) 10);
			if ((src_obj = (*env) -> NewObject(env, class_transaction, method_transaction_constructor)) == 0) {
				JNI_DEBUG("  ERROR NewObject")
				return 0;
			}
			(*env) -> SetIntField(env, src_obj, field_transaction_id, (jint) src);
			(*env) -> SetObjectField(env, event_obj, field_event_transaction, (jobject) src_obj);
			JNI_DEBUG("  ConfPushEvent")
		}
		else if (evt == WSP_EVENT_EXCEPTION_IND_E) {
			(*env) -> SetIntField(env, event_obj, field_event_type, (jint) 11);
			if ((src_obj = (*env) -> NewObject(env, class_exception, method_exception_constructor)) == 0) {
				JNI_DEBUG("  ERROR NewObject")
				return 0;
			}
			(*env) -> SetIntField(env, src_obj, field_exception_id, (jint) src);
			(*env) -> SetObjectField(env, event_obj, field_event_exception, (jobject) src_obj);
			JNI_DEBUG("  ExceptionEvent")
		}
		else {
			(*env) -> SetIntField(env, event_obj, field_event_type, (jint) 0);
			JNI_DEBUG("  UnknownEvent!")

		}
	} else {
		sprintf(dbgbuf, "wspGetEvent: get event failed: %d\n", status);
		JNI_DEBUG( dbgbuf )	
	}

	JNI_DEBUG_ARG1("  Return -> wspGetEvent: %d\n", status)

	return WSPConnStatus(status);
}

/*
 * Class:     com_nokia_comm_wap_wps_Session
 * Method:    wspGetSessionState
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Session_wspGetSessionState(
	JNIEnv *env, jobject obj, jint session_id)
{
	WPS_STATUS    status;
	WSP_SessionState state;

	JNI_DEBUG("Call   -> wspGetSessionState")

	status = WSP_GetSessionState((WSP_SessionHandle) session_id, &state);

	if (status == WSP_NOERROR) {
		if (state == WSP_SESSION_STATE_CONNECTING) {
			(*env) -> SetIntField(env, obj, field_session_state, (jint) 1);
		}
		else if (state == WSP_SESSION_STATE_CONNECTED) {
			(*env) -> SetIntField(env, obj, field_session_state, (jint) 2);
		}
		else if (state == WSP_SESSION_STATE_SUSPENDED) {
			(*env) -> SetIntField(env, obj, field_session_state, (jint) 3);
		}
		else if (state == WSP_SESSION_STATE_RESUMING) {
			(*env) -> SetIntField(env, obj, field_session_state, (jint) 4);
		}
		else if (state == WSP_SESSION_STATE_DISCONNECTED) {
			(*env) -> SetIntField(env, obj, field_session_state, (jint) 5);
		}
		/* WSP_SESSION_STATE_DISCONNECTING (6) is server only */

		else if (state == WSP_SESSION_STATE_REPLYING) {
			(*env) -> SetIntField(env, obj, field_session_state, (jint) 7);
		}
		else {
			(*env) -> SetIntField(env, obj, field_session_state, (jint) 0);
		}
	}

	JNI_DEBUG_ARG1("  Return -> wspGetSessionState: %d\n", status)

	return WSPConnStatus(status);
}

/*
 * Class:     com_nokia_comm_wap_wps_Session
 * Method:    wspGetSessionData
 * Signature: (IILcom/nokia/comm/wap/wps/ByteBuffer;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Session_wspGetSessionData(
	JNIEnv *env, jobject obj, jint session_id, jint selector, jobject buf_obj)
{
	WPS_STATUS status;
	BYTE   *data;
	Length data_len = 1024;	 // maximum size is small for this call
	UINT32 data_selector;
	BOOL   decode = FALSE;

	JNI_DEBUG_ARG1("Call   -> wspGetSessionData (type %d) ", (int)selector);
	JNI_DEBUG_ARG1("for sess ID %d\n", (int)session_id);
    
	switch(selector) {
	case 1:
	    data_selector = WSP_SESSION_SERVER_HEADERS;
	    decode = TRUE;
	    break;
	case 2:
	    data_selector = WSP_SESSION_CLIENT_HEADERS;
	    decode = TRUE;
	    break;
	case 3:
	    data_selector = WSP_SESSION_CAPABILITIES;
	    break;
	case 4:
	    data_selector = WSP_SESSION_REDIRECT_OPTIONS;
	    break;
	case 5:
	    data_selector = WSP_SESSION_REDIRECT_ADDRESSES;
	    break;
	case 6:
	    data_selector = WSP_SESSION_QUEUE_LEN;
	    data_len = 4;
	    break;
	case 7:
	    data_selector = WSP_SESSION_ERROR_HEADERS;
	    decode = TRUE;
	    break;
	case 8:
	    data_selector = WSP_SESSION_ERROR_BODY;
	    break;
	case 9:
	    data_selector = WSP_SESSION_DISCON_REASON;
	    break;
	case 10:
	    data_selector = WSP_SESSION_SUSPEND_REASON;
	    break;
	default:
	    return WSP_ERROR_PARAMETER_ERROR;
	}

	data = (BYTE *) malloc(data_len);
	if (data == NULL) {
		/* Out of memory */
		status = WPS_ERROR_OUT_OF_MEMORY;

	} else {
    
		status = WSP_GetSessionData((WSP_SessionHandle) session_id,
				    data, &data_len, data_selector);
		if (status == WPS_NO_ERROR) {
	    		if (decode == TRUE) {
				/* 
				 * Decoding is needed 
				 */
				BYTE   *decoded_data;
				UINT32 decoded_data_len;

				status = decodeHeader(
				    data, data_len, 0, FALSE,
				    &decoded_data, &decoded_data_len);
				if (status == WPS_NO_ERROR) {
		    			JNI_SetByteBuffer32(env, decoded_data,
						decoded_data_len, buf_obj); 
				}
				decodeHeaderFree(&decoded_data);
	    		} else {
				/* 
				 * Decoding is not needed 
				 */
				JNI_SetByteBuffer32(env, data, data_len, buf_obj);
	    		}
		}
		free(data);
	}

	JNI_DEBUG_ARG1("  Return -> wspGetSessionData (%d)\n",(int)status);

	return WSPConnStatus(status);
}

/*
 * Class:     com_nokia_comm_wap_wps_ExceptionEvent
 * Method:    wspReleaseException
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_ExceptionEvent_wspReleaseException(
	JNIEnv *env, jobject obj, jint exc) 
{
	WPS_STATUS    status;

	JNI_DEBUG("Call   -> wspReleaseException")

	status = WSP_ReleaseException((WSP_ExceptionHandle) exc);

	JNI_DEBUG("  Return -> wspReleaseException")

	return WSPConnStatus(status);
}

/*
 * Class:     com_nokia_comm_wap_wps_ExceptionEvent
 * Method:    wspGetExceptionData
 * Signature: (IILcom/nokia/comm/wap/wps/ByteBuffer;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_ExceptionEvent_wspGetExceptionData(
	JNIEnv *env, jobject obj, jint id, jint selector, jobject buf_obj) 
{
	WPS_STATUS status;
	BYTE   *data;
	UINT16 data_len = (UINT16)DATA_MAX_LENGTH;
	UINT32 data_selector;

	JNI_DEBUG("Call   -> wspGetExceptionData")
	
	switch(selector) {
		case 1:
			data_selector = WSP_EXCEPTION_DATA;
			break;
		default:
			return WSP_ERROR_PARAMETER_ERROR;
	}

	data = (BYTE *) malloc(data_len);
	data_len = (UINT16)DATA_MAX_LENGTH;
	
	status = WSP_GetExceptionData((WSP_ExceptionHandle) id, data, &data_len, data_selector);

	JNI_SetByteBuffer32(env, data, data_len, buf_obj);
	free(data);

	JNI_DEBUG("  Return -> wspGetExceptionData")

	return WSPConnStatus(status);
}

/*
 * Class:     com_nokia_comm_wap_wps_ExceptionEvent
 * Method:    wspGetExceptionOwner
 * Signature: (ILcom/nokia/comm/wap/wps/Session;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_ExceptionEvent_wspGetExceptionOwner(
	JNIEnv *env, jobject obj, jint id, jobject session_obj) 
{
	WPS_STATUS status;
	WSP_SessionHandle session;

	JNI_DEBUG("Call   -> wspGetExceptionOwner")

	status = WSP_GetExceptionOwner((WSP_ExceptionHandle) id, &session);

	if (status == WPS_NO_ERROR) {
		(*env) -> SetIntField(env, session_obj, field_session_id, (jint) session);
	}

	JNI_DEBUG("  Return -> wspGetExceptionOwner")

	return WSPConnStatus(status);
}


// ----------------------------------
//
//        CONNECTION API
//
// ----------------------------------

/*
 * Class:     comm_wap_wps_WpsSocket
 * Method:    wpsCreateConnection
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Connection_wpsCreateConnection(
	JNIEnv *env, jobject obj) 
{
	WPS_STATUS    status;
	WPS_HCONN     conn;
	WPS_HOST      raddr;
	WPS_PORT      rport, lport;
	WPS_LAYER_CFG layer_cfg;
	int           bearer_id;
	unsigned short raddr_len;
	void          *bearer = NULL;

	jbyteArray byte_array;
	jbyte *byte_ptr;
	jboolean is_copy;
	
	JNI_DEBUG("Call   -> WpsCreateConnection")

	byte_array = (jbyteArray) (*env) -> GetObjectField(env, obj, field_connection_raddr);
	raddr_len  = (int) (*env) -> GetArrayLength(env, byte_array);
	byte_ptr   = (*env) -> GetByteArrayElements(env, byte_array, &is_copy);
	raddr      = (WPS_HOST) byte_ptr;
	rport      = (WPS_PORT) (*env) -> GetIntField(env, obj, field_connection_rport);
	lport      = (WPS_PORT) (*env) -> GetIntField(env, obj, field_connection_lport);
	layer_cfg  = (WPS_LAYER_CFG) (*env) -> GetIntField(env, obj, field_connection_layer_cfg);
	bearer_id  = (int) (*env) -> GetIntField(env, obj, field_connection_bearer_id);

	/*if (bearer_id >= num_bearers) {
		return WPS_ERROR_UNKNOWN_BEARER;
	}

	switch (bearer_id) {
	case 0:
		bearer = java_bearer_adapter_factory0();
		break;
	case 1:
		bearer = java_bearer_adapter_factory1();
		break;
	case 2:
		bearer = java_bearer_adapter_factory2();
		break;
	}*/

	bearer = udp_bearer_adapter_factory();

	status = WPS_CreateConnection(
		raddr, (UINT16)raddr_len,
		rport, lport,
		bearer,
		layer_cfg, 
		&conn);

	if (status == WPS_NO_ERROR) {
		(*env) -> SetIntField(env, obj, field_connection_id, (jint) conn);
		/*
		 * If caller asked for any local port, get allocated port & set in conn obj
		 */
		if (lport == 0) {
			WPS_PORT real_port;
			UINT16 hostLenDummy = 0;
			WPS_STATUS st;
			JNI_DEBUG_CMD(printf(" Allocated local port..");)

			st = WPS_GetLocalAddress(conn, NULL, &hostLenDummy, &real_port);
			JNI_DEBUG_CMD(printf(" ..number %d\n", real_port);)
			(*env) -> SetIntField(env, obj, field_connection_lport, (jint)real_port);
		}
	}
	JNI_DEBUG_ARG1("  Return -> WpsCreateConnection: %d\n", status)
	return ConnectionApiStatus(status);
}

/*
 * Class:     comm_wap_wps_WpsServerSocket
 * Method:    wpsCreateBoundConnection
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Connection_wpsCreateBoundConnection(
	JNIEnv *env, jobject obj) 
{
	WPS_STATUS    status;
	WPS_HCONN     conn;
	WPS_PORT	  lport;
	WPS_LAYER_CFG layer_cfg;
	
	JNI_DEBUG("Call   -> WpsCreateBoundConnection")

	lport     = (WPS_PORT) (*env) -> GetIntField(env, obj, field_connection_lport);
	layer_cfg = (WPS_LAYER_CFG) (*env) -> GetIntField(env, obj, field_connection_layer_cfg);

	status = WPS_CreateBoundConnection(lport, layer_cfg, &conn);

	if (status == WPS_NO_ERROR) {
		(*env) -> SetIntField(env, obj, field_connection_id, (jint) conn);
	}
	JNI_DEBUG_CMD(printf("  Return -> WpsCreateBoundConnection (status=%d) %d on port %d\n", 
		status, conn, lport);)
	return ConnectionApiStatus(status);
}

/*
 * Class:     comm_wap_wps_Connection
 * Method:    wpsReleaseConnection
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Connection_wpsReleaseConnection(
	JNIEnv *env, jobject obj, jint conn) 
{
	WPS_STATUS    status;

	JNI_DEBUG("Call   -> WpsReleaseConnection")

	status = WPS_ReleaseConnection((WPS_HCONN) conn);

	JNI_DEBUG("  Return -> WpsReleaseConnection")

	return ConnectionApiStatus(status);
}

/*
 * Class:     comm_wap_wps_Connection
 * Method:    wpsConnectionEqual
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_comm_wap_wps_Connection_wpsConnectionEqual(
	JNIEnv *env, jobject obj, jint conn1, jint conn2) 
{
	BOOL status;

	JNI_DEBUG("Call   -> WpsConnectionEqual")

	status = WPS_ConnectionEqual((WPS_HCONN) conn1, (WPS_HCONN) conn2);

	JNI_DEBUG("  Return -> WpsConnectionEqual")

	return ConnectionApiStatus(status);
}

/*
 * Class:     comm_wap_wps_Connection
 * Method:    wpsIsValidConnection
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_comm_wap_wps_Connection_wpsIsValidConnection(
	JNIEnv *env, jobject obj, jint conn) 
{
	BOOL status;

	JNI_DEBUG("Call   -> WpsIsValidConnection")

	status = WPS_IsValidConnection((WPS_HCONN) conn);

	JNI_DEBUG("  Return -> WpsIsValidConnection")

	return ConnectionApiStatus(status);
}

/*
 * Class:     comm_wap_wps_Connection
 * Method:    wspUnitInvoke
 * Signature: (Lcomm/wap/wps/WspUnitInvokePacket;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Connection_wspUnitInvoke(
		JNIEnv *env, 
		jobject obj, 	/* Connection object */
		jobject p)	/* InvokePacket */ 
{	
	WPS_STATUS  c_status;

	WPS_HCONN	conn;
	UINT8		method;
	UINT32		uri_len;
	UINT32		body_len;
	UINT32		headers_len;
	UINT8		trans_id;
#ifdef DO_HDR_ENCODING
	BYTE*		encoded_hdr;
	UINT32		encoded_hdr_len;
	BOOL		haveContentTypeHdr = FALSE;
#endif

	jbyteArray	uri_array;
	jbyteArray  body_array;
	jbyteArray  headers_array;
	jbyte		*uri_ptr = 0;
	jbyte		*body_ptr = 0;
	jbyte		*headers_ptr = 0;
	jboolean	is_copy;
	
	JNI_DEBUG("Call   -> WpsUnitInvoke")

	conn = (WPS_HCONN) (*env) -> GetIntField(env, obj, field_connection_id);
	method = (UINT8) (*env) -> GetIntField(env, p, field_unit_invoke_packet_method);

	uri_array = (jbyteArray) (*env) -> GetObjectField(env, p, field_unit_invoke_packet_uri);
	uri_len = (int) (*env) -> GetArrayLength(env, uri_array);
	uri_ptr = (*env) -> GetByteArrayElements(env, uri_array, &is_copy);

	body_array = (jbyteArray) (*env) -> GetObjectField(env, p, field_unit_invoke_packet_body);
	body_len = (int) (*env) -> GetArrayLength(env, body_array);
	if (body_len)
		body_ptr = (*env) -> GetByteArrayElements(env, body_array, &is_copy);

	headers_array = (jbyteArray) (*env) -> GetObjectField(env, p, field_unit_invoke_packet_headers);
	headers_len = (int) (*env) -> GetArrayLength(env, headers_array);
	if (headers_len)
		headers_ptr = (*env) -> GetByteArrayElements(env, headers_array, &is_copy);

	trans_id = (UINT8) (*env) -> GetIntField(env, p, field_unit_invoke_packet_trans_id);

	if (method == WSP_METHOD_GET) {
		body_ptr = 0;	/* checked for in stack */
		body_len = 0;
	}

#ifdef DO_HDR_ENCODING
	/*
	 * Encode headers
	 */
	if (useCodec && headers_len != 0) {
		if (method == WSP_METHOD_POST)
			haveContentTypeHdr = TRUE;
		c_status = encodeHeader((BYTE*) headers_ptr, headers_len, haveContentTypeHdr, &encoded_hdr,
                                 &encoded_hdr_len);
			
		if (c_status == WPS_NO_ERROR) {
			c_status = WSP_UnitInvoke(
				conn,
				method,
				(BYTE *)uri_ptr, uri_len,
				(BYTE *) body_ptr, body_len,
				(BYTE *) encoded_hdr_len ==  0 ? 0 : encoded_hdr, 
				(UINT16)encoded_hdr_len,
				trans_id);
		}
		encodeHeaderFree(&encoded_hdr);
	} else {
		c_status = WSP_UnitInvoke(
			conn, 
			method,
			(BYTE *) uri_ptr, uri_len,
			(BYTE *) body_ptr, body_len,
			(BYTE *) headers_len ==  0 ? 0 : headers_ptr, headers_len,
			trans_id);
	}
#else /* OLD WAY */
	c_status = WSP_UnitInvoke(
		conn, 
		method,
		(BYTE *) uri_ptr, uri_len,
		(BYTE *) body_ptr, body_len,
		(BYTE *) headers_len ==  0 ? 0 : headers_ptr, headers_len,
		trans_id);
#endif /* DO_HDR_ENCODING */

	(*env) -> ReleaseByteArrayElements(env, uri_array, uri_ptr, 0);
	if (body_ptr)
		(*env) -> ReleaseByteArrayElements(env, body_array, body_ptr, 0);
	if (headers_ptr)
		(*env) -> ReleaseByteArrayElements(env, headers_array, headers_ptr, 0);

	JNI_DEBUG_ARG1("  Return -> WpsUnitInvoke %d", c_status)
	return WSPClessStatus(c_status);
}

/*
 * Class:     comm_wap_wps_Connection
 * Method:    wspUnitWaitResult
 * Signature: (Lcomm/wap/wps/WspUnitResultPacket;I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Connection_wspUnitWaitResult(
	JNIEnv *env, jobject obj, jobject p, jint timeout) 
{

	WPS_STATUS      c_status, d_status = WPS_CODEC_STATUS_BASE;
	WPS_HCONN	boundConn;
	WPS_HCONN	conn;
	BYTE		*body;
	UINT32		body_len;
	BYTE		*headers;
	UINT32		headers_len;
	INT32		c_timeout;
	UINT8		status;
	UINT8		trans_id;
	BYTE		*final_hdrs;
#ifdef DO_HDR_ENCODING
	BYTE		*encoded_hdr = NULL;
	UINT32		encoded_hdr_len = 0;
#endif

	jobject		connection;
	jbyteArray  byte_array;
	jbyte		*byte_ptr;
	jboolean	is_copy;
	UINT32		i;
	char dbug_buf[256];

	JNI_DEBUG("Call   -> WspUnitWaitResult")

	boundConn	= (WPS_HCONN) (*env) -> GetIntField(env, obj, field_connection_id);
	
	body_len	= BODY_MAX_LENGTH;
	body		= (BYTE *) malloc(BODY_MAX_LENGTH);
	headers_len	= HEADERS_MAX_LENGTH;
	headers		= (BYTE *) malloc(HEADERS_MAX_LENGTH);

	if (timeout == -1) {
		c_timeout = WSP_INFINITE_WAIT;
	}
	else {
		c_timeout = (INT32) timeout;
	}

	JNI_DEBUG_CMD(printf(" WSPWaitRes called with bc=%d bufAddr=%lx bodyLen=%d hdrLen=%d\n",
		boundConn, (long)body, body_len, headers_len);)

	c_status = WSP_UnitWaitResult(
		boundConn, &conn,
		body, &body_len,
		headers, &headers_len,
		c_timeout, 
		&status, &trans_id);

	if (c_status == WSP_NOERROR) {
		final_hdrs = headers;

#ifdef DO_HDR_ENCODING
		/*
		 * Decode headers.  Responses always have Content-Type headers.
		 */
		if (useCodec && headers_len != 0) {
			d_status = decodeHeader((BYTE*) headers, headers_len, 
				body_len, TRUE, &encoded_hdr, &encoded_hdr_len);
		} 
		if (d_status == WSP_NOERROR) {
			JNI_DEBUG_CMD(printf("WspWaitResult: Decoded Headers OK (%d bytes)\n", 
				headers_len);)
			headers_len = (UINT16)encoded_hdr_len;
			final_hdrs = encoded_hdr;
		} else {
			JNI_DEBUG("WspWaitResult: Decoding Headers failed!\n")
		}
#endif /* DO_HDR_ENCODING */

		if (body_len)
			sprintf(dbug_buf, "WspUnitWaitResult: Got msg. Len = %d body [%x %x %x %x]\n", 
				body_len, body[0], body[1], body[2], body[3]);
		else
			sprintf(dbug_buf, "WspUnitWaitResult: Got msg. Len = 0\n");
		JNI_DEBUG(dbug_buf)
		if (headers_len) {
			JNI_DEBUG_CMD(printf("WspUnitWaitResult: HdrLen = %d Hdrs [%x %x]\n", 
				headers_len, headers[0], headers[1]);)
		}

		(*env) -> SetIntField(env, p, field_unit_result_packet_status, (jint) status);
		(*env) -> SetIntField(env, p, field_unit_result_packet_trans_id, (jint) trans_id);

		byte_array = (*env) -> NewByteArray(env, body_len);
		byte_ptr = (*env) -> GetByteArrayElements(env, byte_array, &is_copy);
		for(i = 0; i < body_len; i++) {
			*(byte_ptr + i) = *(body + i);
		}

		free(body);
		(*env) -> ReleaseByteArrayElements(env, byte_array, byte_ptr, 0);
		(*env) -> SetObjectField(env, p, field_unit_result_packet_body, (jobject) byte_array);

		byte_array = (*env) -> NewByteArray(env, headers_len);
		byte_ptr = (*env) -> GetByteArrayElements(env, byte_array, &is_copy);
		for(i = 0; i < headers_len; i++) {
			*(byte_ptr + i) = *(final_hdrs + i);
		}

		free(headers);
		(*env) -> ReleaseByteArrayElements(env, byte_array, byte_ptr, 0);
		(*env) -> SetObjectField(env, p, field_unit_result_packet_headers, (jobject) byte_array);
#ifdef DO_HDR_ENCODING
		if (encoded_hdr)
			encodeHeaderFree(&encoded_hdr);
#endif /* DO_HDR_ENCODING */
		connection = (jobject) (*env) -> GetObjectField(env, p, field_unit_result_packet_conn);
		JNI_SetConnection(env, conn, connection);
	} else {
		sprintf(dbug_buf, "WspUnitWaitResult: Bad return %d\n", c_status);
		JNI_DEBUG(dbug_buf)
	}
	JNI_DEBUG_ARG1("  Return -> WspUnitWaitResult (got %d hdr + body bytes)\n", body_len + headers_len)
	return WSPClessStatus(c_status);
}

/*
 * Class:     comm_wap_wps_WpsServerSocket
 * Method:    wspUnitWaitPush
 * Signature: (Lcomm/wap/wps/WspUnitPushPacket;I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Connection_wspUnitWaitPush(
	JNIEnv *env, jobject obj, jobject p, jint timeout) 
{
	WPS_STATUS  c_status;

	WPS_HCONN	boundConn;
	WPS_HCONN	conn;
	BYTE		*body;
	UINT32		body_len;
	BYTE		*headers;
	UINT32		headers_len;
	INT32		c_timeout;
	UINT8		push_id;

	jobject		connection;
	jbyteArray  byte_array;
	jbyte		*byte_ptr;
	jboolean	is_copy;
	UINT32		i;
	BYTE		*final_hdrs;
#ifdef DO_HDR_ENCODING
	WPS_STATUS	d_status = WPS_CODEC_STATUS_BASE;
	BYTE		*encoded_hdr = NULL;
	UINT32		encoded_hdr_len = 0;
#endif

	JNI_DEBUG("Call   -> WspUnitWaitPush")

	boundConn	= (WPS_HCONN) (*env) -> GetIntField(env, obj, field_connection_id);
	
	body_len	= BODY_MAX_LENGTH;
	body		= (BYTE *) malloc(BODY_MAX_LENGTH);

	headers_len = HEADERS_MAX_LENGTH;
	headers		= (BYTE *) malloc(HEADERS_MAX_LENGTH);

	if (timeout == -1) {
		c_timeout = WSP_INFINITE_WAIT;
	}
	else {
		c_timeout = (INT32) timeout;
	}

	c_status = WSP_UnitWaitPush(
		boundConn, &conn,
		body, &body_len,
		headers, &headers_len,
		c_timeout, 
		&push_id);

	if (c_status == WSP_NOERROR) {
		(*env) -> SetIntField(env, p, field_unit_push_packet_push_id, (jint) push_id);

		final_hdrs = headers;
#ifdef DO_HDR_ENCODING
		/*
		 * Decode headers.  Push messages always have C-Type headers.
		 */
		if (useCodec && headers_len != 0) {
			d_status = decodeHeader((BYTE*) headers, headers_len, 
				body_len, TRUE, &encoded_hdr, &encoded_hdr_len);
		} 
		if (d_status == WSP_NOERROR) {
			JNI_DEBUG_CMD(printf("WspUnitWaitPush: Decoded Headers OK (%d bytes)\n", 
				headers_len);)
			headers_len = (UINT16)encoded_hdr_len;
			final_hdrs = encoded_hdr;
		} else {
			JNI_DEBUG("WspUnitWaitPush: Decoding Headers failed!\n")
		}
#endif /* DO_HDR_ENCODING */
		
		byte_array = (*env) -> NewByteArray(env, body_len);
		byte_ptr = (*env) -> GetByteArrayElements(env, byte_array, &is_copy);
		for(i = 0; i < body_len; i++) {
			*(byte_ptr + i) = *(body + i);
		}

		free(body);
		(*env) -> ReleaseByteArrayElements(env, byte_array, byte_ptr, 0);
		(*env) -> SetObjectField(env, p, field_unit_push_packet_body, (jobject) byte_array);


		byte_array = (*env) -> NewByteArray(env, headers_len);
		byte_ptr = (*env) -> GetByteArrayElements(env, byte_array, &is_copy);
		for(i = 0; i < headers_len; i++) {
			*(byte_ptr + i) = *(final_hdrs + i);
		}

		free(headers);
		(*env) -> ReleaseByteArrayElements(env, byte_array, byte_ptr, 0);
		(*env) -> SetObjectField(env, p, field_unit_push_packet_headers, (jobject) byte_array);
		

		connection = (jobject) (*env) -> GetObjectField(env, p, field_unit_push_packet_conn);
		JNI_SetConnection(env, conn, connection);
	}

	JNI_DEBUG("  Return -> WspUnitWaitPush")

	return WSPClessStatus(c_status);
}
  

/*
 * Local Variables:
 * mode:C
 * c-basic-offset:4
 * End:
 */
