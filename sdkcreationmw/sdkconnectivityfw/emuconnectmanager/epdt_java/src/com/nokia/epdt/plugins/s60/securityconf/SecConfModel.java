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
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.OutputStream;
import java.io.PrintWriter;

/**
 * MIDP security configuration model.
 */
class SecConfModel implements SecurityConstants {

    // Emulater/real life security mode
    private boolean iEmulatedSecurityMode = false;
    private String iSelectedDomainId;
    private ProtectionDomain iUserDefinedDomain;
    private List iProtectionDomains = new ArrayList();
    private List iFunctionGroups = new ArrayList();

    /**
     * Creates MIDP security configuration model.
     * @throws IOException if it fails to load default MIDP security
     * configuration from the resources.
     */
    SecConfModel() throws IOException {
        InputStream in = openPolicyFileResource();
        try {
            PolicyFileParser.parse(in, new InitialScan());
        } finally {
            in.close();
        }

        // Create user defined domain
        iUserDefinedDomain = new ProtectionDomain(USER_DEFINED_DOMAIN);
        iProtectionDomains.add(iUserDefinedDomain);
        int n = iFunctionGroups.size();
        for (int i=0; i<n; i++) {
            String domain = (String)iFunctionGroups.get(i);
            iUserDefinedDomain.setPermission(domain,Permission.Allowed);
        }

        // Select first domain by default
        iSelectedDomainId = ((ProtectionDomain)
                             iProtectionDomains.get(0)).getIdentifier();
    }

    /**
     * Gets the protection domains known to the model, including the
     * "User defined" pseudo-domain.
     *
     * @param aIncludeHidden <code>true</code> to include hidden domains
     * @return array of protection domains
     */
    ProtectionDomain [] getProtectionDomains(boolean aIncludeHidden) {
        List domains = iProtectionDomains;
        if (!aIncludeHidden) {
            // remove hidden domains from the list
            domains = new ArrayList(iProtectionDomains);
            for (int i=domains.size()-1; i>=0; i--) {
                ProtectionDomain domain = (ProtectionDomain)domains.get(i);
                if (domain.isHidden()) domains.remove(i);
            }
        }
        return (ProtectionDomain[]) domains.toArray(
            new ProtectionDomain[domains.size()]);
    }

    /**
     * Gets currently selected protection domain.
     * @return the currently selected protection domain, <code>null</code>
     *    if none.
     */
    ProtectionDomain getSelectedDomain() {
        return findProtectionDomain(iSelectedDomainId);
    }

    /**
     * Selects a protection domain
     * @param aDomain the domain to select
     */
    void setSelectedDomain(ProtectionDomain aDomain) {
        if (aDomain == null) {
            iSelectedDomainId = null;
        } else {
            iSelectedDomainId = aDomain.getIdentifier();
        }
    }

    /**
     * Gets known function groups.
     * @return array of function group names
     */
    String[] getFunctionGroups() {
        return (String[])iFunctionGroups.toArray(
            new String[iFunctionGroups.size()]);
    }

    /**
     * Checks if we are in the "emulated security mode".
     * @return <code>true</code> if we are enforcing the same permissions
     *   for all protection domains.
     */
    boolean isEmulatedSecurityMode() {
        return iEmulatedSecurityMode;
    }

    /**
     * Turns the "emulated security mode" on or off.
     * @param aEmulatedSecurityMode <code>true</code> to enforce the same
     *   permissions for all protection domains, <code>false</code> to use
     *   default JVM secrity settings.
     */
    void setEmulatedSecurityMode(boolean aEmulatedSecurityMode) {
        iEmulatedSecurityMode = aEmulatedSecurityMode;
    }

    /**
     * Opens the policy file resource.
     * @return the input stream for the policy file resource.
     * Never returns null.
     * @throws IOException if resource is not found
     */
    private InputStream openPolicyFileResource() throws IOException {
        InputStream in = getClass().getResourceAsStream(POLICY_FILE_NAME);
        if (in != null) return in;
        throw new IOException(POLICY_FILE_NAME);
    }

