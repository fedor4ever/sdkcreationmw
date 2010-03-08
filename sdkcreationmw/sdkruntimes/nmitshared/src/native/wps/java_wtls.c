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
#include "wps_wtlsc2.h"
#include "wim_apdu.h"
#include "wimi.h"
#include "cme_types.h"

jobject g_obj;
JavaVM *jvm;

struct WIM_Transaction {
    WIMI_Ref_t *pWIMref;
    int id;
};


/**************************************
 *
 * WTLS SECTION
 *
 **************************************/

/*
 * Class:     comm_wap_wps_Wtls
 * Method:    wtlsInitSecureStructure
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_comm_wap_wps_Wtls_wtlsInitSecureStructure(
                                                                                    JNIEnv *env, jclass thisClz, jint conn_id) 
{
    tWTLSC_RET  status;
	
    JNI_DEBUG("Call   -> wtlsInitSecureStructure")

	status = WTLSC_CreateSecureConnection((WPS_HCONN) conn_id, NULL, NULL);

    JNI_DEBUG_ARG1("Return   -> wtlsInitSecureStructure: %s", status==WTLSC_Ok ? "TRUE" : "FALSE")
	
	return (status == WTLSC_Ok ? JNI_TRUE : JNI_FALSE);
}

/*
 * Class:     comm_wap_wps_Wtls
 * Method:    wtlsReleaseSecureStructure
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_comm_wap_wps_Wtls_wtlsReleaseSecureStructure(
                                                                                       JNIEnv *env, jclass thisClz, jint conn_id) 
{
    tWTLSC_RET  status;
	
    JNI_DEBUG("Call   -> wtlsReleaseSecureStructure")

	status = WTLSC_ReleaseSecureConnection((WPS_HCONN)conn_id);

    JNI_DEBUG_ARG1("Return   -> wtlsReleaseSecureStructure: %s", status==WTLSC_Ok ? "TRUE" : "FALSE")
	
	return (status == WTLSC_Ok ? JNI_TRUE : JNI_FALSE);
}

/*
 * Class:     comm_wap_wps_Wtls
 * Method:    wtlsConnect
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_comm_wap_wps_Wtls_wtlsConnect(
                                                                        JNIEnv *env, jclass thisClz, jint conn_id) 
{
    tWTLSC_RET	status;

    JNI_DEBUG("Call   -> wtlsConnect")

	status = WTLSC_Connect_Start((WPS_HCONN)conn_id);
    return (status == WTLSC_Ok ? JNI_TRUE : JNI_FALSE);
}

/*
 * Class:     comm_wap_wps_Wtls
 * Method:    wtlsCloseConnection
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_comm_wap_wps_Wtls_wtlsCloseConnection(
                                                                                JNIEnv *env, jclass thisClz, jint conn_id) 
{
    tWTLSC_RET  status;

    JNI_DEBUG("Call   -> wtlsCloseConnection")

	status = WTLSC_CloseConnection((WPS_HCONN)conn_id);

    JNI_DEBUG_ARG1("Return   -> wtlsCloseConnection: %s", status==WTLSC_Ok ? "TRUE" : "FALSE")
	
	return (status == WTLSC_Ok ? JNI_TRUE : JNI_FALSE);
}

/*
 * Class:     comm_wap_wps_Wtls
 * Method:    wtlsSetKeyExchangeSuite
 * Signature: (IIII)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_comm_wap_wps_Wtls_wtlsSetKeyExchangeSuite(
                                                                                    JNIEnv *env, jclass thisClz, jint conn_id)
{
    BOOL status = TRUE;
    int  i;
    UINT16 keysCount;
    void **KES, **KES_temp;
    //void *Key;
    tWTLSC_RET result;
    UINT8 newKeysCount=0;
    UINT8 kes_type;
    UINT8 idLen;
	

    if (WIMI_GetKESList(NULL, &keysCount,&KES)==WIMI_Ok)
        {
            //JNI_DEBUG_ARG1("WIMI_GetKESList return -> %d", keysCount);
     
            /* Do not use NULL key if there are any other keys available */
            if (keysCount > 0)
                keysCount--;

            if(keysCount > WTLSC_MAX_KES-1)
                keysCount = WTLSC_MAX_KES-1;

            //If you do not want to include KES which have Identifiers, uncomment the below
            KES_temp = malloc(sizeof(void*)*keysCount);
            memcpy(KES_temp, KES, sizeof(void*)*keysCount);
            for(i=0;i<keysCount;i++)
                {
                    WIMI_GetKESInfo(KES_temp[i], NULL, NULL, &kes_type, &idLen, NULL);
                    if(idLen<=1)
                        KES[newKeysCount++] = KES_temp[i];
                }
            keysCount = newKeysCount;
            free(KES_temp);

            //Get KES information for debugging
            /*for( i =0; i < keysCount; i++ ) {
            // WIMI_GetKESInfo( KES[i], NULL, Key, &kes_type, &idLen, NULL);
            //printf(" kes_type is %c\n", kes_type);
            //printf(" id length is %d\n", idLen);
            //printf(" kes_type is %d\n", kes_type);
            } */

            JNI_DEBUG_ARG1 ( " Keys count %d", keysCount)
		for (i=0; i<keysCount; i++)
                    {
			if ((result = WTLSC_AddKeyExchangeSuite((WPS_HCONN)conn_id, KES[i])) !=WTLSC_Ok)
                            {
				JNI_DEBUG_ARG1("AddKeyExchange return -> %d", result);
				status = FALSE;
				break;
                            }
                    }
            free_WIMI_RefList_t(KES);
	}


    JNI_DEBUG_ARG1("Return   -> wtlsSetKeyExchangeSuite: %s", 
                   status ? "TRUE" : "FALSE")

	return (status ? JNI_TRUE : JNI_FALSE);
}

