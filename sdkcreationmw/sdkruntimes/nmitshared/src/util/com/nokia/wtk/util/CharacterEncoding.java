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

//import com.nokia.wtk.util.MessageWriters;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.PushbackInputStream;
import java.io.Reader;

/**
 * A character encoding class which contains IANA MIBenum value and name
 * as well as the Java name and other aliases.  Also various static
 * methods for accessing the list of supported encodings and their aliases.
 *
 */

public class CharacterEncoding {
    /**
     * Create a CharacterEncoding instance
     * @param ianaMIBenum the IANA MIBenum value for the character encoding
     * @param names the set of names that may be used to identify the character encoding
     *  This will always contain at least three elements, but may contain additional aliases.
     *  <code>names[0]</code> is the display name
     *  <code>names[1]</code> is the Java name
     *  <code>names[2]</code> is the Preferred MIME name
     *  <code>names[3..N]</code> are optional to specify any other alias names
     */
    CharacterEncoding(int ianaMIBenum, String[] names) {
        this(ianaMIBenum, names, null);
    }
    
    /**
     * Create a CharacterEncoding instance
     * @param ianaMIBenum the IANA MIBenum value for the character encoding
     * @param names the set of names that may be used to identify the character encoding
     *  This will always contain at least three elements, but may contain additional aliases.
     *  <code>names[0]</code> is the display name
     *  <code>names[1]</code> is the Java name
     *  <code>names[2]</code> is the Preferred MIME name
     *  <code>names[3..N]</code> are optional to specify any other alias names
     * @param stringTerminator the string termination byte sequence
     */
    CharacterEncoding(int ianaMIBenum, String[] names, byte[] stringTerminator) {
        this.ianaMIBenum = ianaMIBenum;
        this.names = names;
        if ((stringTerminator != null) && (stringTerminator.length > 0)) {
            this.stringTerminator = stringTerminator;
        }
//MessageWriters.DetailMsg.println(toString() + ": Terminator=" + toString(getStringTerminator()));
    }
    
    /**
     * Gets the byte array representing the string terminator for this character encoding.
     *
     * @return The string terminator byte array
     */
    public byte[] getStringTerminator() {
        if (stringTerminator == null) {
            stringTerminator = encodeString("\u0000");
            if (stringTerminator.length == 0) {
MessageWriters.DetailMsg.println(toString() + ": UnsupportedEncoding");
                stringTerminator = new byte[] {NULL_BYTE};
            }
        }
        return stringTerminator;
    }

    /**
     * @return the IANA MIBenum value for the character encoding
     */
    public int getMIBenum() { return ianaMIBenum; }
    
    /**
     * @return the Java name for the character encoding
     */
    public String getJavaName() { return names[1]; }
    
    /**
     * @return the preferred MIME name for the character encoding
     */
    public String getMIMEName() { return names[2]; }
    
    /**
     * @return the display name for the character encoding
     */
    public String getDisplayName() { return names[0]; }
    
    /**
     * @return the display name for the character encoding
     */
    public String toString() { return getDisplayName(); }

    /**
     * Find a character encoding that matches a name, whether exactly or by alias.
     * Names and aliases are not case-sensitive.
     * @param name an encoding name to search for
     * @return the matching character encoding or null if not found
     */
    public static CharacterEncoding findEncoding(String name) {
        CharacterEncoding encoding = null;
        for (int i = 0; (encoding == null) && (i < encodings.length); i++) {
            for (int j = 0; (encoding == null) && (j < encodings[i].names.length); j++) {
                if (encodings[i].names[j].equalsIgnoreCase(name)) {
                    encoding = encodings[i];
                }
            }
        }
        return encoding;
    }
    
    /**
     * Find a character encoding that matches a MIBenum value.
     * @param ianaMIBenum a MIBenum value to search for
     * @return the matching character encoding or null if not found
     */
    public static CharacterEncoding findEncoding(int ianaMIBenum) {
        CharacterEncoding encoding = null;
        for (int i = 0; (encoding == null) && (i < encodings.length); i++) {
            if (encodings[i].ianaMIBenum == ianaMIBenum) {
                encoding = encodings[i];
            }
        }
        return encoding;
    }
    
    /**
     * @return the default character encoding if not specified by an input source
     */
    public static CharacterEncoding getDefault() {
        return findEncoding(defaultEncoding);
    }
    