    /**
     * Finds ProtectionDomain object for the specified identifier.
     * @param aId the domain identifier
     * @return ProtectionDomain object if found, null otherwise.
     */
    private ProtectionDomain findProtectionDomain(String aId) {
        int n = iProtectionDomains.size();
        for (int i=0; i<n; i++) {
            ProtectionDomain d = (ProtectionDomain)iProtectionDomains.get(i);
            if (d.getIdentifier().equals(aId)) {
                return d;
            }
        }
        return null;
    }

    /**
     * Writes policy file to the output stream
     * @param aStream the stream to write to
     * @throws IOException if an I/O error occurs
     */
    void writePolicyFile(OutputStream aStream) throws IOException {
        InputStream in = openPolicyFileResource();
        try {
            PrintWriter out = new PrintWriter(aStream, false);
            try {
                if (!iEmulatedSecurityMode) {
                    // Just copy the original file
                    BufferedReader reader = new BufferedReader(
                        new InputStreamReader(in));
                    String line = null;
                    while ((line = reader.readLine()) != null) {
                        out.println(line);
                    }
                } else {
                    // Save modified version of the original file
                    out.println("# This is a generated file.");
                    out.println();
                    PolicyFileParser.parse(in, new PolicyFileWriter(out));
                }
            } finally {
                out.flush();
            }
        } finally {
            in.close();
        }
    }

    /**
     * Saves the state of the model to the output stream
     * @param aStream the stream to write to
     * @throws IOException if an I/O error occurs
     */
    void save(OutputStream aStream) throws IOException {
        ObjectOutputStream out = new ObjectOutputStream(aStream);
        out.writeBoolean(iEmulatedSecurityMode);
        out.writeObject(iSelectedDomainId);
        out.writeObject(iUserDefinedDomain);
    }

    /**
     * Loads the state of the model from the input stream
     * @param aStream the stream to read from
     * @throws IOException if an I/O error occurs
     * @throws ClassNotFoundException if deserialization error occurs
     */
    void load(InputStream aStream) throws IOException, ClassNotFoundException {
        ObjectInputStream in = new ObjectInputStream(aStream);
        boolean emulatedSecurityMode = in.readBoolean();
        String selectedDomainId = (String)in.readObject();
        ProtectionDomain userDomain = (ProtectionDomain)in.readObject();

        // Verify and store deserialized values
        iEmulatedSecurityMode = emulatedSecurityMode;
        if (findProtectionDomain(selectedDomainId) != null) {
            iSelectedDomainId = selectedDomainId;
        }
        if (userDomain.isUserDefined()) {
            // User defined domain is always the last one
            iProtectionDomains.set(iProtectionDomains.size()-1,userDomain);
            iUserDefinedDomain = userDomain;
        }
    }

    /**
     * This handler scans the policy file to collect information about
     * domains, function groups and bindings.
     */
    private class InitialScan implements PolicyFileParser.Handler {
        private String iProtectionDomain;
        private String iFunctionGroup;
        public void handleFormatDecl(String aFormatVersion) {}
        public void handleEmptyLine() {}
        public void handleDecl(String aLine) {}
        public void handleProtectionDomainDecl(String aDomain) {
            ProtectionDomain domain = findProtectionDomain(aDomain);
            if (domain == null) {
                domain = new ProtectionDomain(aDomain);
                iProtectionDomains.add(domain);
            }
        }
        public void handleFunctionGroupDecl(String aGroupName) {
            iFunctionGroups.add(aGroupName);
        }
        public void handleDomainBindingsDecl(String aProtectionDomain) {
            iProtectionDomain = aProtectionDomain;
        }
        public void handleFunctionGroupBindingDecl(String aFunctionGroup) {
            iFunctionGroup = aFunctionGroup;
        }
        public void handlePermission(Permission aPermission) {
            ProtectionDomain domain = findProtectionDomain(iProtectionDomain);
            if (domain != null && iFunctionGroup != null) {
                if (WILDCARD.equals(iFunctionGroup)) {
                    int n = iFunctionGroups.size();
                    for (int i=0; i<n; i++) {
                        domain.setPermission((String)iFunctionGroups.get(i),
                                             aPermission);
                    }
                } else {
                    domain.setPermission(iFunctionGroup, aPermission);
                }
            }
        }

