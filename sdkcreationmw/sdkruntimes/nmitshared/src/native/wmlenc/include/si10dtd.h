/*
 * Service Indication (SI) Document Type Definition.
 * 
 * Copyright Wireless Application Protocol Forum Ltd., 1998,1999.
 * All rights reserved.  
 * 
 * SI is an XML language.  Typical usage:
 *    <?xml version="1.0"?>
 *    <!DOCTYPE si PUBLIC "-//WAPFORUM//DTD SI 1.0//EN"
 *                 "http://www.wapforum.org/DTD/si.dtd">
 *    <si>
 *    ...
 *    </si>
 * 
 * Terms and conditions of use are available from the Wireless Application
 * Protocol Forum Ltd. web site at http://www.wapforum.org/docs/copyright.htm
 */
static char SI10DTD[] = \
"<!ENTITY % Datetime \"CDATA\"> \n"\
"<!ENTITY % URI \"CDATA\"> \n"\
"<!ELEMENT si (indication,info?)>\n"\
"<!ELEMENT indication (#PCDATA)>\n"\
"<!ATTLIST indication\n"\
"href %URI; #IMPLIED\n"\
"si-id CDATA #IMPLIED\n"\
"created %Datetime; #IMPLIED\n"\
"si-expires %Datetime; #IMPLIED\n"\
"action (signal-none|signal-low|\n"\
"signal-medium|signal-high|delete) \"signal-medium\" \n"\
">\n"\
"<!ELEMENT info (item+)>\n"\
"<!ELEMENT item (#PCDATA)>\n"\
"<!ATTLIST item\n"\
"class NMTOKEN #REQUIRED >\n";