    /**
     * @return the default character encoding if not specified by an HTTP input source
     */
    public static CharacterEncoding getHTTPDefault() {
        return findEncoding(defaultHTTPEncoding);
    }
    
    /**
     * @return the character encoding used internally
     */
    public static CharacterEncoding getInternalEncoding() {
        return findEncoding(internalEncoding);
    }
    
    /**
     * Gets the character encoding to use when reading and writing files.
     * @return the character encoding used when reading and writing files.
     */
    public static CharacterEncoding getFileEncoding() {
        if (fileEncoding == null) {
            fileEncoding = findEncoding(Enum_File);
        }
        return fileEncoding;
    }
    
    /**
     * Sets the character encoding to use when reading and writing files.
     * @enc the character encoding used when reading and writing files.
     */
    public static void setFileEncoding(CharacterEncoding enc) {
        if (enc != null) {
            fileEncoding = enc;
        }
    }
    
    /**
     * @return the set of all supported character encodings
     */
    public static CharacterEncoding[] getSupportedEncodings() {
        return encodings;
    }
    
    /**
     * Converts a string into bytes according to this character encoding,
     * storing the result into a new byte array.
     *
     * @param s the string
     * @return The resulting byte array
     */
    public byte[] encodeString(String s) {
        try { return (s != null) ? s.getBytes(getJavaName()) : new byte[0]; }
        catch (java.io.UnsupportedEncodingException ex) { return new byte[0]; }
    }
    
    /**
     * Creates a new string by converting the specified subarray of bytes using
     * this character encoding.  The length of the new string is a function of
     * this encoding, and hence may not be equal to the length of the subarray.
     *
     * @param  bytes   The bytes to be converted into characters
     * @param  offset  Index of the first byte to convert
     * @param  length  Number of bytes to convert
     */
    private static String decodeString(String enc, byte[] bytes, int offset, int length) {
        String s = null;
        try { s = new String(bytes, offset, length, enc); }
        catch (java.io.UnsupportedEncodingException ex) { s = null; }
        catch (IndexOutOfBoundsException ex) { s = null; }
//MessageWriters.SYSOUT.println(toString() + ": decodeString(" + toString(bytes) + ", " + offset + ", " + length + ") = " + StringUtils.getDisplayableString(s));
        return s;
    }
    
    /**
     * Creates a new string by converting the specified subarray of bytes using
     * this character encoding.  The length of the new string is a function of
     * this encoding, and hence may not be equal to the length of the subarray.
     *
     * @param  bytes   The bytes to be converted into characters
     * @param  offset  Index of the first byte to convert
     * @param  length  Number of bytes to convert
     */
    public String decodeString(byte[] bytes, int offset, int length) {
        return decodeString(getJavaName(), bytes, offset, length);
    }
    
    /**
     * Creates a new string by converting the specified subarray of bytes using
     * this character encoding.  The length of the new string is a function of
     * this encoding, and hence may not be equal to the length of the subarray.
     *
     * @param  bytes   The bytes to be converted into characters
     */
    public String decodeString(byte[] bytes) {
        return (bytes != null) ? decodeString(bytes, 0, bytes.length) : null;
    }
    

    /**
     * Inner Class: Local filesystem variant of CharacterEncoding.
     * This uses the system property "file.encoding", which may not have
     * a corresponding MIBEnum value or a standard MIME name.  But its
     * very existence implies that the Java VM knows how to handle it.
     */
    private static class LocalFileCharacterEncoding extends CharacterEncoding {
        /**
         * Creates a CharacterEncoding instance for local files.
         */
        private LocalFileCharacterEncoding() {
            super(Enum_File, new String[] {"File", fileEncoding});
        }
    
        /**
         * @return the Java name for the character encoding
         */
        public String getJavaName() {
            return (fileEncoding != null) ? fileEncoding
                                          : CharacterEncoding.findEncoding(CharacterEncoding.defaultEncoding).getJavaName();
        }
    
        /**
         * @return the preferred MIME name for the character encoding
         */
        public String getMIMEName() { return getJavaName(); }
    
    
        private static final String fileEncoding = System.getProperty("file.encoding");
    }   // End of LocalFileCharacterEncoding


