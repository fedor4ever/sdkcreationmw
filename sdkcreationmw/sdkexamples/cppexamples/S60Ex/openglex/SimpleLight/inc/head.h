/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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


// MACROS
#define objVerticesHead     201
#define objFacesHead        352
#define scalar(a) ((short)(a*300))

// CONSTANTS
/** Vertices for the head object. */
static const GLshort objVertexdataHead[]=
    {
    scalar(-5.0407),scalar(3.96696),scalar(-11.0171),
    scalar(-4.07404),scalar(0.048929),scalar(-11.2059),
    scalar(-4.56468),scalar(1.89941),scalar(-11.2664),
    scalar(0.179193),scalar(7.14659),scalar(-8.12668),
    scalar(-2.08421),scalar(6.68348),scalar(-8.15493),
    scalar(-2.32831),scalar(6.99921),scalar(-10.8724),
    scalar(-5.00611),scalar(5.63595),scalar(-10.6607),
    scalar(0.179193),scalar(7.55878),scalar(-10.9117),
    scalar(0.179193),scalar(-2.89075),scalar(-10.1725),
    scalar(-1.44059),scalar(-2.68295),scalar(-10.1479),
    scalar(-1.32999),scalar(-2.06344),scalar(-11.5653),
    scalar(-2.69861),scalar(-0.688244),scalar(-11.2554),
    scalar(-3.49327),scalar(-1.20665),scalar(-9.068),
    scalar(0.179193),scalar(-2.33336),scalar(-11.5334),
    scalar(-4.7279),scalar(3.73016),scalar(-8.27796),
    scalar(5.39909),scalar(3.96696),scalar(-11.0171),
    scalar(4.92307),scalar(1.89941),scalar(-11.2664),
    scalar(4.43243),scalar(0.048929),scalar(-11.2059),
    scalar(2.4426),scalar(6.68348),scalar(-8.15493),
    scalar(5.01929),scalar(5.2106),scalar(-8.19343),
    scalar(5.36449),scalar(5.63595),scalar(-10.6607),
    scalar(2.6867),scalar(6.99921),scalar(-10.8724),
    scalar(1.79897),scalar(-2.68295),scalar(-10.1479),
    scalar(3.05699),scalar(-0.688244),scalar(-11.2554),
    scalar(1.68837),scalar(-2.06344),scalar(-11.5653),
    scalar(4.91503),scalar(-0.301386),scalar(-8.46043),
    scalar(5.19031),scalar(1.83388),scalar(-8.00828),
    scalar(3.85166),scalar(-1.20665),scalar(-9.068),
    scalar(5.08629),scalar(3.73016),scalar(-8.27796),
    scalar(-1.53974),scalar(-9.0678),scalar(-6.02552),
    scalar(-1.5395),scalar(-8.77108),scalar(-6.2416),
    scalar(-1.96387),scalar(-8.61516),scalar(-5.36582),
    scalar(-3.69084),scalar(-8.12623),scalar(4.64399),
    scalar(-3.47159),scalar(-8.24414),scalar(3.18012),
    scalar(-1.14655),scalar(-8.7204),scalar(2.98159),
    scalar(0.179193),scalar(-7.82001),scalar(-9.78032),
    scalar(0.179193),scalar(-9.18546),scalar(-8.49039),
    scalar(-2.39458),scalar(-8.34207),scalar(-5.50029),
    scalar(-1.97654),scalar(-8.6098),scalar(-4.0849),
    scalar(-2.4606),scalar(-7.82172),scalar(1.7416),
    scalar(-4.11522),scalar(-7.34643),scalar(1.5791),
    scalar(-6.75427),scalar(-0.143525),scalar(2.10142),
    scalar(-7.42616),scalar(3.44928),scalar(0.00244379),
    scalar(-7.69438),scalar(3.46828),scalar(1.77716),
    scalar(-5.22049),scalar(-2.31436),scalar(9.67923),
    scalar(-5.42074),scalar(-5.04697),scalar(8.0),
    scalar(-4.7586),scalar(7.44818),scalar(7.96541),
    scalar(-4.72425),scalar(8.83994),scalar(4.5636),
    scalar(0.179193),scalar(8.11325),scalar(9.00394),
    scalar(0.179193),scalar(8.83117),scalar(-3.31436),
    scalar(-1.94121),scalar(9.82511),scalar(-1.51332),
    scalar(-1.99164),scalar(7.00724),scalar(-5.32075),
    scalar(0.179193),scalar(7.59094),scalar(-5.4182),
    scalar(-4.52278),scalar(8.48671),scalar(-1.53939),
    scalar(0.179193),scalar(7.14659),scalar(-8.12668),
    scalar(-2.08421),scalar(6.68348),scalar(-8.15493),
    scalar(-4.66091),scalar(5.2106),scalar(-8.19343),
    scalar(-4.59757),scalar(5.26225),scalar(-5.32562),
    scalar(0.179193),scalar(-8.87047),scalar(4.91733),
    scalar(0.179193),scalar(-7.03095),scalar(8.90698),
    scalar(-4.42022),scalar(-6.17855),scalar(8.10526),
    scalar(-4.42826),scalar(-4.73928),scalar(9.31893),
    scalar(0.179193),scalar(-4.64915),scalar(10.7538),
    scalar(0.179193),scalar(-2.02154),scalar(11.6676),
    scalar(-2.71371),scalar(-2.25346),scalar(11.0608),
    scalar(0.179193),scalar(2.99567),scalar(11.7249),
    scalar(-5.01001),scalar(2.54279),scalar(10.2183),
    scalar(-5.71283),scalar(-0.795433),scalar(-6.0299),
    scalar(-4.75494),scalar(-2.49341),scalar(-7.63142),
    scalar(-3.69815),scalar(-4.73685),scalar(-8.62122),
    scalar(-1.55241),scalar(-3.0469),scalar(-9.51552),
    scalar(0.179193),scalar(-3.19088),scalar(-9.75108),
    scalar(0.179193),scalar(-2.89075),scalar(-10.1725),
    scalar(0.179193),scalar(-8.85147),scalar(-7.15053),
    scalar(0.179193),scalar(-9.00714),scalar(-6.72957),
    scalar(-1.60649),scalar(-8.4858),scalar(-6.79143),
    scalar(-2.77754),scalar(-8.18762),scalar(-7.30547),
    scalar(-1.96265),scalar(-8.90531),scalar(-8.26602),
    scalar(-2.30468),scalar(-7.66069),scalar(-9.08943),
    scalar(-5.05556),scalar(-7.36251),scalar(-1.83562),
    scalar(-0.683441),scalar(-9.62616),scalar(-6.20165),
    scalar(-0.991367),scalar(-9.72896),scalar(-4.47369),
    scalar(-1.82818),scalar(-9.07218),scalar(-4.88128),
    scalar(0.179193),scalar(-9.84858),scalar(-4.34679),
    scalar(-0.814748),scalar(-9.1879),scalar(-3.6135),
    scalar(-0.568699),scalar(-9.10142),scalar(-2.75475),
    scalar(-1.26957),scalar(-8.63709),scalar(-2.73552),
    scalar(0.179193),scalar(-9.19423),scalar(-2.78839),
    scalar(-0.47978),scalar(-10.3848),scalar(-2.17131),
    scalar(0.179193),scalar(-10.5972),scalar(-2.19421),
    scalar(-0.408889),scalar(-10.5273),scalar(-1.6624),
    scalar(-0.24177),scalar(-9.94334),scalar(-0.718167),
    scalar(0.179193),scalar(-8.95354),scalar(1.79178),
    scalar(-0.374296),scalar(-8.85537),scalar(1.75573),
    scalar(-5.48432),scalar(-7.09258),scalar(2.78426),
    scalar(-4.25675),scalar(-6.99343),scalar(2.39716),
    scalar(-5.88117),scalar(-5.70058),scalar(1.697),
    scalar(-5.12036),scalar(-6.02531),scalar(1.74794),
    scalar(-4.87358),scalar(-6.38269),scalar(1.22074),
    scalar(-2.39945),scalar(-7.54448),scalar(2.52651),
    scalar(-1.12146),scalar(-7.4161),scalar(2.1287),
    scalar(-1.08491),scalar(-7.50429),scalar(1.53477),
    scalar(0.179193),scalar(-10.0822),scalar(-0.690385),
    scalar(0.179193),scalar(-10.7278),scalar(-1.69334),
    scalar(-6.1584),scalar(3.88388),scalar(0.16541),
    scalar(-6.01345),scalar(2.67435),scalar(-2.03051),
    scalar(-7.09143),scalar(2.47945),scalar(-1.91552),
    scalar(-7.76429),scalar(2.56789),scalar(2.70752),
    scalar(-5.84974),scalar(1.26967),scalar(-2.8276),
    scalar(-6.77401),scalar(1.06333),scalar(-2.65513),
    scalar(-4.83192),scalar(1.83388),scalar(-8.00828),
    scalar(-4.55664),scalar(-0.301386),scalar(-8.46043),
    scalar(-4.97809),scalar(-0.338903),scalar(-6.40068),
    scalar(-5.64096),scalar(0.102768),scalar(-3.05807),
    scalar(-6.36839),scalar(-0.365457),scalar(-2.82128),
    scalar(-6.09506),scalar(2.0049),scalar(7.67965),
    scalar(-6.45877),scalar(3.86269),scalar(2.22103),
    scalar(-6.59714),scalar(2.83123),scalar(3.05662),
    scalar(-6.73966),scalar(1.47163),scalar(3.17427),
    scalar(-7.57257),scalar(1.35104),scalar(2.78913),
    scalar(-0.719739),scalar(-9.45149),scalar(-5.35972),
    scalar(0.179193),scalar(-9.54162),scalar(-5.38019),
    scalar(1.89788),scalar(-8.77108),scalar(-6.2416),
    scalar(1.89813),scalar(-9.0678),scalar(-6.02552),
    scalar(2.32226),scalar(-8.61516),scalar(-5.36582),
    scalar(4.04923),scalar(-8.12623),scalar(4.64399),
    scalar(1.50493),scalar(-8.7204),scalar(2.98159),
    scalar(3.82998),scalar(-8.24414),scalar(3.18012),
    scalar(2.75296),scalar(-8.34207),scalar(-5.50029),
    scalar(2.33492),scalar(-8.6098),scalar(-4.0849),
    scalar(4.4736),scalar(-7.34643),scalar(1.5791),
    scalar(2.81898),scalar(-7.82172),scalar(1.7416),
    scalar(7.11266),scalar(-0.143525),scalar(2.10142),
    scalar(8.05276),scalar(3.46828),scalar(1.77716),
    scalar(7.78454),scalar(3.44928),scalar(0.00244379),
    scalar(5.77912),scalar(-5.04697),scalar(8.0),
    scalar(5.57887),scalar(-2.31436),scalar(9.67923),
    scalar(5.08263),scalar(8.83994),scalar(4.5636),
    scalar(2.54711),scalar(9.82049),scalar(4.79819),
    scalar(5.11698),scalar(7.44818),scalar(7.96541),
    scalar(2.2996),scalar(9.82511),scalar(-1.51332),
    scalar(2.29862),scalar(8.26404),scalar(-3.2671),
    scalar(2.35003),scalar(7.00724),scalar(-5.32075),
    scalar(4.88116),scalar(8.48671),scalar(-1.53939),
    scalar(4.89992),scalar(6.43354),scalar(-3.36064),
    scalar(2.4426),scalar(6.68348),scalar(-8.15493),
    scalar(4.95595),scalar(5.26225),scalar(-5.32562),
    scalar(5.01929),scalar(5.2106),scalar(-8.19343),
    scalar(4.7786),scalar(-6.17855),scalar(8.10526),
    scalar(4.78664),scalar(-4.73928),scalar(9.31893),
    scalar(3.02995),scalar(-4.79921),scalar(10.1289),
    scalar(3.03141),scalar(0.230177),scalar(11.4206),
    scalar(3.00364),scalar(2.84804),scalar(11.2805),
    scalar(5.36839),scalar(2.54279),scalar(10.2183),
    scalar(5.11333),scalar(-2.49341),scalar(-7.63142),
    scalar(6.07121),scalar(-0.795433),scalar(-6.0299),
    scalar(1.91079),scalar(-3.0469),scalar(-9.51552),
    scalar(3.88625),scalar(-1.69899),scalar(-7.90938),
    scalar(1.79897),scalar(-2.68295),scalar(-10.1479),
    scalar(1.96488),scalar(-8.4858),scalar(-6.79143),
    scalar(2.32104),scalar(-8.90531),scalar(-8.26602),
    scalar(3.13592),scalar(-8.18762),scalar(-7.30547),
    scalar(2.66307),scalar(-7.66069),scalar(-9.08943),
    scalar(4.84487),scalar(-7.54156),scalar(-3.49366),
    scalar(5.41395),scalar(-7.36251),scalar(-1.83562),
    scalar(1.04183),scalar(-9.62616),scalar(-6.20165),
    scalar(1.34975),scalar(-9.72896),scalar(-4.47369),
    scalar(1.17313),scalar(-9.1879),scalar(-3.6135),
    scalar(1.62796),scalar(-8.63709),scalar(-2.73552),
    scalar(0.927084),scalar(-9.10142),scalar(-2.75475),
    scalar(0.838166),scalar(-10.3848),scalar(-2.17131),
    scalar(0.600156),scalar(-9.94334),scalar(-0.718167),
    scalar(0.767274),scalar(-10.5273),scalar(-1.6624),
    scalar(0.732681),scalar(-8.85537),scalar(1.75573),
    scalar(5.84271),scalar(-7.09258),scalar(2.78426),
    scalar(5.47875),scalar(-6.02531),scalar(1.74794),
    scalar(6.23955),scalar(-5.70058),scalar(1.697),
    scalar(4.61514),scalar(-6.99343),scalar(2.39716),
    scalar(5.23197),scalar(-6.38269),scalar(1.22074),
    scalar(2.75784),scalar(-7.54448),scalar(2.52651),
    scalar(1.47984),scalar(-7.4161),scalar(2.1287),
    scalar(1.4433),scalar(-7.50429),scalar(1.53477),
    scalar(6.51678),scalar(3.88388),scalar(0.16541),
    scalar(7.44982),scalar(2.47945),scalar(-1.91552),
    scalar(6.37183),scalar(2.67435),scalar(-2.03051),
    scalar(8.12268),scalar(2.56789),scalar(2.70752),
    scalar(7.13239),scalar(1.06333),scalar(-2.65513),
    scalar(6.20812),scalar(1.26967),scalar(-2.8276),
    scalar(5.33648),scalar(-0.338903),scalar(-6.40068),
    scalar(4.91503),scalar(-0.301386),scalar(-8.46043),
    scalar(5.19031),scalar(1.83388),scalar(-8.00828),
    scalar(3.85166),scalar(-1.20665),scalar(-9.068),
    scalar(5.82858),scalar(-0.0358482),scalar(-4.37113),
    scalar(5.99935),scalar(0.102768),scalar(-3.05807),
    scalar(6.72678),scalar(-0.365457),scalar(-2.82128),
    scalar(5.08629),scalar(3.73016),scalar(-8.27796),
    scalar(6.81716),scalar(3.86269),scalar(2.22103),
    scalar(6.95553),scalar(2.83123),scalar(3.05662),
    scalar(7.09804),scalar(1.47163),scalar(3.17427),
    scalar(7.93095),scalar(1.35104),scalar(2.78913),
    scalar(1.07812),scalar(-9.45149),scalar(-5.35972)
    };