/*
 * Class:     comm_wap_wps_Wtls
 * Method:    wxtlsSetCipherSuite
 * Signature: (IB[)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_comm_wap_wps_Wtls_wtlsSetCipherSuite(
                                                                               JNIEnv *env, jclass thisClz, jint conn_id)
{
    tWTLSC_RET	status;

    UINT8 macCount=0;
    UINT8 bulkCount=0;
    UINT8 *mac = NULL;
    UINT8 *bulk = NULL;
    UINT8 *ciphers;
    UINT8 maxCount = 0;
    UINT8 minCount = 0;
    int i;

    //change so that every possible combination is considered.
    if (WIMI_GetMACAList(&macCount, &mac)==WIMI_Ok)
	{
            if (WIMI_GetCipherList(&bulkCount, &bulk)==WIMI_Ok)
		{
                    maxCount = (macCount<bulkCount) ? bulkCount : macCount;
                    minCount = (macCount>bulkCount) ? bulkCount : macCount;
                    ciphers = malloc(2 * maxCount);
                    if (ciphers != NULL)
                        {
                            for (i=0;i<minCount;i++)
                                {
                                    ciphers[2 * i] = mac[i];
                                    ciphers[2 * i + 1] = bulk[i];
                                }
                            for (;i<maxCount;i++)
                                {
                                    if (macCount > bulkCount)
                                        {
                                            ciphers[2 * i] = mac[i];
                                            ciphers[2 * i + 1] = bulk[bulkCount - 1];
                                        }
                                    else
                                        {
                                            ciphers[2 * i] = mac[macCount - 1];
                                            ciphers[2 * i + 1] = bulk[i];
                                        }
                                }
                        }
                    else
			maxCount = 0;
		}
	}

	free(bulk);
	free(mac);
    if(maxCount > WTLSC_MAX_CS-1)
        maxCount = WTLSC_MAX_CS-1;
    if(maxCount > 0) {
        status = WTLSC_SetCipherSuites((WPS_HCONN)conn_id, maxCount, ciphers);
		free(ciphers);
	}
    else
        status = WTLSC_ERR_NoSuitesSet;


    JNI_DEBUG_ARG1("Return   -> wtlsSetCipherSuites: %s", 
                   status==WTLSC_Ok ? "TRUE" : "FALSE")

	return (status == WTLSC_Ok ? JNI_TRUE : JNI_FALSE);
}

/*
 * Class:     comm_wap_wps_Wtls
 * Method:    wtlsSetSequenceNumberMode
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_comm_wap_wps_Wtls_wtlsSetSequenceNumberMode(
                                                                                      JNIEnv *env, jclass thisClz, jint conn_id, jbyte mode)
{
    tWTLSC_RET	status;
    UINT8 		wsp_mode;

    JNI_DEBUG_ARG1("Call   -> wtlsSetSequenceNumberMode to %d", mode)

	switch (mode) {
        case 1:
            wsp_mode = WTLS_SNM_Implicite; break;
        case 2:
            wsp_mode = WTLS_SNM_Explicite; break;
        case 0:
        default:
            wsp_mode = WTLS_SNM_NotUsed; break;
	}

    status = WTLSC_SetSequenceNumberMode((WPS_HCONN)conn_id, wsp_mode);

    JNI_DEBUG_ARG1("Return   -> wtlsSetSequenceNumberMode: %s", 
                   status==WTLSC_Ok ? "TRUE" : "FALSE")

	return (status == WTLSC_Ok ? JNI_TRUE : JNI_FALSE);
}

/*
 * Class:     comm_wap_wps_Wtls
 * Method:    wtlsSetKeyRefresh
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_comm_wap_wps_Wtls_wtlsSetKeyRefresh(
                                                                              JNIEnv *env, jclass thisClz, jint conn_id, jbyte frequency)
{
    tWTLSC_RET	status;

    JNI_DEBUG_ARG1("Call   -> wtlsSetKeyRefresh to %d", frequency)

	status = WTLSC_SetKeyRefresh((WPS_HCONN)conn_id, (UINT8)frequency);

    JNI_DEBUG_ARG1("Return   -> wtlsSetKeyRefresh: %s", 
                   status==WTLSC_Ok ? "TRUE" : "FALSE")

	return (status == WTLSC_Ok ? JNI_TRUE : JNI_FALSE);
}


/*
 * Class:     comm_wap_wps_Wtls
 * Method:    wtlsSetRecordLengthUsage
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_comm_wap_wps_Wtls_wtlsSetRecordLengthUsage(
                                                                                     JNIEnv *env, jclass thisClz, jint conn_id, jboolean jusage)
{
    BOOL	wsp_usage;
    tWTLSC_RET status;

    JNI_DEBUG_ARG1("Call   -> wtlsSetRecordLengthUsage to %d", jusage)

	wsp_usage = jusage == JNI_TRUE ? TRUE : FALSE;

    status = WTLSC_SetRecordLengthUsage((WPS_HCONN)conn_id, wsp_usage);

    JNI_DEBUG_ARG1("Return   -> wtlsSetRecordLengthUsage: %s", 
                   status==WTLSC_Ok ? "TRUE" : "FALSE")

	return (status == WTLSC_Ok ? JNI_TRUE : JNI_FALSE);
}


/*
 * Class:     com_nokia_comm_wap_wps_Wtls
 * Method:    wtlsCloseSession
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_comm_wap_wps_Wtls_wtlsCloseSession(
                                                                             JNIEnv *env, jclass thisClz, jint conn_id)
{
    tWTLSC_RET status;

    JNI_DEBUG("Call   -> wtlsCloseSession")

	status = WTLSC_CloseSession((WPS_HCONN)conn_id);

    JNI_DEBUG_ARG1("Return   -> wtlsCloseSession: %s", 
                   status==WTLSC_Ok ? "TRUE" : "FALSE")

	return (status == WTLSC_Ok ? JNI_TRUE : JNI_FALSE);

}


/*
 * Class:     com_nokia_comm_wap_wps_Wtls
 * Method:    wtlsGetHandShakeState
 * Signature: (I)B
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_comm_wap_wps_Wtls_wtlsGetAlert ( 
                                                                          JNIEnv *env, jclass cls, jint conn_id, jbyteArray alert_desc) {
	
    tWTLSC_RET is_alert;
    tWTLSC_Alert alert;
    BYTE *buf;
    jboolean status = JNI_FALSE;

    is_alert = WTLSC_GetAlert ( conn_id, &alert);

    if(is_alert == WTLSC_Ok) {
        buf = malloc( 2* sizeof(BYTE) );
        buf[0] = alert.Level;
        buf[1] = alert.Description;
        (*env)->SetByteArrayRegion(env,alert_desc,0,2,buf);
        free(buf);
        status = JNI_TRUE;
    }
    return status;
}

/*
 * Class:     com_nokia_comm_wap_wps_Wtls
 * Method:    wtlsGetSessionInfo
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_nokia_comm_wap_wps_Wtls_wtlsGetSessionInfo ( 
                                                                            JNIEnv *env, jclass cls, jint conn_id) {

    tWTLSC_RET ret;
    void *sessRef;
    BYTE sid[9];
    WIMI_STAT st;

    ret = WTLSC_GetSession(conn_id, &sessRef, sid);
    if(ret == WTLSC_Ok) {
        st = WIMI_ReadSessionReq( NULL, sessRef);
    }
	free(sessRef);

}
/*
 * Class:     com_nokia_comm_wap_wps_Wtls
 * Method:    wtlsGetHandShakeState
 * Signature: (I)B
 */