    /**
     * Inner Class: Unicode variant of CharacterEncoding.
     * Unicode is a special case in that there are a few byte order variants.
     * The byte-order mark may or may not be present, so handle any case when decoding byte arrays.
     * Otherwise, use the default big-endian order (and also when encoding).
     */
    private static class UnicodeCharacterEncoding extends CharacterEncoding {
        /**
         * Creates a CharacterEncoding instance for Unicode
         * @param ianaMIBenum the IANA MIBenum value for the character encoding
         * @param names the set of names that may be used to identify the character encoding
         *  This will always contain at least three elements, but may contain additional aliases.
         *  <code>names[0]</code> is the display name
         *  <code>names[1]</code> is the Java name
         *  <code>names[2]</code> is the Preferred MIME name
         *  <code>names[3..N]</code> are optional to specify any other alias names
         */
        private UnicodeCharacterEncoding(int ianaMIBenum, String[] names) {
            super(ianaMIBenum, names, new byte[] {NULL_BYTE, NULL_BYTE});
        }
    
        /**
         * Creates a new string by converting the specified subarray of bytes using
         * this character encoding.  The length of the new string is a function of
         * this encoding, and hence may not be equal to the length of the subarray.
         *
         * @param  bytes   The bytes to be converted into characters
         * @param  offset  Index of the first byte to convert
         * @param  length  Number of bytes to convert
         */
        //TODO: Does this work for UTF-16?  And what about encodeString()?
        public String decodeString(byte[] bytes, int offset, int length) {
            String encName = this.getJavaName();
            if ((bytes != null) && (bytes.length >= 2)) {
                if ((bytes[0] == (byte)0xFF) && (bytes[1] == (byte)0xFE)) {
                    encName = "Unicode";
                } else if ((bytes[0] == (byte)0xFE) && (bytes[1] == (byte)0xFF)) {
                    encName = "Unicode";
                }
            }
            return CharacterEncoding.decodeString(encName, bytes, offset, length);
        }
    }   // End of UnicodeCharacterEncoding
    
    
    //
    // Class member data:
    //
    
    private static final byte NULL_BYTE = (byte)0;  // '\0'-valued byte
    
    
    // A few well-known encoding types:
    public static final int Enum_ASCII = 3;
    public static final int Enum_Latin1 = 4;
    public static final int Enum_Latin2 = 5;
    public static final int Enum_Latin3 = 6;
    public static final int Enum_Latin4 = 7;
    public static final int Enum_Cyrillic = 8;
    public static final int Enum_Arabic = 9;
    public static final int Enum_Greek = 10;
    public static final int Enum_Hebrew = 11;
    public static final int Enum_Latin5 = 12;
    public static final int Enum_SJIS = 17;
    public static final int Enum_EUCJP = 18;
    public static final int Enum_ISO2022JP = 39;
    public static final int Enum_GB2312 = 2025;
    public static final int Enum_Big5 = 2026;
    public static final int Enum_UTF7 = 1012;
    public static final int Enum_UTF8 = 106;
    public static final int Enum_UTF16 = 1015;
    public static final int Enum_Unicode = 1000;
    
