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


/* WBXML token-set / validation rules for WML 1.1 */
static char WML11TOK[] = 
"**********************************\n"\
"***** SECTION 1:  Tag tokens *****\n"\
"**********************************\n"\
"  Name       Code Element handler\n"\
"---------------------------------\n"\
"T a           1c  anchor\n"\
"T access      23  access\n"\
"T anchor      22  anchor\n"\
"T b           24  default\n"\
"T big         25  default\n"\
"T br          26  default\n"\
"T card        27  card\n"\
"T do          28  do\n"\
"T em          29  default\n"\
"T fieldset    2a  default\n"\
"T go          2b  task\n"\
"T head        2c  default\n"\
"T i           2d  default\n"\
"T img         2e  default\n"\
"T input       2f  default\n"\
"T meta        30  meta\n"\
"T noop        31  task\n"\
"T onevent     33  default\n"\
"T optgroup    34  default\n"\
"T option      35  option\n"\
"T p           20  default\n"\
"T postfield   21  default\n"\
"T prev        32  task\n"\
"T refresh     36  task\n"\
"T select      37  default\n"\
"T setvar      3e  default\n"\
"T small       38  default\n"\
"T strong      39  default\n"\
"T table       1f  default\n"\
"T td          1d  default\n"\
"T template    3b  card\n"\
"T timer       3c  default\n"\
"T tr          1e  default\n"\
"T u           3d  default\n"\
"T wml         3f  default\n"\
"*********************************************\n"\
"***** SECTION 2: Attribute start tokens *****\n"\
"*********************************************\n"\
"  Name              Prefix          Code    Handler\n"\
"---------------------------------------------------\n"\
"A accept-charset    -               05      CDATA\n"\
"A align             -               52      CDATA\n"\
"A align             bottom          06      CDATA\n"\
"A align             center          07      CDATA\n"\
"A align             left            08      CDATA\n"\
"A align             middle          09      CDATA\n"\
"A align             right           0a      CDATA\n"\
"A align             top             0b      CDATA\n"\
"A alt               -               0c      vdata\n"\
"A class             -               54      CDATA\n"\
"A columns           -               53      notzero\n"\
"A content           -               0d      CDATA\n"\
"A domain            -               0f      CDATA\n"\
"A emptyok           false           10      boolean\n"\
"A emptyok           true            11      boolean\n"\
"A format            -               12      CDATA\n"\
"A forua             false           56      boolean\n"\
"A forua             true            57      boolean\n"\
"A height            -               13      length\n"\
"A href              -               4a      HREF\n"\
"A href              http://         4b      HREF\n"\
"A href              https://        4c      HREF\n"\
"A hspace            -               14      length\n"\
"A http-equiv        -               5a      CDATA\n"\
"A http-equiv        Content-Type    5b      CDATA\n"\
"A http-equiv        Expires         5d      CDATA\n"\
"A id                -               55      ID\n"\
"A ivalue            -               15      vdata\n"\
"A iname             -               16      NMTOKEN\n"\
"A label             -               18      vdata\n"\
"A localsrc          -               19      HREF\n"\
"A maxlength         -               1a      number\n"\
"A method            get             1b      enum\n"\
"A method            post            1c      enum\n"\
"A mode              nowrap          1d      enum\n"\
"A mode              wrap            1e      enum\n"\
"A multiple          false           1f      enum\n"\
"A multiple          true            20      enum\n"\
"A name              -               21      vdata\n"\
"A newcontext        false           22      boolean\n"\
"A newcontext        true            23      boolean\n"\
"A onenterbackward   -               25      HREF_event\n"\
"A onenterforward    -               26      HREF_event\n"\
"A onpick            -               24      HREF_event\n"\
"A ontimer           -               27      HREF_event\n"\
"A optional          false           28      boolean\n"\
"A optional          true            29      boolean\n"\
"A path              -               2a      CDATA\n"\
"A scheme            -               2e      CDATA\n"\
"A sendreferer       false           2f      boolean\n"\
"A sendreferer       true            30      boolean\n"\
"A size              -               31      number\n"\
"A src               -               32      HREF\n"\
"A src               http://         58      HREF\n"\
"A src               https://        59      HREF\n"\
"A ordered           true            33      boolean\n"\
"A ordered           false           34      boolean\n"\
"A tabindex          -               35      number\n"\
"A title             -               36      vdata\n"\
"A type              -               37      CDATA\n"\
"A type              accept          38      CDATA\n"\
"A type              delete          39      CDATA\n"\
"A type              help            3a      CDATA\n"\
"A type              password        3b      CDATA\n"\
"A type              onpick          3c      CDATA\n"\
"A type              onenterbackward 3d      CDATA\n"\
"A type              onenterforward  3e      CDATA\n"\
"A type              ontimer         3f      CDATA\n"\
"A type              options         45      CDATA\n"\
"A type              prev            46      CDATA\n"\
"A type              reset           47      CDATA\n"\
"A type              text            48      CDATA\n"\
"A type              vnd.            49      CDATA\n"\
"A value             -               4d      vdata\n"\
"A vspace            -               4e      length\n"\
"A width             -               4f      length\n"\
"A xml:lang          -               50      NMTOKEN\n"\
"*********************************************\n"\
"***** SECTION 3: Attribute value tokens *****\n"\
"*********************************************\n"\
"  Name             Code \n"\
"-------------------------\n"\
"V .com/             85\n"\
"V .edu/             86\n"\
"V .net/             87\n"\
"V .org/             88\n"\
"V accept            89\n"\
"V bottom            8a\n"\
"V clear             8b\n"\
"V delete            8c\n"\
"V help              8d\n"\
"V http://           8e\n"\
"V http://www.       8f\n"\
"V https://          90\n"\
"V https://www.      91\n"\
"V middle            93\n"\
"V nowrap            94\n"\
"V onenterbackward   96\n"\
"V onenterforward    97\n"\
"V onpick            95\n"\
"V ontimer           98\n"\
"V options           99\n"\
"V password          9a\n"\
"V reset             9b\n"\
"V text              9d\n"\
"V top               9e\n"\
"V unknown           9f\n"\
"V wrap              a0\n"\
"V www.              a1\n"\
"**********************************************************\n"\
"***** SECTION 4: Context dependent attribute handlers ****\n"\
"**********************************************************\n"\
"  Element   Attribute        Attribute handler\n"\
"------------------------------------------\n"\
"C wml       xml:lang         NMTOKEN\n"\
"C wml       id               ID\n"\
"C wml       class            CDATA\n"\
"C card      title            vdata\n"\
"C card      newcontext       boolean\n"\
"C card      ordered          boolean\n"\
"C card      xml:lang         NMTOKEN\n"\
"C card      onenterforward   HREF_event\n"\
"C card      onenterbackward  HREF_event\n"\
"C card      ontimer          HREF_event\n"\
"C card      id               ID\n"\
"C card      class            CDATA\n"\
"C do        type             do_type\n"\
"C do        label            vdata\n"\
"C do        name             do_name\n"\
"C do        optional         boolean\n"\
"C do        xml:lang         NMTOKEN\n"\
"C do        id               ID\n"\
"C do        class            CDATA\n"\
"C onevent   type             oneventtype\n"\
"C onevent   id               ID\n"\
"C onevent   class            CDATA\n"\
"C head      id               ID\n"\
"C head      class            CDATA\n"\
"C template  onenterforward   HREF_event\n"\
"C template  onenterbackward  HREF_event\n"\
"C template  ontimer          HREF_event\n"\
"C template  id               ID\n"\
"C template  class            CDATA\n"\
"C access    domain           CDATA\n"\
"C access    path             CDATA\n"\
"C access    id               ID\n"\
"C access    class            CDATA\n"\
"C meta      http-equiv       CDATA\n"\
"C meta      name             CDATA\n"\
"C meta      forua            boolean\n"\
"C meta      content          CDATA\n"\
"C meta      scheme           CDATA\n"\
"C meta      id               ID\n"\
"C meta      class            CDATA\n"\
"C go        href             HREF\n"\
"C go        sendreferer      boolean\n"\
"C go        method           enum\n"\
"C go        accept-charset   CDATA\n"\
"C go        id               ID\n"\
"C go        class            CDATA\n"\
"C prev      id               ID\n"\
"C prev      class            CDATA\n"\
"C refresh   id               ID\n"\
"C refresh   class            CDATA\n"\
"C noop      id               ID\n"\
"C noop      class            CDATA\n"\
"C postfield name             vdata\n"\
"C postfield value            vdata\n"\
"C postfield id               ID\n"\
"C postfield class            CDATA\n"\
"C setvar    name             var_name\n"\
"C setvar    value            vdata\n"\
"C setvar    id               ID\n"\
"C setvar    class            CDATA\n"\
"C select    title            vdata\n"\
"C select    name             NMTOKEN\n"\
"C select    value            vdata\n"\
"C select    iname            NMTOKEN\n"\
"C select    ivalue           vdata\n"\
"C select    multiple         boolean\n"\
"C select    tabindex         number\n"\
"C select    xml:lang         NMTOKEN\n"\
"C select    id               ID\n"\
"C select    class            CDATA\n"\
"C optgroup  title            vdata\n"\
"C optgroup  xml:lang         NMTOKEN\n"\
"C optgroup  id               ID\n"\
"C optgroup  class            CDATA\n"\
"C option    value            vdata\n"\
"C option    title            vdata\n"\
"C option    onpick           HREF_event\n"\
"C option    xml:lang         NMTOKEN\n"\
"C option    id               ID\n"\
"C option    class            CDATA\n"\
"C input     name             NMTOKEN\n"\
"C input     type             enum\n"\
"C input     value            vdata\n"\
"C input     format           CDATA\n"\
"C input     emptyok          boolean\n"\
"C input     size             number\n"\
"C input     maxlength        number\n"\
"C input     tabindex         number\n"\
"C input     title            vdata\n"\
"C input     xml:lang         NMTOKEN\n"\
"C input     id               ID\n"\
"C input     class            CDATA\n"\
"C fieldset  title            vdata\n"\
"C fieldset  xml:lang         NMTOKEN\n"\
"C fieldset  id               ID\n"\
"C fieldset  class            CDATA\n"\
"C timer     name             NMTOKEN\n"\
"C timer     value            vdata\n"\
"C timer     id               ID\n"\
"C timer     class            CDATA\n"\
"C img       alt              vdata\n"\
"C img       src              HREF\n"\
"C img       localsrc         HREF\n"\
"C img       vspace           length\n"\
"C img       hspace           length\n"\
"C img       align            enum\n"\
"C img       height           length\n"\
"C img       width            length\n"\
"C img       xml:lang         NMTOKEN\n"\
"C img       id               ID\n"\
"C img       class            CDATA\n"\
"C anchor    title            vdata\n"\
"C anchor    xml:lang         NMTOKEN\n"\
"C anchor    id               ID\n"\
"C anchor    class            CDATA\n"\
"C a         href             HREF\n"\
"C a         title            vdata\n"\
"C a         xml:lang         NMTOKEN\n"\
"C a         id               ID\n"\
"C a         class            CDATA\n"\
"C table     title            vdata\n"\
"C table     align            CDATA\n"\
"C table     columns          notzero\n"\
"C table     xml:lang         NMTOKEN\n"\
"C table     id               ID\n"\
"C table     class            CDATA\n"\
"C tr        id               ID\n"\
"C tr        class            CDATA\n"\
"C td        xml:lang         NMTOKEN\n"\
"C td        id               ID\n"\
"C td        class            CDATA\n"\
"C em        xml:lang         NMTOKEN\n"\
"C em        id               ID\n"\
"C em        class            CDATA\n"\
"C strong    xml:lang         NMTOKEN\n"\
"C strong    id               ID\n"\
"C strong    class            CDATA\n"\
"C b         xml:lang         NMTOKEN\n"\
"C b         id               ID\n"\
"C b         class            CDATA\n"\
"C i         xml:lang         NMTOKEN\n"\
"C i         id               ID\n"\
"C i         class            CDATA\n"\
"C u         xml:lang         NMTOKEN\n"\
"C u         id               ID\n"\
"C u         class            CDATA\n"\
"C big       xml:lang         NMTOKEN\n"\
"C big       id               ID\n"\
"C big       class            CDATA\n"\
"C small     xml:lang         NMTOKEN\n"\
"C small     id               ID\n"\
"C small     class            CDATA\n"\
"C p         align            enum\n"\
"C p         mode             enum\n"\
"C p         xml:lang         NMTOKEN\n"\
"C p         id               ID\n"\
"C p         class            CDATA\n"\
"C br        xml:lang         NMTOKEN\n"\
"C br        id               ID\n"\
"C br        class            CDATA\n";