JNIEXPORT jbyte JNICALL Java_com_nokia_comm_wap_wps_Wtls_wtlsGetHandShakeState ( 
                                                                                JNIEnv *env, jclass cls, jint conn_id) {

    tWTLSC_ConnState st;
    BYTE ret;

    st = WTLSC_GetConnState ( conn_id );

    switch(st) {
    case WTLSC_Conn_None:
        ret = 0;
        break;
    case WTLSC_Conn_Requesting:
        ret = 1;
        break;
    case WTLSC_Conn_Ok:
        ret = 2;
        break;
    case WTLSC_Conn_Error:
        ret = 3;
        break;
    case WTLSC_Conn_CloseWait:
        ret = 4;
        break;
    case WTLSC_Conn_Closed:
        ret = 5;
        break;
    }
    return ret;
}

/*
 * Class:     com_nokia_comm_wap_wps_Wtls
 * Method:    wtlsSetAlertDestination
 * Signature: (B)V
 */
JNIEXPORT void JNICALL Java_com_nokia_comm_wap_wps_Wtls_wtlsSetAlertDestination (
                                                                                 JNIEnv *env, jclass thisClz, jbyte dest) {

    JNI_DEBUG_ARG1("Call   -> wtlsSetAlertDestination to %d", dest)

	WTLSC_SetAlertDestination( dest );
 
}
/*********************************************************************************************
 *
 *  CallBacks and related native Interfaces
 *
 *********************************************************************************************/

