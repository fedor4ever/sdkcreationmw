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



package com.nokia.mms;

import java.text.DateFormat;
import java.text.ParseException;

import java.util.Calendar;
import java.util.Enumeration;
import java.util.Locale;
import java.util.StringTokenizer;
import java.util.TimeZone;
import java.util.Vector;

import com.nokia.wtk.http.InternetDateFormat;
import com.nokia.wtk.http.HeaderCharEncoding;

/**
 * A class designed to mediate between String representations of
 * MMS headers and their encodings in an original MMS message.
 *
 * MMHeader is the generice interface.
 *
 * A set of general derived interface types (String, Boolean, etc.) follow,
 * finally instance classes of each header type, which constrains the header values.
 *
 */
public abstract class MMHeader {

    public static final String ADDR_SEPARATOR_TOKEN = ",";

    /* 
     * String versions of header names
     */
    public static final String N_MESSAGE_TYPE = "X-Mms-Message-Type";
    public static final String N_TRANSACTION_ID = "X-Mms-Transaction-ID";
    public static final String N_MMS_VERSION = "X-Mms-MMS-Version";
    public static final String N_MESSAGE_ID = "Message-ID";
    public static final String N_DATE = "Date";
    public static final String N_TO = "To";
    public static final String N_CC = "Cc";
    public static final String N_BCC = "Bcc";
    public static final String N_SUBJECT = "Subject";
    public static final String N_FROM = "From";
    public static final String N_MESSAGE_CLASS = "X-Mms-Message-Class";
    public static final String N_MESSAGE_SIZE = "X-Mms-Message-Size";
    public static final String N_EXPIRY = "X-Mms-Expiry";
    public static final String N_PRIORITY = "X-Mms-Priority";
    public static final String N_DELIVERY_REPORT = "X-Mms-Delivery-Report";
    public static final String N_DELIVERY_TIME = "X-Mms-Delivery-Time";
    public static final String N_REPORT_ALLOWED = "X-Mms-Report-Allowed";
    public static final String N_READ_REPLY = "X-Mms-Read-Reply";
    public static final String N_STATUS = "X-Mms-Status";
    public static final String N_SENDER_VISIBILITY = "X-Mms-Sender-Visibility";
    public static final String N_CONTENT_LOCATION = "X-Mms-Content-Location";
    public static final String N_RESPONSE_STATUS = "X-Mms-Response-Status";
    public static final String N_RESPONSE_TEXT = "X-Mms-Response-Text";
    public static final String N_CONTENT_TYPE = "Content-Type";

    /**  
     * Return the String representation of the name of this header.
     **/
    public abstract String getName();

    /**  
     * Return the String representation of the value for this header.
     **/
    public abstract String getStrValue();

    /**  
     * Encode this header into an MMS message.
     * @param mm MMS Message to encode value into.
     **/
    public abstract void encode(MMMessage mm);

    /**  
     * Decode the header value from an MMS message into this MMHeader object.
     * @param mm MMS Message to encode value from.
     **/
    public abstract void decode(MMMessage mm);

    /**  
     * Set the value of this header.
     * @param Object Value of header. Type will be constrained by derived class.
     **/
    public abstract void setValue(Object o);

    /**  
     * Is this header available in the MMS Message?
     * @param mm MMS Message holding this type of header.
     **/
    public abstract boolean isAvailable(MMMessage mm);

    /**  
     * Set header to default value.
     **/
    public abstract void setDefault();

    /** Debugging */
    private static void dbgOut(String s) {
        System.out.println("MMHeader: " + s);
    }

    /**  
     * A String-value header whose value is guarenteed NEVER null;
     */
    public abstract static class StrType extends MMHeader {

        private String mStrVal = "";

        /**
         * Return the String representation of the value for this header.
         */ 
        public String getStrValue() {
            return mStrVal; 
        };