/** Surface normal vectors for the head object. */
static const GLbyte objNormaldataHead[]=
    {
    -125,-14,13,
    -97,-77,-24,
    -125,-18,-5,
    0,0,0,
    -43,119,7,
    -41,118,17,
    -90,88,15,
    -8,125,18,
    0,0,0,
    0,0,0,
    -41,-107,-52,
    -82,-89,-36,
    0,0,0,
    0,-117,-48,
    0,0,0,
    126,-6,6,
    123,-24,-15,
    93,-78,-34,
    44,117,17,
    91,84,23,
    116,44,22,
    42,117,20,
    66,-96,-49,
    77,-89,-46,
    43,-108,-49,
    116,-46,-21,
    126,-10,-2,
    77,-91,-41,
    125,4,15,
    -87,-88,-26,
    -65,-89,-62,
    -75,-94,-39,
    -43,-116,26,
    -33,-95,-76,
    -48,-115,-21,
    -2,-34,-122,
    -5,-124,-25,
    -62,-109,-18,
    -67,-105,21,
    -3,-126,-14,
    -62,-107,-27,
    -117,-39,26,
    -80,91,-35,
    -121,36,-4,
    -96,-21,79,
    -114,-31,44,
    -101,52,55,
    -68,105,18,
    0,94,84,
    1,105,-70,
    -24,119,-35,
    -42,109,-47,
    9,119,-42,
    -100,68,-35,
    -1,126,0,
    0,0,0,
    0,0,0,
    -115,49,-16,
    0,-124,22,
    -1,-100,77,
    -67,-88,60,
    -59,-55,97,
    -8,-59,111,
    4,-23,124,
    -50,-21,114,
    -11,26,123,
    -80,16,96,
    -107,38,-55,
    -98,5,-79,
    -99,-28,-73,
    -58,-46,-102,
    3,-70,-105,
    0,-112,-58,
    -6,-125,-16,
    6,-94,-85,
    -39,-120,2,
    -76,-100,-12,
    -73,-99,-25,
    -56,-58,-97,
    -81,-97,0,
    -40,-103,-62,
    -49,-116,12,
    -78,-99,-13,
    -5,-125,20,
    -61,-101,45,
    -47,-113,-30,
    -84,-93,14,
    4,-115,-53,
    -52,-76,-86,
    -5,-96,-82,
    -73,-100,22,
    -72,-91,50,
    -6,-123,30,
    -84,-94,7,
    -83,-93,-23,
    -45,-100,-63,
    -102,-73,-15,
    -79,-98,-4,
    -55,-114,9,
    -13,-125,-14,
    -55,-89,-70,
    -60,-106,-33,
    -1,-111,60,
    14,-124,21,
    -70,100,-33,
    -102,48,-56,
    -76,63,-79,
    -77,38,92,
    -73,23,-101,
    -96,1,-82,
    0,0,0,
    0,0,0,
    -116,10,-48,
    -99,39,-68,
    -119,-5,-43,
    -122,6,32,
    -101,70,27,
    -94,40,74,
    -105,-5,70,
    -113,-41,40,
    -38,-119,-16,
    1,-125,-18,
    70,-83,-64,
    87,-88,-26,
    75,-95,-35,
    52,-112,27,
    32,-117,-36,
    33,-101,-68,
    67,-103,-26,
    63,-107,21,
    50,-99,-60,
    5,-126,7,
    117,-39,26,
    103,67,30,
    81,91,-35,
    112,-44,37,
    91,-21,84,
    99,78,7,
    14,123,26,
    87,66,64,
    31,115,-41,
    45,95,-70,
    51,110,-36,
    94,71,-44,
    94,65,-53,
    48,116,-10,
    109,60,-23,
    108,66,1,
    58,-88,70,
    68,-55,91,
    37,-55,108,
    39,-9,120,
    36,38,115,
    92,15,85,
    91,6,-87,
    103,41,-60,
    53,-25,-112,
    91,-17,-85,
    54,-96,-61,
    42,-119,-6,
    52,-109,-37,
    83,-94,-11,
    84,-41,-85,
    96,-78,-26,
    73,-102,10,
    28,-117,-39,
    49,-116,-9,
    42,-109,47,
    88,-91,5,
    46,-104,-55,
    59,-91,-65,
    82,-84,47,
    88,-87,24,
    80,-97,11,
    70,-93,-48,
    59,-105,-38,
    108,-66,-1,
    58,-107,-32,
    68,-106,6,
    19,-111,-58,
    51,-112,-28,
    65,-107,-17,
    96,78,-25,
    98,43,-67,
    91,58,-65,
    93,31,79,
    75,16,-101,
    92,18,-84,
    117,-10,-47,
    112,-53,-24,
    125,0,-16,
    91,-82,-30,
    116,40,-29,
    84,33,-88,
    121,0,-36,
    126,6,2,
    85,92,17,
    91,43,76,
    113,-5,55,
    119,-28,32,
    48,-117,-4
    };

