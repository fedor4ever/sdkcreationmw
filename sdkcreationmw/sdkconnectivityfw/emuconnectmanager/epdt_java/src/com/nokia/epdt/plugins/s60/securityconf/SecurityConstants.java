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

/**
 * MIDP security configuration constants
 */
interface SecurityConstants {

    static final String MODEL_BACKUP_FILE = "epoc32/data/perm.dat";
    static final String POLICY_FILE_NAME = "midp2_rp.xpf";
    static final String BACKUP_POLICY_FILE_NAME = "midp2_rp.xpf.bak";
    static final String POLICY_FILE_PATH_RELATIVE =
        "epoc32/release/winscw/udeb/z/system/data/midp2/security/policy/";

    // Protection domain identifiers
    static final String MANUFACTURER_DOMAIN = "*MFD*";
    static final String OPERATOR_DEV_DOMAIN = "*ODOPD*";
    static final String OPERATOR_DOMAIN = "*OPD*";
    static final String THIRD_PARTY_DOMAIN = "*TTPD*";
    static final String UNTRUSTED_DOMAIN = "[UNTRUSTED]";
    static final String USER_DEFINED_DOMAIN = "[USER_DEFINED]";
    static final String WILDCARD = "[*]";
    static final String MINIMAL_DOMAIN = "Minimal";

    // Keywords in the model file
    static final String FORMAT_VERSION_KEYWORD = "FormatVersion:";
    static final String EXPECTED_FORMAT_VERSION = "1.0";
    static final String END_KEYWORD_PREFIX = "End";
    static final String DOMAINS_KEYWORD = "Domains";
    static final String   DOMAIN_KEYWORD = "Domain:";
    static final String   END_DOMAIN_KEYWORD = "EndDomain";
    static final String END_DOMAINS_KEYWORD = "EndDomains";
    static final String FUNCTION_GROUPS_KEYWORD = "FunctionGroups";
    static final String   FUNCTION_GROUP_KEYWORD = "FunctionGroup:";
    static final String   END_FUNCTION_GROUP_KEYWORD = "EndFunctionGroup";
    static final String END_FUNCTION_GROUPS_KEYWORD = "EndFunctionGroups";
    static final String BINDINGS_KEYWORD = "Bindings";
    static final String   DOMAIN_BINDINGS_KEYWORD = "DomainBindings:";
    static final String     FUNCTION_GROUP_BINDING_KEYWORD = "FunctionGroupBinding:";
    static final String       PERMISSION_KEYWORD = "Permission:";
    static final String       DEFAULT_MODE_KEYWORD = "DefaultMode:";
    static final String       MAXIMUM_MODE_KEYWORD = "MaximumMode:";
    static final String       INTERACTION_MODES_KEYWORD = "InteractionModes";
    static final String       END_INTERACTION_MODES_KEYWORD = "EndInteractionModes";
    static final String       RULES_KEYWORD = "Rules";
    static final String       END_RULES_KEYWORD = "EndRules";
    static final String     END_FUNCTION_GROUP_BINDING_KEYWORD = "EndFunctionGroupBinding";
    static final String   END_DOMAIN_BINDINGS_KEYWORD = "EndDomainBindings";
    static final String END_BINDINGS_KEYWORD = "EndBindings";
}