        /**
         * Set the value for this header.  Ignore if value is null.
         */ 
        public void setValue(Object o) {
            if (o == null)
                return;
            if (o instanceof String) {
                mStrVal = (String)o;
            } else {
                mStrVal = o.toString();
            }
        }

        /**
         * Set default value (no string)
         */
        public void setDefault() { mStrVal = ""; }

        /**
         * Transform a vector of MMAddress objects into a 
         * String of ADDR_TOKEN-separated addresses.
         */
        protected void processAddrVector(Vector v) {
            Enumeration en = v.elements();
            String list = null;
            boolean first = true;
            while (en.hasMoreElements()) {
                MMAddress addr = (MMAddress)en.nextElement();
                if (first == true) {
                    list = addr.getFullAddress(); 
                    first = false;
                } else {
                    list += ADDR_SEPARATOR_TOKEN + " " + addr.getFullAddress(); 
                }
                this.setValue(list);
            }
        }

        /**
         * Transform an ADDR_TOKEN-separated String of addresses
         * to an array of String addresses.
         */
        protected String[] processAddrString(String list) {
            if (list == null || list.length() == 0)
                return new String[0];
            StringTokenizer tok = new StringTokenizer(list, ADDR_SEPARATOR_TOKEN);
            String[] addrList = new String[tok.countTokens()];
            int i = 0;
            while (tok.hasMoreTokens()) {
                String name = tok.nextToken(); 
                addrList[i++] =  name;
            }
            return addrList;
        }

        /**
         * Is our value empty?
         */
        protected boolean isEmpty() {
            if (mStrVal == null || mStrVal.length() == 0)
               return true;
            return false;
        }

    }

    /**
     * To address header
     */
    public static class To extends StrType {

        public To(String addr) {
            super.setValue(addr);
        }

        public String getName() { return N_TO; }

        public void encode(MMMessage mm) {
            if (isEmpty()) {
                return;
            }
            String[] addrs = processAddrString(super.getStrValue());
            for (int i = 0; i < addrs.length; i++) {
                mm.addToAddress(addrs[i]);
            }
        }

        public void decode(MMMessage mm) {
            if (mm.isToAvailable()) {
                processAddrVector(mm.getTo());
            }
        }

        public boolean isAvailable(MMMessage mm) { return mm.isToAvailable(); }
    }

    /**
     * From address header
     */
    public static class From extends StrType {

        public From(String addr) {
            super.setValue(addr);
        }

        public String getName() { return N_FROM; }

        public void decode(MMMessage mm) {
            if (mm.isFromAvailable()) {
                MMAddress addr = mm.getFrom();
                super.setValue(addr.getFullAddress());
            }
        }

        public void encode(MMMessage mm) {
            mm.setFrom(super.getStrValue());
        }

        public boolean isAvailable(MMMessage mm) { return mm.isFromAvailable(); }
    }

    /**
     * Bcc address header
     */
    public static class Bcc extends StrType {

        public Bcc(String addr) {
            super.setValue(addr);
        }

        public String getName() { return N_BCC; }

        public void decode(MMMessage mm) {
            if (mm.isBccAvailable()) {
                processAddrVector(mm.getBcc());
            }
        }

        public void encode(MMMessage mm) {
            if (isEmpty()) {
                return;
            }
            String[] addrs = processAddrString(super.getStrValue());
            for (int i = 0; i < addrs.length; i++) {
                mm.addBccAddress(addrs[i]);
            }
        }

        public boolean isAvailable(MMMessage mm) { return mm.isBccAvailable(); }
    }

    /**
     * Cc address header
     */
    public static class Cc extends StrType {

        public Cc(String addr) {
            super.setValue(addr);
        }

        public String getName() { return N_CC; }

        public void decode(MMMessage mm) {
            if (mm.isCcAvailable()) {
                processAddrVector(mm.getCc());
            }
        }

