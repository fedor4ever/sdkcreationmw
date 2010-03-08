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
 * WDP bearer adapter for Java. Uses Java JNI to call java methods.
 *
 * NB: bearer adapter include files are devided into two parts: 
 *
 * 1) private include file and 
 * 2) public include file that can be distributed. 
 * In public include file only the bearer adapter factory should be shown.
 */

#include "java_wps_client.h"

#define JNI_MAX_NUM_BEARERS 3

#define JNI_DEBUG_JVM(x) 
#define JNI_DEBUG_JVM_CMD(x)

/* Implementation for java WPS bearer */

static jobject bearer_object[JNI_MAX_NUM_BEARERS];
static int is_initialized = 0;
int num_bearers = 0;

extern jclass bearer_class;
extern jmethodID bearer_open_method;
extern jmethodID bearer_close_method;
extern jmethodID bearer_close_all_method;
extern jmethodID bearer_receive_buffer_method;
extern jmethodID bearer_send_buffer_method;
extern jmethodID bearer_number_of_open_ports_method;
extern jmethodID bearer_init_method;
extern jmethodID bearer_get_default_method;
extern jmethodID bearer_get_local_host_name_method;
extern jmethodID bearer_shutdown_method;
extern jfieldID bearer_id_field;

extern jclass bearermessage_class;
extern jmethodID bearermessage_constructor;
extern jfieldID bearermessage_buf_field;
extern jfieldID bearermessage_buflen_field;
extern jfieldID bearermessage_lport_field;
extern jfieldID bearermessage_laddr_field;
extern jfieldID bearermessage_laddrlen_field;
extern jfieldID bearermessage_rport_field;
extern jfieldID bearermessage_raddr_field;
extern jfieldID bearermessage_raddrlen_field;
extern jfieldID bearermessage_type_field;

extern JavaVM *jvm;

extern jclass class_bytebuffer;
extern jmethodID method_bytebuffer_constructor;
extern jfieldID field_bytebuffer_buf;
extern jfieldID field_bytebuffer_len;
extern jfieldID field_bytebuffer_max_len;

static CLASSINIT bearerClassesToInit[] = {
    { "com/nokia/comm/wap/wps/Bearer", &bearer_class },
    { "com/nokia/comm/wap/wps/BearerMessage", &bearermessage_class }
};

static METHODINIT bearerMethodsToInit[] = {
    { &bearer_open_method, &bearer_class, "open", "(IZ)I"}, 
    { &bearer_close_method, &bearer_class, "close", "(I)I" },
    { &bearer_close_all_method, &bearer_class, "closeAll", "()I" },
    { &bearer_receive_buffer_method, &bearer_class, "receiveBuffer",
      "(Lcom/nokia/comm/wap/wps/BearerMessage;)I" },
    { &bearer_send_buffer_method, &bearer_class, "sendBuffer",
      "(Lcom/nokia/comm/wap/wps/BearerMessage;)I" },
    { &bearer_number_of_open_ports_method, &bearer_class,
      "numberOfOpenPorts", "()I" },
    { &bearer_init_method, &bearer_class, "init", "()I" },
    { &bearer_shutdown_method, &bearer_class, "shutdown", "()I" },
    { &bearer_get_default_method, &bearer_class, "getDefault", "(I)I" },
    { &bearer_get_local_host_name_method, &bearer_class, "getLocalHostName",
      "([B)I" },
    { &bearermessage_constructor, &bearermessage_class, "<init>", "()V" }
};

static FIELDINIT bearerFieldsToInit[] = {
    { &bearermessage_buf_field, &bearermessage_class, "dataBuffer", "[B" },
    { &bearermessage_buflen_field, &bearermessage_class, "dataLength", "I" },
    { &bearermessage_type_field, &bearermessage_class, "messageType", "I" },
    { &bearermessage_laddr_field, &bearermessage_class, "localAddress", "[B" },
    { &bearermessage_laddrlen_field, &bearermessage_class, 
      "localAddressLength", "I" },
    { &bearermessage_lport_field, &bearermessage_class, "localPort", "I" },
    { &bearermessage_raddr_field, &bearermessage_class, "remoteAddress", "[B"},
    { &bearermessage_raddrlen_field, &bearermessage_class,
      "remoteAddressLength", "I" },
    { &bearermessage_rport_field, &bearermessage_class, "remotePort", "I" }
};

/*
 * Translate Default Config values from Java class value 
 * (listed in the javah-generated header) to Stack WDP #defines.
 */

