
#include "GapInterpolationTestBase.hh"

TEST_F(GapInterpolationTest, UUBecomes0WithoutNeighbors)
{
    DataList data(96400, 211, 330);
    data.add("2012-07-18 00:00:00",       8.3,       8.3, "0111000000100010", "")
        .add("2012-07-18 01:00:00",       8.1,       8.1, "0111000000100010", "")
        .add("2012-07-18 02:00:00",       7.8,       7.8, "0111000000100010", "")
        .add("2012-07-18 03:00:00",    -32767,       8.1, "0000601000000000", "") // obs: 7.8 from dev-vm205
        .add("2012-07-18 04:00:00",       7.9,       7.9, "0111000000100010", "")
        .add("2012-07-18 05:00:00",       7.7,       7.7, "0111000000100010", "")
        .add("2012-07-18 06:00:00",       7.9,       7.9, "0111000000100010", "");
    data.setParam(213);
    data.setStation(96400).setType(330)
        .add("2012-07-18 00:00:00",       8.2,       8.2, "0111000000000000", "")
        .add("2012-07-18 01:00:00",       8.1,       8.1, "0111000000000000", "")
        .add("2012-07-18 02:00:00",       7.8,       7.8, "0111000000000000", "")
        .add("2012-07-18 03:00:00",    -32767,    -32767, "0000003000000000", "") // obs: 7.8
        .add("2012-07-18 04:00:00",       7.6,       7.6, "0111000000000000", "")
        .add("2012-07-18 05:00:00",       7.6,       7.6, "0111000000000000", "")
        .add("2012-07-18 06:00:00",       7.6,       7.6, "0111000000000000", "");
    data.setParam(215);
    data.setStation(96400).setType(330)
        .add("2012-07-18 00:00:00",       8.3,       8.3, "0111000000000000", "")
        .add("2012-07-18 01:00:00",       8.3,       8.3, "0111000000000000", "")
        .add("2012-07-18 02:00:00",       8.1,       8.1, "0111000000000000", "")
        .add("2012-07-18 03:00:00",    -32767,    -32767, "0000003000000000", "") // obs: 7.9
        .add("2012-07-18 04:00:00",       7.9,       7.9, "0111000000000000", "")
        .add("2012-07-18 05:00:00",       7.9,       7.9, "0111000000000000", "")
        .add("2012-07-18 06:00:00",       7.9,       7.9, "0111000000000000", "");
    data.setParam(262);
    data.setStation(96400).setType(330)
        .add("2012-07-18 00:00:00",      87.9,      87.9, "0101000000000010", "")
        .add("2012-07-18 01:00:00",      88.8,      88.8, "0101000000000010", "")
        .add("2012-07-18 02:00:00",      90.3,      90.3, "0101000000000010", "")
        .add("2012-07-18 03:00:00",    -32767,      79.0, "0000601000000000", "") // obs: 89.9
        .add("2012-07-18 04:00:00",      89.3,      89.3, "0101000000000010", "")
        .add("2012-07-18 05:00:00",      89.2,      89.2, "0101000000000010", "")
        .add("2012-07-18 06:00:00",      87.6,      87.6, "0101000000000010", "");
    ASSERT_NO_THROW(data.insert(db));

    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(96400, 71.084, 28.218, 8, 0, 'SLETTNES FYR', NULL, 96400, NULL, NULL, NULL, 8, 't', '1956-08-25 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   07\n"
           << "Start_DD   =   18\n"
           << "Start_hh   =   00\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   07\n"
           << "End_DD     =   18\n"
           << "End_hh     =   06\n"
           << "TypeId     =  330\n"
           << "Parameter  =  par=211,minPar=213,maxPar=215,offsetCorrectionLimit=15,fluctuationLevel=0.5\n"
           << "Parameter  =  par=262,minPar=264,maxPar=265,minVal=0,maxVal=100,offsetCorrectionLimit=5,fluctuationLevel=2\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 6);
    EXPECT_NEAR(7.8, bc->update(0).corrected(), 0.1);
    EXPECT_NEAR(7.7, bc->update(1).corrected(), 0.5);
    EXPECT_NEAR(7.6, bc->update(2).corrected(), 0.5);
    EXPECT_NEAR(7.7, bc->update(3).corrected(), 0.5);
    EXPECT_NEAR(7.6, bc->update(4).corrected(), 0.5);
    EXPECT_NEAR(90,  bc->update(5).corrected(), 2);

    ASSERT_RUN(algo, bc, 0);
}