    // The set of predefined encoding types:
    private static CharacterEncoding[] encodings = {
        new LocalFileCharacterEncoding(),
        
        new CharacterEncoding(3, new String[] {
            /* Display Name */          "US ASCII",
            /* Java Name */             "ASCII",
            /* Preferred MIME Name */   "US-ASCII",
            /* Name Alias */            "ANSI_X3.4-1968",
            /* Name Alias */            "ANSI_X3.4-1986",
            /* Name Alias */            "iso-ir-6",
            /* Name Alias */            "ISO_646.irv:1991",
            /* Name Alias */            "ISO646-US",
            /* Name Alias */            "us",
            /* Name Alias */            "IBM367",
            /* Name Alias */            "cp367",
            /* Name Alias */            "csASCII"
        }),
        new CharacterEncoding(4, new String[] {
            /* Display Name */          "Latin1",
            /* Java Name */             "ISO8859_1",
            /* Preferred MIME Name */   "ISO-8859-1",
            /* Name Alias */            "ISO_8859-1:1987",
            /* Name Alias */            "iso-ir-100",
            /* Name Alias */            "l1",
            /* Name Alias */            "IBM819",
            /* Name Alias */            "CP819",
            /* Name Alias */            "csISOLatin1"
        }),
        new CharacterEncoding(5, new String[] {
            /* Display Name */          "Latin2",
            /* Java Name */             "ISO8859_2",
            /* Preferred MIME Name */   "ISO-8859-2",
            /* Name Alias */            "ISO_8859-2:1987",
            /* Name Alias */            "iso-ir-101",
            /* Name Alias */            "l2",
            /* Name Alias */            "csISOLatin2"
        }),
        new CharacterEncoding(6, new String[] {
            /* Display Name */          "Latin3",
            /* Java Name */             "ISO8859_3",
            /* Preferred MIME Name */   "ISO-8859-3",
            /* Name Alias */            "ISO_8859-3:1988",
            /* Name Alias */            "iso-ir-109",
            /* Name Alias */            "l3",
            /* Name Alias */            "csISOLatin3"
        }),
        new CharacterEncoding(7, new String[] {
            /* Display Name */          "Latin4",
            /* Java Name */             "ISO8859_4",
            /* Preferred MIME Name */   "ISO-8859-4",
            /* Name Alias */            "ISO_8859-4:1988",
            /* Name Alias */            "iso-ir-110",
            /* Name Alias */            "l4",
            /* Name Alias */            "csISOLatin4"
        }),
        new CharacterEncoding(8, new String[] {
            /* Display Name */          "Cyrillic",
            /* Java Name */             "ISO8859_5",
            /* Preferred MIME Name */   "ISO-8859-5",
            /* Name Alias */            "ISO_8859-5:1988",
            /* Name Alias */            "iso-ir-144",
            /* Name Alias */            "cyrillic",
            /* Name Alias */            "csISOLatinCyrillic"
        }),
        new CharacterEncoding(9, new String[] {
            /* Display Name */          "Arabic",
            /* Java Name */             "ISO8859_6",
            /* Preferred MIME Name */   "ISO-8859-6",
            /* Name Alias */            "ISO_8859-6:1987",
            /* Name Alias */            "iso-ir-127",
            /* Name Alias */            "ECMA-114",
            /* Name Alias */            "ASMO-708",
            /* Name Alias */            "arabic",
            /* Name Alias */            "csISOLatinArabic"
        }),
        new CharacterEncoding(10, new String[] {
            /* Display Name */          "Greek",
            /* Java Name */             "ISO8859_7",
            /* Preferred MIME Name */   "ISO-8859-7",
            /* Name Alias */            "ISO_8859-7:1987",
            /* Name Alias */            "iso-ir-126",
            /* Name Alias */            "ELOT_928",
            /* Name Alias */            "ECMA-118",
            /* Name Alias */            "greek",
            /* Name Alias */            "greek8",
            /* Name Alias */            "csISOLatinGreek"
        }),
        new CharacterEncoding(11, new String[] {
            /* Display Name */          "Hebrew",
            /* Java Name */             "ISO8859_8",
            /* Preferred MIME Name */   "ISO-8859-8",
            /* Name Alias */            "ISO_8859-8:1988",
            /* Name Alias */            "iso-ir-138",
            /* Name Alias */            "hebrew",
            /* Name Alias */            "csISOLatinHebrew"
        }),
        new CharacterEncoding(12, new String[] {
            /* Display Name */          "Latin5",
            /* Java Name */             "ISO8859_9",
            /* Preferred MIME Name */   "ISO-8859-9",
            /* Name Alias */            "ISO_8859-9:1989",
            /* Name Alias */            "iso-ir-148",
            /* Name Alias */            "l5",
            /* Name Alias */            "csISOLatin5"
        }),
        new CharacterEncoding(17, new String[] {
            /* Display Name */          "Shift JIS",
            /* Java Name */             "SJIS",
            /* Preferred MIME Name */   "Shift_JIS",
            /* Name Alias */            "Shift-JIS",
            /* Name Alias */            "MS_Kanji",
            /* Name Alias */            "csShiftJIS",
        }),
        new CharacterEncoding(18, new String[] {
            /* Display Name */          "Extended_UNIX_Code_Packed_Format_for_Japanese",
            /* Java Name */             "EUC_JP",
            /* Preferred MIME Name */   "EUC-JP",
            /* Name Alias */            "csEUCPkdFmtJapanese",
        }),
        new CharacterEncoding(39, new String[] {
            /* Display Name */          "ISO-2022-JP",
            /* Java Name */             "ISO2022JP",
            /* Preferred MIME Name */   "ISO-2022-JP",
            /* Name Alias */            "csISO2022JP",
        }),
        new CharacterEncoding(106, new String[] {
            /* Display Name */          "UTF 8",
            /* Java Name */             "UTF8",
            /* Preferred MIME Name */   "UTF-8",
            /* Name Alias */            "UTF_8",
        }),
        new CharacterEncoding(1012, new String[] {
            /* Display Name */          "UTF 7",
            /* Java Name */             "UTF7",
            /* Preferred MIME Name */   "UTF-7",
            /* Name Alias */            "UTF_7",
        }),
        new UnicodeCharacterEncoding(1000, new String[] {
            /* Display Name */          "Unicode",
            /* Java Name */             "UnicodeBig",
            /* Preferred MIME Name */   "ISO-10646-UCS-2",
            /* Name Alias */            "UCS2",
            /* Name Alias */            "csUnicode"
        }),
        new UnicodeCharacterEncoding(1015, new String[] {
            /* Display Name */          "UTF 16",
            /* Java Name */             "UTF16",
            /* Preferred MIME Name */   "UTF-16",
            /* Name Alias */            "UTF_16",
        }),
        new CharacterEncoding(2025, new String[] {
            /* Display Name */          "Simplified Chinese",
            /* Java Name */             "EUC_CN",
            /* Preferred MIME Name */   "GB2312",
            /* Name Alias */            "csGB2312",
        }),
        new CharacterEncoding(2026, new String[] {
            /* Display Name */          "Traditional Chinese",
            /* Java Name */             "Big5",
            /* Preferred MIME Name */   "Big5",
            /* Name Alias */            "csBig5",
        }),
    };
    
    
    private static final int defaultEncoding = Enum_UTF8;       // Default Encoding is UTF-8
    private static final int defaultHTTPEncoding = Enum_Latin1; // Default HTTP Encoding is Latin1
    private static final int internalEncoding = Enum_Unicode;   // Internal Encoding is Unicode
    
