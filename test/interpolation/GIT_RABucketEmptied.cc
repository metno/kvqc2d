
#include "GapInterpolationTestBase.hh"

TEST_F(GapInterpolationTest, RABucketEmptied)
{
    DataList data(18700, 104, 330);
    data.add("2012-05-08 06:00:00",     183.6, "0101000000000100")
        .add("2012-05-08 07:00:00",     183.6, "0101000000000100")
        .add("2012-05-08 08:00:00",     183.6, "0101000000000100")
        .add("2012-05-08 09:00:00",     183.5, "0101000000000100")
        .add("2012-05-08 10:00:00",     183.5, "0101000000000100")
        .add("2012-05-08 11:00:00",     183.5, "0101000000000100")
        .add("2012-05-08 12:00:00",  -32767.0, "0000003000000000")
        .add("2012-05-08 13:00:00",     183.4, "0100000000000100")
        .add("2012-05-08 14:00:00",     183.4, "0101000000000100")
        .add("2012-05-08 15:00:00",     183.4, "0101000000000100")
        .add("2012-05-08 16:00:00",     183.4, "0101000000000100")
        .add("2012-05-08 17:00:00",  -32767.0, "0000003000000000")
        .add("2012-05-08 18:00:00",       0.0, "0101000000000100")
        .add("2012-05-08 19:00:00",       0.0, "0101000000000100")
        .add("2012-05-08 20:00:00",       0.3, "0101000000000100")
        .add("2012-05-08 21:00:00",       1.5, "0101000000000100")
        .add("2012-05-08 22:00:00",       2.0, "0101000000000100")
        .add("2012-05-08 23:00:00",       2.3, "0101000000000100")
        .add("2012-05-09 00:00:00",       2.5, "0101000000000100")
        .add("2012-05-09 01:00:00",       2.8, "0101000000000100")
        .add("2012-05-09 02:00:00",       5.0, "0101000000000100")
        .add("2012-05-09 03:00:00",       6.5, "0101000000000100")
        .add("2012-05-09 04:00:00",       8.4, "0101000000000100")
        .add("2012-05-09 05:00:00",       8.9, "0101000000000100");
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =    5\n"
           << "Start_DD   =    1\n"
           << "Start_hh   =    6\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =    5\n"
           << "End_DD     =   10\n"
           << "End_hh     =   14\n"
           << "TypeId     =  330\n"
           << "Parameter  = par=211,minPar=213,maxPar=215,minVal=-80,maxVal=80,offsetCorrectionLimit=15,fluctuationLevel=0.5\n"
           << "Parameter  = par=262,minPar=264,maxPar=265,minVal=0,maxVal=100,offsetCorrectionLimit=5,fluctuationLevel=2\n"
           << "Parameter  = par=104,minVal=0,maxVal=999999\n";
    AlgorithmConfig params;
    ASSERT_PARSE_CONFIG(params, config);
    ASSERT_CONFIGURE(algo, params);

    ASSERT_RUN(algo, bc, 1);
    ASSERT_OBS_CONTROL_CFAILED("2012-05-08 12:00:00", "0000001100000000", "QC2d-2-I", bc->update(0));

    ASSERT_RUN(algo, bc, 0);
}