        public void encode(MMMessage mm) {
            if (isEmpty()) {
                return;
            }
            String[] addrs = processAddrString(super.getStrValue());
            for (int i = 0; i < addrs.length; i++) {
                mm.addCcAddress(addrs[i]);
            }
        }

        public boolean isAvailable(MMMessage mm) { return mm.isCcAvailable(); }
    }

    /**
     * Subject header
     */
    public static class Subject extends StrType {

        public Subject(String text) {
            super.setValue(text);
        }

        public String getName() { return N_SUBJECT; }

        public void decode(MMMessage mm) {
            if (mm.isSubjectAvailable()) {
                super.setValue(mm.getSubject());
            }
        }

        public void encode(MMMessage mm) {
            if (!isEmpty()) {
                mm.setSubject(super.getStrValue());
            }
        }

        public boolean isAvailable(MMMessage mm) { return mm.isSubjectAvailable(); }
    }

    /**
     * X-MMS-Transaction-ID header
     */
    public static class TransactionId extends StrType {

        public TransactionId(String text) {
            super.setValue(text);
        }

        public String getName() { return N_TRANSACTION_ID; }

        public void decode(MMMessage mm) {
            if (mm.isTransactionIdAvailable()) {
                super.setValue(mm.getTransactionId());
            }
        }

        public void encode(MMMessage mm) {
            if (!isEmpty()) {
                mm.setTransactionId(super.getStrValue());
            }
        }

        public boolean isAvailable(MMMessage mm) { return mm.isTransactionIdAvailable(); }
    }

    /**
     * X-Mms-MMS-Version header
     */
    public static class Version extends StrType {

        private static String DEFAULT_VERSION = "1.0";

        public Version(String text) {
            this.setValue(text);
        }

        /** Do validity checking here **/
        public void setValue(String text) {
            if (text == null || text.length() == 0) {
                setDefault();
            } else if (text.equals("1.0") ||
                       text.equals("1.1")) {
                super.setValue(text);
            } else {
                setDefault();
            }
        }

        public void setDefault() { super.setValue(DEFAULT_VERSION); }

        public String getName() { return N_MMS_VERSION; }

        public void decode(MMMessage mm) {
            if (mm.isVersionAvailable()) {
                super.setValue(mm.getVersionAsString());
            }
        }

        public void encode(MMMessage mm) {
            if (!isEmpty()) {
                if (super.getStrValue().equals("1.0")) {
                    mm.setVersion(IMMConstants.MMS_VERSION_10);
                } else if (super.getStrValue().equals("1.1")) {
                    mm.setVersion(IMMConstants.MMS_VERSION_11);
                } else {
                    // error!  Ignore.  Use sane default.
                    dbgOut(N_MMS_VERSION + ": unknown version " + this.getStrValue());
                    mm.setVersion(IMMConstants.MMS_VERSION_10);
                }
            }
        }

        public boolean isAvailable(MMMessage mm) { return mm.isVersionAvailable(); }
    }

    /**
     * Message-ID header
     */
    public static class MessageId extends StrType {

        public MessageId(String text) {
            super.setValue(text);
        }

        public String getName() { return N_MESSAGE_ID; }

        public void decode(MMMessage mm) {
            if (mm.isMessageIdAvailable()) {
                super.setValue(mm.getMessageId());
            }
        }

        public void encode(MMMessage mm) {
            if (!isEmpty()) {
                mm.setMessageId(super.getStrValue());
            }
        }

        public boolean isAvailable(MMMessage mm) { return mm.isMessageIdAvailable(); }
    }

    /**
     * Message-Size header (currently a placeholder)
     */
    public static class MessageSize extends StrType {

        public MessageSize(String text) {
            super.setValue(text);
        }

        public String getName() { return N_MESSAGE_SIZE; }

        public void decode(MMMessage mm) {
            if (mm.isMessageSizeAvailable()) {
                String val = Integer.toString(mm.getMessageSize());
                super.setValue(val);
            }
        }