    private static final int Enum_File = 0;
    private static CharacterEncoding fileEncoding = null;       // Current File Encoding
    
    
    //
    // Instance member data:
    //
    private int ianaMIBenum;        // The IANA MIBenum value for the encoding
    private String[] names;         // The set of names and  aliases for the encoding
                                    // This will always contain at least three elements
                                    // The first element is always the display name
                                    // The second element is always the Java name
                                    // The third element is always the Preferred MIME name
                                    // Any additional elements are other alias names
    private byte[] stringTerminator;// The string termination byte sequence

    //
    // XML Support
    //
    private static final int MAXPUSHBACK = 512;

    /*
     * Given an input stream of XML, return the detected character
     * encoding or null if not found.
     */
    public static String getEncodingFromXml(InputStream stream) throws IOException
    {
	PushbackInputStream	pb;
        byte			buf [];
	int			len;

	pb = new PushbackInputStream (stream, MAXPUSHBACK);
        //
        // See if we can figure out the character encoding used
        // in this file by peeking at the first few bytes.
        //
	buf = new byte [4];
	len = pb.read (buf);
	if (len > 0)
	    pb.unread (buf, 0, len);

	if (len == 4) switch (buf [0] & 0x0ff) {
            case 0:
              // 00 3c 00 3f == illegal UTF-16 big-endian
              if (buf [1] == 0x3c && buf [2] == 0x00 && buf [3] == 0x3f) {
                  return "UnicodeBig";
              }
	      // else it's probably UCS-4
	      break;

            case '<':      // 0x3c: the most common cases!
              switch (buf [1] & 0x0ff) {
                // First character is '<'; could be XML without
		// an XML directive such as "<hello>", "<!-- ...",
		// and so on.
                default:
                  break;

                // 3c 00 3f 00 == illegal UTF-16 little endian
                case 0x00:
                  if (buf [2] == 0x3f && buf [3] == 0x00) {
		      return "UnicodeLittle";
                  }
		  // else probably UCS-4
		  break;

                // 3c 3f 78 6d == ASCII and supersets '<?xm'
                case '?': 
                  if (buf [2] != 'x' || buf [3] != 'm')
		      break;
		  //
		  // One of several encodings could be used:
                  // Shift-JIS, ASCII, UTF-8, ISO-8859-*, etc
		  //
		  return useEncodingDecl (pb, "UTF8");
              }
	      break;

            // 4c 6f a7 94 ... some EBCDIC code page
            case 0x4c:
              if (buf [1] == 0x6f
		    && (0x0ff & buf [2]) == 0x0a7
		    && (0x0ff & buf [3]) == 0x094) {
		  return useEncodingDecl (pb, "CP037");
	      }
	      // whoops, treat as UTF-8
	      break;

            // UTF-16 big-endian
            case 0xfe:
              if ((buf [1] & 0x0ff) != 0xff)
                  break;
              return "UTF-16";

            // UTF-16 little-endian
            case 0xff:
              if ((buf [1] & 0x0ff) != 0xfe)
                  break;
	      return "UTF-16";

            // default ... no XML declaration
            default:
              break;
        }
	//
        // If all else fails, return null and depend on the input preference.
	//
	return null;
    }