/** Indices to the duck object vertices. */
static const GLushort objFacedataHead[]=
    {
    29,31,30,
    31,37,30,
    39,40,101,
    60,61,45,
    76,77,75,
    37,38,79,
    88,86,85,
    95,33,94,
    95,99,33,
    101,40,98,
    43,42,106,
    112,113,108,
    46,47,116,
    119,41,118,
    31,29,120,
    90,86,88,
    122,124,123,
    122,128,124,
    181,130,131,
    135,149,148,
    154,163,162,
    159,160,161,
    169,168,170,
    174,127,177,
    127,179,177,
    178,130,181,
    183,134,199,
    188,189,190,
    187,193,192,
    198,132,199,
    200,123,124,
    170,168,172,
    6,57,4,
    4,5,6,
    4,54,7,
    7,5,4,
    112,1,11,
    112,2,1,
    2,112,57,
    57,0,2,
    57,6,0,
    13,72,10,
    21,18,19,
    19,20,21,
    21,7,54,
    54,18,21,
    17,25,27,
    27,23,17,
    16,26,25,
    25,17,16,
    15,28,26,
    26,16,15,
    28,15,20,
    20,19,28,
    72,13,24,
    24,22,72,
    22,24,23,
    23,27,22,
    32,33,34,
    34,58,32,
    31,82,37,
    41,119,114,
    43,114,119,
    115,45,44,
    46,48,47,
    138,47,48,
    50,47,138,
    47,50,53,
    51,50,49,
    49,52,51,
    51,52,54,
    54,4,51,
    51,4,57,
    32,58,59,
    59,60,32,
    60,45,94,
    94,32,60,
    61,60,59,
    59,62,61,
    62,63,64,
    64,61,62,
    65,64,63,
    64,65,66,
    66,44,64,
    64,44,45,
    45,61,64,
    65,48,46,
    46,66,65,
    114,67,68,
    68,69,114,
    69,68,70,
    35,69,70,
    70,71,35,
    72,71,70,
    70,10,72,
    74,30,73,
    30,75,73,
    36,77,78,
    78,35,36,
    69,78,77,
    77,76,69,
    69,35,78,
    76,79,69,
    114,69,79,
    114,79,96,
    76,75,37,
    37,79,76,
    98,96,79,
    86,79,38,
    75,30,37,
    30,80,29,
    80,30,74,
    74,165,80,
    120,80,165,
    165,121,120,
    29,80,120,
    81,82,120,
    120,121,83,
    83,81,120,
    83,84,81,
    84,38,82,
    82,81,84,
    38,37,82,
    84,85,86,
    86,38,84,
    85,84,83,
    83,87,85,
    88,85,87,
    87,89,88,
    86,90,91,
    93,92,34,
    33,32,94,
    95,94,96,
    96,97,95,
    96,98,97,
    97,98,40,
    40,95,97,
    33,99,100,
    100,34,33,
    99,95,40,
    40,39,99,
    101,100,99,
    99,39,101,
    86,101,98,
    98,79,86,
    86,91,93,
    93,101,86,
    92,93,91,
    91,102,92,
    102,91,90,
    90,103,102,
    75,77,36,
    36,73,75,
    104,105,106,
    106,42,104,
    106,105,108,
    108,109,106,
    105,112,108,
    112,11,70,
    112,70,68,
    68,67,112,
    114,113,112,
    112,67,114,
    11,10,70,
    43,106,109,
    109,114,43,
    114,109,108,
    108,113,114,
    104,53,105,
    57,112,105,
    105,53,57,
    66,46,115,
    115,44,66,
    53,104,116,
    116,47,53,
    116,117,46,
    46,117,118,
    118,115,46,
    115,118,45,
    45,96,94,
    118,41,45,
    107,119,118,
    118,117,107,
    107,117,116,
    116,43,107,
    43,116,104,
    104,42,43,
    107,43,119,
    45,41,96,
    114,96,41,
    101,93,34,
    34,100,101,
    51,57,53,
    53,50,51,
    82,31,120,
    90,88,89,
    89,103,90,
    125,126,127,
    166,124,128,
    133,185,199,
    199,134,133,
    137,138,139,
    48,139,138,
    138,140,50,
    140,138,137,
    140,141,49,
    49,50,140,
    52,49,141,
    141,142,52,
    137,143,140,
    143,144,141,
    141,140,143,
    145,54,52,
    52,142,145,
    146,147,145,
    145,142,146,
    58,34,126,
    126,125,58,
    58,125,59,
    125,148,59,
    125,174,135,
    135,148,125,
    150,59,148,
    148,149,150,
    59,150,62,
    63,62,150,
    63,150,151,
    151,152,65,
    65,63,151,
    152,151,153,
    151,150,136,
    136,153,151,
    149,135,136,
    136,150,149,
    152,139,48,
    48,65,152,
    139,152,153,
    154,155,163,
    163,155,194,
    156,157,154,
    154,162,156,
    156,35,71,
    158,156,71,
    71,72,158,
    159,73,36,
    73,159,74,
    159,122,74,
    35,162,160,
    160,36,35,
    161,160,162,
    35,156,162,
    163,161,162,
    164,163,194,
    163,128,159,
    159,161,163,
    129,163,164,
    129,128,163,
    122,159,128,
    165,74,122,
    122,123,165,
    165,200,121,
    165,123,200,
    166,83,121,
    121,200,166,
    83,166,167,
    129,167,166,
    128,129,166,
    129,168,169,
    169,167,129,
    87,83,167,
    167,169,87,
    89,87,169,
    169,170,89,
    168,171,172,
    126,34,92,
    92,173,126,
    125,127,174,
    175,176,174,
    174,177,175,
    176,175,178,
    177,130,178,
    178,175,177,
    126,180,179,
    179,127,126,
    131,130,177,
    177,179,131,
    131,179,180,
    180,181,131,
    164,178,181,
    164,176,178,
    181,168,164,
    168,129,164,
    181,173,171,
    171,168,181,
    102,171,173,
    173,92,102,
    103,172,171,
    171,102,103,
    159,36,160,
    134,183,184,
    184,182,134,
    186,187,184,
    184,183,186,
    192,184,187,
    188,157,191,
    191,189,188,
    155,154,157,
    157,188,155,
    192,194,155,
    155,188,192,
    194,192,193,
    157,156,158,
    158,191,157,
    188,190,192,
    199,186,183,
    193,187,186,
    186,194,193,
    143,184,144,
    143,182,184,
    184,192,146,
    144,184,146,
    195,147,146,
    190,195,146,
    146,192,190,
    153,198,139,
    153,136,198,
    137,196,182,
    182,143,137,
    197,196,137,
    137,139,197,
    197,139,198,
    136,135,198,
    198,135,132,
    197,198,199,
    199,185,197,
    133,196,197,
    197,185,133,
    134,182,196,
    196,133,134,
    199,132,194,
    194,186,199,
    135,176,132,
    194,132,176,
    164,194,176,
    176,135,174,
    180,126,173,
    173,181,180,
    141,144,146,
    146,142,141,
    166,200,124,
    103,89,170,
    170,172,103
    };