        public void encode(MMMessage mm) {
            if (!isEmpty()) {
                Integer val = Integer.getInteger(super.getStrValue());
                if (val == null) {
                    return;
                }
                mm.setMessageSize(val.intValue());
            }
        }

        public boolean isAvailable(MMMessage mm) { return false; }
    }

    /**
     * X-Mms-Content-Location header (currently a placeholder)
     */
    public static class ContentLocation extends StrType {

        public ContentLocation(String text) {
            super.setValue(text);
        }

        public String getName() { return N_CONTENT_LOCATION; }

        public void decode(MMMessage mm) {
            if (!isEmpty()) {
                mm.setContentLocation(super.getStrValue());
            }
        }

        public void encode(MMMessage mm) {
            if (mm.isContentLocAvailable()) {
                super.setValue(mm.getContentLocation());
            }
        }

        public boolean isAvailable(MMMessage mm) { return false; }
    }

    /**
     * A user-defined header, identified by a String name and having a String value.
     *
     * This is used for any user-defined header, WSP-encoded as text header:
     *     name 0x0 value 0x0
     */
    public static class UserDefined extends StrType {
        private String mName;

        public UserDefined(String name) {
            mName = name;
        }

        public UserDefined(String name, String val) {
            this(name);
            super.setValue(val);
        }

        public String getName() { return mName; }

        public void encode(MMMessage mm) {
            if (!isEmpty()) {
                mm.addUserDefinedHeader(mName, super.getStrValue());
            }
        }

        public void decode(MMMessage mm) {
            if (mm.isUserDefinedAvailable()) {
                String val = mm.getUserDefinedHeaderValue(mName);
                if (val != null) {
                    super.setValue(val);
                }
            }
        }

        /** we don't check ; assume yes */
        public boolean isAvailable(MMMessage mm) { return true; }
    }

    /**
     * Class of headers whose values are a constrained list of values.
     */
    public static abstract class EnumType extends StrType {
        /**
         * Return an array of String representations of possible values for this header
         */
        public abstract String[] getChoices();
    }

    /**
     * X-Mms-Message-Type header
     */
    public static class MessageType extends EnumType {

        public static final String SEND_REQ = "m-send-req";
        public static final String SEND_CONF = "m-send-conf";
        public static final String NOTIF_IND = "m-notification-ind";
        public static final String RETRIEVE_CONF = "m-retrieve-conf";
        public static final String ACK_IND = "m-acknowledge-ind";
        public static final String DELIVERY_IND = "m-delivery-ind";

        private static final String[] gChoices = { 
            SEND_REQ, SEND_CONF, NOTIF_IND, RETRIEVE_CONF, ACK_IND, DELIVERY_IND
        };

        public MessageType() {
            setDefault();
        }

        public MessageType(String val) {
            if (val == null)
                val = RETRIEVE_CONF;
            super.setValue(val);
        }

        public void setDefault() { super.setValue(RETRIEVE_CONF); }

        public String getName() { return N_MESSAGE_TYPE; }

        public String[] getChoices() {
            return gChoices;
        }

        public void decode(MMMessage mm) {
            if (!mm.isMessageTypeAvailable())
                return;
            byte val = mm.getMessageType();
            switch(val) {
              case IMMConstants.MESSAGE_TYPE_M_SEND_REQ:
              	super.setValue(SEND_REQ); 
                break;
              case IMMConstants.MESSAGE_TYPE_M_SEND_CONF:
              	super.setValue(SEND_CONF); 
                break;
              case IMMConstants.MESSAGE_TYPE_M_NOTIF_IND:
              	super.setValue(NOTIF_IND); 
                break;
              case IMMConstants.MESSAGE_TYPE_M_ACK_IND:
              	super.setValue(ACK_IND); 
                break;
              case IMMConstants.MESSAGE_TYPE_M_DELIVERY_IND:
              	super.setValue(DELIVERY_IND); 
                break;
              case IMMConstants.MESSAGE_TYPE_M_RETRIEVE_CONF:
              // Fall through to..
              default:
              	super.setValue(RETRIEVE_CONF); 
                break;
            }
        }

