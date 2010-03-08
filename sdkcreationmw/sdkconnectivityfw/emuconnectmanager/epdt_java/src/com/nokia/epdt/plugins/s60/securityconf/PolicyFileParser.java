/*
* Copyright (c) 2000 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.epdt.plugins.s60.securityconf;

/* java.util */
import java.util.List;
import java.util.ArrayList;

/* java.io */
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.BufferedReader;

/**
 * Simple parser for XPF files. Really parses only the sections that we are
 * interested in.
 */
class PolicyFileParser implements SecurityConstants {

    /**
     * Interface to receive parsed information from the parser.
     */
    interface Handler {
        void handleFormatDecl(String aFormatVersion) throws IOException;
        void handleEmptyLine() throws IOException;
        void handleDecl(String aLine) throws IOException;
        void handleProtectionDomainDecl(String aId) throws IOException;
        void handleFunctionGroupDecl(String aGroupName) throws IOException;
        void handleDomainBindingsDecl(String aProtectionDomain) throws IOException;
        void handleFunctionGroupBindingDecl(String aFunctionGroup) throws IOException;
        void handlePermission(Permission aPermission) throws IOException;
        void handleEndFunctionGroupBinding() throws IOException;
        void handleEndDomainBindings() throws IOException;
    }

    // Policy file parsing states
    private static final int PARSE_INIT = 0;
    private static final int PARSE_TOP = 1;
    private static final int PARSE_DOMAINS = 2;
    private static final int PARSE_FUNCTION_GROUPS = 3;
    private static final int PARSE_BINDINGS = 4;
    private static final int PARSE_DOMAIN_BINDINGS = 5;
    private static final int PARSE_FUNCTION_GROUP_BINDING = 6;

