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
 * JNI Interface to C Header Coder/Decoder. 
 * Implements the native methods called from JNI.
 *
 */

#include <s_lib.h>
#include <hcodec.h>                             /* Header Codec API header */
#include "java_util.h"                          /* JDK public headers */
#include "com_nokia_comm_wap_wps_HeaderCodec.h" /* javah -jni generated file */

JavaVM *jvm;

static jclass class_headercodec;
static jfieldID field_headers_in;;
static jfieldID field_headers_out;
static jfieldID field_buf_out_len;

/*
 * Some assumptions we make:
 *
 * When the content length
 */
#define DEFAULT_CONTENT_LEN 16

/*
 * Translate hc_api error codes to index
 * into error messages in java error message array
 * (See HeaderCodec.java)
 *
 * Encoder return values
 */
int HdrEncApiStatus(HC_STATUS s) {
	switch((int) s) {
	case WPS_HC_STATUS_OK:
		return 1;
	case WPS_HC_ERROR_HTTP_ILLEGAL_NEWLINE :
		return 2;
	case WPS_HC_ERROR_HTTP_NO_FIELD_SEPARATOR:
		return 3;
	case WPS_HC_ERROR_HTTP_BAD_Q_VALUE:
		return 4;
	case WPS_HC_ERROR_HTTP_BAD_DATE:
		return 5;
	case WPS_HC_ERROR_HTTP_BAD_MONTH:
		return 6;
	case WPS_HC_ERROR_HTTP_INVALID_DATE:
		return 7;
	case WPS_HC_ERROR_HTTP_UNKNOWN_METHOD:
		return 8;
	case WPS_HC_ERROR_HTTP_UNKNOWN_CONTENT_DISP:
		return 9;
	case WPS_HC_ERROR_HTTP_BAD_CONTENT_RANGE:
		return 10;
	case WPS_HC_ERROR_HTTP_BAD_RANGE:
		return 11;
	case WPS_HC_ERROR_HTTP_BAD_WARNING:
		return 12;
	case WPS_CODEC_ERROR_INT_BUFFER_TOO_SMALL:
		return 13;
	case WPS_CODEC_ERROR_DEST_BUFFER_TOO_SMALL:
		return 14;
	}
	return 0;
}

/*
 * Decoder return values
 */
int HdrDecApiStatus(HC_STATUS s) {
	switch((int) s) {
	case WPS_HC_STATUS_OK:
		return 1;
	case  WPS_HC_ERROR_WSP_INVALID_FIELD_CODE:
		return 2;
	case  WPS_HC_ERROR_WSP_ILLEGAL_CODEPAGE_SHIFT:
		return 3;
	case  WPS_HC_ERROR_WSP_INVALID_CONTENT_ENC:
		return 4;
	case  WPS_HC_ERROR_WSP_INVALID_ACCEPT_RANGE:
		return 5;
	case  WPS_HC_ERROR_WSP_INVALID_CONNECTION:
		return 6;
	case  WPS_HC_ERROR_WSP_INVALID_TRANSFER_ENC:
		return 7;
	case  WPS_HC_ERROR_WSP_INVALID_Q_CODE:
		return 8;
	case  WPS_HC_ERROR_WSP_INVALID_TIME_CODE:
		return 9;
	case  WPS_HC_ERROR_WSP_UNKNOWN_LANG_CODE:
		return 10;
	case  WPS_HC_ERROR_WSP_UNKNOWN_CNT_TYPE_CODE:
		return 11;
	case  WPS_HC_ERROR_WSP_UNKNOWN_CHARSET_CODE:
		return 12;
	case  WPS_HC_ERROR_WSP_UNKNOWN_METHOD_CODE:
		return 13;
	case  WPS_HC_ERROR_WSP_UNKNOWN_CACHE_CTL_CODE:
		return 14;
	case  WPS_HC_ERROR_WSP_UNKNOWN_CONTENT_DISP:
		return 15;
	case  WPS_HC_ERROR_WSP_UNKNOWN_RANGE_CODE:
		return 16;
	case  WPS_HC_ERROR_WSP_INTEGER_EXPECTED:
		return 17;
	case  WPS_HC_ERROR_WSP_LENGTH_EXPECTED:
		return 18;
	case  WPS_HC_ERROR_WSP_INCORRECT_VALUE_LEN:
		return 19;
	case  WPS_HC_ERROR_WSP_EMPTY_TEXT:
		return 20;
	case  WPS_HC_ERROR_WSP_DATA_OVERFLOW:
		return 21;
	case  WPS_HC_ERROR_WSP_INCORRECT_MD5_LEN:
		return 22;
	case WPS_CODEC_ERROR_INT_BUFFER_TOO_SMALL:
		return 24;;
	case WPS_CODEC_ERROR_DEST_BUFFER_TOO_SMALL:
		return 25;
	}
	return 0;
}

static int hc_init_done = 0;

/*
 * Initialize class variables
 * Return: 0 on failure, 1 on success.
 */
int 
hc_init(JNIEnv *env)
{
	jclass tmp_class;

	JNI_DEBUG("Class initializing HeaderCodec")

	if (((*env) -> GetJavaVM(env, &jvm)) != 0) {
		JNI_DEBUG("  ERROR getJavaVM")
		return 0;
	}

	if ((tmp_class = (*env) -> FindClass(env, "com/nokia/comm/wap/wps/HeaderCodec")) == 0) {
		JNI_DEBUG("ERROR: Class not found")
		return 0;
	}
	if ((class_headercodec = (*env) -> NewGlobalRef(env, tmp_class)) == 0) {
		JNI_DEBUG("ERROR: Cannot create class global reference")
		return 0;
	}

	JNI_DEBUG("  Getting object fields")
	field_headers_in = (*env) -> GetFieldID(env, class_headercodec, "m_headers_in", "[B");
	field_headers_out = (*env) -> GetFieldID(env, class_headercodec, "m_headers_out", "[B");
	field_buf_out_len = (*env) -> GetFieldID(env, class_headercodec, "m_buf_out_len", "I");
	if (field_headers_in == NULL || 
	    field_headers_out == NULL || 
	    field_buf_out_len == NULL) {
		JNI_DEBUG("	ERROR: Field Not Found");
		return 0;
	}
	hc_init_done = 1;
	return 1;
}