        public void encode(MMMessage mm) {
            // most common one first...
            if (super.getStrValue().equalsIgnoreCase(RETRIEVE_CONF)) {
                mm.setMessageType(IMMConstants.MESSAGE_TYPE_M_RETRIEVE_CONF);
            } else if (super.getStrValue().equalsIgnoreCase(SEND_REQ)) {
                mm.setMessageType(IMMConstants.MESSAGE_TYPE_M_SEND_REQ);
            } else if (super.getStrValue().equalsIgnoreCase(SEND_CONF)) {
                mm.setMessageType(IMMConstants.MESSAGE_TYPE_M_SEND_CONF);
            } else if (super.getStrValue().equalsIgnoreCase(NOTIF_IND)) {
                mm.setMessageType(IMMConstants.MESSAGE_TYPE_M_NOTIF_IND);
            } else if (super.getStrValue().equalsIgnoreCase(ACK_IND)) {
                mm.setMessageType(IMMConstants.MESSAGE_TYPE_M_ACK_IND);
            } else if (super.getStrValue().equalsIgnoreCase(DELIVERY_IND)) {
                mm.setMessageType(IMMConstants.MESSAGE_TYPE_M_DELIVERY_IND);
            } else {
                mm.setMessageType(IMMConstants.MESSAGE_TYPE_M_RETRIEVE_CONF);
            }
        }

        public boolean isAvailable(MMMessage mm) { return mm.isMessageTypeAvailable(); }
    }

    /**
     * X-Mms-Priority header
     */
    public static class Priority extends EnumType {

        public static final String LOW = "low";
        public static final String NORMAL = "normal";
        public static final String HIGH = "high";

        private static final String[] gChoices = { LOW, NORMAL, HIGH };

        public Priority() {
            setDefault();
        }

        public Priority(String val) {
            if (val == null) {
                setDefault();
            } else {
                super.setValue(val);
            }
        }

        public String getName() { return N_PRIORITY; }

        public void setDefault() { super.setValue(LOW); }

        public String[] getChoices() {
            return gChoices;
        }

        public void decode(MMMessage mm) {
            if (!mm.isPriorityAvailable())
                return;
            byte val = mm.getPriority();
            switch(val) {
              case IMMConstants.PRIORITY_LOW: 
              	super.setValue(LOW); break;
              case IMMConstants.PRIORITY_HIGH: 
              	super.setValue(HIGH); break;
              case IMMConstants.PRIORITY_NORMAL: 
              	super.setValue(NORMAL); break;
              default:
                dbgOut("unknown Priority " + (val & 0xff) + " using default");
              	super.setValue(NORMAL); break;
            }
        }

        public void encode(MMMessage mm) {
            if (super.getStrValue().equalsIgnoreCase(LOW)) {
                mm.setPriority(IMMConstants.PRIORITY_LOW);
            } else if (super.getStrValue().equalsIgnoreCase(NORMAL)) {
                mm.setPriority(IMMConstants.PRIORITY_NORMAL);
            } else if (super.getStrValue().equalsIgnoreCase(HIGH)) {
                mm.setPriority(IMMConstants.PRIORITY_HIGH);
            } else {
                dbgOut("unknown Priority " + super.getStrValue() + " using default");
                // internal error!
                mm.setPriority(IMMConstants.PRIORITY_NORMAL);
            }
        }

        public boolean isAvailable(MMMessage mm) { return mm.isPriorityAvailable(); }
    }

    /**
     * X-Mms-Message-Class header
     */
    public static class MessageClass extends EnumType {

        public static final String PERSONAL = "Personal";
        public static final String ADVERT = "Advertisement";
        public static final String INFO = "Info";
        public static final String AUTO = "Auto";