//extern void __stdcall MessageBoxA(void*,char*,char*,int);
void WTLS_ProcessCallBack(tWTLSC_Event e, WPS_HCONN p, void *uData){
    //tWTLSC_RET is_alert;
    //tWTLSC_Alert alert;
    jclass clazz;
    jmethodID mid;
    JNIEnv *env;
    int event_type;


    JNI_DEBUG_ARG1( "Call   -> ProcessCallBack %d\n", (int)e )

	switch(e) {
        case WTLSC_EV_Handshake_Ok:
            event_type = 0;
            break;
        case WTLSC_EV_Handshake_Error:
            event_type = 1;
            break;
        case WTLSC_EV_Handshake_UserAbort:
            event_type = 2;
            break;
        case WTLSC_EV_ConnClosed:
            event_type = 3;
            break;
        case WTLSC_EV_SessionClosed:
            event_type = 4;
            break;
        case WTLSC_EV_Alert:
            event_type = 5;
            break;
	}

	//MessageBoxA(NULL, "java_wtls", "WTLS_ProcessCallback", 0);
    if(g_obj != NULL){

        (*jvm)->AttachCurrentThread(jvm, (void **)&env, NULL);
		clazz = (*env)->FindClass( env, "com/nokia/wap/sdk/http/GatewayLoader" );

		//For some reason GetObjectClass does not fetch the methodID correctly. 
		//What is interesting it does not happen always. Sometimes it fetches
		//it correctly.
        //clazz = (*env)->GetObjectClass( env, g_obj );

        if(clazz != NULL){
            mid = ( *env )->GetMethodID( env, clazz, "WtlsEventHandler" , "(II)V" );
			(*env)->CallVoidMethod( env, g_obj, mid, event_type, (int)p);
        }
    }

	//(*jvm)->DetachCurrentThread(jvm);
}

/******************************************************************/

void WIMI_cb()
{
    JNI_DEBUG( "Call   -> WIMICallBack")
 }

UINT8 WIM_Open( UINT8 reader ) 
{
    return TOOLKIT_WIMAPDU_Open(reader);
}

UINT8 WIM_Close( UINT8 reader ) 
{
    return TOOLKIT_WIMAPDU_Close(reader);
}

void WIM_APDUReq( UINT8 reader,BYTE *apdu,UINT16 apdulen ) 
{

    WIMI_BinData_t ret_apdu;
    UINT16 sw;
    UINT8 status = TOOLKIT_WIMAPDU_SendAPDU( NULL, reader, apdu, apdulen, 
                                             &sw,&ret_apdu);
    WIMI_APDUResp(reader, status, &ret_apdu, sw );
}

void WIM_GetATRReq( UINT8 reader ) 
{
    WIMI_BinData_t bd;

    bd.ui_buf_length = 0;
    bd.pb_buf = "";

    WIMI_GetATRResp( reader,WIMI_APDU_OK,&bd );
}

void WIM_GetReaderListReq() 
{

    WIMI_BinData_t list;
    TOOLKIT_WIMAPDU_RequestList(NULL, &list);
    WIMI_GetReaderListResp(WIMI_APDU_OK,&list);
}

/*******************************************************************/

/**
 * Utility to copy from  BYTE[] to jchar[]
 *
 * @param ch BYTE source 
 * @param jchar destination 
 * @param size size to copy
 */ 
void byteTochar(BYTE* byte, char* ch, long size) 
{
    int i = 0;
    for (i = 0; i < size ; i++ )
        ch[i] = byte[i];
}

