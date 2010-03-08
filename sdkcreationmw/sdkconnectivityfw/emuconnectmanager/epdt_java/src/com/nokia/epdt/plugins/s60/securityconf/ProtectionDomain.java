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
import java.util.Map;
import java.util.HashMap;

/* java.io */
import java.io.Serializable;

/**
 * Protection domain with associated function group bindings.
 * This object is seializable. Serialization is used for storing
 * MIDP security settings.
 */
class ProtectionDomain implements SecurityConstants, Serializable {

    private String iIdentifier;
    private String iName;
    private Map iBindings;   // String -> Permission

    /**
     * No-arg constructor for serialization purposes
     */
    protected ProtectionDomain() {
    }

    /**
     * Creates a new protection domain object
     * @param aIdentifier the protection domain identifier
     */
    ProtectionDomain(String aIdentifier) {
        this(aIdentifier, null);
    }

    /**
     * Creates a new protection domain object
     * @param aIdentifier the protection domain identifier
     * @param aName human readable name for this protection domain,
     *    <code>null</code> to use default
     */
    ProtectionDomain(String aIdentifier, String aName) {
        if (aName == null) aName = defaultName(aIdentifier);
        iName = aName;
        iIdentifier = aIdentifier;
        iBindings = new HashMap();
    }

    /**
     * Picks default human readable domain name for the specified identifier.
     * @param aId the protection domain identifier
     * @return default human readable domain name for the specified domain.
     */
    private static String defaultName(String aId) {
        if (aId.equals(MANUFACTURER_DOMAIN)) return "Manufacturer";
        if (aId.equals(OPERATOR_DEV_DOMAIN)) return "Operator Development";
        if (aId.equals(OPERATOR_DOMAIN)) return "Operator";
        if (aId.equals(THIRD_PARTY_DOMAIN)) return "Trusted Third Party";
        if (aId.equals(UNTRUSTED_DOMAIN)) return "Untrusted";
        if (aId.equals(USER_DEFINED_DOMAIN)) return "User defined";
        if (aId.equals(MINIMAL_DOMAIN)) return "Minimal";
        return aId;
    }

    /**
     * Checks whether this security domain needs to be hidden from the UI
     * @return <code>true</code> if domain should be hidden,
     *   <code>false</code> otherwise
     */
    boolean isHidden() {
        return OPERATOR_DEV_DOMAIN.equals(iIdentifier);
    }

    /**
     * Checks whether this security domain can be changed by the user.
     * @return <code>true</code> if domain settings can be changed,
     *   <code>false</code> otherwise
     */
    boolean isUserDefined() {
        return USER_DEFINED_DOMAIN.equals(iIdentifier);
    }

    /**
     * Gets the human readable domain name.
     * @return human readable domain name.
     */
    String getName() {
        return iName;
    }

    /**
     * Gets the uniqie protection domain identifier.
     * @return unique protection domain identifier.
     */
    String getIdentifier() {
        return iIdentifier;
    }

    /**
     * Sets permission for the specified function group
     * @param aFunctionGroup the function group name
     * @param aPermission permission to set
     */
    void setPermission(String aFunctionGroup, Permission aPermission) {
        if (aPermission == null ||
            Permission.Denied.getType().equals(aPermission.getType())) {
            iBindings.remove(aFunctionGroup);
        } else {
            iBindings.put(aFunctionGroup, aPermission);
        }
    }

    /**
     * Gets permission for the specified function group.
     * @param aFunctionGroup the function group name
     * @return permission for the specified function group,
     *   <code>Permission.Denied</code> if none.
     */
    Permission getPermission(String aFunctionGroup) {
        Permission permission = (Permission)iBindings.get(aFunctionGroup);
        return ((permission == null) ? Permission.Denied : permission);
    }

    /**
     * Checks if binding with the specified function group exists for this
     * protection domain.
     *
     * @param aFunctionGroup the function group name
     * @return <code>true</code> if permission is specified for this function
     *   group, <code>false</code> otherwise.
     */
    boolean hasBinding(String aFunctionGroup) {
        return iBindings.containsKey(aFunctionGroup);
    }

    /**
     * Returns a string that describes this object.
     * @return human readable domain name.
     */
    public String toString() {
        return iName;
    }
}
