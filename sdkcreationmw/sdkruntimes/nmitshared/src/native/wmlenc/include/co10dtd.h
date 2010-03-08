/*
 * Cache Operation (CO) Document Type Definition.
 * 
 * Copyright Wireless Application Protocol Forum Ltd., 1998,1999.
 * All rights reserved.  
 * 
 * CO is an XML language.  Typical usage:
 *    <?xml version="1.0"?>
 *    <!DOCTYPE co PUBLIC "-//WAPFORUM//DTD CO 1.0//EN"
 *                 "http://www.wapforum.org/DTD/co_1.0.dtd">
 *    <co>
 *    ...
 *    </co>
 * 
 * Terms and conditions of use are available from the Wireless Application
 * Protocol Forum Ltd. web site at http://www.wapforum.org/docs/copyright.htm
 */
static char CO10DTD[] = \
"<!ELEMENT co (invalidate-object | invalidate-service)+>\n"\
"<!ELEMENT invalidate-object EMPTY>\n"\
"<!ATTLIST invalidate-object \n"\
"uri CDATA #REQUIRED >\n"\
"<!ELEMENT invalidate-service EMPTY>\n"\
"<!ATTLIST invalidate-service\n"\
"uri CDATA #REQUIRED >";