void ReadSessionResp(WIMI_TransactId_t tr_id, WIMI_STAT st, UINT8 opt, UINT8 sess_opt, UINT8 id_len,
                     BYTE *id, UINT8 mac, UINT8 bulk, UINT8 comp, WIMI_Ref_t *kes, UINT16 key_size, UINT16 scer_len, 
                     BYTE *scer, WIMI_Ref_t *ClCert) {

    WIMI_WTLS_CertInfo_t certinfo;
    WIMI_STAT status = CME_ERR_CertParseError;
    JNIEnv *env;
    jmethodID mid;
    jclass jcls;
    jstring subject, issuer;
    char *subj, *iss;


    UINT8 kesType = 15, kidLen, keyType;
    int before = -1, after = -1;
	//UINT16 keyUsage, keyLen;
    void *WIMRef, *keyRef;
    BYTE *kid;
	//WIMI_BinData_t hash, label;


    (*jvm)->AttachCurrentThread(jvm, (void **)&env, NULL);
    jcls = (*env)->FindClass(env, "java/lang/String");

    if(st == WIMI_Ok) {

		//Get information about the key exchange suite that was used in WTLS handshake
        if( WIMI_GetKESInfo(kes, &WIMRef, &keyRef, &kesType, &kidLen, &kid) == WIMI_Ok )
            JNI_DEBUG("Get KES Info Ok\n");

		if(keyRef != NULL) {
			/* Key information if client authentication was done. KeyRef will be null if
			   not a WTLS Class 3 connection */
			//WIMI_GetKeyInfo(keyRef, &WIMRef, NULL, &keyType, &keyUsage, &hash, &label, &keyLen);  
		}
		else
			key_size = 0xcdcd; //since no key is used, set key size to uninitialised value.
		                        //strangely stack sometimes returns arbitrary values.

        if((scer_len > 0) && (scer != NULL)) {
            status = WIMI_ParseWTLSCert(scer_len, scer+3, &certinfo);
        }

        if(status == CME_Ok) {

            before = certinfo.ValidNotBefore;
            after = certinfo.ValidNotAfter;

            subj = malloc(((certinfo.Subject)[3] + 1) * sizeof(char));
            iss = malloc(((certinfo.Issuer)[3] + 1)* sizeof(char));
			
            byteTochar((certinfo.Subject)+4, subj, (certinfo.Subject)[3]);
            subj[(certinfo.Subject)[3]] = '\0';
            byteTochar((certinfo.Issuer)+4, iss, (certinfo.Issuer)[3]);
            iss[(certinfo.Issuer)[3]] = '\0';
			
            subject = (*env)->NewStringUTF(env, subj);
            issuer = (*env)->NewStringUTF(env, iss);

        }
        else {
			before = -1;
            after = -1;

            subj = malloc(strlen("No certificate available"));
            iss = malloc(strlen("No certificate available"));
            subj = "No certificate available";
            iss = "No certificate available";

            subject = (*env)->NewStringUTF(env, subj);
            issuer = (*env)->NewStringUTF(env, iss);

        }

        jcls = (*env)->GetObjectClass( env, g_obj );

        if(jcls != NULL){
            mid = ( *env )->GetMethodID( env, jcls,"WtlsSessionInfo", 
                                         "(BBBCLjava/lang/String;Ljava/lang/String;II)V" );
            if(mid != NULL)
                (*env)->CallObjectMethod( env, g_obj, mid, bulk, mac, kesType, key_size, 
                                          subject, issuer, before, after);
        }

    }

	//(*jvm)->DetachCurrentThread(jvm);

}

void InitResp( WIMI_STAT status) {
    jclass jcls;
    JNIEnv *env;
    jmethodID mid;
    jboolean st;

    if(status == WIMI_Ok)
        st = JNI_TRUE;
    else st = JNI_FALSE;

    JNI_DEBUG_ARG1("WIMI_InitResp_t called %d", status);
    (*jvm)->AttachCurrentThread(jvm, (void **)&env, NULL);
    //jcls = (*env)->GetObjectClass( env, g_obj );
    jcls = (*env)->FindClass(env, "com/nokia/comm/wap/wps/Wtls");
    /*if(jcls != NULL){
      mid = ( *env )->GetMethodID( env, jcls, "WtlsSetWimRunning" , "(Z)V" );
      (*env)->CallVoidMethod( env, g_obj, mid, st);
      }*/

    if(jcls != NULL){
        mid = ( *env )->GetStaticMethodID( env, jcls, "WtlsSetWimRunning" , "(Z)V" );
        (*env)->CallStaticVoidMethod( env, jcls, mid, st);
    }

	//(*jvm)->DetachCurrentThread(jvm);
}


void ClAcceptAuthReq(WPS_HCONN conn, UINT8 certtype, 
                            UINT16 certlen, BYTE *cert) 
{
//TODO:User feedback
    WIMI_ClAcceptAuthResp( conn,TRUE);
}  

