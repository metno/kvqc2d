
#include "GapInterpolationTestBase.hh"

// ------------------------------------------------------------------------

TEST_F(GapInterpolationTest, reproduce_kvalobs_19940)
{
    std::ostringstream sql;
    INSERT_STATION(sql, 19940, "LIER",                        59.790, 10.261,   39);
    ASSERT_NO_THROW_X(db->exec(sql.str()));
    DataList data(19940, 211, 502);
    data.add("2013-05-29 04:00:00",      15.4,      15.4, "0111100000100010", "")
        .add("2013-05-29 05:00:00",      15.3,      15.3, "0111100000100010", "")
        .add("2013-05-29 06:00:00",      15.8,      15.8, "0111100000100010", "")
        .add("2013-05-29 07:00:00",      17.3,      17.3, "0111100000100010", "")
        .add("2013-05-29 08:00:00",  -32767.0,      18.9, "0000601000000000", "QC1-4-211")
        .add("2013-05-29 09:00:00",  -32767.0,      20.0, "0000601000000000", "QC1-4-211")
        .add("2013-05-29 10:00:00",  -32767.0,      21.3, "0000601000000000", "QC1-4-211")
        .add("2013-05-29 11:00:00",      21.2,      21.2, "0110100000100010", "")
        .add("2013-05-29 12:00:00",      20.5,      20.5, "0111100000100010", "")
        .add("2013-05-29 13:00:00",      18.0,      18.0, "0111100000100010", "")
        .add("2013-05-29 14:00:00",      17.4,      17.4, "0111100000100010", "")
        .add("2013-05-29 15:00:00",      17.2,      17.2, "0111100000100010", "");
    data.setParam(213);
    data.add("2013-05-29 04:00:00",      14.8,      14.8, "0111000000000000", "")
        .add("2013-05-29 05:00:00",      15.1,      15.1, "0111000000000000", "")
        .add("2013-05-29 06:00:00",      15.1,      15.1, "0111000000000000", "")
        .add("2013-05-29 07:00:00",      15.8,      15.8, "0111000000000000", "")
        .add("2013-05-29 08:00:00",  -32767.0,  -32767.0, "0000003000000000", "")
        .add("2013-05-29 09:00:00",  -32767.0,  -32767.0, "0000003000000000", "")
        .add("2013-05-29 10:00:00",  -32767.0,  -32767.0, "0000003000000000", "")
        .add("2013-05-29 11:00:00",      19.5,      19.5, "0110000000000000", "")
        .add("2013-05-29 12:00:00",      20.5,      20.5, "0111000000000000", "")
        .add("2013-05-29 13:00:00",      17.4,      17.4, "0111000000000000", "")
        .add("2013-05-29 14:00:00",      16.6,      16.6, "0111000000000000", "")
        .add("2013-05-29 15:00:00",      17.2,      17.2, "0111000000000000", "");
    data.setParam(215);
    data.add("2013-05-29 04:00:00",      15.4,      15.4, "0111000000000000", "")
        .add("2013-05-29 05:00:00",      15.6,      15.6, "0111000000000000", "")
        .add("2013-05-29 06:00:00",      15.8,      15.8, "0111000000000000", "")
        .add("2013-05-29 07:00:00",      17.6,      17.6, "0111000000000000", "")
        .add("2013-05-29 08:00:00",  -32767.0,  -32767.0, "0000003000000000", "")
        .add("2013-05-29 09:00:00",  -32767.0,  -32767.0, "0000003000000000", "")
        .add("2013-05-29 10:00:00",  -32767.0,  -32767.0, "0000003000000000", "")
        .add("2013-05-29 11:00:00",      21.2,      21.2, "0110000000000000", "")
        .add("2013-05-29 12:00:00",      21.8,      21.8, "0111000000000000", "")
        .add("2013-05-29 13:00:00",      20.6,      20.6, "0111000000000000", "")
        .add("2013-05-29 14:00:00",      18.0,      18.0, "0111000000000000", "")
        .add("2013-05-29 15:00:00",      17.9,      17.9, "0111000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2013\n"
           << "Start_MM   =   05\n"
           << "Start_DD   =   29\n"
           << "Start_hh   =   04\n"
           << "End_YYYY   = 2013\n"
           << "End_MM     =   05\n"
           << "End_DD     =   29\n"
           << "End_hh     =   15\n"
           << "TypeId     = -32767\n"
           << "missing_flagchange_common = fmis=3->fmis=1;fmis=[02]->fmis=4\n"
           << "Parameter  = par=211,minPar=213,maxPar=215,minVal=-80,maxVal=100,offsetCorrectionLimit=15,fluctuationLevel=0\n";
    AlgorithmConfig params;
    params.Parse(config);
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 11);

    data.setParam(211)
        .add("2013-05-29 08:00:00",      19.9,      19.9, "0111100000100010", "")
        .add("2013-05-29 09:00:00",      16.7,      16.7, "0111100000100010", "")
        .add("2013-05-29 10:00:00",      19.5,      19.5, "0111100000100010", "");
    data.setParam(213)
        .add("2013-05-29 08:00:00",   -6999.0,  -32767.0, "0700003000000000", "QC1-1-213,QC1-1-213x")
        .add("2013-05-29 09:00:00",      16.7,      16.7, "0112000000000000", "QC1-3a-213")
        .add("2013-05-29 10:00:00",   -6999.0,  -32767.0, "0700003000000000", "QC1-1-213,QC1-1-213x");
    data.setParam(215)
        .add("2013-05-29 08:00:00",      19.9,      19.9, "0111000000000000", "")
        .add("2013-05-29 09:00:00",      21.8,      21.8, "0111000000000000", "")
        .add("2013-05-29 10:00:00",      19.5,      19.5, "0111000000000000", "");
    ASSERT_NO_THROW(data.update(db));

    const int NUP2 = 6;
    ASSERT_RUN(algo, bc, NUP2);
    for(int i=0; i<NUP2; ++i)
        ASSERT_LT(-50, bc->update(i).corrected()) << "i=" << i;
}