/*
 * Class:     com_nokia_comm_wap_wps_HeaderCodec
 * Method:    cEncode
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_HeaderCodec_cEncode(
	JNIEnv *env, 
	jobject obj,
	jint flags) 
{
	jbyteArray	inbuf_array;
	jbyteArray	outbuf_array;
	jbyte		*inbuf_ptr;
	jbyte		*outbuf_ptr;
	UINT32          inbuf_len;
	UINT32          outbuf_len;
	jboolean	is_copy;
	HC_STATUS st;
	char dbg_buf[256];

	if (!hc_init_done) {
		if (!hc_init(env))
			return 0;
	}
	
	JNI_DEBUG("Call   -> cEncode");

	inbuf_array = (*env) -> GetObjectField(env, obj, field_headers_in);
	inbuf_len = (UINT32) (*env) -> GetArrayLength(env, inbuf_array);
	inbuf_ptr = (*env) -> GetByteArrayElements(env, inbuf_array, &is_copy);

	outbuf_array = (*env) -> GetObjectField(env, obj, field_headers_out);
	outbuf_len = (UINT32) (*env) -> GetArrayLength(env, outbuf_array);
	outbuf_ptr = (*env) -> GetByteArrayElements(env, outbuf_array, &is_copy);

	JNI_DEBUG_CMD(printf("Call   -> cEncode of %d bytes To %d byte buf:", inbuf_len, outbuf_len);)
	JNI_DEBUG_CMD(printf("%s\n", inbuf_ptr);)

	st = HC_EncodeHeader((UCHAR *) inbuf_ptr, 0, 0, (HC_BITF)flags, 
         HC_WapVersion1_2, outbuf_ptr, &outbuf_len);

	if (st == WPS_HC_STATUS_OK || WPS_HC_STATUS_IS_WARNING(st)) {
		sprintf(dbg_buf, "	Encoded %d bytes OK\n", outbuf_len);
		JNI_DEBUG( dbg_buf );
	} else {
		outbuf_len = 0;
		sprintf(dbg_buf, "	HC_EncHdr error status: %d", st);
		JNI_DEBUG( dbg_buf );
	}
	(*env) -> SetIntField(env, obj, field_buf_out_len, outbuf_len);
	(*env) -> ReleaseByteArrayElements(env, inbuf_array, inbuf_ptr, 0);
	(*env) -> ReleaseByteArrayElements(env, outbuf_array, outbuf_ptr, 0);
		
	JNI_DEBUG("  Return -> cEncode");

	return HdrEncApiStatus(st);
}
/*
 * Class:     com_nokia_comm_wap_wps_HeaderCodec
 * Method:    cDecode
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_HeaderCodec_cDecode(
	JNIEnv *env, 
	jobject obj,
	jint flags) 
{
	jbyteArray	inbuf_array;
	jbyteArray	outbuf_array;
	jbyte		*inbuf_ptr;
	jbyte		*outbuf_ptr;
	UINT32          inbuf_len;
	UINT32          outbuf_len;
	jboolean	is_copy;
	HC_STATUS st;
	char dbg_buf[1024];
	UINT32 i;

#define DEFAULT_CONTENT_LEN 16
	
	if (!hc_init_done) {
		if (!hc_init(env))
			return 0;
	}
	

	inbuf_array = (*env) -> GetObjectField(env, obj, field_headers_in);
	inbuf_len = (UINT32) (*env) -> GetArrayLength(env, inbuf_array);
	inbuf_ptr = (*env) -> GetByteArrayElements(env, inbuf_array, &is_copy);

	outbuf_array = (*env) -> GetObjectField(env, obj, field_headers_out);
	outbuf_len = (UINT32) (*env) -> GetArrayLength(env, outbuf_array);
	outbuf_ptr = (*env) -> GetByteArrayElements(env, outbuf_array, &is_copy);

	JNI_DEBUG_CMD(printf("Call   -> cDecode of %d bytes To %d byte buf", inbuf_len, outbuf_len);)
	for (i = 0; i < outbuf_len; i++)
		*(outbuf_ptr + i) = 0;

	st = HC_DecodeHeader((UCHAR *)inbuf_ptr, inbuf_len, DEFAULT_CONTENT_LEN, 0, 0,
		(HC_BITF)flags, HC_WapVersionLatest, outbuf_ptr, &outbuf_len);


	if (st == WPS_HC_STATUS_OK || WPS_HC_STATUS_IS_WARNING(st)) {
		JNI_DEBUG_CMD(printf("Decoded to %d bytes OK\n", outbuf_len);)
		JNI_DEBUG_CMD(printf("%s\n", outbuf_ptr);)
	} else {
		sprintf(dbg_buf, "	HC_DecHdr error status: %d", st);
		JNI_DEBUG( dbg_buf );
		outbuf_len = 0;
	}
	(*env) -> SetIntField(env, obj, field_buf_out_len, outbuf_len);
	(*env) -> ReleaseByteArrayElements(env, inbuf_array, inbuf_ptr, 0);
	(*env) -> ReleaseByteArrayElements(env, outbuf_array, outbuf_ptr, 0);
		
	JNI_DEBUG("  Return -> cDecode");

	return HdrDecApiStatus(st);
}