void ClCertSelectReq(WPS_HCONN conn_id, WIMI_Ref_t *pKeyRef, UINT8 num, WIMI_RefList_t certList) {

    char ** cert_labels;
    WIMI_BinData_t **lab_list;
    WIMI_STAT wim_stat;
    int i, selCert;
    //UINT8 num;
    jobjectArray labelArray;
    jclass jcls;
    jstring label;
    JNIEnv *env;
    jmethodID mid;

    JNI_DEBUG( "Call   -> WIMICallBack: ClCertSelectReq")

	//num = certNum;
	(*jvm)->AttachCurrentThread(jvm, (void **)&env, NULL);
    jcls = (*env)->FindClass(env, "java/lang/String");
    labelArray = (*env)->NewObjectArray(env, num, jcls, NULL);	
	
    //Get Certificate labels from certlist
    wim_stat = WIMI_GetLabelList(num, certList, &lab_list);
    if (wim_stat == WIMI_Ok)
	{
            cert_labels = malloc(sizeof(char*) * num);
            if(cert_labels != NULL) {
                for (i = 0; i < num; i++)
                    {
                        cert_labels[i]=malloc((sizeof(char))*(lab_list[i]->ui_buf_length));
                        if (cert_labels != NULL) {
                            byteTochar(lab_list[i]->pb_buf,cert_labels[i], lab_list[i]->ui_buf_length);
                            label = (*env)->NewStringUTF(env, cert_labels[i]);
                            (*env)->SetObjectArrayElement(env, labelArray, i, label);
                        }
                    }
            }
            free_WIMI_labellist_t(lab_list);
            (*env)->DeleteLocalRef(env, label);
	}


    //Call the JAVA selection dialog with the above labels list in labelArray	
    jcls = (*env)->GetObjectClass( env, g_obj );
    if(jcls != NULL){
        mid = ( *env )->GetMethodID( env, jcls, "WtlsSelectClientCert" , "([Ljava/lang/String;)I" );
		
        if(mid != NULL)
            JNI_DEBUG( "Call   -> WIMICallBack: ClCertSelectReq mid not null")
		selCert = (*env)->CallIntMethod( env, g_obj, mid, labelArray);
    }

    if(selCert >= 0)
	//Inform the stack about the selected certificate
        WIMI_ClCertSelectedResp(conn_id, certList[selCert]);
    else
        WIMI_ClCertSelectedResp(conn_id, NULL);

	//(*jvm)->DetachCurrentThread(jvm);
}

/*void EnterPIN(WPS_HCONN conn_id, char *pin_label, WIMI_Ref_t *pPINref) {

jstring pinLabel;
JNIEnv *env;
jmethodID mid;
jclass jcls;
jstring pin;
struct WIM_Transaction tr;
int i;
jboolean isCopy;
const char *pinChars;
BYTE *wim_pin;
UINT8 pin_len;
WIMI_STAT wim_stat;

(*jvm)->AttachCurrentThread(jvm, (void **)&env, NULL);
pinLabel = (*env)->NewStringUTF(env, pin_label);

//Call the JAVA prompt dialog with this label	
jcls = (*env)->GetObjectClass( env, g_obj );
if(jcls != NULL){
mid = ( *env )->GetMethodID( env, jcls, "WtlsPromptPIN" , "(Ljava/lang/String;)Ljava/lang/String;" );
pin = (jstring)(*env)->CallObjectMethod( env, g_obj, mid, pinLabel);
}

pinChars = (*env)->GetStringUTFChars(env, pin, &isCopy);
pin_len = ((*env)->GetStringUTFLength(env, pin));


wim_pin = (BYTE *)malloc(pin_len);
for(i = 0; i < pin_len; i++)
wim_pin[i] = pinChars[i];
	
if(isCopy == JNI_TRUE)
(*env)->ReleaseStringUTFChars(env, pin, pinChars);
//verify the PIN
tr.id = conn_id;
wim_stat = WIMI_VerifyPINReq(&tr, pPINref, pin_len, wim_pin);
}*/