        private static final String[] gChoices = { PERSONAL, ADVERT, INFO, AUTO };

        public MessageClass() {
            setDefault();
        }

        public MessageClass(String val) {
            if (val == null)
                val = PERSONAL;
            super.setValue(val);
        }

        public String getName() { return N_MESSAGE_CLASS; }

        public void setDefault() { super.setValue(PERSONAL); }

        public String[] getChoices() {
            return gChoices;
        }

        public void decode(MMMessage mm) {
            if (!mm.isMessageClassAvailable())
                return;
            byte val = mm.getMessageClass();
            switch(val) {
              case IMMConstants.MESSAGE_CLASS_AUTO: 
              	super.setValue(AUTO); break;
              case IMMConstants.MESSAGE_CLASS_INFORMATIONAL: 
              	super.setValue(INFO); break;
              case IMMConstants.MESSAGE_CLASS_ADVERTISEMENT: 
              	super.setValue(ADVERT); break;
              case IMMConstants.MESSAGE_CLASS_PERSONAL: 
              default:
              	super.setValue(PERSONAL); break;
            }
        }

        public void encode(MMMessage mm) {
            if (super.getStrValue().equalsIgnoreCase(PERSONAL)) {
                mm.setMessageClass(IMMConstants.MESSAGE_CLASS_PERSONAL);
            } else if (super.getStrValue().equalsIgnoreCase(ADVERT)) {
                mm.setMessageClass(IMMConstants.MESSAGE_CLASS_ADVERTISEMENT);
            } else if (super.getStrValue().equalsIgnoreCase(INFO)) {
                mm.setMessageClass(IMMConstants.MESSAGE_CLASS_INFORMATIONAL);
            } else if (super.getStrValue().equalsIgnoreCase(AUTO)) {
                mm.setMessageClass(IMMConstants.MESSAGE_CLASS_AUTO);
            } else {
                // Error!  Spec says default is 'personal'
                dbgOut("unknown Message-Class " + super.getStrValue() + "; using default");
                mm.setMessageClass(IMMConstants.MESSAGE_CLASS_PERSONAL);
            }
        }

        public boolean isAvailable(MMMessage mm) { return mm.isMessageClassAvailable(); }
    }

    /**
     * A Boolean-value header.  The MMS Header specs use
     * "yes/no" instead of "true/false" so we do that translation here.
     */
    public abstract static class BoolType extends EnumType {

        private static String YES = "Yes";
        private static String NO = "No";

        private static String[] gChoices = new String[] { YES, NO };

        /** value field (we ignore the parent string field) */
        private Boolean mBoolVal = Boolean.FALSE;

        /**
         * setValue: accepts either Boolean or String value
         */
        public void setValue(Object val) {
            if (val == null) {
                return;
            }
            if (val instanceof Boolean) {
                setBoolValue((Boolean)val);
                return;
            }
            if (val instanceof String) {
              setStrValue((String)val);
            }
        }

        public String getStrValue() { 
            return mBoolVal.equals(Boolean.FALSE) ? NO : YES;
        };

        /**
         * Converts yes/no string to Boolean value.
         */
        public void setStrValue(String sVal) { 
            if (sVal.equalsIgnoreCase(YES)) {
                mBoolVal = Boolean.TRUE; 
            } else if (sVal.equalsIgnoreCase(NO)) {
                mBoolVal = Boolean.FALSE; 
            } else {
                dbgOut("set: illegal boolean value: " + (sVal == null ? "<null>" : sVal));
            }
        };

        /** get/set with Boolean arguments */

        protected boolean getBoolValue() { 
            return mBoolVal.booleanValue();
        };

        protected void setBoolValue(Boolean val) { 
            mBoolVal = val;
        };

        /** Enum method */
        public String[] getChoices() {
            return gChoices;
        }

    }

    /**
     * X-MMS-Read-Reply header
     */
    public static class ReadReply extends BoolType {

