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
 * An interface to WIM services.
 * This interface must be implemented by a device simulator component,
 * if it is to support WIM services.
 *
 * For more information, see <a href=http://www.wapforum.org>WAP WIM specification</a>.
 *
 */
public interface DeviceWIM {

    /**
     * Gets the WIM service status.
     * @return          true if WIM service is started and running; false otherwise.
     */
    public boolean isServiceStarted();

    /**
     * Enables or disables the WIM present in the specified reader.
     * @param rdr       the WIM reader ID
     * @param enable    if true, enables the specifed WIM; else disables the WIM
     * @return          int <code>status word</code> as defined in WAP WIM#11.3.8
     */
    public int setWIMEnabled(int rdr, boolean enable);

    /**
     * Gets the IDs of the currently active WIM readers.
     * To distinguish between currently active WIMs,
     * each WIM reader is assigned a unique ID number.
     * @return          the set of WIM reader ID's. Return value array
     *                  length indicates the currently running WIM's count.
     */
    public int[] getWimReaders();

    /**
     * Gets the information of the WIM present in the specified reader.
     * @param rdr the WIM reader ID
     * @return    the WIM Token information in the following String array format
     *            [0] - WIM version
     *            [1] - Serial number that uniquely identifies the device (serialNumber)
     *            [2] - Manufacturer information (manufacturerID)
     *            [3] - Application label (tokenInfo.label)
     *            [4] - Names of predefined security environments,with carriage return as delimiter
     *            [5] - Names of supported algorithms, with carriage return as delimiter
     *            [6] - Names of supported token flags, with carriage return as delimiter
     *            An array element can be <code>null</code> if corresponding information is
     *            not present or not applicable. The encoding of the String's are as specified
     *            in WAP WIM specifications.
     */
    public String[] getWimInfo(int rdr);

    /**
     * Gets the IDs of all the certificates available in the specified WIM reader.
     * To distinguish between currently available certificates in a WIM,
     * each certificate is assigned an unique certificate ID number.
     * @param rdr       the WIM reader ID
     * @return          the set of certificate IDs.
     */
    public int[] getCertificates(int rdr);

    /**
     * Gets a WIM certificate object.
     * @param rdr       the WIM reader ID
     * @param cert      the certificate ID
     * @return          the specified certificate or <code>null</code> if not found.
     */
    public Certificate getCertificate(int rdr, int cert);

    /**
     * Gets the IDs of all the keys present in the specified WIM reader.
     * To distinguish between currently available keys in a WIM,
     * each key is assigned an unique key ID number.
     * @param rdr       the WIM reader ID
     * @return          the set of key IDs.
     */
    public int[] getKeys(int rdr);

    /**
     * Gets a WIM key object.
     * @param rdr       the WIM reader ID
     * @param key       the certificate key ID
     * @return          the specified certificate key or <code>null</code> if not found.
     */
    public Key getKey(int rdr, int key);

    /**
     * Gets the IDs of all the PINs present in the specified WIM reader.
     * To distinguish between currently available PINs in a WIM,
     * each PIN is assigned an unique PIN ID number.
     * @param rdr       the WIM reader ID
     * @return          the set of PIN IDs.
     */
    public int[] getPINs(int rdr);

    /**
     * Gets a WIM PIN object.
     * @param rdr       the WIM reader ID
     * @param pin       the PIN ID
     * @return          the specified PIN or <code>null</code> if not found.
     */
    public PIN getPIN(int rdr, int pin);

    /**
     * Gets the ID of the general PIN present in the specified WIM reader.
     * @param rdr       the WIM reader ID
     * @return          ID of the general PIN.
     */
    public int getGeneralPIN(int rdr);

    /**
     * Adds a certificate to the specified WIM.
     * @param rdr       the WIM reader ID
     * @param cert      certificate to be saved
     * @return          int <code>status word</code> as defined in WAP WIM#11.3.8
     */
    public int addCertificate(int rdr, Certificate cert);

    /**
     * Removes a certificate from the specified WIM.
     * @param rdr       the WIM reader ID
     * @param od        the ID of the certificate to be deleted
     * @return          int <code>status word</code> as defined in WAP WIM#11.3.8
     */
    public int removeCertificate(int rdr, int id);

    /**
     * Adds a key to the specified WIM.
     * @param rdr       the WIM reader ID
     * @param key       the key to be saved
     * @return          int <code>status word</code> as defined in WAP WIM#11.3.8
     */
    //NB: Not in WIMI
    public int addKey(int rdr, Key key);