UINT32 BearerConfigValue(int v) {

	switch((long) v) {
	case com_nokia_comm_wap_wps_Bearer_WDP_D_MAX_RECEIVE_BUFFER_LENGTH:
		return WDP_D_MAX_RECEIVE_BUFFER_LENGTH;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_MAX_SEND_BUFFER_LENGTH:
		return WDP_D_MAX_SEND_BUFFER_LENGTH;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_MAX_RECEIVE_ADDRESS_LENGTH:
		return WDP_D_MAX_RECEIVE_ADDRESS_LENGTH;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_MAX_SEND_ADDRESS_LENGTH:
		return WDP_D_MAX_SEND_ADDRESS_LENGTH;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_MAX_ACK_TIMER_EXPIRATIONS:
		return WDP_D_MAX_ACK_TIMER_EXPIRATIONS;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_MAX_RETRANSMISSIONS:
		return WDP_D_MAX_RETRANSMISSIONS;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_TIMER_ACK_T:
		return WDP_D_TIMER_ACK_T;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_TIMER_U_ACK_T:
		return WDP_D_TIMER_U_ACK_T;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_TIMER_S_ACK_T:
		return WDP_D_TIMER_S_ACK_T;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_TIMER_L_ACK_T:
		return WDP_D_TIMER_L_ACK_T;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_TIMER_TTR_T:
		return WDP_D_TIMER_TTR_T;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_TIMER_S_TTR_T:
		return WDP_D_TIMER_S_TTR_T;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_TIMER_L_TTR_T:
		return WDP_D_TIMER_L_TTR_T;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_TIMER_GTR_T:
		return WDP_D_TIMER_GTR_T;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_TIMER_WAIT_T:
		return WDP_D_TIMER_WAIT_T;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_BEARER_TYPE:
		return WDP_D_BEARER_TYPE;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_CAS_CACHE_USE:
		return WDP_D_CAS_CACHE_USE;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_CAS_CACHE_T:
		return WDP_D_CAS_CACHE_T;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_CAS_FLUSH_LIMIT:
		return WDP_D_CAS_FLUSH_LIMIT;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_CAS_BAG_ELEM_LIMIT:
		return WDP_D_CAS_BAG_ELEM_LIMIT;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_CAS_CACHE_EXPIRATION_LIMIT:
		return WDP_D_CAS_CACHE_EXPIRATION_LIMIT;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_SAR_MEDIAN_ROUND_TRIP_TIME:
		return WDP_D_SAR_MEDIAN_ROUND_TRIP_TIME;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_SAR_PACKET_DELAY_TIME:
		return WDP_D_SAR_PACKET_DELAY_TIME;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_SAR_GROUP_DELAY_TIME:
		return WDP_D_SAR_GROUP_DELAY_TIME;
	case com_nokia_comm_wap_wps_Bearer_WDP_D_SAR_MAX_GROUP_SIZE:
		return WDP_D_SAR_MAX_GROUP_SIZE;
	}
	return 0;
}


/* 
 * Function   : JavaBearer_Init
 *
 * Initializes the bearers module.
 *
 * Parameters : - 
 * Returns    : 0 on success,
 *              1 on failure
 */

int JavaBearer_Init(JNIEnv* env)
{
    int  result = 0;

    /* 
     * JNI and virtual machine installation 
     */
    if ((*env)->GetJavaVM(env, &jvm) != 0) {
        /* 
         * Failed to get virtual machine 
         */
        result = 1;
        jvm = NULL;
    } else {
        result = StartupClassEnvironment(env,
                    bearerClassesToInit, ASIZE(bearerClassesToInit, CLASSINIT),
                    bearerMethodsToInit, ASIZE(bearerMethodsToInit, METHODINIT), NULL, 0,
                    bearerFieldsToInit, ASIZE(bearerFieldsToInit, FIELDINIT), NULL, 0);
    }

    /* 
     * Module specific installation 
     */
    if (result != 0) {
        JNI_DEBUG("Bearer_Init failed")
        return 1;
    }

    return 0;
}

/* 
 * Function   : JavaBearer_Finish
 *
 * Finishes the bearers module.
 *
 * Parameters : - 
 * Returns    : nothing
 */

void JavaBearer_Finish(JNIEnv* env)
{
	/* 
	 * TODO: Free all bearers & memory 
	 */
	if ((*env)->GetJavaVM(env, &jvm) == 0) {
		ShutdownClassEnvironment(env, bearerClassesToInit, 
			ASIZE(bearerClassesToInit, CLASSINIT));
		jvm = NULL;
	}
}


#ifndef JAVA1_1
/*
 * get the environment to the virtual machine
 * return 1 when we needed to attach
 * return 0 when the thread was already attached
 * return -1 is case of error
 * env is null when something went wrong
 */
int attach_to_vm(JNIEnv **env) {
    int answer;

    answer = (*jvm)->GetEnv(jvm, (void **)env, JNI_VERSION_1_2);
    
    switch (answer) {
    case JNI_EDETACHED:
        if (((*jvm) -> AttachCurrentThread(jvm, (void **)env, 0)) != 0) {
            return -1;
        }
        JNI_DEBUG_JVM("We are gonna attach the thread\n");
        return 1;
        
    case JNI_EVERSION:
        return -1;
        
    default:
        return 0;
    }
}


/*
 * detach the current thread if needed
 */

void detach_from_vm(int val) {
    JNI_DEBUG_JVM_CMD(printf("detach_from_vm: val: %i\n", val);)
    if (val) {
        JNI_DEBUG_JVM("detach_from_vm: we are gonna detach the current thread\n");
        (*jvm) -> DetachCurrentThread(jvm);
    }
}

#else 

/*
 * used for JDK1.1.7
 */
int attach_to_vm(JNIEnv **env) {
  if (((*jvm) -> AttachCurrentThread(jvm, env, 0)) != 0) {
    return -1;
  }
  return 1;
}

/*
 * used for JDK1.1.7
 */
void detach_from_vm(int val) {
}

#endif

JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Bearer_install (JNIEnv *env, jobject listener) {
	int id = -1;
	JNI_DEBUG("Call   -> bearerInstall")

	if (num_bearers == JNI_MAX_NUM_BEARERS) return -1;

	bearer_object[num_bearers] = (*env) -> NewGlobalRef(env, listener);

	switch (num_bearers) {
	case 0:
		WPS_AttachBearer(java_bearer_adapter_factory0());
		break;
	case 1:
		WPS_AttachBearer(java_bearer_adapter_factory1());
		break;
	case 2:
		WPS_AttachBearer(java_bearer_adapter_factory2());
		break;
	}
	id = num_bearers;
	num_bearers++;

	JNI_DEBUG("  Return -> BearerInstall")
	return id;
}

JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Bearer_uninstall (JNIEnv *env, jobject obj) {
	int id;

	id = (int) ((*env) -> GetIntField(env, obj, bearer_id_field));
	JNI_DEBUG_CMD(printf("Call   -> bearerUninstall ID %d (Not implemented)", id);)

	JNI_DEBUG("  Return -> BearerUninstall")

	return -1;
}

/*
 * Opens the bearer adapter for receiving messages from a local port.
 *
 * @param local_port Local port
 * @param random_port_flag We request a random local port if this flag is set.
 *
 * Returns the local port or 0 in the event of error.
 */
UINT16 java_bearer_adapter_open( UINT16 local_port, BOOL randomFlag, int bnum )
{
	JNIEnv *env;
	int answer;
	int attached;

	JNI_DEBUG("Call   -> bearerOpen")
	if ((attached = attach_to_vm(&env)) == -1) {
        	JNI_DEBUG("   Error -> AttachCurrentThread");
        	return -1;
	}  
	answer = (*env) -> CallIntMethod(env, bearer_object[bnum], bearer_open_method, local_port, 
		randomFlag);
	detach_from_vm(attached);

	JNI_DEBUG_CMD(printf("  RESULT: open: %i\n", answer);)
	JNI_DEBUG("  Return -> BearerOpen")

	return answer;
}

UINT16 java_bearer_adapter_open0( BEARER_INSTANCE_ID instID, UINT16 local_port, BOOL randomFlag )
{
	return java_bearer_adapter_open(local_port, randomFlag, 0);
}
UINT16 java_bearer_adapter_open1( BEARER_INSTANCE_ID instID, UINT16 local_port, BOOL randomFlag )
{
	return java_bearer_adapter_open(local_port, randomFlag, 1);
}
UINT16 java_bearer_adapter_open2( BEARER_INSTANCE_ID instID, UINT16 local_port, BOOL randomFlag )
{
	return java_bearer_adapter_open(local_port, randomFlag, 2);
}


/*
 * Closes the bearer for the given local port.
 * Returns 0 if the operation was successful or -1 if the operation failed.
 */
INT32 java_bearer_adapter_close( UINT16 local_port, int bnum )
{
	JNIEnv *env;
	int answer;
	int attached;
	JNI_DEBUG("Call   -> bearerClose");
    
	if ((attached = attach_to_vm(&env)) == -1) {
        	JNI_DEBUG("   Error -> AttachCurrentThread");
        	return -1;
	}  
	answer = (*env) -> CallIntMethod(env, bearer_object[bnum], bearer_close_method, local_port);
	detach_from_vm(attached);
	JNI_DEBUG_CMD(printf("  RESULT: close: %i\n", answer);)
	JNI_DEBUG("  Return -> BearerClose")

	return answer;
}

INT32 java_bearer_adapter_close0( BEARER_INSTANCE_ID instID, UINT16 local_port )
{
	return java_bearer_adapter_close(local_port, 0);
}
INT32 java_bearer_adapter_close1( BEARER_INSTANCE_ID instID, UINT16 local_port )
{
	return java_bearer_adapter_close(local_port, 1);
}
INT32 java_bearer_adapter_close2( BEARER_INSTANCE_ID instID, UINT16 local_port )
{
	return java_bearer_adapter_close(local_port, 2);
}


/*
 * Closes the bearer for all open local ports.
 * Returns 0 if the operation was successful or -1 if the operation failed.
 */
INT32 java_bearer_adapter_close_all( int bnum )
{
	JNIEnv *env;
	int answer;
	int attached;
	JNI_DEBUG("Call   -> bearerCloseAll")

	if ((attached = attach_to_vm(&env)) == -1) {
        	JNI_DEBUG("   Error -> AttachCurrentThread");
        	return WDP_ERROR;
	}  
	answer = (*env) -> CallIntMethod(env, bearer_object[bnum], bearer_close_all_method);
	detach_from_vm(attached);

	JNI_DEBUG_CMD(printf("  RESULT: %i\n", answer);)
	JNI_DEBUG("  Return -> BearerCloseAll")

	return answer;
}

INT32 java_bearer_adapter_close_all0( BEARER_INSTANCE_ID instID )
{
	return java_bearer_adapter_close_all(0);
}
INT32 java_bearer_adapter_close_all1( BEARER_INSTANCE_ID instID )
{
	return java_bearer_adapter_close_all(1);
}
INT32 java_bearer_adapter_close_all2( BEARER_INSTANCE_ID instID )
{
	return java_bearer_adapter_close_all(2);
}

  /**
   * Receives a buffer.
   *
   * @param buf Data buffer
   * @param buf_length Pointer that on return gives the length of 
   *        the data buffer. 
   * @param buf_max_length The allocated size of the data buffer.
   * @param local_port Pointer that on return gives the local port.
   * @param local_address Pointer to the local address buffer.
   * @param local_address_length Pointer that on return gives 
   *        the length of the local address buffer.
   * @param local_address_max_length The allocated size of the 
   *        local address buffer.
   * @param remote_port Pointer that on return gives the remote port.
   * @param remote_address Remote address buffer.
   * @param remote_address_length Pointer that on return gives 
   *        the length of the remote address buffer.
   * @param remote_address_max_length The allocated size of the 
   *        remote address buffer.
   * @param bufferType Pointer that on return gives the type of 
   *        the received buffer. The bufferType can be one of the following:
   *        WDP_DATAGRAM; for normal WDP datagrams.
   *        WDP_WCMP_MESSAGE; for WCMP messages.
   *        WDP_UNKNOWN_MESSAGE; type unknown.
   *
   * @return receiveBuffer returns WDP_SUCCESS if the operation was 
   * successful or WDP_ERROR (* _GG_ check this out *) if the operation failed. 
   * On return the buf_length and the address_length variables give the 
   * length of the corresponding buffers. The bufferType gives the 
   * type of the received buffer.
   */