    /*
     * Read the encoding decl on the stream, knowing that it should
     * be readable using the specified encoding (basically, ASCII or
     * EBCDIC).  The body of the document may use a wider range of
     * characters than the XML/Text decl itself, so we switch to use
     * the specified encoding as soon as we can.  (ASCII is a subset
     * of UTF-8, ISO-8859-*, ISO-2022-JP, EUC-JP, and more; EBCDIC
     * has a variety of "code pages" that have these characters as
     * a common subset.)
     *
     * @return String encoding or null if we can't figure it out.
     */
    private static String useEncodingDecl (PushbackInputStream pb, String encoding)
    throws IOException
    {
	byte			buffer [] = new byte [MAXPUSHBACK];
	int			len;
	Reader			r;
	int			c;

	//
	// Buffer up a bunch of input, and set up to read it in
	// the specified encoding ... we can skip the first four
	// bytes since we know that "<?xm" was read to determine
	// what encoding to use!
	//
	len = pb.read (buffer, 0, buffer.length);
	pb.unread (buffer, 0, len);
	r = new InputStreamReader (
		new ByteArrayInputStream (buffer, 4, len),
		encoding);

	//
	// Next must be "l" (and whitespace) else we conclude
	// error and punt.
	//
	if ((c = r.read ()) != 'l') {
	    return null;
	}

	//
	// Then, we'll skip any
	// 	S version="..." 	[or single quotes]
	// bit and get any subsequent 
	// 	S encoding="..." 	[or single quotes]
	//
	// We put an arbitrary size limit on how far we read; lots
	// of space will break this algorithm.
	//
	StringBuffer	buf = new StringBuffer ();
	StringBuffer	keyBuf = null;
	String		key = null;
	boolean		sawEq = false;
	char		quoteChar = 0;
	boolean		sawQuestion = false;

    XmlDecl:
	for (int i = 0; i < MAXPUSHBACK - 5; ++i) {
	    if ((c = r.read ()) == -1)
		break;

	    // ignore whitespace before/between "key = 'value'"
	    if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
		continue;

	    // ... but require at least a little!
	    if (i == 0)
		break;
	    
	    // terminate the loop ASAP
	    if (c == '?')
		sawQuestion = true;
	    else if (sawQuestion) {
		if (c == '>')
		    break;
		sawQuestion = false;
	    }
	    
	    // did we get the "key =" bit yet?
	    if (key == null || !sawEq) {
		if (keyBuf == null) {
		    if (Character.isWhitespace ((char) c))
			continue;
		    keyBuf = buf;
		    buf.setLength (0);
		    buf.append ((char)c);
		    sawEq = false;
		} else if (Character.isWhitespace ((char) c)) {
		    key = keyBuf.toString ();
		} else if (c == '=') {
		    if (key == null)
			key = keyBuf.toString ();
		    sawEq = true;
		    keyBuf = null;
		    quoteChar = 0;
		} else
		    keyBuf.append ((char)c);
		continue;
	    }

	    // space before quoted value
	    if (Character.isWhitespace ((char) c))
		continue;
	    if (c == '"' || c == '\'') {
		if (quoteChar == 0) {
		    quoteChar = (char) c;
		    buf.setLength (0);
		    continue;
		} else if (c == quoteChar) {
		    if ("encoding".equals (key)) {
			String assignedEncoding = buf.toString ();

			// [81] Encname ::= [A-Za-z] ([A-Za-z0-9._]|'-')*
			for (i = 0; i < assignedEncoding.length(); i++) {
			    c = assignedEncoding.charAt (i);
			    if ((c >= 'A' && c <= 'Z')
				    || (c >= 'a' && c <= 'z'))
				continue;
			    if (i == 0)
				break XmlDecl;
			    if (i > 0 && (c == '-'
				    || (c >= '0' && c <= '9')
				    || c == '.' || c == '_'))
				continue;
			    // map illegal names to UTF-8 default
			    break XmlDecl;
			}
			return assignedEncoding;

		    } else {
			key = null;
			continue;
		    }
		}
	    }
	    buf.append ((char) c);
	}
	return null;
    }
}
