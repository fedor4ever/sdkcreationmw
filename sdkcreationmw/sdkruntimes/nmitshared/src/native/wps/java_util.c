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
 * Generic routines used by all client JNI code.
 */

#include "java_util.h"

extern jclass class_bytebuffer;
extern jmethodID method_bytebuffer_constructor;
extern jfieldID field_bytebuffer_buf;
extern jfieldID field_bytebuffer_len;
extern jfieldID field_bytebuffer_max_len;

WPS_STATUS JNI_GetByteBuffer(JNIEnv *env, BYTE *dst, UINT16 *len, UINT16 max_len, 
	jobject byte_buffer) 
{
	jbyteArray  bytebuffer_buf_array;
	jbyte		*bytebuffer_buf_ptr;
	jsize		bytebuffer_buf_len;
	jboolean	is_copy;
	int			i;

	bytebuffer_buf_array = (*env) -> GetObjectField(env, byte_buffer, field_bytebuffer_buf);
	bytebuffer_buf_ptr = (*env) -> GetByteArrayElements(env, bytebuffer_buf_array, &is_copy);
	bytebuffer_buf_len = (*env) -> GetIntField(env, byte_buffer, field_bytebuffer_len);	
	for(i = 0; i < bytebuffer_buf_len && i < max_len; i++) {
		*(dst + i) = *(bytebuffer_buf_ptr + i);
	}
	*len = (UINT16) bytebuffer_buf_len;

	(*env) -> ReleaseByteArrayElements(env, bytebuffer_buf_array, bytebuffer_buf_ptr, 0);

	return WPS_NO_ERROR;
}

WPS_STATUS JNI_SetByteBuffer(JNIEnv *env, BYTE *src, UINT16 len, jobject bytebuffer) {
	jbyteArray  bytebuffer_buf_array;
	jbyte		*bytebuffer_buf_ptr;
	jboolean	is_copy;
	int			i;

	bytebuffer_buf_array = (*env) -> NewByteArray(env, len);
	bytebuffer_buf_ptr = (*env) -> GetByteArrayElements(env, bytebuffer_buf_array, &is_copy);
	for(i = 0; i < len; i++) {
		*(bytebuffer_buf_ptr + i) = *(src + i);
	}

	(*env) -> ReleaseByteArrayElements(env, bytebuffer_buf_array, bytebuffer_buf_ptr, 0);

	(*env) -> SetObjectField(env, bytebuffer, field_bytebuffer_buf, (jobject) bytebuffer_buf_array);
	(*env) -> SetIntField(env, bytebuffer, field_bytebuffer_len, (jint) len);

	return WPS_NO_ERROR;
}

WPS_STATUS JNI_GetByteBuffer32(JNIEnv *env, BYTE *dst, UINT32 *len, UINT32 max_len, 
	jobject byte_buffer) 
{
	jbyteArray  bytebuffer_buf_array;
	jbyte		*bytebuffer_buf_ptr;
	jsize		bytebuffer_buf_len;
	jboolean	is_copy;
	int i, l, m;

	bytebuffer_buf_array = (*env) -> GetObjectField(env, byte_buffer, field_bytebuffer_buf);
	bytebuffer_buf_ptr = (*env) -> GetByteArrayElements(env, bytebuffer_buf_array, &is_copy);
	bytebuffer_buf_len = (*env) -> GetIntField(env, byte_buffer, field_bytebuffer_len);	
	l = (int) bytebuffer_buf_len ;
	m = (int) max_len;
	for(i = 0; i < l && i < m; i++) {
		*(dst + i) = *(bytebuffer_buf_ptr + i);
	}
	*len = (UINT16) bytebuffer_buf_len;

	(*env) -> ReleaseByteArrayElements(env, bytebuffer_buf_array, bytebuffer_buf_ptr, 0);

	return WPS_NO_ERROR;
}