INT32 java_bearer_adapter_receive( BYTE *buf,
			 UINT32 *buf_length,
			 UINT32 buf_max_length,
			 UINT16 *local_port,
			 BYTE *local_address,
			 UINT16 *local_address_length,
			 UINT16 local_address_max_length,
			 UINT16 *remote_port,
			 BYTE *remote_address,
			 UINT16 *remote_address_length,
			 UINT16 remote_address_max_length,
			 UINT32 *buffer_type,
			 int bnum)
{
	JNIEnv *env;
	jobject msg /*,tmp*/;
	jobject msg_buf, msg_laddr, msg_raddr;
	int answer;
	int attached;

	/*
	 * Sanity checks
	 */
	if (!buf ||
	    !buf_length ||
	    !local_port ||
	    !local_address || 
	    !local_address_length ||
	    !remote_address ||
	    !remote_address_length ||
	    !buffer_type)
		return WDP_ERROR;

	/*
	 * Buffer type is always DATAGRAM
	 */
	*buffer_type = WDP_DATAGRAM;

	/*
	 * Hardcoded local address
	 */
#define LOCALNAME "localhost"
#define LOCALNAME_LENGTH 9

	strcpy(local_address, LOCALNAME);
	*local_address_length = LOCALNAME_LENGTH;

	JNI_DEBUG_CMD(printf("Call  -> bearerReceive maxlen:%d", buf_max_length);)

	if (buf_max_length <= 0  || buf_length == 0 || *buf_length < 0) {
		JNI_DEBUG("Return -> bearerReceive ERROR: ZERO LENGTH BUFFER!")
		return WDP_ERROR_TOO_SMALL_BUFFER;
	}	

	if ((attached = attach_to_vm(&env)) == -1) {
        	JNI_DEBUG("   Error -> AttachCurrentThread");
        	return -1;
	}  
	if ((msg = (*env) -> NewObject(env, bearermessage_class, bearermessage_constructor)) == 0) {
		JNI_DEBUG("  ERROR NewObject")
		detach_from_vm(attached);
		return -1;
	}
	msg_buf   = (*env) -> GetObjectField(env, msg, bearermessage_buf_field);
	msg_laddr = (*env) -> GetObjectField(env, msg, bearermessage_laddr_field);
	msg_raddr = (*env) -> GetObjectField(env, msg, bearermessage_raddr_field);

	(*env) -> SetIntField(env, msg_buf, field_bytebuffer_max_len, buf_max_length);
	(*env) -> SetIntField(env, msg_laddr, field_bytebuffer_max_len, local_address_max_length);
	(*env) -> SetIntField(env, msg_raddr, field_bytebuffer_max_len, remote_address_max_length);

	answer = (*env) -> CallIntMethod(env, bearer_object[bnum], bearer_receive_buffer_method, 
		msg);

	if (answer == WDP_SUCCESS) {

		JNI_GetByteBuffer32(env, buf, buf_length, buf_max_length, msg_buf);
		JNI_GetByteBuffer(env, local_address, local_address_length, 
			local_address_max_length, msg_laddr);
		JNI_GetByteBuffer(env, remote_address, remote_address_length, 
			remote_address_max_length, msg_raddr);

		*local_port = (unsigned short) ((*env) -> GetIntField(env, msg, 
			bearermessage_lport_field));
		*remote_port = (unsigned short) ((*env) -> GetIntField(env, msg, 
			bearermessage_rport_field));

		JNI_DEBUG_CMD(printf("  RESULT: received %i bytes, type %d\n", *buf_length,
			*buffer_type);)

		(*env) -> DeleteLocalRef(env, msg);

	} else {
		JNI_DEBUG("Receive failed!\n");
	}
	detach_from_vm(attached);
	JNI_DEBUG("  Return -> BearerReceive")
	return answer;
}

INT32 java_bearer_adapter_receive0(
	BEARER_INSTANCE_ID instID,
	BYTE *buf, 
	UINT32 *buf_length, 
	UINT32 buf_max_length,
	UINT16 *local_port, 
	BYTE *local_address, 
	UINT16 *local_address_length, 
	UINT16 local_address_max_length,
	UINT16 *remote_port,
	BYTE *remote_address, 
	UINT16 *remote_address_length, 
	UINT16 remote_address_max_length,
	UINT32 *buffer_type)
{
	printf("Call -> java_bearer_adapter_receive0 InstID=%d\n", instID);

	return java_bearer_adapter_receive( 
		buf, buf_length, buf_max_length,
		local_port,
		local_address, local_address_length, local_address_max_length,
		remote_port,
		remote_address, remote_address_length, remote_address_max_length,
		buffer_type,
		0);
}

