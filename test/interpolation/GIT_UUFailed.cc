
#include "GapInterpolationTestBase.hh"

/*
  May not overwrite model value at 03:00 and 04:00.
 */

TEST_F(GapInterpolationTest, UUFailed)
{
    std::ostringstream sql;
    INSERT_STATION(sql, 89920, "NJUNIS",                      68.751, 19.470, 1655);
    ASSERT_NO_THROW_X(db->exec(sql.str()));
    DataList data(89920, 262, 502);
    data.add("2013-09-16 00:00:00",      99.9,      99.9, "0101100000000010", "")
        .add("2013-09-16 01:00:00",     100.0,     100.0, "0101100000000010", "")
        .add("2013-09-16 02:00:00",     100.0,     100.0, "0101100000000010", "")
        .add("2013-09-16 03:00:00",     -99.9,      90.9, "0700601000000000", "QC1-1-262h,QC1-1-262x,QC1-4-262")
        .add("2013-09-16 04:00:00",     -99.9,      89.0, "0700601000000000", "QC1-1-262h,QC1-1-262x,QC1-4-262")
        .add("2013-09-16 05:00:00",     100.0,     100.0, "0100100000000010", "")
        .add("2013-09-16 06:00:00",     100.0,     100.0, "0101100000000010", "")
        .add("2013-09-16 07:00:00",     100.0,     100.0, "0101100000000010", "");
    data.setParam(211);
    data.add("2013-09-16 00:00:00",       0.4,       0.4, "0111100000100010", "")
        .add("2013-09-16 01:00:00",       0.3,       0.3, "0111100000100010", "")
        .add("2013-09-16 02:00:00",       0.6,       0.6, "0111100000100010", "")
        .add("2013-09-16 03:00:00",       0.9,       0.9, "0111100000100010", "")
        .add("2013-09-16 04:00:00",       1.0,       1.0, "0111100000100010", "")
        .add("2013-09-16 05:00:00",       0.5,       0.5, "0111100000100010", "")
        .add("2013-09-16 06:00:00",       0.2,       0.2, "0111100000100010", "")
        .add("2013-09-16 07:00:00",       0.2,       0.2, "0111100000100010", "");
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2013\n"
           << "Start_MM   =   09\n"
           << "Start_DD   =   16\n"
           << "Start_hh   =   00\n"
           << "End_YYYY   = 2013\n"
           << "End_MM     =   09\n"
           << "End_DD     =   16\n"
           << "End_hh     =   07\n"
           << "TypeId     = -32767\n"
           << "Parameter  =  par=211,minPar=213,maxPar=215,offsetCorrectionLimit=15,fluctuationLevel=0.5\n"
           << "Parameter  =  par=262,minPar=264,maxPar=265,minVal=0,maxVal=100,offsetCorrectionLimit=5,fluctuationLevel=2\n";
    AlgorithmConfig params;
    params.Parse(config);
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
}