        public void handleEndFunctionGroupBinding() {
            iFunctionGroup = null;
        }
        public void handleEndDomainBindings() {
            iProtectionDomain = null;
        }
    }

    /**
     * This handler writes a modified version of the policy file.
     * Must only be used in the "emulated" security mode.
     */
    private class PolicyFileWriter implements PolicyFileParser.Handler {
        private PrintWriter iWriter;
        private ProtectionDomain iEmulatedDomain;
        private String iProtectionDomain;

        PolicyFileWriter(PrintWriter aWriter) {
            if (!isEmulatedSecurityMode()) throw new IllegalStateException();
            iWriter = aWriter;
            iEmulatedDomain = getSelectedDomain();
        }
        private void printDecl(String aKeyword, String aValue) {
            iWriter.print('\t');
            iWriter.print(aKeyword);
            iWriter.print(' ');
            // For some reason, strings enclosed in square brackets (such as
            // [UNTRUSTED] or [*]) are not quoted. That may have some special
            // meaning to the phone software.
            int len = aValue.length();
            if (len >= 2 &&
                aValue.charAt(0) == '[' &&
                aValue.charAt(len-1) == ']') {
                iWriter.println(aValue);
            } else {
                iWriter.print('"');
                iWriter.print(aValue);
                iWriter.println('"');
            }
        }

        // PolicyFileParser.Handler interface
        public void handleFormatDecl(String aFormatVersion) {
            iWriter.print(FORMAT_VERSION_KEYWORD);
            iWriter.print(" ");
            iWriter.println(aFormatVersion);
        }
        public void handleEmptyLine() {
            if (iProtectionDomain == null) {
                iWriter.println();
            }
        }
        public void handleDecl(String aLine) {
            if (iProtectionDomain == null) {
                iWriter.println(aLine);
            }
        }
        public void handleProtectionDomainDecl(String aProtectionDomain) {
            printDecl(DOMAIN_KEYWORD, aProtectionDomain);
        }
        public void handleFunctionGroupDecl(String aGroupName) {
            iWriter.print('\t');
            iWriter.print(FUNCTION_GROUP_KEYWORD);
            iWriter.print(" \"");
            iWriter.print(aGroupName);
            iWriter.println('"');
        }
        public void handleDomainBindingsDecl(String aProtectionDomain) {
            printDecl(DOMAIN_BINDINGS_KEYWORD, aProtectionDomain);
            iWriter.println();
            iProtectionDomain = aProtectionDomain;
        }
        public void handleFunctionGroupBindingDecl(String aFunctionGroup) {}
        public void handlePermission(Permission aPermission) {}
        public void handleEndFunctionGroupBinding() {}
        public void handleEndDomainBindings() {
            int n = iFunctionGroups.size();
            for (int i=0; i<n; i++) {
                String group = (String)iFunctionGroups.get(i);
                Permission p = iEmulatedDomain.getPermission(group);
                if (!p.isDenied()) {
                    iWriter.print('\t');
                    printDecl(FUNCTION_GROUP_BINDING_KEYWORD, group);
                    iWriter.println();
                    p.print(iWriter);
                    iWriter.print("\t\t");
                    iWriter.println(END_FUNCTION_GROUP_BINDING_KEYWORD);
                    iWriter.println();
                }
            }
            iWriter.print('\t');
            iWriter.println(END_DOMAIN_BINDINGS_KEYWORD);
            iProtectionDomain = null;
        }
    }
}