void PINEnterReq(WPS_HCONN conn_id, WIMI_Ref_t *pWIMref) {
    //JNI variables
    JNIEnv *env;
    jmethodID mid;
    jclass jcls;
    jstring pinLabel;
    jstring pin;
    jboolean isCopy;

    struct WIM_Transaction tr;
    int i;
    const char *pinChars;
    BYTE *wim_pin;
    UINT8 pin_len;

    //variables for GetWIMInfo
    UINT16 flags;
    UINT8 se;
    WIMI_BinData_t wim_id, man_id, label;
    UINT8 reader_id, version;
    WIMI_Ref_t *pPINref;
    BOOL sim;

    //PIN label
    void* ref_list[1];
    WIMI_BinData_t **lab_list;
    char * pin_label;
    WIMI_STAT wim_stat;


    JNI_DEBUG( "Call   -> WIMICallBack: PINEnterReq")

	//Get PIN ref from WIM ref. You have get WIM Info and then get the PIN ref from there.
	WIMI_GetWIMInfo(pWIMref, &flags, &se, &wim_id, &man_id, &label, &reader_id, &pPINref, &sim, &version);

    //Get the PIN label from PIN ref
    (*jvm)->AttachCurrentThread(jvm, (void **)&env, NULL);
    //jcls = (*env)->FindClass(env, "java/lang/String");
	
    ref_list[0] = pPINref;
    wim_stat = WIMI_GetLabelList(1, ref_list, &lab_list);
    printf("in pin enter req %d\n", wim_stat); 
    if (wim_stat == WIMI_Ok)
	{
            pin_label=malloc((sizeof(char))*((lab_list[0]->ui_buf_length) + 1));
            if (pin_label != NULL) {
                byteTochar(lab_list[0]->pb_buf, pin_label, lab_list[0]->ui_buf_length);
                pin_label[lab_list[0]->ui_buf_length] = '\0';
                pinLabel = (*env)->NewStringUTF(env, pin_label);
            }
            free_WIMI_labellist_t(lab_list);
	}

    //pinLabel = (*env)->NewStringUTF(env, pin_label);

    //Call the JAVA prompt dialog with this label	
    jcls = (*env)->GetObjectClass( env, g_obj );
    if(jcls != NULL){
        mid = ( *env )->GetMethodID( env, jcls, "WtlsPromptPIN" , "(Ljava/lang/String;)Ljava/lang/String;" );
        pin = (jstring)(*env)->CallObjectMethod( env, g_obj, mid, pinLabel);
    }

    pinChars = (*env)->GetStringUTFChars(env, pin, &isCopy);
    pin_len = ((*env)->GetStringUTFLength(env, pin));


    wim_pin = (BYTE *)malloc(pin_len);
    for(i = 0; i < pin_len; i++)
        wim_pin[i] = pinChars[i];
	
    if(isCopy == JNI_TRUE)
        (*env)->ReleaseStringUTFChars(env, pin, pinChars);
    //verify the PIN
    tr.id = conn_id;
    tr.pWIMref = pWIMref;
    wim_stat = WIMI_VerifyPINReq(&tr, pPINref, pin_len, wim_pin);

    //EnterPIN(conn_id, pin_label,pPINref);

	//(*jvm)->DetachCurrentThread(jvm);
    free(pin_label);

}

void ServerCertCheckreq(WPS_HCONN conn_id, UINT8 reason, UINT8 cert_type, UINT16 cert_len,
                        BYTE *cert) {
	
    int res;
    BOOL accepted;
    //JNI variables
    JNIEnv *env;
    jmethodID mid;
    jclass jcls;
	
    JNI_DEBUG( "Call   -> WIMICallBack: ServerCertCheckreq")
	//call the java callback which prompts user
	(*jvm)->AttachCurrentThread(jvm, (void **)&env, NULL);
    jcls = (*env)->GetObjectClass( env, g_obj );
    if(jcls != NULL){
        mid = ( *env )->GetMethodID( env, jcls, "WtlsServerCertCheck" , "(I)I" );
        res = (*env)->CallIntMethod( env, g_obj, mid, reason);
    }

    //Indicate user response to the stack
    if(res==0)
        accepted = TRUE;
    else
        accepted = FALSE;

    JNI_DEBUG_ARG1("ServerCertCheckResp %d", res)

	WIMI_ServerCertCheckResp(conn_id, accepted);
	//(*jvm)->DetachCurrentThread(jvm);

}

void VerifyPINResp( WIMI_TransactId_t tr, WIMI_STAT status) {
    WPS_HCONN conn_id;
    WIMI_Ref_t *pWIMref;
    WIMI_STAT st;

	conn_id = ((struct WIM_Transaction *)tr)->id;
    //Check status. If verified  continue else prompt again for PIN if Bad PIN.
    if(status == WIMI_Ok) {
        st = WIMI_PINEnteredResp(conn_id, TRUE);
        JNI_DEBUG_ARG1(" Return -> WIMI_PINEnteredResp %d", st)
	}
    else if(status == WIMI_ERR_BadPIN) {
		pWIMref = ((struct WIM_Transaction *)tr)->pWIMref;
        PINEnterReq(conn_id, pWIMref);
    }
    else {
        st = WIMI_PINEnteredResp(conn_id, FALSE);
        JNI_DEBUG_ARG1(" Return -> WIMI_PINEnteredResp %d", st)
	}
}