INT32 java_bearer_adapter_receive1(
	BEARER_INSTANCE_ID instID,
	BYTE *buf, 
	UINT32 *buf_length, 
	UINT32 buf_max_length,
	UINT16 *local_port, 
	BYTE *local_address, UINT16 *local_address_length, UINT16 local_address_max_length,
	UINT16 *remote_port,
	BYTE *remote_address, UINT16 *remote_address_length, UINT16 remote_address_max_length,
	UINT32 *buffer_type)
{
	return java_bearer_adapter_receive( 
		buf, buf_length, buf_max_length,
		local_port,
		local_address, local_address_length, local_address_max_length,
		remote_port,
		remote_address, remote_address_length, remote_address_max_length,
		buffer_type,
		1);
}
INT32 java_bearer_adapter_receive2(
	BEARER_INSTANCE_ID instID,
	BYTE *buf, UINT32 *buf_length, UINT32 buf_max_length,
	UINT16 *local_port, 
	BYTE *local_address, UINT16 *local_address_length, UINT16 local_address_max_length,
	UINT16 *remote_port,
	BYTE *remote_address, UINT16 *remote_address_length, UINT16 remote_address_max_length,
	UINT32 *buffer_type)
{
	return java_bearer_adapter_receive( 
		buf, buf_length, buf_max_length,
		local_port,
		local_address, local_address_length, local_address_max_length,
		remote_port,
		remote_address, remote_address_length, remote_address_max_length,
		buffer_type,
		2);
}


  /**
   * sendBuffer sends a buffer to a remote address. 
   *
   * @param buf Binary buffer.
   * @param buf_length The length of the binary buffer.
   * @param local_port Local port for the send. 0 = use any free port.
   * @param local_address Local address for the send.
   * @param local_address_length The length of the local address.
   * @param remote_port Remote port for the send.
   * @param remote_address Remote address for the send.
   * @param remote_address_length The length of the remote address.
   * @param bufferType Gives the type of the received buffer. The bufferType
   *        can be one of the following:
   *        WDP_DATAGRAM; for normal WDP datagrams.
   *        WDP_WCMP_MESSAGE; for WCMP messages.
   *        WDP_UNKNOWN_MESSAGE; type unknown.
   *
   * @return sendBuffer returns WDP_SUCCESS if the operation was successful 
   * or WDP_ERROR if the operation failed. (* _GG_ check this out *)
   */
INT32 java_bearer_adapter_send(BYTE *buf,
		      UINT32 buf_length,
		      UINT16 local_port,
		      BYTE *local_address,
		      UINT16 local_address_length,
		      UINT16 remote_port,
		      BYTE *remote_address,
		      UINT16 remote_address_length,
		      UINT32 buffer_type,
			  int bnum)
{
	JNIEnv *env;
	jobject msg, msg_buf, msg_laddr, msg_raddr;
	int answer;
	int attached;

	JNI_DEBUG_ARG1("Call   -> bearerSend (%d bytes)\n", buf_length)
	if ((attached = attach_to_vm(&env)) == -1) {
        	JNI_DEBUG("   Error -> AttachCurrentThread");
        	return WDP_ERROR;
	}    
	if ((msg = (*env) -> NewObject(env, bearermessage_class, bearermessage_constructor)) == 0) {
		JNI_DEBUG("  ERROR NewObject")
		  detach_from_vm(attached);
		return WDP_ERROR;
	}

	msg_buf   = (*env) -> GetObjectField(env, msg, bearermessage_buf_field);
	msg_laddr = (*env) -> GetObjectField(env, msg, bearermessage_laddr_field);
	msg_raddr = (*env) -> GetObjectField(env, msg, bearermessage_raddr_field);

	JNI_SetByteBuffer32(env, buf, buf_length, msg_buf);	
	JNI_SetByteBuffer(env, local_address, local_address_length, msg_laddr);	
	JNI_SetByteBuffer(env, remote_address, remote_address_length, msg_raddr);	

	(*env) -> SetIntField(env, msg, bearermessage_lport_field, local_port);
	(*env) -> SetIntField(env, msg, bearermessage_rport_field, remote_port);
	(*env) -> SetIntField(env, msg, bearermessage_type_field, buffer_type);

	answer = (*env) -> CallIntMethod(env, bearer_object[bnum], bearer_send_buffer_method, msg);

	(*env) -> DeleteLocalRef(env, msg);

	detach_from_vm(attached);
	JNI_DEBUG_ARG1("  Return -> BearerSend: %d\n", answer)

	return answer;
}

INT32 java_bearer_adapter_send0(
	BEARER_INSTANCE_ID instID,
	BYTE *buf, UINT32 buf_length,
	UINT16 local_port,
	BYTE *local_address, UINT16 local_address_length,
	UINT16 remote_port, 
	BYTE *remote_address, UINT16 remote_address_length,
	UINT32 buffer_type)
{
	return java_bearer_adapter_send(
		buf, buf_length,
		local_port,
		local_address, local_address_length,
		remote_port,
		remote_address, remote_address_length,
		buffer_type,
		0);
}

INT32 java_bearer_adapter_send1(
	BEARER_INSTANCE_ID instID,
	BYTE *buf, UINT32 buf_length,
	UINT16 local_port,
	BYTE *local_address, UINT16 local_address_length,
	UINT16 remote_port, 
	BYTE *remote_address, UINT16 remote_address_length,
	UINT32 buffer_type)
{
	return java_bearer_adapter_send(
		buf, buf_length,
		local_port,
		local_address, local_address_length,
		remote_port,
		remote_address, remote_address_length,
		buffer_type,
		1);
}

