
#include "GapInterpolationTestBase.hh"

TEST_F(GapInterpolationTest, reproduce_kvalobs_18950)
{
    std::ostringstream sql;
    INSERT_NEIGHBOR(sql, 18950, 211, 18700, -1.684330,  0.887957,  1.838630);
    INSERT_NEIGHBOR(sql, 18950, 211, 18500,  0.434024,  0.884770,  1.935190);
    INSERT_NEIGHBOR(sql, 18950, 211,  4780, -0.220821,  0.849841,  1.972610);
    INSERT_NEIGHBOR(sql, 18950, 211, 11500,  0.000975,  0.864541,  1.982130);
    INSERT_NEIGHBOR(sql, 18950, 211, 27470, -2.141830,  0.935601,  2.015760);
    INSERT_NEIGHBOR(sql, 18950, 211, 12320,  0.006134,  0.843867,  2.029280);
    INSERT_NEIGHBOR(sql, 18950, 211, 30420, -1.436890,  0.905848,  2.077660);
    INSERT_NEIGHBOR(sql, 18950, 211, 17150, -1.955650,  0.907668,  2.093620);
    INSERT_NEIGHBOR(sql, 18950, 211, 17280, -2.664340,  0.916048,  2.112970);
    INSERT_NEIGHBOR(sql, 18950, 211,  4920,  0.418590,  0.811225,  2.168600);
    INSERT_NEIGHBOR(sql, 18950, 211, 17850, -0.745249,  0.837860,  2.168740);
    INSERT_NEIGHBOR(sql, 18950, 211, 18230, -1.208550,  0.835981,  2.174460);
    INSERT_NEIGHBOR(sql, 18950, 211, 27010, -0.161486,  0.819797,  2.226050);
    INSERT_NEIGHBOR(sql, 18950, 211, 20301, -0.325095,  0.815111,  2.236120);
    INSERT_NEIGHBOR(sql, 18950, 211, 26990, -1.089160,  0.855358,  2.245910);
    INSERT_NEIGHBOR(sql, 18950, 211, 31620,  2.682290,  0.964083,  2.250570);
    INSERT_NEIGHBOR(sql, 18950, 211, 23410,  2.557340,  0.871717,  2.256840);
    INSERT_NEIGHBOR(sql, 18950, 211, 29950, -3.354850,  0.961792,  2.257220);
    INSERT_NEIGHBOR(sql, 18950, 211,  4200, -0.719996,  0.821495,  2.271470);
    INSERT_NEIGHBOR(sql, 18950, 211,  5660, -0.049545,  0.800750,  2.273160);
    INSERT_NEIGHBOR(sql, 18950, 211, 12680,  0.817526,  0.812531,  2.286240);
    INSERT_NEIGHBOR(sql, 18950, 211, 34130, -3.716750,  1.013420,  2.332620);
    INSERT_NEIGHBOR(sql, 18950, 211, 28922,  0.323592,  0.831884,  2.334200);
    INSERT_NEIGHBOR(sql, 18950, 211, 27500, -4.169880,  1.025580,  2.339030);
    INSERT_NEIGHBOR(sql, 18950, 211,  5590, -0.123055,  0.829071,  2.348900);
    INSERT_NEIGHBOR(sql, 18950, 211, 27450, -1.814500,  0.901286,  2.350670);
    INSERT_NEIGHBOR(sql, 18950, 211, 12290,  0.138168,  0.790201,  2.375240);
    INSERT_NEIGHBOR(sql, 18950, 211, 28380, -0.100165,  0.814384,  2.389590);
    INSERT_NEIGHBOR(sql, 18950, 211, 24710,  0.050977,  0.786988,  2.390460);
    INSERT_NEIGHBOR(sql, 18950, 211,  4460,  0.029779,  0.811693,  2.390750);
    INSERT_NEIGHBOR(sql, 18950, 211,  6020,  0.074650,  0.813749,  2.408410);
    INSERT_NEIGHBOR(sql, 18950, 211, 23420,  1.332620,  0.795531,  2.413850);
    INSERT_NEIGHBOR(sql, 18950, 211, 26900, -0.828213,  0.804935,  2.415850);
    INSERT_NEIGHBOR(sql, 18950, 211, 17000, -3.310270,  0.976607,  2.420160);
    INSERT_NEIGHBOR(sql, 18950, 211, 12550,  0.018212,  0.844466,  2.426620);
    INSERT_NEIGHBOR(sql, 18950, 211,  3290, -1.173240,  0.869275,  2.428340);
    INSERT_NEIGHBOR(sql, 18950, 211, 27270, -2.201610,  0.835229,  2.439460);
    INSERT_NEIGHBOR(sql, 18950, 211, 32060, -1.015440,  0.854682,  2.484240);
    INSERT_NEIGHBOR(sql, 18950, 211, 25630,  2.268860,  0.899041,  2.673330);
    INSERT_NEIGHBOR(sql, 18950, 211,  2650, -0.273941,  0.833646,  2.686790);
    INSERT_NEIGHBOR(sql, 18950, 211, 32890,  1.517070,  0.857361,  2.698130);
    INSERT_NEIGHBOR(sql, 18950, 211, 25110,  1.935530,  0.846160,  2.715080);
    INSERT_NEIGHBOR(sql, 18950, 211, 24890,  1.402000,  0.751390,  2.790460);
    INSERT_NEIGHBOR(sql, 18950, 211, 29720,  2.748400,  0.874567,  2.806220);
    INSERT_NEIGHBOR(sql, 18950, 211, 27045, -0.700726,  0.851466,  2.847490);
    INSERT_NEIGHBOR(sql, 18950, 211, 30650, -0.021524,  0.765478,  2.881790);
    INSERT_NEIGHBOR(sql, 18950, 211, 18210, -1.931990,  0.459230,  2.886240);
    INSERT_NEIGHBOR(sql, 18950, 211, 18815, -2.996110,  0.158515,  3.197280);
    INSERT_NEIGHBOR(sql, 18950, 211, 17980, -2.391840,  0.261479,  3.367050);
    INSERT_STATION(sql,  2650, "AURSKOG II",                  59.912, 11.580,  128);
    INSERT_STATION(sql,  3290, "RAKKESTAD",                   59.386, 11.387,  100);
    INSERT_STATION(sql,  4200, "KJELLER",                     59.971, 11.038,  109);
    INSERT_STATION(sql,  4460, "HAKADAL JERNBANESTASJON",     60.117, 10.829,  170);
    INSERT_STATION(sql,  4780, "GARDERMOEN",                  60.206, 11.080,  202);
    INSERT_STATION(sql,  4920, "ÅRNES",                       60.127, 11.393,  160);
    INSERT_STATION(sql,  5590, "KONGSVINGER",                 60.190, 12.007,  148);
    INSERT_STATION(sql,  5660, "ROVERUD",                     60.254, 12.091,  150);
    INSERT_STATION(sql,  6020, "FLISA II",                    60.614, 12.012,  185);
    INSERT_STATION(sql, 11500, "ØSTRE TOTEN - APELSVOLL",     60.700, 10.870,  264);
    INSERT_STATION(sql, 12290, "HAMAR II",                    60.801, 11.098,  132);
    INSERT_STATION(sql, 12320, "HAMAR - STAVSBERG",           60.818, 11.070,  221);
    INSERT_STATION(sql, 12550, "KISE PA HEDMARK",             60.773, 10.806,  128);
    INSERT_STATION(sql, 12680, "LILLEHAMMER - SÆTHERENGEN",   61.093, 10.477,  240);
    INSERT_STATION(sql, 17000, "STRØMTANGEN FYR",             59.151, 10.829,   10);
    INSERT_STATION(sql, 17150, "RYGGE",                       59.379, 10.775,   40);
    INSERT_STATION(sql, 17280, "GULLHOLMEN",                  59.435, 10.578,   14);
    INSERT_STATION(sql, 17850, "ÅS",                          59.660, 10.782,   89);
    INSERT_STATION(sql, 17980, "OSLO - LJABRUVEIEN",          59.843, 10.824,   92);
    INSERT_STATION(sql, 18210, "OSLO - HOVIN",                59.923, 10.804,  100);
    INSERT_STATION(sql, 18230, "ALNA",                        59.927, 10.835,   90);
    INSERT_STATION(sql, 18500, "BJØRNHOLT",                   60.051, 10.688,  360);
    INSERT_STATION(sql, 18700, "OSLO - BLINDERN",             59.942, 10.720,   94);
    INSERT_STATION(sql, 18815, "OSLO - BYGDØY",               59.905, 10.683,   15);
    INSERT_STATION(sql, 18950, "TRYVANNSHØGDA",               59.985, 10.669,  514);
    INSERT_STATION(sql, 20301, "HØNEFOSS - HØYBY",            60.166, 10.248,  140);
    INSERT_STATION(sql, 23410, "FAGERNES LUFTHAVN",           61.013,  9.293,  822);
    INSERT_STATION(sql, 23420, "FAGERNES",                    60.987,  9.237,  365);
    INSERT_STATION(sql, 24710, "GULSVIK II",                  60.383,  9.605,  142);
    INSERT_STATION(sql, 24890, "NESBYEN - TODOKK",            60.567,  9.134,  166);
    INSERT_STATION(sql, 25110, "HEMSEDAL II",                 60.855,  8.593,  604);
    INSERT_STATION(sql, 25630, "GEILO - OLDEBRÅTEN",          60.530,  8.195,  772);
    INSERT_STATION(sql, 26900, "DRAMMEN - BERSKOG",           59.754, 10.124,    8);
    INSERT_STATION(sql, 26990, "SANDE - GALLEBERG",           59.619, 10.215,   60);
    INSERT_STATION(sql, 27010, "KONNERUD",                    59.713, 10.146,  193);
    INSERT_STATION(sql, 27045, "SANDE - VALLE",               59.587, 10.192,   10);
    INSERT_STATION(sql, 27270, "TØNSBERG - KILEN",            59.278, 10.431,    3);
    INSERT_STATION(sql, 27450, "MELSOM",                      59.230, 10.348,   26);
    INSERT_STATION(sql, 27470, "TORP",                        59.184, 10.255,   88);
    INSERT_STATION(sql, 27500, "FÆRDER FYR",                  59.027, 10.524,    6);
    INSERT_STATION(sql, 28380, "KONGSBERG BRANNSTASJON",      59.625,  9.638,  170);
    INSERT_STATION(sql, 28922, "VEGGLI II",                   60.044,  9.147,  275);
    INSERT_STATION(sql, 29720, "DAGALI LUFTHAVN",             60.416,  8.501,  798);
    INSERT_STATION(sql, 29950, "SVENNER FYR",                 58.969, 10.148,   15);
    INSERT_STATION(sql, 30420, "SKIEN - GEITERYGGEN",         59.183,  9.567,  136);
    INSERT_STATION(sql, 30650, "NOTODDEN FLYPLASS",           59.567,  9.210,   20);
    INSERT_STATION(sql, 31620, "MØSSTRAND II",                59.840,  8.179,  977);
    INSERT_STATION(sql, 32060, "GVARV - NES",                 59.382,  9.213,   93);
    INSERT_STATION(sql, 32890, "HØYDALSMO II",                59.497,  8.199,  560);
    INSERT_STATION(sql, 34130, "JOMFRULAND",                  58.856,  9.575,    5);
    ASSERT_NO_THROW_X(db->exec(sql.str()));
    DataList data(2650, 211, 330);
    data
        .add("2012-10-27 23:00:00",      -6.7,      -6.7, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -7.8,      -7.8, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -7.9,      -7.9, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -8.0,      -8.0, "0111100000100010", "");
    data.setStation(3290).setType(342)
        .add("2012-10-27 23:00:00",      -5.2,      -5.2, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -5.7,      -5.7, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -6.2,      -6.2, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -6.5,      -6.5, "0111100000100010", "");
    data.setStation(4200).setType(342)
        .add("2012-10-27 23:00:00",      -4.8,      -4.8, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -5.9,      -5.9, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -6.5,      -6.5, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -6.5,      -6.5, "0111100000100010", "");
    data.setStation(4460).setType(342)
        .add("2012-10-27 23:00:00",      -5.1,      -5.1, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -5.8,      -5.8, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -5.6,      -5.6, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -6.3,      -6.3, "0111100000100010", "");
    data.setStation(4780).setType(501)
        .add("2012-10-27 23:00:00",      -3.2,      -3.2, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -4.8,      -4.8, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -4.5,      -4.5, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -5.2,      -5.2, "0111100000100010", "");
    data.setStation(4920).setType(342)
        .add("2012-10-27 23:00:00",      -4.8,      -4.8, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -5.1,      -5.1, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -5.7,      -5.7, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -6.2,      -6.2, "0111100000100010", "");
    data.setStation(5590).setType(330)
        .add("2012-10-27 23:00:00",      -5.1,      -5.1, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -5.6,      -5.6, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -5.8,      -5.8, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -6.4,      -6.4, "0111100000100010", "");
    data.setStation(5660).setType(342)
        .add("2012-10-27 23:00:00",      -6.5,      -6.5, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -6.5,      -6.5, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -7.3,      -7.3, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -7.8,      -7.8, "0111100000100010", "");
    data.setStation(6020).setType(330)
        .add("2012-10-27 23:00:00",      -6.6,      -6.6, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -6.5,      -6.5, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -6.9,      -6.9, "0111100000100010", "")
        .add("2012-10-28 02:00:00",  -32767.0,      -5.1, "0000601000000000", "QC1-4-211");
    data.setStation(11500).setType(342)
        .add("2012-10-27 23:00:00",      -2.5,      -2.5, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -3.9,      -3.9, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -3.5,      -3.5, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -4.8,      -4.8, "0111100000100010", "");
    data.setStation(12290).setType(342)
        .add("2012-10-27 23:00:00",      -3.3,      -3.3, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -4.6,      -4.6, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -5.0,      -5.0, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -6.1,      -6.1, "0111100000100010", "");
    data.setStation(12320).setType(330)
        .add("2012-10-27 23:00:00",      -2.2,      -2.2, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -3.6,      -3.6, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -4.1,      -4.1, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -4.0,      -4.0, "0111100000100010", "");
    data.setStation(12550).setType(342)
        .add("2012-10-27 23:00:00",      -4.4,      -4.4, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -4.0,      -4.0, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -5.2,      -5.2, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -5.7,      -5.7, "0111100000100010", "");
    data.setStation(12680).setType(330)
        .add("2012-10-27 23:00:00",      -3.2,      -3.2, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -3.7,      -3.7, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -4.1,      -4.1, "0111100000100010", "")
        .add("2012-10-28 02:00:00",  -32767.0,      -3.9, "0000601000000000", "QC1-4-211");
    data.setStation(17000).setType(330)
        .add("2012-10-27 23:00:00",       0.1,       0.1, "0111100000100010", "")
        .add("2012-10-28 00:00:00",       0.2,       0.2, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -0.7,      -0.7, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -0.7,      -0.7, "0111100000100010", "");
    data.setStation(17150).setType(342)
        .add("2012-10-27 23:00:00",      -3.1,      -3.1, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -2.8,      -2.8, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -3.8,      -3.8, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -3.6,      -3.6, "0111100000100010", "");
    data.setStation(17280).setType(330)
        .add("2012-10-27 23:00:00",       1.6,       1.6, "0111100000100010", "")
        .add("2012-10-28 00:00:00",       2.7,       2.7, "0111100000100010", "")
        .add("2012-10-28 01:00:00",       2.0,       2.0, "0111100000100010", "")
        .add("2012-10-28 02:00:00",       1.9,       1.9, "0111100000100010", "");
    data.setStation(17850).setType(330)
        .add("2012-10-27 23:00:00",      -5.4,      -5.4, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -5.9,      -5.9, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -6.3,      -6.3, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -6.5,      -6.5, "0111100000100010", "");
    data.setStation(18210).setType(502)
        .add("2012-10-27 23:00:00",  -32767.0,      -3.8, "0000601000000000", "QC1-4-211")
        .add("2012-10-28 00:00:00",      -3.2,      -3.2, "0100100000100010", "")
        .add("2012-10-28 01:00:00",      -4.4,      -4.4, "0101100000100010", "")
        .add("2012-10-28 02:00:00",      -5.0,      -5.0, "0101100000100010", "");
    data.setStation(18230).setType(330)
        .add("2012-10-27 23:00:00",      -4.1,      -4.1, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -4.8,      -4.8, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -4.7,      -4.7, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -5.0,      -5.0, "0111100000100010", "");
    data.setStation(18500).setType(342)
        .add("2012-10-27 23:00:00",      -5.7,      -5.7, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -6.2,      -6.2, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -6.3,      -6.3, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -6.3,      -6.3, "0111100000100010", "");
    data.setStation(18700).setType(330)
        .add("2012-10-27 23:00:00",      -1.8,      -1.8, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -1.2,      -1.2, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -2.9,      -2.9, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -3.6,      -3.6, "0111100000100010", "");
    data.setStation(18950).setType(330)
        .add("2012-10-27 23:00:00",      -2.9,      -2.9, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -3.3,      -3.3, "0111100000100010", "")
        .add("2012-10-28 01:00:00",  -32767.0,      -4.8, "0000601000000000", "QC1-4-211")
        .add("2012-10-28 02:00:00",      -3.3,      -3.3, "0110100000100010", "");
    data.setStation(20301).setType(330)
        .add("2012-10-27 23:00:00",      -4.3,      -4.3, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -4.9,      -4.9, "0111100000100010", "")
        .add("2012-10-28 01:00:00",  -32767.0,      -5.7, "0000601000000000", "QC1-4-211")
        .add("2012-10-28 02:00:00",      -5.7,      -5.7, "0110100000100010", "");
    data.setStation(23410).setType(311)
        .add("2012-10-27 23:00:00",     -12.1,     -12.1, "0111100000100010", "")
        .add("2012-10-28 00:00:00",     -13.1,     -13.1, "0111100000100010", "")
        .add("2012-10-28 01:00:00",     -13.2,     -13.2, "0111100000100010", "")
        .add("2012-10-28 02:00:00",     -13.7,     -13.7, "0111100000100010", "");
    data.setStation(23420).setType(330)
        .add("2012-10-27 23:00:00",      -5.3,      -5.3, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -6.0,      -6.0, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -6.5,      -6.5, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -7.0,      -7.0, "0111100000100010", "");
    data.setStation(24710).setType(342)
        .add("2012-10-27 23:00:00",      -2.5,      -2.5, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -2.9,      -2.9, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -3.6,      -3.6, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -3.8,      -3.8, "0111100000100010", "");
    data.setStation(24890).setType(330)
        .add("2012-10-27 23:00:00",      -5.5,      -5.5, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -6.2,      -6.2, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -7.6,      -7.6, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -8.2,      -8.2, "0111100000100010", "");
    data.setStation(25110).setType(311)
        .add("2012-10-27 23:00:00",      -5.6,      -5.6, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -7.1,      -7.1, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -6.6,      -6.6, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -9.6,      -9.6, "0111100000100010", "");
    data.setStation(25630).setType(330)
        .add("2012-10-27 23:00:00",     -10.1,     -10.1, "0111100000100010", "")
        .add("2012-10-28 00:00:00",     -10.8,     -10.8, "0111100000100010", "")
        .add("2012-10-28 01:00:00",     -10.4,     -10.4, "0111100000100010", "")
        .add("2012-10-28 02:00:00",  -32767.0,      -7.1, "0000601000000000", "QC1-4-211");
    data.setStation(26900).setType(330)
        .add("2012-10-27 23:00:00",      -2.7,      -2.7, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -2.8,      -2.8, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -3.4,      -3.4, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -3.8,      -3.8, "0111100000100010", "");
    data.setStation(26990).setType(342)
        .add("2012-10-27 23:00:00",      -2.2,      -2.2, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -4.1,      -4.1, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -3.8,      -3.8, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -3.3,      -3.3, "0111100000100010", "");
    data.setStation(27010).setType(330)
        .add("2012-10-27 23:00:00",      -6.1,      -6.1, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -6.5,      -6.5, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -6.7,      -6.7, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -6.7,      -6.7, "0111100000100010", "");
    data.setStation(27270).setType(4)
        .add("2012-10-27 23:00:00",      -2.8,      -2.3, "0010604000000600", "QC1-0-211,QC1-4-211")
        .add("2012-10-28 00:00:00",      -3.8,      -2.4, "0010604000000600", "QC1-0-211,QC1-4-211")
        .add("2012-10-28 01:00:00",      -4.0,      -2.5, "0010604000000600", "QC1-0-211,QC1-4-211")
        .add("2012-10-28 02:00:00",      -4.6,      -2.4, "0010604000000600", "QC1-0-211,QC1-4-211");
    data.setStation(27450).setType(330)
        .add("2012-10-27 23:00:00",      -2.9,      -2.9, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -2.0,      -2.0, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -2.3,      -2.3, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -3.0,      -3.0, "0111100000100010", "");
    data.setStation(27470).setType(1)
        .add("2012-10-27 23:00:00",  -32767.0,      -2.6, "0000601000000000", "QC1-4-211")
        .add("2012-10-28 00:00:00",  -32767.0,      -2.9, "0000601000000000", "QC1-4-211")
        .add("2012-10-28 01:00:00",  -32767.0,      -3.0, "0000601000000000", "QC1-4-211")
        .add("2012-10-28 02:00:00",  -32767.0,      -3.1, "0000601000000000", "QC1-4-211");
    data.setStation(27500).setType(330)
        .add("2012-10-27 23:00:00",       5.0,       5.0, "0111100000100010", "")
        .add("2012-10-28 00:00:00",       5.0,       5.0, "0111100000100010", "")
        .add("2012-10-28 01:00:00",       4.9,       4.9, "0111100000100010", "")
        .add("2012-10-28 02:00:00",       4.3,       4.3, "0111100000100010", "");
    data.setStation(28380).setType(330)
        .add("2012-10-27 23:00:00",      -4.4,      -4.4, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -5.1,      -5.1, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -5.4,      -5.4, "0111100000100010", "")
        .add("2012-10-28 02:00:00",  -32767.0,      -4.9, "0000601000000000", "QC1-4-211");
    data.setStation(28922).setType(330)
        .add("2012-10-27 23:00:00",      -5.4,      -5.4, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -5.5,      -5.5, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -5.8,      -5.8, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -6.6,      -6.6, "0111100000100010", "");
    data.setStation(29720).setType(311)
        .add("2012-10-27 23:00:00",     -13.3,     -13.3, "0111100000100010", "")
        .add("2012-10-28 00:00:00",     -13.9,     -13.9, "0111100000100010", "")
        .add("2012-10-28 01:00:00",     -14.4,     -14.4, "0111100000100010", "")
        .add("2012-10-28 02:00:00",     -14.6,     -14.6, "0111100000100010", "");
    data.setStation(29950).setType(330)
        .add("2012-10-27 23:00:00",       2.0,       2.0, "0111100000100010", "")
        .add("2012-10-28 00:00:00",       2.1,       2.1, "0111100000100010", "")
        .add("2012-10-28 01:00:00",       1.9,       1.9, "0111100000100010", "")
        .add("2012-10-28 02:00:00",       2.0,       2.0, "0111100000100010", "");
    data.setStation(30420).setType(311)
        .add("2012-10-27 23:00:00",      -0.3,      -0.3, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -1.0,      -1.0, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -1.3,      -1.3, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -1.7,      -1.7, "0111100000100010", "");
    data.setStation(30650).setType(311)
        .add("2012-10-27 23:00:00",      -5.3,      -5.3, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -5.7,      -5.7, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -5.9,      -5.9, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -7.0,      -7.0, "0111100000100010", "");
    data.setStation(31620).setType(330)
        .add("2012-10-27 23:00:00",      -9.3,      -9.3, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -9.0,      -9.0, "0111100000100010", "")
        .add("2012-10-28 01:00:00",  -32767.0,      -7.0, "0000601000000000", "QC1-4-211")
        .add("2012-10-28 02:00:00",     -10.2,     -10.2, "0110100000100010", "");
    data.setStation(32060).setType(330)
        .add("2012-10-27 23:00:00",      -2.6,      -2.6, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -3.3,      -3.3, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -3.7,      -3.7, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -4.1,      -4.1, "0111100000100010", "");
    data.setStation(32890).setType(330)
        .add("2012-10-27 23:00:00",      -7.7,      -7.7, "0111100000100010", "")
        .add("2012-10-28 00:00:00",      -8.1,      -8.1, "0111100000100010", "")
        .add("2012-10-28 01:00:00",      -8.5,      -8.5, "0111100000100010", "")
        .add("2012-10-28 02:00:00",      -9.0,      -9.0, "0111100000100010", "");
    data.setStation(34130).setType(330)
        .add("2012-10-27 23:00:00",       1.5,       1.5, "0111100000100010", "")
        .add("2012-10-28 00:00:00",       1.0,       1.0, "0111100000100010", "")
        .add("2012-10-28 01:00:00",  -32767.0,       1.3, "0000601000000000", "QC1-4-211")
        .add("2012-10-28 02:00:00",       0.7,       0.7, "0110100000100010", "");
    ASSERT_NO_THROW(data.insert(db));


    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   10\n"
           << "Start_DD   =   27\n"
           << "Start_hh   =   06\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   10\n"
           << "End_DD     =   28\n"
           << "End_hh     =   02\n"
           << "TypeId     =  330\n"
           << "Parameter = par=211,minPar=213,maxPar=215,minVal=-80,maxVal=100,offsetCorrectionLimit=15,fluctuationLevel=0\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);

    // too close to time span limits
    // ASSERT_RUN(algo, bc, 1);

    ASSERT_RUN(algo, bc, 0);
}
