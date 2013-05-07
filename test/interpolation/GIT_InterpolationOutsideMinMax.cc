
#include "GapInterpolationTestBase.hh"

#include "helpers/timeutil.h"

static kvalobs::kvData getDb(DBInterface* db, int stationid, int paramid, const kvtime::time& obstime)
{
    std::list<kvalobs::kvData> series
        = db->findDataOrderObstime(stationid, paramid, TimeRange(obstime, obstime));
    if( series.size() == 1 )
        return *series.begin();
    else
        return kvalobs::kvData();
}

TEST_F(GapInterpolationTest, InterpolationOutsideMinMax)
{
    DataList data(94500, 211, 330);
    data.add("2012-08-26 16:00:00",       5.0, "0111100000100010", "")
        .add("2012-08-26 17:00:00",       5.0, "0111100000100010", "")
        .add("2012-08-26 18:00:00",       5.0, "0111100000100010", "")
        .add("2012-08-26 19:00:00",       5.0, "0111100000100010", "")
        .add("2012-08-26 20:00:00",       5.0, "0111100000100010", "")
        .add("2012-08-26 21:00:00",       5.0, "0111100000100010", "")
        .add("2012-08-26 22:00:00",       5.0, "0111100000100010", "")
        .add("2012-08-26 23:00:00",       5.0, "0111100000100010", "")
        .add("2012-08-27 00:00:00",       5.0, "0111100000100010", "")
        .add("2012-08-27 01:00:00",       5.0, "0111100000100010", "")
        .add("2012-08-27 02:00:00",       5.0, "0111100000100010", "");
    data.setStation(96310).setType(999)
        .add("2012-08-26 16:00:00",    4.0, "0111100000100010", "")
        .add("2012-08-26 17:00:00",    4.0, "0111100000100010", "")
        .add("2012-08-26 18:00:00", -32767, "0700003000000000", "QC1-1-211,QC1-1-211x")
        .add("2012-08-26 19:00:00", -32767, "0700003000000000", "QC1-1-211,QC1-1-211x")
        .add("2012-08-26 20:00:00", -32767, "0700003000000000", "QC1-1-211,QC1-1-211x")
        .add("2012-08-26 21:00:00", -32767, "0700003000000000", "QC1-1-211,QC1-1-211x")
        .add("2012-08-26 22:00:00", -32767, "0700003000000000", "QC1-1-211,QC1-1-211x")
        .add("2012-08-26 23:00:00", -32767, "0700003000000000", "QC1-1-211,QC1-1-211x")
        .add("2012-08-27 00:00:00",    4.0, "0110500000100012", "QC1-4-211,hqc")
        .add("2012-08-27 01:00:00",    4.0, "0111500000100012", "QC1-4-211,hqc")
        .add("2012-08-27 02:00:00",    4.0, "0111100000100010", "");
    data.setStation(96310).setParam(213).setType(999)
        .add("2012-08-26 16:00:00",     3.0, "0111000000000000", "")
        .add("2012-08-26 17:00:00",     3.0, "0111000000000000", "")
        .add("2012-08-26 18:00:00",  -32767, "0700003000000000", "QC1-1-213,QC1-1-213x")
        .add("2012-08-26 19:00:00",  -32767, "0700003000000000", "QC1-1-213,QC1-1-213x")
        .add("2012-08-26 20:00:00",     8.0, "0111000000000000", "")
        .add("2012-08-26 21:00:00",     8.0, "0111000000000000", "")
        .add("2012-08-26 22:00:00",  -32767, "0700003000000000", "QC1-1-213,QC1-1-213x")
        .add("2012-08-26 23:00:00",  -32767, "0700003000000000", "QC1-1-213,QC1-1-213x")
        .add("2012-08-27 00:00:00",     3.0, "0110000000000000", "")
        .add("2012-08-27 01:00:00",     3.0, "0110000000000000", "")
        .add("2012-08-27 02:00:00",     3.0, "0110000000000000", "");
    data.setParam(215)
        .add("2012-08-26 16:00:00",     5.0, "0111000000000000", "")
        .add("2012-08-26 17:00:00",     5.0, "0111000000000000", "")
        .add("2012-08-26 18:00:00",  -32767, "0700003000000000", "QC1-1-215,QC1-1-213x")
        .add("2012-08-26 19:00:00",  -32767, "0700003000000000", "QC1-1-215,QC1-1-213x")
        .add("2012-08-26 20:00:00",    10.0, "0111000000000000", "")
        .add("2012-08-26 21:00:00",    10.0, "0111000000000000", "")
        .add("2012-08-26 22:00:00",  -32767, "0700003000000000", "QC1-1-215,QC1-1-215x")
        .add("2012-08-26 23:00:00",  -32767, "0700003000000000", "QC1-1-215,QC1-1-215x")
        .add("2012-08-27 00:00:00",     5.0, "0110000000000000", "")
        .add("2012-08-27 01:00:00",     5.0, "0110000000000000", "")
        .add("2012-08-27 02:00:00",     5.0, "0110000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    std::ostringstream sql;
    INSERT_STATION(sql, 94500, "FRUHOLMEN FYR",               71.094, 23.982,   13);
    INSERT_STATION(sql, 96310, "MEHAMN LUFTHAVN",             71.033, 27.830,   13);

    INSERT_NEIGHBOR(sql, 96310, 211, 94500,   -2.14313,  1.25887, 2.13727);
    ASSERT_NO_THROW_X(db->exec(sql.str()));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   08\n"
           << "Start_DD   =   26\n"
           << "Start_hh   =   16\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   08\n"
           << "End_DD     =   27\n"
           << "End_hh     =   02\n"
           << "TypeId     =  999\n"
           << "Parameter  =  par=211,minPar=213,maxPar=215,offsetCorrectionLimit=15,fluctuationLevel=0.5\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);

    ASSERT_RUN(algo, bc, 18);

    const int stationid = bc->update(0).stationID();
    const kvtime::time t0 = kvtime::maketime("2012-08-26 16:00:00");
    for(int i=0; i<11; ++i) {
        const kvtime::time t = Helpers::plusHour(t0, i);
        const float TA  = getDb(db, stationid, 211, t).corrected();
        const float TAN = getDb(db, stationid, 213, t).corrected();
        const float TAX = getDb(db, stationid, 215, t).corrected();
        ASSERT_LE(TAN, TAX) << "TAX < TAN for i=" << i << " t=" << t;
        ASSERT_LE(TAN, TA)  << "TA < TAN for i=" << i << " t=" << t;
        ASSERT_GE(TAX, TA)  << "TA > TAX for i=" << i << " t=" << t;
    }

    ASSERT_RUN(algo, bc, 0);
}