/*
 * Class:     com_nokia_comm_wap_wps_Wtls
 * Method:    wimiInitializeReq
 * Signature: ()Z
 */
JNIEXPORT void JNICALL Java_com_nokia_comm_wap_wps_Wtls_wimiInitializeReq (
                                                                           JNIEnv *env, jclass thisClz) {

    WIMI_Callbacks_t WIMIcbs = 
	{ 
            InitResp,
            (WIMI_CloseDownResp_t)WIMI_cb,
            PINEnterReq,
            ServerCertCheckreq,
			ClAcceptAuthReq,
            ClCertSelectReq,
            (WIMI_SignResp_t)WIMI_cb,
            (WIMI_CertificateResp_t)WIMI_cb,
            (WIMI_CertificateDeleteResp_t)WIMI_cb,
            (WIMI_CertificateStoreResp_t)WIMI_cb,
            (WIMI_ReadPeerResp_t)WIMI_cb,
            (WIMI_WritePeerResp_t)WIMI_cb,
            (WIMI_DeletePeerResp_t)WIMI_cb,
            //(WIMI_ReadSessionResp_t)WIMI_cb,
            ReadSessionResp,
            (WIMI_SetSessionNotValidResp_t)WIMI_cb,
            VerifyPINResp,
            (WIMI_ChangePINResp_t)WIMI_cb,
            (WIMI_UnblockPINResp_t)WIMI_cb,
            (WIMI_EnablePINResp_t)WIMI_cb,
            (WIMI_CardInsertedResp_t)WIMI_cb,
            /*WIMAPDU_Open,
              WIMAPDU_Close,
              WIM_APDUReq,
              WIM_GetATRReq,
              WIM_GetReaderListReq*/
            WIM_Open,
            WIM_Close,
            WIM_APDUReq,
            WIM_GetATRReq,
            WIM_GetReaderListReq
	};

    JNI_DEBUG("Call -> wimiInitializeReq");

    WIMI_InitializeReq(&WIMIcbs);

}

/*
 * Class:     com_nokia_comm_wap_wps_Wtls
 * Method:    wimiCloseDownReq
 * Signature: ()Z
 */
JNIEXPORT void JNICALL Java_com_nokia_comm_wap_wps_Wtls_wimiCloseDownReq (
                                                                          JNIEnv *env, jclass thisClz) {

    WIMI_STAT st;
    JNI_DEBUG("Call -> CloseDownReq");

    st = WIMI_CloseDownReq();

    JNI_DEBUG_ARG1("CloseDownReq exit %d", st)
}


/*
 * Class:     com_nokia_wap_sdk_http_GatewayLoader
 * Method:    registerNative
 */
JNIEXPORT void JNICALL Java_com_nokia_wap_sdk_http_GatewayLoader_registerNativeObject(
                                                                                      JNIEnv *env, jobject obj) {

    JNI_DEBUG( "Call   -> registerNativeObject\n" )
	g_obj = ( *env )->NewWeakGlobalRef( env, obj );
    //(*env)->GetJavaVM(env, &jvm);
	
}

/*
 * Class:     com_nokia_wap_sdk_http_GatewayLoader
 * Method:    registerNative
 */
JNIEXPORT void JNICALL Java_com_nokia_comm_wap_wps_Wtls_registerNativeVM (
                                                                          JNIEnv *env, jclass jcls) {

    JNI_DEBUG( "Call   -> registerNativeVM\n" )
	(*env)->GetJavaVM(env, &jvm);
	
}

/*
 * Class:     com_nokia_comm_wap_wps_Wtls
 * Method:    wtlsRegisterEventCallback
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_comm_wap_wps_Wtls_wtlsRegisterEventCallback (
                                                                                       JNIEnv *env, jclass thisClz, jint conn_id ) {

    tWTLSC_RET status = WTLSC_Ok;
    //WPS_HCONN conn_id;

    JNI_DEBUG_ARG1( "Call   -> wtlsRegisterEventCallback: %d \n", conn_id )

	status = WTLSC_Register_Notify_CB ( (WPS_HCONN)conn_id, WTLS_ProcessCallBack );

    JNI_DEBUG_ARG1("Return   -> wtlsRegisterEventCallback: %s", 
                   status == WTLSC_Ok ? "TRUE" : "FALSE")
   
	return (status == WTLSC_Ok ? JNI_TRUE : JNI_FALSE);
}
/*
 * Local Variables:
 * c-basic-offset:4
 * indent-tabs-mode: nil
 * End:
 */
