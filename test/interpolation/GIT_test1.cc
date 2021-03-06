
#include "GapInterpolationTestBase.hh"

TEST_F(GapInterpolationTest, test1)
{
    DataList data(68290, 211, 330);
    data.add("2012-03-25 18:00:00",      4.0, "0111000000100010", "")
        .add("2012-03-25 19:00:00",      3.0, "0111000000100010", "")
        .add("2012-03-25 20:00:00",      2.0, "0111000000100010", "")
        .add("2012-03-25 21:00:00",      3.0, "0111000000100010", "")
        .add("2012-03-25 22:00:00",      2.0, "0111000000100010", "")
        .add("2012-03-25 23:00:00",      1.0, "0111000000100010", "")
        .add("2012-03-26 00:00:00",      1.0, "0111000000100010", "")
        .add("2012-03-26 01:00:00",      1.0, "0111000000100010", "");
    data.setStation(69100)
        .add("2012-03-25 18:00:00",      4.1, "0111000000100010", "")
        .add("2012-03-25 19:00:00",      3.1, "0111000000100010", "")
        .add("2012-03-25 20:00:00",      2.1, "0111000000100010", "")
        .add("2012-03-25 21:00:00",      3.1, "0111000000100010", "")
        .add("2012-03-25 22:00:00",      2.1, "0111000000100010", "")
        .add("2012-03-25 23:00:00",      1.1, "0111000000100010", "")
        .add("2012-03-26 00:00:00",      1.1, "0111000000100010", "")
        .add("2012-03-26 01:00:00",      1.1, "0111000000100010", "");
    data.setStation(69150)
        .add("2012-03-25 18:00:00",      3.0, "0111000000100010", "")
        .add("2012-03-25 19:00:00",      2.0, "0111000000100010", "")
        .add("2012-03-25 20:00:00",      1.0, "0111000000100010", "")
        .add("2012-03-25 21:00:00", -32767.0, "0000003000000000", "")
        .add("2012-03-25 22:00:00", -32767.0, "0000003000000000", "")
        .add("2012-03-25 23:00:00",      0.0, "0110000000100010", "")
        .add("2012-03-26 00:00:00",      0.0, "0111000000100010", "")
        .add("2012-03-26 01:00:00",      0.0, "0111000000100010", "");
    data.setStation(69380)
        .add("2012-03-25 18:00:00",      4.2, "0111000000100010", "")
        .add("2012-03-25 19:00:00",      3.2, "0111000000100010", "")
        .add("2012-03-25 20:00:00",      2.2, "0111000000100010", "")
        .add("2012-03-25 21:00:00",      3.2, "0111000000100010", "")
        .add("2012-03-25 22:00:00",      2.2, "0111000000100010", "")
        .add("2012-03-25 23:00:00",      1.2, "0111000000100010", "")
        .add("2012-03-26 00:00:00",      1.2, "0111000000100010", "")
        .add("2012-03-26 01:00:00",      1.2, "0111000000100010", "");
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   03\n"
           << "Start_DD   =   25\n"
           << "Start_hh   =   18\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   03\n"
           << "End_DD     =   26\n"
           << "End_hh     =   01\n"
           << "TypeId     =  330\n"
           << "Parameter  =  par=211,minVal=-100,maxVal=100,offsetCorrectionLimit=15\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);
    EXPECT_NEAR(2, bc->update(0).corrected(), 0.01);
    EXPECT_NEAR(1, bc->update(1).corrected(), 0.01);

    ASSERT_RUN(algo, bc, 0);
}
