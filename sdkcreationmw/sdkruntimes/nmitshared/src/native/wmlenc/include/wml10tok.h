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


/* WBXML token-set / validation rules for WML 1.0 */
static char WML10TOK[] = 
"**********************************\n"\
"***** SECTION 1:  Tag tokens *****\n"\
"**********************************\n"\
"\n"\
"  Name     Code Element handler\n"\
"-------------------------------\n"\
"T A         22  anchor\n"\
"T ACCESS    23  access\n"\
"T B         24  default\n"\
"T BIG       25  default\n"\
"T BR        26  default\n"\
"T CARD      27  card\n"\
"T DO        28  do\n"\
"T EM        29  default\n"\
"T FIELDSET  2a  default\n"\
"T GO        2b  task\n"\
"T HEAD      2c  default\n"\
"T I         2d  default\n"\
"T IMG       2e  default\n"\
"T INPUT     2f  default\n"\
"T META      30  meta\n"\
"T NOOP      31  task\n"\
"T PREV      32  task\n"\
"T ONEVENT   33  default\n"\
"T OPTGROUP  34  default\n"\
"T OPTION    35  option\n"\
"T REFRESH   36  task\n"\
"T SELECT    37  default\n"\
"T SMALL     38  default\n"\
"T STRONG    39  default\n"\
"T TAB       3a  default\n"\
"T TEMPLATE  3b  card\n"\
"T TIMER     3c  default\n"\
"T U         3d  default\n"\
"T VAR       3e  default\n"\
"T WML       3F  default\n"\
"\n"\
"*********************************************\n"\
"***** SECTION 2: Attribute start tokens *****\n"\
"*********************************************\n"\
"\n"\
"  Name            Prefix          Code Handler\n"\
"----------------------------------------------\n"\
"A ACCEPT-CHARSET  -               05   CDATA\n"\
"\n"\
"A ALIGN           BOTTOM          06   enum\n"\
"A ALIGN           CENTER          07   enum\n"\
"A ALIGN           LEFT            08   enum\n"\
"A ALIGN           MIDDLE          09   enum\n"\
"A ALIGN           RIGHT           0a   enum\n"\
"A ALIGN           TOP             0b   enum\n"\
"A ALT             -               0c   vdata\n"\
"A CONTENT         -               0d   CDATA\n"\
"A DEFAULT         -               0e   vdata\n"\
"A DOMAIN          -               0f   CDATA\n"\
"A EMPTYOK         FALSE           10   enum\n"\
"A EMPTYOK         TRUE            11   enum\n"\
"A FORMAT          -               12   CDATA\n"\
"A HEIGHT          -               13   length\n"\
"A HSPACE          -               14   length\n"\
"A HTTP-EQUIV      -               00   ignore\n"\
"A IDEFAULT        -               15   vdata\n"\
"A IKEY            -               16   NMTOKEN\n"\
"A KEY             -               17   NMTOKEN\n"\
"A LABEL           -               18   vdata\n"\
"A LOCALSRC        -               19   vdata\n"\
"A MAXLENGTH       -               1a   number\n"\
"A METHOD          GET             1b   enum\n"\
"A METHOD          POST            1c   enum\n"\
"A MODE            NOWRAP          1d   enum\n"\
"A MODE            WRAP            1e   enum\n"\
"A MULTIPLE        FALSE           1f   enum\n"\
"A MULTIPLE        TRUE            20   enum\n"\
"A NAME            -               21   vdata\n"\
"A NEWCONTEXT      FALSE           22   enum\n"\
"A NEWCONTEXT      TRUE            23   enum\n"\
"A ONCLICK         -               24   HREF_event\n"\
"A ONENTERBACKWARD -               25   HREF_event\n"\
"A ONENTERFORWARD  -               26   HREF_event\n"\
"A ONTIMER         -               27   HREF_event\n"\
"A OPTIONAL        FALSE           28   enum\n"\
"A OPTIONAL        TRUE            29   enum\n"\
"A PATH            -               2a   CDATA\n"\
"A POSTDATA        -               2b   vdata\n"\
"A PUBLIC          FALSE           2c   enum\n"\
"A PUBLIC          TRUE            2d   enum\n"\
"A SCHEME          -               2e   CDATA\n"\
"A SENDREFERER     FALSE           2f   enum\n"\
"A SENDREFERER     TRUE            30   enum\n"\
"A SIZE            -               31   number\n"\
"A SRC             -               32   URL\n"\
"A STYLE           LIST            33   enum\n"\
"A STYLE           SET             34   enum\n"\
"A TABINDEX        -               35   number\n"\
"A TITLE           -               36   vdata\n"\
"A TYPE            -               37   CDATA\n"\
"A TYPE            ACCEPT          38   CDATA\n"\
"A TYPE            DELETE          39   CDATA\n"\
"A TYPE            HELP            3a   CDATA\n"\
"A TYPE            PASSWORD        3b   CDATA\n"\
"A TYPE            ONCLICK         3c   CDATA\n"\
"A TYPE            ONENTERBACKWARD 3d   CDATA\n"\
"A TYPE            ONENTERFORWARD  3e   CDATA\n"\
"A TYPE            ONTIMER         3f   CDATA\n"\
"A TYPE            OPTIONS         45   CDATA\n"\
"A TYPE            PREV            46   CDATA\n"\
"A TYPE            RESET           47   CDATA\n"\
"A TYPE            TEXT            48   CDATA\n"\
"A TYPE            vnd.            49   CDATA\n"\
"A URL             -               4a   URL\n"\
"A URL             http://         4b   URL\n"\
"A URL             https://        4c   URL\n"\
"A USER-AGENT      -               4d   CDATA\n"\
"A VALUE           -               4e   vdata\n"\
"A VSPACE          -               4f   length\n"\
"A WIDTH           -               50   length\n"\
"A xml:lang        -               51   NMTOKEN\n"\
"\n"\
"*********************************************\n"\
"***** SECTION 3: Attribute value tokens *****\n"\
"*********************************************\n"\
"\n"\
"  Name            Code \n"\
"------------------------\n"\
"V .com/            85\n"\
"V .edu/            86\n"\
"V .net/            87\n"\
"V .org/            88\n"\
"V ACCEPT           89\n"\
"V BOTTOM           8a\n"\
"V CLEAR            8b\n"\
"V DELETE           8c\n"\
"V HELP             8d\n"\
"V http://          8e\n"\
"V http://www.      8f\n"\
"V https://         90\n"\
"V https://www.     91\n"\
"V LIST             92\n"\
"V MIDDLE           93\n"\
"V NOWRAP           94\n"\
"V ONCLICK          95\n"\
"V ONENTERBACKWARD  96\n"\
"V ONENTERFORWARD   97\n"\
"V ONTIMER          98\n"\
"V OPTIONS          99\n"\
"V PASSWORD         9a\n"\
"V RESET            9b\n"\
"V SET              9c\n"\
"V TEXT             9d\n"\
"V TOP              9e\n"\
"V UNKNOWN          9f\n"\
"V WRAP             a0\n"\
"V www.             a1\n"\
"\n"\
"**********************************************************\n"\
"***** SECTION 4: Context dependent attribute handlers ****\n"\
"**********************************************************\n"\
"\n"\
"\n"\
"  Element  Attribute       Attribute handler\n"\
"----------------------------------------\n"\
"C WML      xml:lang        NMTOKEN     \n"\
"C CARD     NAME            NMTOKEN     \n"\
"C CARD     TITLE           vdata       \n"\
"C CARD     NEWCONTEXT      boolean     \n"\
"C CARD     STYLE           enum        \n"\
"C CARD     ONENTERFORWARD  HREF_event  \n"\
"C CARD     ONENTERBACKWARD HREF_event  \n"\
"C CARD     ONTIMER         HREF_event  \n"\
"C DO       TYPE            do_type     \n"\
"C DO       LABEL           vdata       \n"\
"C DO       NAME            do_name     \n"\
"C DO       OPTIONAL        boolean     \n"\
"C ONEVENT  TYPE            oneventType \n"\
"C TEMPLATE ONENTERFORWARD  HREF_event  \n"\
"C TEMPLATE ONENTERBACKWARD HREF_event  \n"\
"C TEMPLATE ONTIMER         HREF_event  \n"\
"C ACCESS   DOMAIN          CDATA       \n"\
"C ACCESS   PATH            CDATA       \n"\
"C ACCESS   PUBLIC          boolean     \n"\
"C META     HTTP-EQUIV      ignore      \n"\
"C META     NAME            CDATA       \n"\
"C META     USER-AGENT      CDATA       \n"\
"C META     CONTENT         CDATA       \n"\
"C META     SCHEME          CDATA       \n"\
"C GO       URL             URL         \n"\
"C GO       SENDREFERER     boolean     \n"\
"C GO       METHOD          enum        \n"\
"C GO       ACCEPT-CHARSET  CDATA       \n"\
"C GO       POSTDATA        vdata       \n"\
"C VAR      NAME            var_name    \n"\
"C VAR      VALUE           vdata       \n"\
"C SELECT   TITLE           vdata       \n"\
"C SELECT   KEY             NMTOKEN     \n"\
"C SELECT   DEFAULT         vdata       \n"\
"C SELECT   IKEY            NMTOKEN     \n"\
"C SELECT   IDEFAULT        vdata       \n"\
"C SELECT   MULTIPLE        boolean     \n"\
"C SELECT   TABINDEX        number      \n"\
"C OPTION   VALUE           vdata       \n"\
"C OPTION   TITLE           vdata       \n"\
"C OPTION   ONCLICK         HREF_event  \n"\
"C INPUT    KEY             NMTOKEN     \n"\
"C INPUT    TYPE            enum        \n"\
"C INPUT    VALUE           vdata       \n"\
"C INPUT    DEFAULT         vdata       \n"\
"C INPUT    FORMAT          CDATA       \n"\
"C INPUT    EMPTYOK         boolean     \n"\
"C INPUT    SIZE            number      \n"\
"C INPUT    MAXLENGTH       number      \n"\
"C INPUT    TABINDEX        number      \n"\
"C INPUT    TITLE           vdata       \n"\
"C TIMER    KEY             NMTOKEN     \n"\
"C TIMER    DEFAULT         vdata       \n"\
"C IMG      ALT             vdata       \n"\
"C IMG      SRC             URL         \n"\
"C IMG      LOCALSRC        vdata       \n"\
"C IMG      VSPACE          length      \n"\
"C IMG      HSPACE          length      \n"\
"C IMG      ALIGN           enum\n"\
"C IMG      HEIGHT          length      \n"\
"C IMG      WIDTH           length      \n"\
"C A        TITLE           vdata       \n"\
"C TAB      ALIGN           enum        \n"\
"C BR       ALIGN           enum        \n"\
"C BR       MODE            enum\n";
