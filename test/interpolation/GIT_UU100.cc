
#include "GapInterpolationTestBase.hh"

TEST_F(GapInterpolationTest, UU100)
{
    DataList data(56420, 211, 502);
    data.add("2024-06-19 00:00:00", 20.1, "0101100000000010", "")
        .add("2024-06-19 01:00:00", 20.2, "0101100000000010", "")
        .add("2024-06-19 02:00:00", 20.3, "0101100000000010", "")
        .add("2024-06-19 03:00:00", 20.4, "0101100000000010", "")
        .add("2024-06-19 04:00:00", 20.5, "0000601100000005", "")
        .add("2024-06-19 05:00:00", 20.6, "0100100000000010", "")
        .add("2024-06-19 06:00:00", 20.7, "0101100000000010", "")
        .add("2024-06-19 07:00:00", 20.8, "0101100000000010", "")
        .add("2024-06-19 08:00:00", 20.9, "0101100000000010", "");
    data.setParam(262);
    data.add("2024-06-19 00:00:00",   90.3,   90.3, "0101100000000010", "")
        .add("2024-06-19 01:00:00",   90.3,   90.3, "0101100000000010", "")
        .add("2024-06-19 02:00:00",   86.1,   86.1, "0101100000000010", "")
        .add("2024-06-19 03:00:00",   83.1,   83.1, "0101100000000010", "")
        .add("2024-06-19 04:00:00", -32767, -32767, "0000601100000005", "QC1-4-262,hqc-m")
        .add("2024-06-19 05:00:00",   74.9,   74.9, "0100100000000010", "")
        .add("2024-06-19 06:00:00",   67.9,   67.9, "0101100000000010", "")
        .add("2024-06-19 07:00:00",   67.9,   67.9, "0101100000000010", "")
        .add("2024-06-19 08:00:00",   66.2,   66.2, "0101100000000010", "");

    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2024\n"
           << "Start_MM   =    6\n"
           << "Start_DD   =   19\n"
           << "Start_hh   =   00\n"
           << "End_YYYY   = 2024\n"
           << "End_MM     =    6\n"
           << "End_DD     =   19\n"
           << "End_hh     =    8\n"
           << "TypeId     =  502\n"
           << "Parameter  = par=211,minPar=213,maxPar=215,minVal=-80,maxVal=80,offsetCorrectionLimit=15,fluctuationLevel=0.5\n"
           << "Parameter  = par=262,minPar=264,maxPar=265,minVal=0,maxVal=100,offsetCorrectionLimit=5,fluctuationLevel=2\n";
    AlgorithmConfig params;
    ASSERT_PARSE_CONFIG(params, config);
    ASSERT_CONFIGURE(algo, params);

    ASSERT_RUN(algo, bc, 0);
}
