
#include "GapInterpolationTestBase.hh"

TEST_F(GapInterpolationTest, OnlyMissing)
{
    DataList data(1380, 211, 502);
    kvtime::time t = kvtime::maketime("2012-04-01 00:00:00");
    for(int i=0; i<21*24; ++i) {
        data.add(t, -32767, "0000003000000000", "");
        kvtime::addHours(t, 1);
    }
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   04\n"
           << "Start_DD   =   10\n"
           << "Start_hh   =    0\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   04\n"
           << "End_DD     =   10\n"
           << "End_hh     =    3\n"
           << "TypeId     =  502\n"
           << "Parameter  =  par=211,minVal=-100,maxVal=100,offsetCorrectionLimit=15\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
}