    /**
     * Removes a key from the specified WIM.
     * @param rdr       the WIM reader ID
     * @param id        the ID of the key to be deleted
     * @return          int <code>status word</code> as defined in WAP WIM#11.3.8
     */
    //NB: Not in WIMI
    public int removeKey(int rdr, int id);

    /**
     * Changes or unblocks a PIN in the specified WIM.
     * @param rdr       the WIM reader ID
     * @param pin       the ID of the the PIN whose value to be changed
     * @param oldVal    old value of the PIN
     *                  (or master PIN if <code>master</code> is <code>true</code>)
     * @param newVal    new value of the PIN
     * @param master    if <code>true</code> then treat <code>oldValue</code>
     *                  as the master PIN value. (used to "unblock" a PIN)
     * @return          int <code>status word</code> as defined in WAP WIM#11.3.8
     */
    public int changePIN(int rdr,
                         int pin,
                         int oldVal,
                         int newVal,
                         boolean master);

    /**
     * Enables or disables the PIN of the specified WIM.
     * @param rdr the WIM reader ID
     * @param pin the ID of the the PIN to be enabled
     * @param value value of the PIN
     * @param enable    if true, enables the specifed PIN; else disables the PIN
     * @return          int <code>status word</code> as defined in WAP WIM#11.3.8
     */
    public int enablePIN(int rdr, int pin, int value, boolean enable);

    /**
     * Verifies the PIN of the specified WIM.
     * @param rdr the WIM reader ID
     * @param pin the ID of the the PIN to be enabled
     * @param value value of the PIN
     * @return          int <code>status word</code> as defined in WAP WIM#11.3.8
     */
    public int verifyPIN(int rdr, int pin, int value);

    /**
     * A WIM certificate object.
     */
    public interface Certificate {

        /**
         * Gets the label for WIM certificate.
         * @return      the String Label of the WIM certificate.
         *              Returns <code>null<\code> for a non-WIM storage cert.
         */
        public String getLabel();

        /**
         * Gets the key hash.
         * @return      the 20 byte SHA-1 hash of the public key
         */
        public byte[] getKeyHash();

        /**
         * Gets the key hash for the issuer.
         * @return      the 20 byte SHA-1 hash of the issuer's public key
         */
        public byte[] getIssuerKeyHash();

        /**
         * Gets the usage of the certificate.
         * @return      any one of the DeviceWIM.Certificate{USAGE_USER,
         *                                                   USAGE_TRUSTED_CA,
         *                                                   USAGE_USEFUL_CA}
         */
        public int getUsage();

        /**
         * Gets the format of the certificate data
         * @return      any one of the DeviceWIM.Certificate{FORMAT_URL,
         *                                                   FORMAT_X509,
         *                                                   FORMAT_WTLS,
         *                                                   FORMAT_968}
         */
        public int getFormat();

        /**
         * Gets the encoded certificate data.
         * The format of the data depends on the certificate type
         * @return      the certificate data
         */
        public byte[] getData();


        // Certificate formats:

        /**
         * Specifies that the format of certificate is
         * X509v3 as defined in the WAP Certificate Profiles specification.
         */
        public static final int FORMAT_X509 = 1;

        /**
         * Specifies that the format of certificate is
         * WTLS as defined in WAP WTLS specification.
         */
        public static final int FORMAT_WTLS = 2;

        /**
         * Specifies that the format of certificate is
         * X9.68 as defined in WAP Certificate Profiles specification.
         */
        public static final int FORMAT_X968 = 3;

        /**
         * Specifies that the format of certificate is
         * a URL (URL-encoded) that specifies the location of certificate data.
         */
        public static final int FORMAT_URL = 4;

        // Certificate Usage(Type):

        /**
         * Specifies that the certificate is the WIM user certificate.
         */
        public static final int USAGE_USER = 1;

        /**
         * Specifies that the certificate is non-modifiable (root) CA certifcate.
         */
        public static final int USAGE_TRUSTED_CA = 2;

        /**
         * Specifies that the certificate is modifiable CA certifcate.
         */
        public static final int USAGE_USEFUL_CA = 3;


    }   // End of DeviceWIM.Certificate


    /**
     * A WIM key object.
     */
    public interface Key {

        /**
         * Gets the label for this WIM key.
         * @return      the label of this WIM key.
         */
        public String getLabel();

        /**
         * Gets the key hash.
         * @return      the 20 byte SHA-1 hash of public key
         */
        public byte[] getKeyHash();

        /**
         * Gets the key type
         * @return      any one of DeviceWIM.Key.{TYPE_RSA, TYPE_ECC}
         */
        public int getType();

