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

/* java.io */
import java.io.PrintWriter;
import java.io.Serializable;
import java.io.ObjectStreamException;

/**
 * Class Permission
 */
class Permission implements Serializable, SecurityConstants {

    static final String Interactive = "User";

   //======================================================================
   // Standard permissions
   //======================================================================

   static final Permission Allowed = new Permission("Allowed", "Allowed");
   static final Permission Denied = new Permission("Denied", "Denied");
   static final Permission Session = new Permission("Ask once", Interactive, "Session", "Session");
   static final Permission OneShot = new Permission("Ask every time", Interactive, "Oneshot", "Oneshot");

   /**
    * Creates a new Permission object, or creates one of the predefined
    * permissions.
    *
    * @param aType the permission type
    * @param aDefaultMode the default interaction mode for "User" permission
    * @param aMaximumMode the maximum interaction mode for "User" permission
    * @return a new Permission object, or one of the standard permissions
    */
   static Permission getPermission(String aType,
                                   String aDefaultMode,
                                   String aMaximumMode) {
       if (aType == null) throw new NullPointerException("type");

       // First check standard permissions
       if (Allowed.match(aType, aDefaultMode, aMaximumMode)) return Allowed;
       if (Denied.match(aType, aDefaultMode, aMaximumMode)) return Denied;
       if (Session.match(aType, aDefaultMode, aMaximumMode)) return Session;
       if (OneShot.match(aType, aDefaultMode, aMaximumMode)) return OneShot;

       // Select a name for the new permission
       String name;
       if (Allowed.iType.equals(aType)) {
           name = Allowed.iName;
       } else if (Denied.iType.equals(aType)) {
           name = Denied.iName;
       } else if (aType.equals(Interactive)) {
           if (Session.iDefaultMode.equals(aDefaultMode)) {
               name = Session.iName;
           } else if (OneShot.iDefaultMode.equals(aDefaultMode)) {
               name = OneShot.iName;
           } else {
               name = aType;
           }
       } else {
           name = aType;
       }

       // Create a new one
       return new Permission(name, aType, aDefaultMode, aMaximumMode);
   }

   //======================================================================
   // Methods and fields of the FunctionGroup object. Private constructor
   // ensures that static variables declared above are the only instances
   // of this class.
   //======================================================================

    //Name on the UI:
    private String iName;

    //These members are policy configuration file syntax related:
    private String iType;
    private String iDefaultMode;
    private String iMaximumMode;

    private Permission(String aName, String aType) {
        this(aName, aType, null, null);
    }

    private Permission(String aName, String aType,
                       String aDefaultMode,
                       String aMaximumMode) {
        iName = aName;
        iType = aType;
        iDefaultMode = aDefaultMode;
        iMaximumMode = aMaximumMode;
        if (iMaximumMode == null) {
            iMaximumMode = iDefaultMode;
        } else if (iDefaultMode == null) {
            iDefaultMode = iMaximumMode;
        }
    }

    /**
     * Gets the permission name. This string shows up in the user interface.
     * @return the permission name.
     */
    String getName() {
        return iName;
    }

    /**
     * Gets permission type string
     * @return the permission type string
     */
    String getType() {
        return iType;
    }

    /**
     * Tests if this permission denies access to the function group.
     * @return <code<true</code> if this permission denies access to the
     *         function group, <code>false</code> otherwise
     */
    boolean isDenied() {
        return iType.equals(Denied.iType);
    }

    /**
     * Tests if this is an interactive (User) permission.
     * @return <code<true</code> if this is an interactive (User) permission,
     *         <code>false</code> otherwise
     */
    boolean isInteractive() {
        return iType.equals(Session.iType);
    }

    /**
     * Returns a string describing this object. This string shows up
     * in the user interface.
     * @return a string describing this object
     */
    public String toString() {
        return iName;
    }

    /**
     * Matches this object against type, DefaultMode and MaximumMode strings
     * from the XPF file.
     * @param aType the permission type to match
     * @param aDefaultMode the default mode to match
     * @param aMaxMode the maximum mode to match
     * @return <code<true</code> if we have a match,
     *         <code>false</code> otherwise
     */
    private boolean match(String aType, String aDefaultMode, String aMaxMode) {
        return iType.equals(aType) &&

            ((iDefaultMode == null &&
             (aDefaultMode == null || aDefaultMode.length() == 0) ||
             (iDefaultMode != null &&
              iDefaultMode.equals(aDefaultMode)))) &&

            ((iMaximumMode == null &&
             (aMaxMode == null || aMaxMode.length() == 0) ||
             (iMaximumMode != null &&
              iMaximumMode.equals(aMaxMode))));
    }

    /**
     * Optimizes the deserialization process. The Permission objects that get
     * serialized equal one of the static members of this class. This method
     * substitutes deserialized Permision object with one of static Permission
     * objects.
     * @return the substituted object
     * @throws ObjectStreamException in case of trouble. There can't be any
     * trouble, but exception attribute is part of the method signature.
     */
    private Object readResolve() throws ObjectStreamException {
       if (Allowed.equals(this)) return Allowed;
       if (Denied.equals(this)) return Denied;
       if (Session.equals(this)) return Session;
       if (OneShot.equals(this)) return OneShot;
       return this;
    }

    /**
     * Prints a permission inside a domain binding/function group binding
     * block in the policy configuration file
     *
     * @param aWriter the writer to print to
     */
    void print(PrintWriter aWriter) {
        aWriter.print("\t\t\t");
        aWriter.print(PERMISSION_KEYWORD);
        aWriter.print("\t\t");
        aWriter.println(iType);
        aWriter.println();
        if (iDefaultMode != null) {
            aWriter.print("\t\t\t");
            aWriter.print(DEFAULT_MODE_KEYWORD);
            aWriter.print('\t');
            aWriter.println(iDefaultMode);
            aWriter.println();
        }
        if (iMaximumMode != null) {
            aWriter.print("\t\t\t");
            aWriter.print(MAXIMUM_MODE_KEYWORD);
            aWriter.print('\t');
            aWriter.println(iMaximumMode);
            aWriter.println();
        }
    }

    /**
     * Indicates whether some other object is "equal to" this one.
     *
     * @param obj the reference object with which to compare.
     * @return  <code>true</code> if this object is the same as the obj
     *          argument; <code>false</code> otherwise.
     */
    public boolean equals(Object obj) {
        if (obj == this) {
            return true;
        } else if (obj instanceof Permission) {
            Permission that = (Permission)obj;
            return iType.equals(that.iType) &&
                ((iDefaultMode == null && that.iDefaultMode == null) ||
                 (iDefaultMode != null && iDefaultMode.equals(that.iDefaultMode))) &&
                ((iMaximumMode == null && that.iMaximumMode == null) ||
                 (iMaximumMode != null && iMaximumMode.equals(that.iMaximumMode)));
        } else {
            return false;
        }
    }

    /**
     * Returns a hash code value for the object.
     * @return a hash code value for this object.
     */
    public int hashCode() {
        return 31*(31*iType.hashCode() +
            (iDefaultMode==null ? 0 : iDefaultMode.hashCode())) +
            (iMaximumMode==null ? 0 : iMaximumMode.hashCode());
    }
}