        public ReadReply(Boolean val) {
            if (val == null)
                setDefault();
            setBoolValue(val);
        }

        public String getName() { return N_READ_REPLY; }

        public void setDefault() { setBoolValue(Boolean.FALSE); }

        public void decode(MMMessage mm) {
            if (mm.isReadReplyAvailable()) {
                Boolean bVal = new Boolean(mm.getReadReply());
                setBoolValue(bVal);
            }
        }

        public void encode(MMMessage mm) {
            mm.setReadReply(getBoolValue());
        }

        public boolean isAvailable(MMMessage mm) { return mm.isReadReplyAvailable(); }
    }

    /**
     * X-MMS-Delivery-Report header
     */
    public static class DeliveryReport extends BoolType {

        public DeliveryReport(Boolean val) {
            if (val == null)
                setDefault();
            setBoolValue(val);
        }

        public String getName() { return N_DELIVERY_REPORT; }

        public void setDefault() { setBoolValue(Boolean.FALSE); }

        public void encode(MMMessage mm) {
            mm.setDeliveryReport(getBoolValue());
        }

        public void decode(MMMessage mm) {
            if (mm.isDeliveryReportAvailable()) {
                Boolean bVal = new Boolean(mm.getDeliveryReport());
                setBoolValue(bVal);
            }
        }

        public boolean isAvailable(MMMessage mm) { return mm.isDeliveryReportAvailable(); }
    }

    /**
     * X-MMS-Sender-Visibility header
     *
     * This is boolean type, where true means 'Show' and false means 'Hide'.
     */
    public static class SenderVisibility extends BoolType {

        private static final String HIDE = "Hide";
        private static final String SHOW = "Show";
        private static final String[] gChoices = { HIDE, SHOW };

        public SenderVisibility(Boolean val) {
            if (val == null)
                setDefault();
            setBoolValue(val);
        }

        public String getName() { return N_SENDER_VISIBILITY; }

        public void setDefault() { setBoolValue(Boolean.TRUE); }

        /**
         * MMMessage doesn't really decoe this for us.
         *    0x80 = hide
         *    0x81 = show
         */
        public void encode(MMMessage mm) {
            boolean isVis = getBoolValue();
            if (isVis) {
                mm.setSenderVisibility((byte)0x81);
            } else {
                mm.setSenderVisibility((byte)0x80);
            }
        }

        public void decode(MMMessage mm) {
            if (mm.isSenderVisibilityAvailable()) {
                boolean isVis = true;	// default
                byte val = mm.getSenderVisibility();
                if (val == (byte)0x80) {
                   isVis = false;
                }
                setBoolValue(new Boolean(isVis));
            }
        }

        public boolean isAvailable(MMMessage mm) { 
            return mm.isSenderVisibilityAvailable(); 
        }

        /** 
         * Overrides BoolType method
         **/
        public String getStrValue() { 
            return (getBoolValue() ? SHOW : HIDE);
        };

        /**
         * Overrides BoolType method
         */
        public void setStrValue(String sVal) { 
            if (sVal.equalsIgnoreCase(SHOW)) {
                setBoolValue(Boolean.TRUE); 
            } else if (sVal.equalsIgnoreCase(HIDE)) {
                setBoolValue(Boolean.FALSE); 
            } else {
                dbgOut("set: illegal boolean value: " + (sVal == null ? "<null>" : sVal));
            }
        };

        /** Enum method */
        public String[] getChoices() {
            return gChoices;
        }

    }

    /**
     * A header based on a time value.  Must be GMT per HTTP spec.
     * The value may be absolute or relative.
     */
    public abstract static class TimeType extends MMHeader {

        private java.util.Date mDateVal = new java.util.Date();
        private boolean mIsAbsolute = true;  // is the Date value absolute or relative?