INT32 java_bearer_adapter_send2(
	BEARER_INSTANCE_ID instID,
	BYTE *buf, UINT32 buf_length,
	UINT16 local_port,
	BYTE *local_address, UINT16 local_address_length,
	UINT16 remote_port, 
	BYTE *remote_address, UINT16 remote_address_length,
	UINT32 buffer_type)
{
	return java_bearer_adapter_send(
		buf, buf_length,
		local_port,
		local_address, local_address_length,
		remote_port,
		remote_address, remote_address_length,
		buffer_type,
		2);
}

/*
 * Returns the number of open ports
 */
UINT32 java_bearer_adapter_number_of_open_ports( int bnum )
{
	JNIEnv *env;
	int answer;
	int attached;

	JNI_DEBUG("Call   -> numberOfOpenPorts")
	if ((attached = attach_to_vm(&env)) == -1) {
		JNI_DEBUG("   Error -> AttachCurrentThread");
		return -1;
	}  
	answer = (*env) -> CallIntMethod(env, bearer_object[bnum], 
		bearer_number_of_open_ports_method);
	detach_from_vm(attached);

	JNI_DEBUG_ARG1("  Return -> BearerNumberOfOpenPorts: %d\n", answer)
	return answer;
}

UINT32 java_bearer_adapter_number_of_open_ports0( BEARER_INSTANCE_ID instID )
{
	return java_bearer_adapter_number_of_open_ports( 0 );
}
UINT32 java_bearer_adapter_number_of_open_ports1( BEARER_INSTANCE_ID instID )
{
	return java_bearer_adapter_number_of_open_ports( 1 );
}
UINT32 java_bearer_adapter_number_of_open_ports2( BEARER_INSTANCE_ID instID )
{
	return java_bearer_adapter_number_of_open_ports( 2 );
}

/*
 * Inits the Java bearer adapter
 */
INT32 java_bearer_adapter_init( int bnum )
{
	JNIEnv *env;
	int answer;
	int attached;

	JNI_DEBUG("Call   -> bearerInit")

	if (is_initialized == -1) {
		JNI_DEBUG("  Return -> BearerInit")
		return WDP_ERROR_SUCCESS; /*_GG_ 981201*/
	}

	is_initialized = -1;

	if ((attached = attach_to_vm(&env)) == -1) {
        	JNI_DEBUG("   Error -> AttachCurrentThread");
        	return WDP_ERROR;
	}  	
	answer = (*env) -> CallIntMethod(env, bearer_object[bnum], bearer_init_method);
	detach_from_vm(attached);
	JNI_DEBUG("  Return -> BearerInit")

	return answer;
}
INT32 java_bearer_adapter_init0( BEARER_INSTANCE_ID instID )
{
	return java_bearer_adapter_init( 0 );
}
INT32 java_bearer_adapter_init1( BEARER_INSTANCE_ID instID )
{
	return java_bearer_adapter_init( 1 );
}
INT32 java_bearer_adapter_init2( BEARER_INSTANCE_ID instID )
{
	return java_bearer_adapter_init( 2 );
}


/************************************************************
 * Function:   udp_get_defaults
 *		(from stack wdp_udp bearer adapter)
 * 
 * Description: 
 *             Gets default values for this bearer.
 *
 * Params:	
 *             instance ID of bearer
 *             defaultCode Code for the default value
 *
 * Return:     Returns the default value or 0 if the defaultCode
 *             was not found or supported.
 *************************************************************/
UINT32 
udp_get_defaults(BEARER_INSTANCE_ID InstID, UINT32 defaultCode) 
{
	UINT32 rtn;
	JNI_DEBUG_CMD(printf("Call   -> bearerGetDefault for %d\n", defaultCode);)

	switch(defaultCode) {
	    case WDP_D_MAX_RECEIVE_BUFFER_LENGTH: 
	    case WDP_D_MAX_SEND_BUFFER_LENGTH:
		rtn = 32000; /* bytes */
		break;
	    case WDP_D_MAX_RECEIVE_ADDRESS_LENGTH:
	    case WDP_D_MAX_SEND_ADDRESS_LENGTH:
		rtn = 1024; /* bytes */
		break;
	    case WDP_D_MAX_ACK_TIMER_EXPIRATIONS:
	    case WDP_D_MAX_RETRANSMISSIONS:
		rtn = 5; /* tries */
		break;
	    case WDP_D_TIMER_ACK_T:
	    case WDP_D_TIMER_S_ACK_T:
	    case WDP_D_TIMER_L_ACK_T:
		rtn = 5000; /* ms */
		break;
	    case WDP_D_TIMER_U_ACK_T:
		rtn = 7000; /* ms */
		break;
	    case WDP_D_TIMER_TTR_T:
	    case WDP_D_TIMER_S_TTR_T:
	    case WDP_D_TIMER_L_TTR_T:
	    case WDP_D_TIMER_GTR_T:
		rtn = 10000; /* ms */
		break;
	    case WDP_D_TIMER_WAIT_T:
		rtn = 30000; /* ms */
		break;
	    case WDP_D_BEARER_TYPE:
		rtn = 0; /* types not yet defined */
		break;
	   /*
	    * CAS parameters
	    */
	    case WDP_D_CAS_CACHE_USE:
		rtn = 0; /* off */
		break;
	    case WDP_D_CAS_CACHE_T:
		rtn = 1000; /* ms */
		break;
	    case WDP_D_CAS_FLUSH_LIMIT:
	    case WDP_D_CAS_CACHE_EXPIRATION_LIMIT:
		rtn = 2; /* tries */
		break;
	    case WDP_D_CAS_BAG_ELEM_LIMIT:
		rtn = 100; /* elements */
		break;
    
	    /*
	     * SAR parameters
	     */
	    case WDP_D_SAR_MEDIAN_ROUND_TRIP_TIME:
		rtn = 100; /* ms */
		break;
	    case WDP_D_SAR_PACKET_DELAY_TIME:
		rtn = 0; /* ms */
		break;
	    case WDP_D_SAR_GROUP_DELAY_TIME:
		rtn = 200; /* ms */
		break;
	    case WDP_D_SAR_MAX_GROUP_SIZE:
		rtn = 5; /* packets in a group */
		break;
    
	    default:
		rtn = 0;
		break;
	}
	JNI_DEBUG_CMD(printf("Return   -> bearerGetDefault %d = %d\n", 
		defaultCode, rtn);)
	return rtn;    
}