        /**
         * Gets the PKCS15 Key Usage Flags.
         * <p>
         * PKSS15KeyUsageFlags is defined by PKCS #15: Cryptographic Token
         * Information Standard", version 1.0, RSA Laboratories, April 1999.
         * URL: ftp://ftp.rsa.com/pub/pkcs/pkcs-15/pkcs15v1.doc as
         * <p>
         * <pre>
         *   PKCS15KeyUsageFlags ::= BIT STRING {
         *           encrypt         (0),
         *           decrypt         (1),
         *           sign            (2),
         *           signRecover     (3),
         *           wrap            (4),
         *           unwrap          (5),
         *           verify          (6),
         *           verifyRecover   (7),
         *           derive          (8),
         *           nonRepudiation  (9)
         *           }
         * </pre>
         * @return      the PKCS15KeyUsageFlags
         */
        public int getFlags();

        /**
         * Gets the public key length in bits.
         * For RSA key it returns the modulus length.
         * @return      the length of the key
         */
        public int getLength();

        /**
         * Gets the PIN ID for this key
         * @return      the PIN ID for this key
         */
        public int getPinId();

        /**
         * Gets the encoded key data.
         * <p>
         * For RSA keys data returned is <code>PKCS15RSAPublicKey</code> bytes
         * followed by <code>PKCS15RSAPrivateKey</code> bytes. For ECC keys data
         * returned is <code>PKCS15ECPublicKey</code> bytes followed by
         * <code>PKCS15ECPrivateKey</code> bytes. PKCS15 attributes are defined in
         * PKCS #15: Cryptographic Token Information Standard", version 1.0, RSA
         * Laboratories, April 1999.
         * URL: ftp://ftp.rsa.com/pub/pkcs/pkcs-15/pkcs15v1.doc as
         * </p>
         * <pre>
         *      PKCS15RSAPublicKey ::= SEQUENCE {
         *        modulus         INTEGER, -- n
         *        publicExponent  INTEGER  -- e
         *      }
         * </pre>
         * <pre>
         *      PKCS15RSAPrivateKey ::= SEQUENCE {
         *        modulus         [0] INTEGER OPTIONAL, -- n
         *        publicExponent  [1] INTEGER OPTIONAL, -- e
         *        privateExponent [2] INTEGER OPTIONAL, -- d
         *        prime1          [2] INTEGER OPTIONAL, -- p
         *        prime2          [3] INTEGER OPTIONAL, -- q
         *        exponent1       [4] INTEGER OPTIONAL, -- d mod (p-1)
         *        exponent2       [5] INTEGER OPTIONAL, -- d mod (q-1)
         *        coefficient     [6] INTEGER OPTIONAL  -- inv(q) mod p
         *        } (CONSTRAINED BY
         *             {-- must be possible to reconstruct modulus and
         *              -- privateExponent from selected fields --})
         *             }
         * </pre>
         * <pre>
         *      PKCS15ECPublicKey ::= PKCS15ECPoint
         *      PKCS15ECPoint       ::= ECPoint -- See ANSI X9.62
         * </pre>
         * <pre>
         *      PKCS15ECPrivateKey ::= INTEGER
         * </pre>
         *
         * @return  the key data
         */
        public byte[] getData();

        // KEY Types:

        /**
         * Specifies that the key type is RSA.
         */
        public static final int TYPE_RSA = 1;

        /**
         * Specifies that the key type is ECC.
         */
        public static final int TYPE_ECC = 2;

    }   // End of DeviceWIM.Key


    /**
     * A WIM PIN object.
     */
    public interface PIN {

        /**
         * Gets the label for WIM PIN.
         * @return      the String Label of the WIM PIN.
         */
        public String getLabel();

        /**
         * Gets the PKCS15 Pin Flags.
         * <p>
         * PKCS15PinFlags is defined by PKCS #15: Cryptographic Token
         * Information Standard", version 1.0, RSA Laboratories, April 1999.
         * URL: ftp://ftp.rsa.com/pub/pkcs/pkcs-15/pkcs15v1.doc as
         * <p>
         * <pre>
         *   PKCS15PinFlags ::= BIT STRING {
         *           case-sensitive   (0),
         *           local            (1),
         *           change-disabled  (2),
         *           unblock-disabled (3),
         *           initialized      (4),
         *           needs-padding    (5),
         *           unblockingPin    (6),
         *           soPin            (7),
         *           disable-allowed  (8)
         *           }
         * </pre>
         * @return      the PKCS15 Pin Flags
         */
        public int getFlags();

        /**
         * Gets the minimum PIN length
         * @return      the minimum PIN length
         */
        public int getMinimumLength();

    }   // End of DeviceWIM.PIN

}   //End of DeviceWIM