        /**
         * @param Object either a Date or formated date String
         */
        public void setValue(Object o) {
            if (o != null) {
               if (o instanceof java.util.Date) {
                  mDateVal = (java.util.Date)o;
               } else if (o instanceof java.lang.String) {
                  String val = (String)o;
                  val = HeaderCharEncoding.decode(val.getBytes());
                  setStrValue(val);
               }
            }
        }

        /**
         * Return a printable date in spec format.
         * We can handle non-English default.
         */
        public String getStrValue() { 
            DateFormat idf = new InternetDateFormat(Locale.getDefault());
            String dateStr = idf.format(mDateVal);
            return dateStr;
        };

        /**
         * Convert printable date to Date
         * @param dateStr RFC822 format date string
         */
        public void setStrValue(String dateStr) { 
            if (dateStr == null) 
                return;
            DateFormat idf = new InternetDateFormat(Locale.getDefault());
            try {
                java.util.Date d = idf.parse(dateStr);
                setValue(d);
            } catch (ParseException ex) {
                System.out.println("MMHeader.TimeType: can't parse " + dateStr);
            }
        };

        public void setDefault() { this.setValue(new Date()); }

        protected java.util.Date getTimeVal() {
            return mDateVal;
        }

        protected boolean isAbsolute() { return mIsAbsolute; }

        protected void setAbsolute(boolean abs) { mIsAbsolute = abs; }
    }

    /**
     * Date header
     */
    public static class Date extends TimeType {

        public Date() {
            super.setAbsolute(true);	// always an absolute value
        }

        public Date(java.util.Date val) {
            if (val != null)
                super.setValue(val);
        }

        public String getName() { return N_DATE; }

        public void encode(MMMessage mm) {
            mm.setDate(super.getTimeVal());
        }

        public void decode(MMMessage mm) {
            if (mm.isDateAvailable()) {
                super.setValue(mm.getDate());
            }
        }

        public boolean isAvailable(MMMessage mm) { return mm.isDateAvailable(); }
    }

    /**
     * Expiry header
     * Date value can be absolute (same as 'Date') or relative (in seconds)
     */
    public static class Expiry extends TimeType {

        public Expiry() { }

        /*
         * Constructor
         * @param val Date when (absolute) or seconds until (relative) this expires
         */
        public Expiry(java.util.Date val, boolean isAbs) {
            if (val != null) {
                super.setValue(val);
                super.setAbsolute(isAbs);
            }
        }

        public String getName() { return N_EXPIRY; }

        public void encode(MMMessage mm) {
            mm.setExpiry(super.getTimeVal());
            mm.setExpiryAbsolute(isAbsolute());
        }

        public void decode(MMMessage mm) {
            if (mm.isExpiryAvailable()) {
                super.setValue(mm.getExpiry());
                super.setAbsolute(mm.isExpiryAbsolute());
            }
        }

        public boolean isAvailable(MMMessage mm) { 
            return mm.isExpiryAvailable(); 
        }
    }

    /**
     * Delivery Time header
     * Date value can be absolute (same as 'Date') or relative (in seconds).
     */
    public static class DeliveryTime extends TimeType {

        public DeliveryTime() { }

        /*
         * Constructor
         * @param val Date when (absolute) or seconds until (relative) this expires
         */
        public DeliveryTime(java.util.Date val, boolean isAbs) {
            if (val != null) {
                super.setValue(val);
                super.setAbsolute(isAbs);
            }
        }

        public String getName() { return N_DELIVERY_TIME; }

        public void encode(MMMessage mm) {
            mm.setDeliveryTime(super.getTimeVal());
            mm.setDeliveryTimeAbsolute(isAbsolute());
        }

        public void decode(MMMessage mm) {
            if (mm.isDeliveryTimeAvailable()) {
                super.setValue(mm.getExpiry());
                super.setAbsolute(mm.isDeliveryTimeAbsolute());
            }
        }

        public boolean isAvailable(MMMessage mm) { 
            return mm.isDeliveryTimeAvailable(); 
        }
    }
}