/*
* getDefault returns various default values for the bearer
* The defaultCode can be one of the WDP_D_* constants defined
* in the begining of this header file. Returns 0 if the
* defaultCode was not found or supported.
*/
UINT32 java_bearer_adapter_get_default(UINT32 default_code, int bnum)
{
#ifdef OUT
	JNIEnv *env;
	unsigned int answer;
	int attached;

	JNI_DEBUG_CMD(printf("Call   -> bearerGetDefault for %d\n", default_code);)

	if ((attached = attach_to_vm(&env)) == -1) {
        	JNI_DEBUG("   Error -> AttachCurrentThread");
        	return -1;
	}  
	answer = (*env) -> CallIntMethod(env, bearer_object[bnum], bearer_get_default_method, 
		default_code);
	detach_from_vm(attached);
	JNI_DEBUG_CMD(printf("  RESULT: %d = %d\n", default_code, answer);)
	JNI_DEBUG("  Return -> BearerGetDefault")

	return BearerConfigValue(answer);
#endif OUT
	return udp_get_defaults(0, default_code);
}

UINT32 java_bearer_adapter_get_default0( BEARER_INSTANCE_ID instID, UINT32 default_code)
{
	return java_bearer_adapter_get_default(default_code, 0);
}
UINT32 java_bearer_adapter_get_default1( BEARER_INSTANCE_ID instID, UINT32 default_code)
{
	return java_bearer_adapter_get_default(default_code, 1);
}
UINT32 java_bearer_adapter_get_default2( BEARER_INSTANCE_ID instID, UINT32 default_code)
{
	return java_bearer_adapter_get_default(default_code, 2);
}

/*
 * getLocalName returns the local host name for the bearer.
 *
 * @param pHostName Buffer for the local host name.
 * @param pLength On return gives the length of the local host name.
 * @param maxLength The allocated size of the pHostName buffer.
 *
 * @return Returns WDP_SUCCESS if the operation was successfull or
 *         WDP_ERROR if the operation failed. (* _GG_ check this out *)
 */
INT32 java_bearer_adapter_get_local_host_name(
	BYTE *pHostName,
	UINT16 *pLength,
	UINT16 maxLength,
	int bnum) 
{
	JNIEnv *env;
	jobject bytebuffer;
	unsigned int answer;
	int attached;
    
	JNI_DEBUG("Call   -> bearerGetLocalHostName")

	if ((attached = attach_to_vm(&env)) == -1) {
        	JNI_DEBUG("   Error -> AttachCurrentThread");
        	return -1;
	}  
	if ((bytebuffer = (*env) -> NewObject(env, class_bytebuffer, 
	     method_bytebuffer_constructor)) == 0) {
		JNI_DEBUG("  ERROR NewObject")
		return -1;
	}
	(*env) -> SetIntField(env, bytebuffer, field_bytebuffer_max_len, maxLength);
	answer = (*env) -> CallIntMethod(env, bearer_object[bnum], bearer_get_local_host_name_method, 
		bytebuffer);

	JNI_GetByteBuffer(env, pHostName, pLength, maxLength, bytebuffer); 
	detach_from_vm(attached);

	JNI_DEBUG_ARG1("Return   -> bearerGetLocalHostName %s\n",
		answer == WDP_SUCCESS ? "OK" : "FAILED")
	return answer;
}

INT32 java_bearer_adapter_get_local_host_name0(
	BEARER_INSTANCE_ID instID,
	BYTE *pHostName, 
	UINT16 *pLength, 
	UINT16 maxLength) 
{
	return java_bearer_adapter_get_local_host_name(pHostName, pLength, maxLength, 0);
}
INT32 java_bearer_adapter_get_local_host_name1(
	BEARER_INSTANCE_ID instID,
	BYTE *pHostName, 
	UINT16 *pLength, 
	UINT16 maxLength) 
{
	return java_bearer_adapter_get_local_host_name(pHostName, pLength, maxLength, 1);
}
INT32 java_bearer_adapter_get_local_host_name2(
	BEARER_INSTANCE_ID instID,
	BYTE *pHostName, 
	UINT16 *pLength, 
	UINT16 maxLength) 
{
	return java_bearer_adapter_get_local_host_name(pHostName, pLength, maxLength, 2);
}

/*
 * Shuts down the Java bearer adapter
 */
INT32 java_bearer_shutdown(int bnum)
{
	JNIEnv *env;
	jint answer;
	int attached;
    
	JNI_DEBUG("Call   -> bearerShutdown")

	if ((attached = attach_to_vm(&env)) == -1) {
        	JNI_DEBUG("   Error -> AttachCurrentThread");
        	return -1;
	}  
        answer = (*env)->CallIntMethod(env, bearer_object[bnum], bearer_shutdown_method);
	detach_from_vm(attached);
	JNI_DEBUG("  Return -> BearerShutdown")

	return answer;
}

