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




package com.nokia.wtk.device;


/**
 * A listener for WTLS events.
 * This interface will be implemented by the hosting application.
 *
 */
//NB: RB Must implement all methods as new hooks
public interface DeviceWTLSListener {

    /**
     * Called when a WTLS session has been opened.
     * @param session         the session information object
     */
    public void sessionOpened(SessionInfo session);

    /**
     * Called when the WTLS session has been closed.
     * @param sessionID         the ID of the secure session
     */
    public void sessionClosed(int sessionID);

    /**
     * Called upon receiving an WTLS alert
     * @param sessionID the ID of the secure session
     * @param level     the alert level, can be one of {ALERT_WARNING,
     *                                                  ALERT_CRITICAL,
     *                                                  ALERT_FATAL,
     *                                                  ALERT_NORMAL}
     * @param code      alert code as defined WAP WTLS <code>AlertDescription</code>
     *                  structure, for ALERT_NORMAL levels code=256 can be used.
     * @param message   the WTLS alert message
     */
    public void alert(int sessionID, int level, int code, String message);

    // WTLS Alert Levels:

    /**
     * Specifies an normal alert level. This level is user for receiving WTLS
     * normal operation messages.
     */
    public static final int ALERT_NORMAL = 0;

    /**
     * Specifies an warning alert level.
     */
    public static final int ALERT_WARNING = 1;

    /**
     * Specifies an critical alert level.
     */
    public static final int ALERT_CRITICAL = 2;

    /**
     * Specifies an fatal alert level.
     */
    public static final int ALERT_FATAL = 3;

    /**
     * A WTLS Session info object.
     */
    public interface SessionInfo {

        /**
         * Gets the ID of the secure session.
         * @return      the int ID of the secure session.
         */
        public int getSessionID();

        /**
         * Gets the name of algorithm used in this connectoin for the specified algorithm type.
         * @param type  the int type of the algorithm, any one of:
         *              {ALGORITHM_BULK,
         *               ALGORITHM_MAC,
         *               ALGORITHM_KEY_EXCHANGE,
         *               ALGORITHM_COMPRESSION}
         * @return      the name of the algorithm.
         *              <code>null</code> if invalid type or if no algorithm used.
         */
        public String getAlgorithm(int algorithmType);

        /**
         * Gets the size of the key used in this connectoin for the specified algorithm type.
         * @param type  the int type of the algorithm, any one of:
         *              {ALGORITHM_BULK,
         *               ALGORITHM_MAC,
         *               ALGORITHM_KEY_EXCHANGE,
         *               ALGORITHM_COMPRESSION}
         * @return      the size of the key in bits.
         *              <code>-1</code> if key size is not applicable with the algorithm type.
         */
        public int getKeySize(int algorithmType);

        /**
         * Gets the certificate used in this connection for the specified certificate type.
         * @param type  the int type of the certificate, any one of:
         *              {CERTIFICATE_SERVER,
         *               CERTIFICATE_CLIENT,
         *               CERTIFICATE_CA}
         *
         * @return      the Certificate used for the connection.
         *              <code>null</code> if certificate not used for this connection.
         * @see DeviceWIM.Certificate
         */
        public DeviceWIM.Certificate getCertificate(int certType);

        /**
         * Gets the WTLS protocol version used for this connection.
         * @return      the int WTLS protocol version.
         */
        public int getVersion();

        /**
         * Gets the WTLS implementation class used for this connection.
         * @return      the int WTLS class number, any one of:
         *              {CLASS_ONE,
         *               CLASS_TWO,
         *               CLASS_THREE}
         */
        public int getClassType();

        /**
         * Gets the key refresh frequency used for this connection.
         * @return      the key refresh frequency (units?).
         */
        public int getKeyRefresh();

        /**
         * Gets the sequence numbering mode used for this connection.
         * @return     the int sequence numbering mode, any one of:
         *             {SEQUENCE_NUMBER_IMPLICIT,
         *              SEQUENCE_NUMBER_EXPLICIT,
         *              SEQUENCE_NUMBER_OFF}
         */
        public int getSequenceNumberingMode();

        /**
         * Gets the handshake type used for this connection.
         * @return     the int handshake type, any one of:
         *             {HANDSHAKE_FULL,
         *              HANDSHAKE_ABBREVIATED,
         *              HANDSHAKE_OPTIMISED,
         *              HANDSHAKE_SHARED_SECRET}
         */
        public int getHandshakeType();

        // Sequence Number Mode:

        /**
         * Specifies an implicit sequence numbering.
         */
        public static final int SEQUENCE_NUMBER_IMPLICIT = 1;

        /**
         * Specifies an explicit sequence numbering.
         */
        public static final int SEQUENCE_NUMBER_EXPLICIT = 2;

        /**
         * Specifies that no sequence numbering is used.
         */
        public static final int SEQUENCE_NUMBER_OFF = 3;

        // WTLS Implementation Classes:

        /**
         * Specifies the WTLS class 1 implementation.
         */
        public static final int CLASS_ONE = 1;

        /**
         * Specifies the WTLS class 2 implementation.
         */
        public static final int CLASS_TWO = 2;

        /**
         * Specifies the WTLS class 3 implementation.
         */
        public static final int CLASS_THREE = 3;

        // Handshake Types:

        /**
         * Specifies full handshake usage for the secure connection.
         */
        public static final int HANDSHAKE_FULL = 1;

        /**
         * Specifies abbreviated handshake usage for the secure connection.
         */
        public static final int HANDSHAKE_ABBREVIATED = 2;

        /**
         * Specifies optimised handshake usage for the secure connection.
         */
        public static final int HANDSHAKE_OPTIMISED = 3;

        /**
         * Specifies shared secret handshake usage for the secure connection.
         */
        public static final int HANDSHAKE_SHARED_SECRET = 4;

        // Algorithm Types:

        /**
         * Specifies bulk encryption algorithm.
         */
        public static final int ALGORITHM_BULK = 1;

        /**
         * Specifies MAC algorithm.
         */
        public static final int ALGORITHM_MAC = 2;

        /**
         * Specifies Key exchange suite algorithm.
         */
        public static final int ALGORITHM_KEY_EXCHANGE = 3;

        /**
         * Specifies compression encryption.
         */
        public static final int ALGORITHM_COMPRESSION = 4;

        // Certificate Types:

        /**
         * Specifies server certificate type.
         */
        public static final int CERTIFICATE_SERVER = 1;

        /**
         * Specifies client certificate type.
         */
        public static final int CERTIFICATE_CLIENT = 2;

        /**
         * Specifies CA certificate type.
         */
        public static final int CERTIFICATE_CA = 3;

    } // End of SessionInfo


}   // End of DeviceWTLSListener