WPS_STATUS JNI_SetByteBuffer32(JNIEnv *env, BYTE *src, UINT32 len, jobject bytebuffer) {
	jbyteArray  bytebuffer_buf_array;
	jbyte		*bytebuffer_buf_ptr;
	jboolean	is_copy;
	unsigned int i;

	bytebuffer_buf_array = (*env) -> NewByteArray(env, len);
	bytebuffer_buf_ptr = (*env) -> GetByteArrayElements(env, bytebuffer_buf_array, &is_copy);
	for(i = 0; i < len; i++) {
		*(bytebuffer_buf_ptr + i) = *(src + i);
	}

	(*env) -> ReleaseByteArrayElements(env, bytebuffer_buf_array, bytebuffer_buf_ptr, 0);

	(*env) -> SetObjectField(env, bytebuffer, field_bytebuffer_buf, (jobject) bytebuffer_buf_array);
	(*env) -> SetIntField(env, bytebuffer, field_bytebuffer_len, (jint) len);

	return WPS_NO_ERROR;
}

/* 
 * Function   : StartupClassEnvironment
 *
 * Parameters : - env: java environment
 *              - object: service library object
 * Returns    : - 0 on success,
 *              - 1 on failure
 */

jint StartupClassEnvironment(JNIEnv *env,
	CLASSINIT* classArray, int classSize,
	METHODINIT* methodArray, int methodSize,
	METHODINIT* stMethodArray, int stMethodSize,
	FIELDINIT*  fieldArray, int fieldSize,
	FIELDINIT*  stFieldArray, int stFieldSize)
{
	int i;

	/* 
	 * Initialize classes 
	 */
	for (i = 0; i < classSize; i++) {
		jclass tmp;
        
		if ((tmp = (*env)->FindClass(env, classArray[i].name)) == NULL)
		{
			fprintf(stderr, "  ERROR FindClass %s",
				classArray[i].name);
			return 1;
		}
		if ((*classArray[i].ptr = (*env)->NewGlobalRef(env, tmp)) == NULL)
		{
			fprintf(stderr, "  ERROR NewGlobalRef %s",
			classArray[i].name);
			return 1;
		}
	}

	/* 
	 * Initialize methods 
	 */
	for (i = 0; i < methodSize; i++) {
		if ((*methodArray[i].ptr = 
			(*env)->GetMethodID(env,
			 *methodArray[i].classPtr,
	 		methodArray[i].name,
	 		methodArray[i].signature)) == NULL)
		{
			fprintf(stderr, "  ERROR GetMethodID %s",
				methodArray[i].name);
			return 1;
		}
	}

	/* 
	 * Initialize fields 
	 */
	for (i = 0; i < fieldSize; i++) {
		if ((*fieldArray[i].ptr = 
			(*env)->GetFieldID(env,
			*fieldArray[i].classPtr,
			fieldArray[i].name,
			fieldArray[i].signature)) == NULL)
		{
			fprintf(stderr, "  ERROR GetFieldID %s",
				fieldArray[i].name);
			return 1;
		}
	}

	/* 
	 * Initialize static methods 
	 */
	for (i = 0; i < stMethodSize; i++) {
		if ((*stMethodArray[i].ptr = 
			(*env)->GetStaticMethodID(env,
	       		*stMethodArray[i].classPtr,
	       		stMethodArray[i].name,
	       		stMethodArray[i].signature)) == NULL)
		{
			fprintf(stderr, "  ERROR GetStaticMethodID %s",
				stMethodArray[i].name);
			return 1;
		}
	}

	/* 
	 * Initialize static fields 
	 */
	for (i = 0; i < stFieldSize; i++) {
		if ((*stFieldArray[i].ptr = 
			(*env)->GetFieldID(env,
			*stFieldArray[i].classPtr,
			stFieldArray[i].name,
			stFieldArray[i].signature)) == NULL)
		{
			fprintf(stderr, "  ERROR GetStaticFieldID %s",
				stFieldArray[i].name);
			return 1;
		}
	}
	return 0;
}


/* 
 * Function   : ShutdownClassEnvironment
 *
 * Parameters : - env: java environment
 *
 * Returns    : - 0 on success,
 * - 1 on failure
 * 
 */

jint ShutdownClassEnvironment(
	JNIEnv *env,
	CLASSINIT* classArray, 
	int classSize)
{
	int i;

	for (i = 0; i < classSize; i++) {
		(*env) -> DeleteGlobalRef(env, *classArray[i].ptr);
		*classArray[i].ptr = NULL;
	}
	return 0;
}