INT32 java_bearer_shutdown0(BEARER_INSTANCE_ID InstID)
{
	return java_bearer_shutdown( 0 );
}
INT32 java_bearer_shutdown1(BEARER_INSTANCE_ID InstID)
{
	return java_bearer_shutdown( 1 );
}
INT32 java_bearer_shutdown2(BEARER_INSTANCE_ID InstID)
{
	return java_bearer_shutdown( 2 );
}

/*
 * Returns a void pointer to a bearer_adapter_info struct. 
 * NOTE: We only have one instance ID per bearer!
 */
bearer_adapter_info *
java_bearer_adapter_factory0( void ) 
{
	static bearer_adapter_info *Java_bi = 0;

	if ( Java_bi ) 
		return Java_bi;
  
	if (!(Java_bi = (bearer_adapter_info *) malloc(sizeof(bearer_adapter_info)))) 
		return (bearer_adapter_info *)0;

	memset( Java_bi, 0, sizeof( bearer_adapter_info ) );
  
	Java_bi->id = JNI_FIRST_BEARER_ID + 0; /* Java 0, value 0 is not used!! */
	Java_bi->InstID = 0;
	Java_bi->factory = 
	( bearer_adapter_info * (*)( void ) ) java_bearer_adapter_factory0;
	Java_bi->open = java_bearer_adapter_open0; 
	Java_bi->close = java_bearer_adapter_close0;
	Java_bi->closeAll = java_bearer_adapter_close_all0;
	Java_bi->receiveBuffer = java_bearer_adapter_receive0; 
	Java_bi->sendBuffer = java_bearer_adapter_send0;
	Java_bi->numberOfOpenPorts = java_bearer_adapter_number_of_open_ports0;
	Java_bi->init = java_bearer_adapter_init0;
        Java_bi->shutdown = java_bearer_shutdown0;
	Java_bi->getDefault = java_bearer_adapter_get_default0;
	Java_bi->getLocalHostName = java_bearer_adapter_get_local_host_name0;
	Java_bi->lastStatus = 0;  /* status from last operation, used by WDP */

	return Java_bi;
}

bearer_adapter_info *
java_bearer_adapter_factory1( void ) 
{
	static bearer_adapter_info *Java_bi = 0;

	if ( Java_bi ) 
		return Java_bi;
	if (!(Java_bi = (bearer_adapter_info *) malloc(sizeof(bearer_adapter_info)))) 
		return (bearer_adapter_info *)0;
	memset( Java_bi, 0, sizeof( bearer_adapter_info ) );
  
	Java_bi->id = JNI_FIRST_BEARER_ID + 1; /* Java 0, value 0 is not used!! */
	Java_bi->InstID = 0;
	Java_bi->factory = 
	( bearer_adapter_info * (*)( void ) ) java_bearer_adapter_factory1;
	Java_bi->open = java_bearer_adapter_open1; 
	Java_bi->close = java_bearer_adapter_close1;
	Java_bi->closeAll = java_bearer_adapter_close_all1;
	Java_bi->receiveBuffer = java_bearer_adapter_receive1; 
	Java_bi->sendBuffer = java_bearer_adapter_send1;
	Java_bi->numberOfOpenPorts = java_bearer_adapter_number_of_open_ports1;
	Java_bi->init = java_bearer_adapter_init1;
        Java_bi->shutdown = java_bearer_shutdown1;
	Java_bi->getDefault = java_bearer_adapter_get_default1;
	Java_bi->getLocalHostName = java_bearer_adapter_get_local_host_name1;
	Java_bi->lastStatus = 0;  /* status from last operation, used by WDP */

	return Java_bi;
}

bearer_adapter_info *
java_bearer_adapter_factory2( void ) 
{
	static bearer_adapter_info *Java_bi = 0;

	if ( Java_bi ) 
		return Java_bi;
	if (!(Java_bi = (bearer_adapter_info *) malloc(sizeof(bearer_adapter_info)))) 
		return (bearer_adapter_info *)0;
	memset( Java_bi, 0, sizeof( bearer_adapter_info ) );
  
	Java_bi->id = JNI_FIRST_BEARER_ID + 2; /* Java 0, value 0 is not used!! */
	Java_bi->InstID = 0;
	Java_bi->factory = 
	( bearer_adapter_info * (*)( void ) ) java_bearer_adapter_factory2;
	Java_bi->open = java_bearer_adapter_open2; 
	Java_bi->close = java_bearer_adapter_close2;
	Java_bi->closeAll = java_bearer_adapter_close_all2;
	Java_bi->receiveBuffer = java_bearer_adapter_receive2; 
	Java_bi->sendBuffer = java_bearer_adapter_send2;
	Java_bi->numberOfOpenPorts = java_bearer_adapter_number_of_open_ports2;
	Java_bi->init = java_bearer_adapter_init2;
        Java_bi->shutdown = java_bearer_shutdown2;
	Java_bi->getDefault = java_bearer_adapter_get_default2;
	Java_bi->getLocalHostName = java_bearer_adapter_get_local_host_name2;
	Java_bi->lastStatus = 0;  /* status from last operation, used by WDP */

	return Java_bi;
}


/*
 * Local Variables:
 * mode:C
 * c-basic-offset:4
 * End:
 */
