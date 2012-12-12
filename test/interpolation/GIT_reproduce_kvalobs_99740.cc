
#include "GapInterpolationTestBase.hh"

TEST_F(GapInterpolationTest, reproduce_kvalobs_99740)
{
    std::ostringstream sql;
    INSERT_STATION(sql, 99740, "KONGSØYA",                    78.928, 28.892,   20);
    ASSERT_NO_THROW_X(db->exec(sql.str()));
    DataList data(99740, 211, 330);
    data
        .add("2012-10-13 01:00:00",      -1.4,      -1.4, "0111100000100010", "")
        .add("2012-10-13 02:00:00",      -1.5,      -1.5, "0111100000100010", "")
        .add("2012-10-13 03:00:00",      -1.6,      -1.6, "0111100000100010", "")
        .add("2012-10-13 04:00:00",      -1.3,      -1.3, "0111100000100010", "")
        .add("2012-10-13 05:00:00",  -32767.0,      -0.4, "0000601000000000", "QC1-4-211")
        .add("2012-10-13 06:00:00",      -1.2,      -1.2, "0110100000100010", "");
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   10\n"
           << "Start_DD   =   13\n"
           << "Start_hh   =   01\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   10\n"
           << "End_DD     =   13\n"
           << "End_hh     =   06\n"
           << "TypeId     = -32767\n"
           << "Parameter = par=211,minPar=213,maxPar=215,minVal=-80,maxVal=100,offsetCorrectionLimit=15,fluctuationLevel=0\n";
    AlgorithmConfig params;
    params.Parse(config);
    ASSERT_CONFIGURE(algo, params);

    // too close to time span limits
    // ASSERT_RUN(algo, bc, 1);

    ASSERT_RUN(algo, bc, 0);
}
