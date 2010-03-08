/*
 * Wireless Markup Language (WML) Document Type Definition.
 * 
 * Copyright Wireless Application Protocol Forum Ltd., 1998,1999.
 * All rights reserved.
 * 
 * WML is an XML language. Typical usage:
 * <?xml version="1.0"?>
 * <!DOCTYPE wml PUBLIC "-//WAPFORUM//DTD WML 1.0//EN"
 * "http://www.wapforum.org/DTD/wml.xml">
 * <wml>
 * ...
 * </wml>
 * 
 * Terms and conditions of use are available from the Wireless Application 
 * Protocol Forum Ltd. web site at http://www.wapforum.org/docs/copyright.htm
 */
static char WML10DTD[] =
"<!ENTITY % length \"CDATA\">\n"\
"<!ENTITY % vdata \"CDATA\">\n"\
"<!ENTITY % URL \"%vdata;\">\n"\
"<!ENTITY % boolean \"(TRUE|FALSE)\">\n"\
"<!ENTITY % number \"NMTOKEN\">\n"\
"<!ENTITY % emph \"EM | STRONG | B | I | U | BIG | SMALL\">\n"\
"<!ENTITY % tab \"TAB\">\n"\
"<!ENTITY % layout \"BR\">\n"\
"<!ENTITY % text \"#PCDATA | %emph; | %tab;\">\n"\
"<!ENTITY % inline \"%text; | %layout;\">\n"\
"<!ENTITY % flow \"%inline; | IMG | A\">\n"\
"<!ENTITY % task \"GO | PREV | NOOP | REFRESH\">\n"\
"<!ENTITY % navelmts \"DO | ONEVENT\">\n"\
"<!ELEMENT WML ( HEAD?, TEMPLATE?, CARD+ )>\n"\
"<!ATTLIST WML\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
">\n"\
"<!ENTITY % cardev\n"\
"\"ONENTERFORWARD %URL; #IMPLIED\n"\
"ONENTERBACKWARD %URL; #IMPLIED\n"\
"ONTIMER %URL; #IMPLIED\"\n"\
">\n"\
"<!ENTITY % fields \"%flow; | INPUT | SELECT | FIELDSET\">\n"\
"<!ELEMENT CARD (%fields; | %navelmts; | TIMER)*>\n"\
"<!ATTLIST CARD\n"\
"NAME NMTOKEN #IMPLIED\n"\
"TITLE %vdata; #IMPLIED\n"\
"NEWCONTEXT %boolean; \"FALSE\"\n"\
"STYLE (LIST|SET) \"LIST\"\n"\
"%cardev;>\n"\
"<!ELEMENT DO (%task;)>\n"\
"<!ATTLIST DO\n"\
"TYPE CDATA #REQUIRED\n"\
"LABEL %vdata; #IMPLIED\n"\
"NAME NMTOKEN #IMPLIED\n"\
"OPTIONAL %boolean; \"FALSE\"\n"\
">\n"\
"<!ELEMENT ONEVENT (%task;)>\n"\
"<!ATTLIST ONEVENT\n"\
"TYPE CDATA #REQUIRED\n"\
">\n"\
"<!ELEMENT HEAD ( ACCESS | META )+>\n"\
"<!ELEMENT TEMPLATE (%navelmts;)*>\n"\
"<!ATTLIST TEMPLATE %cardev;>\n"\
"<!ELEMENT ACCESS EMPTY>\n"\
"<!ATTLIST ACCESS\n"\
"DOMAIN CDATA #IMPLIED\n"\
"PATH CDATA #IMPLIED\n"\
"PUBLIC %boolean; \"FALSE\"\n"\
">\n"\
"<!ELEMENT META EMPTY>\n"\
"<!ATTLIST META\n"\
"HTTP-EQUIV CDATA #IMPLIED\n"\
"NAME CDATA #IMPLIED\n"\
"USER-AGENT CDATA #IMPLIED\n"\
"CONTENT CDATA #REQUIRED\n"\
"SCHEME CDATA #IMPLIED\n"\
">\n"\
"<!ELEMENT GO (VAR)*>\n"\
"<!ATTLIST GO\n"\
"URL %URL; #REQUIRED\n"\
"SENDREFERER %boolean; \"FALSE\"\n"\
"METHOD (POST|GET) \"GET\"\n"\
"ACCEPT-CHARSET CDATA #IMPLIED\n"\
"POSTDATA %vdata; #IMPLIED\n"\
">\n"\
"<!ELEMENT PREV (VAR)*>\n"\
"<!ELEMENT REFRESH (VAR)+>\n"\
"<!ELEMENT NOOP EMPTY>\n"\
"<!ELEMENT VAR EMPTY>\n"\
"<!ATTLIST VAR\n"\
"NAME %vdata; #REQUIRED\n"\
"VALUE %vdata; #REQUIRED\n"\
">\n"\
"<!ELEMENT SELECT (OPTGROUP|OPTION)+>\n"\
"<!ATTLIST SELECT\n"\
"TITLE %vdata; #IMPLIED\n"\
"KEY o NMTOKEN #IMPLIED\n"\
"DEFAULT %vdata; #IMPLIED\n"\
"IKEY NMTOKEN #IMPLIED\n"\
"IDEFAULT %vdata; #IMPLIED\n"\
"MULTIPLE %boolean; \"FALSE\"\n"\
"TABINDEX %number; #IMPLIED\n"\
">\n"\
"<!ELEMENT OPTGROUP (OPTGROUP|OPTION)+ >\n"\
"<!ATTLIST OPTGROUP\n"\
"TITLE %vdata; #IMPLIED\n"\
">\n"\
"<!ELEMENT OPTION (%text; | ONEVENT)*>\n"\
"<!ATTLIST OPTION\n"\
"VALUE %vdata; #IMPLIED\n"\
"TITLE %vdata; #IMPLIED\n"\
"ONCLICK %URL; #IMPLIED\n"\
">\n"\
"<!ELEMENT INPUT EMPTY>\n"\
"<!ATTLIST INPUT\n"\
"KEY NMTOKEN #REQUIRED\n"\
"TYPE (TEXT|PASSWORD) \"TEXT\"\n"\
"VALUE %vdata; #IMPLIED\n"\
"DEFAULT %vdata; #IMPLIED\n"\
"FORMAT CDATA #IMPLIED\n"\
"EMPTYOK %boolean; \"FALSE\"\n"\
"SIZE %number; #IMPLIED\n"\
"MAXLENGTH %number; #IMPLIED\n"\
"TABINDEX %number; #IMPLIED\n"\
"TITLE %vdata; #IMPLIED\n"\
">\n"\
"<!ELEMENT FIELDSET (%fields;)* >\n"\
"<!ATTLIST FIELDSET\n"\
"TITLE %vdata; #IMPLIED\n"\
">\n"\
"<!ELEMENT TIMER EMPTY>\n"\
"<!ATTLIST TIMER\n"\
"KEY NMTOKEN #IMPLIED\n"\
"DEFAULT %vdata; #REQUIRED\n"\
">\n"\
"<!ENTITY % IAlign \"(TOP|MIDDLE|BOTTOM)\" >\n"\
"<!ELEMENT IMG EMPTY>\n"\
"<!ATTLIST IMG\n"\
"ALT %vdata;#IMPLIED\n"\
"SRC %URL; #IMPLIED\n"\
"LOCALSRC %vdata; #IMPLIED\n"\
"VSPACE %length; \"0\"\n"\
"HSPACE %length; \"0\"\n"\
"ALIGN %IAlign; \"BOTTOM\"\n"\
"HEIGHT %length; #IMPLIED\n"\
"WIDTH %length; #IMPLIED\n"\
">\n"\
"<!ELEMENT A ( %inline; | GO | PREV | REFRESH )*>\n"\
"<!ATTLIST A\n"\
"TITLE %vdata; #IMPLIED\n"\
">\n"\
"<!ENTITY % TAlign \"(LEFT|RIGHT|CENTER)\" >\n"\
"<!ELEMENT TAB EMPTY>\n"\
"<!ATTLIST TAB\n"\
"ALIGN %TAlign; \"LEFT\"\n"\
">\n"\
"<!ELEMENT EM (%flow;)*>\n"\
"<!ELEMENT STRONG (%flow;)*>\n"\
"<!ELEMENT B (%flow;)*>\n"\
"<!ELEMENT I (%flow;)*>\n"\
"<!ELEMENT U (%flow;)*>\n"\
"<!ELEMENT BIG (%flow;)*>\n"\
"<!ELEMENT SMALL (%flow;)*>\n"\
"<!ENTITY % BRMode \"(WRAP|NOWRAP)\" >\n"\
"<!ELEMENT BR EMPTY>\n"\
"<!ATTLIST BR\n"\
"ALIGN %TAlign; \"LEFT\"\n"\
"MODE %BRMode; #IMPLIED\n"\
">\n"\
"<!ENTITY quot \"&#34;\">\n"\
"<!ENTITY amp \"&#38;#38;\">\n"\
"<!ENTITY apos \"&#39;\">\n"\
"<!ENTITY lt \"&#38;#60;\">\n"\
"<!ENTITY gt \"&#62;\">\n"\
"<!ENTITY nbsp\"&#160;\">\n"\
"<!ENTITY shy \"&#173;\">\n";