    /**
     * Parses the policy file collecting information about protection domains,
     * functio groups and bindings.
     *
     * @param aStream the input stream to read.
     * @param aHandler the handler to invoke
     * @throws IOException in case of I/O error
     */
    static void parse(InputStream aStream, Handler aHandler)
        throws IOException {

        List functionGroups = new ArrayList();
        List protectionDomains = new ArrayList();

        BufferedReader in = new BufferedReader(new InputStreamReader(aStream));
        String domain = null;
        String functionGroup = null;
        String permissionType = null;
        String defaultMode = null;
        String maximumMode = null;
        Permission permission = null;

        String line = null;
        int state = PARSE_INIT;
        while ((line = in.readLine()) != null) {
            String s = prepareLine(line);
            if (s.length() == 0) {
                switch (state) {
                default:
                    if (line.trim().length() == 0) {
                        aHandler.handleEmptyLine();
                    } else {
                         aHandler.handleDecl(line);
                   }
                    // no break
                case PARSE_INIT:
                case PARSE_FUNCTION_GROUP_BINDING:
                    // not reporting empty lines in these modes
                    continue;
                }
            }

            String value;
            switch (state) {
            case PARSE_INIT:
                value = parseNameValue(s, FORMAT_VERSION_KEYWORD);
                if (value != null) {
                    if (value.equals(EXPECTED_FORMAT_VERSION)) {
                        aHandler.handleFormatDecl(value);
                        state = PARSE_TOP;
                    } else {
                        throw new IOException("Unrecognized format version "
                                              + value);
                    }
                } else {
                    throw new IOException("Unrecognized file format");
                }
                break;

            case PARSE_TOP:
                aHandler.handleDecl(line);
                if (s.equals(DOMAINS_KEYWORD)) {
                    state = PARSE_DOMAINS;
                } else if (s.equals(FUNCTION_GROUPS_KEYWORD)) {
                    state = PARSE_FUNCTION_GROUPS;
                } else if (s.equals(BINDINGS_KEYWORD)) {
                    state = PARSE_BINDINGS;
                    domain = null;
                } else {
                    skip(in, aHandler, END_KEYWORD_PREFIX + s);
                }
                break;

            case PARSE_DOMAINS:
                value = parseNameValue(s, DOMAIN_KEYWORD);
                if (value != null) {
                    aHandler.handleProtectionDomainDecl(value);
                    protectionDomains.add(value);
                } else {
                    aHandler.handleDecl(line);
                    if (s.equals(END_DOMAINS_KEYWORD)) {
                        state = PARSE_TOP;
                    } else if (!s.equals(END_DOMAIN_KEYWORD)) {
                        skip(in, aHandler, END_KEYWORD_PREFIX + s);
                    }
                }
                break;

            case PARSE_FUNCTION_GROUPS:
                value = parseNameValue(s, FUNCTION_GROUP_KEYWORD);
                if (value != null) {
                    aHandler.handleFunctionGroupDecl(value);
                    functionGroups.add(value);
                    skip(in, aHandler, END_FUNCTION_GROUP_KEYWORD);
                } else {
                    aHandler.handleDecl(line);
                    if (s.equals(END_FUNCTION_GROUPS_KEYWORD)) {
                        state = PARSE_TOP;
                    } else if (!s.equals(END_FUNCTION_GROUP_KEYWORD)) {
                        skip(in, aHandler, END_KEYWORD_PREFIX + s);
                    }
                }
                break;

            case PARSE_BINDINGS:
                domain = parseNameValue(s, DOMAIN_BINDINGS_KEYWORD);
                if (domain != null) {
                    if (protectionDomains.contains(domain)) {
                        aHandler.handleDomainBindingsDecl(domain);
                        state = PARSE_DOMAIN_BINDINGS;
                        functionGroup = null;
                    } else {
                        aHandler.handleDecl(line);
                        skip(in, aHandler, END_DOMAIN_BINDINGS_KEYWORD);
                    }
                } else if (s.equals(END_BINDINGS_KEYWORD)) {
                    aHandler.handleDecl(line);
                    state = PARSE_TOP;
                } else {
                    aHandler.handleDecl(line);
                    skip(in, aHandler, END_KEYWORD_PREFIX + s);
                }
                break;

            case PARSE_DOMAIN_BINDINGS:
                value = parseNameValue(s, FUNCTION_GROUP_BINDING_KEYWORD);
                if (value != null) {
                    if (value.equals(WILDCARD) || functionGroups.contains(value)) {
                        aHandler.handleFunctionGroupBindingDecl(value);
                        state = PARSE_FUNCTION_GROUP_BINDING;
                        functionGroup = value;
                        permissionType = null;
                        defaultMode = null;
                        maximumMode = null;
                        permission = null;
                    } else {
                        aHandler.handleDecl(line);
                        skip(in, aHandler, END_FUNCTION_GROUP_BINDING_KEYWORD);
                    }
                } else if (s.equals(END_DOMAIN_BINDINGS_KEYWORD)) {
                    aHandler.handleEndDomainBindings();
                    domain = null;
                    state = PARSE_BINDINGS;
                } else {
                    aHandler.handleDecl(line);
                    skip(in, aHandler, END_KEYWORD_PREFIX + s);
                }
                break;

            case PARSE_FUNCTION_GROUP_BINDING:
                value = parseNameValue(s, PERMISSION_KEYWORD);
                if (value != null) {
                    permissionType = value;
                } else {
                    value = parseNameValue(s, DEFAULT_MODE_KEYWORD);
                    if (value != null) {
                        defaultMode = value;
                    } else {
                        value = parseNameValue(s, MAXIMUM_MODE_KEYWORD);
                        if (value != null) {
                            maximumMode = value;
                        } else {
                            if (permissionType != null && permission == null) {
                                permission = Permission.getPermission(
                                    permissionType, defaultMode, maximumMode);
                                aHandler.handlePermission(permission);
                            }

                            if (s.equals(END_FUNCTION_GROUP_BINDING_KEYWORD)) {
                                aHandler.handleEndFunctionGroupBinding();
                                state = PARSE_DOMAIN_BINDINGS;
                            } else {
                                aHandler.handleDecl(line);
                                skip(in, aHandler, END_KEYWORD_PREFIX + s);
                            }
                        }
                    }
                }
            }
        }
    }

    /**
     * Strips comments, leading and trailing whitespaces.
     *
     * @param aLine the line to prepare
     * @return prepared string
     * @throws IOException
     */
    private static String prepareLine(String aLine) {
        int comment = aLine.indexOf('#');
        if (comment == 0) {
            return "";
        } else {
            if (comment > 0) aLine = aLine.substring(0,comment);
            aLine = aLine.trim();
            return aLine;
        }
    }

    /**
     * Skips all statements until the specified keyword, including the keyword.
     * @param aReader the reader to read
     * @param aHandler the handler to invoke
     * @param aKeyword the keyword to look for
     * @throws IOException in case of I/O error
     */
    private static void skip(BufferedReader aReader,
                             Handler aHandler,
                             String aKeyword) throws IOException {
        String line = null;
        while ((line = aReader.readLine()) != null) {
           String s = prepareLine(line);
           if (s.length() == 0) {
               aHandler.handleEmptyLine();
           } else {
               aHandler.handleDecl(line);
           }
           if (aKeyword.equals(s)) {
               break;
           }
       }
    }

    /**
     * Parses a name-value pair from the policy file
     * @param aLine the string to parse
     * @param aPrefix the prefix, normally ends with colon
     * @return the value, or null if the input string does not look right
     */
    private static String parseNameValue(String aLine, String aPrefix) {
        if (aLine.startsWith(aPrefix)) {
            String s = aLine.substring(aPrefix.length()).trim();
            int len = s.length();
            if (len >= 2 && s.charAt(0) == '"' && s.charAt(len-1) == '"') {
                // remove quotes
                s = s.substring(1,len-1);
            }
            return s;
        } else {
            return null;
        }
    }
}
