
#include <gtest/gtest.h>

#include "AlgorithmConfig.h"
#include "AlgorithmHelpers.h"
#include "CorrelatedNeighborInterpolator.h"
#include "foreach.h"

#include <puTools/miTime.h>
#include <stdexcept>

#define NDEBUG 1
#include "debug.h"

typedef std::vector<float> series_t;

class CorrelatedNeighborInterpolatorTest : public ::testing::Test {
public:
    void SetUp();
    void TearDown();

private:
    class TestDataAccess : public CorrelatedNeighbors::DataAccess {
    public:
        series_t fetchObservations(const Instrument& instrument, const TimeRange& t);
        series_t fetchModelValues (const Instrument& instrument, const TimeRange& t);
        CorrelatedNeighbors::neighbors_t findNeighbors(const Instrument& instrument, double maxsigma);

        void setCenter(int stationid, const series_t& observations, const series_t& modelvalues);
        void addNeighbor(int stationid, const series_t& observations);

    private:
        typedef std::map<int,series_t> mNeighborObservations_t;
        mNeighborObservations_t mNeighborObservations;
        int mCenterStationId;
        series_t mCenterObservations, mCenterModelvalues;
    };

protected:
    TestDataAccess* mDax;
    CorrelatedNeighbors::Interpolator* mCNI;
};

// ------------------------------------------------------------------------

void CorrelatedNeighborInterpolatorTest::SetUp()
{
    mDax = new TestDataAccess();
    mCNI = new CorrelatedNeighbors::Interpolator(mDax);

    std::stringstream config;
    config << "Parameter  =  par=178,minVal=800,maxVal=1200,offsetCorrectionLimit=5,fluctuationLevel=0.5\n"
           << "Parameter  =  par=211,minPar=213,maxPar=215,minVal=-80,maxVal=100,offsetCorrectionLimit=15,fluctuationLevel=0.5\n"
           << "Parameter  =  par=217,minPar=219,maxPar=220,minVal=-80,maxVal=100,offsetCorrectionLimit=15,fluctuationLevel=0.5\n"
           << "Parameter  =  par=262,minPar=264,maxPar=265,minVal=0,maxVal=100,offsetCorrectionLimit=5,fluctuationLevel=2\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_NO_THROW(mCNI->configure(params));
}

// ------------------------------------------------------------------------

void CorrelatedNeighborInterpolatorTest::TearDown()
{
    delete mCNI;
    delete mDax;
}

// ------------------------------------------------------------------------

series_t CorrelatedNeighborInterpolatorTest::TestDataAccess::fetchObservations(const Instrument& instrument, const TimeRange& t)
{
    if( t.hours() != (int)mCenterObservations.size()-1 )
        throw std::runtime_error("bad series length");

    if( instrument.stationid == mCenterStationId )
        return mCenterObservations;

    mNeighborObservations_t::const_iterator it = mNeighborObservations.find(instrument.stationid);
    if( it != mNeighborObservations.end() )
        return it->second;

    throw std::runtime_error("bad station id for observation data");
}

// ------------------------------------------------------------------------

series_t CorrelatedNeighborInterpolatorTest::TestDataAccess::fetchModelValues(const Instrument& instrument, const TimeRange& t)
{
    if( t.hours() != (int)mCenterObservations.size()-1 )
        throw std::runtime_error("bad series length");

    if( instrument.stationid == mCenterStationId )
        return mCenterModelvalues;

    throw std::runtime_error("bad station id for model data");
}

// ------------------------------------------------------------------------

CorrelatedNeighbors::neighbors_t CorrelatedNeighborInterpolatorTest::TestDataAccess::findNeighbors(const Instrument& instrument, double)
{
    if( instrument.stationid != mCenterStationId )
        throw std::runtime_error("bad station id for neighbor list");

    CorrelatedNeighbors::neighbors_t neighbors;
    foreach(mNeighborObservations_t::value_type& n, mNeighborObservations)
        neighbors.push_back(CorrelatedNeighbors::NeighborData(n.first, 0, 1, 2));
    return neighbors;
}

// ------------------------------------------------------------------------

void CorrelatedNeighborInterpolatorTest::TestDataAccess::setCenter(int stationid, const series_t& observations, const series_t& modelvalues)
{
    mCenterStationId = stationid;
    mCenterObservations = observations;
    mCenterModelvalues = modelvalues;
}

// ------------------------------------------------------------------------

void CorrelatedNeighborInterpolatorTest::TestDataAccess::addNeighbor(int stationid, const series_t& observations)
{
    mNeighborObservations[stationid] = observations;
}

// ========================================================================

TEST_F(CorrelatedNeighborInterpolatorTest, Constant)
{
    const int Ngap = 4, N = Ngap + 2 + 4;
    const float centerObs[N] = { 1, 1, 1, -32767, -32767, -32767, -32767, 1, 1, 1 };
    const float centerMdl[N] = { 1, 1, 1,      1,      1,      1,      1, 1, 1, 1 };
    mDax->setCenter(1234, series_t(centerObs, centerObs+N), series_t(centerMdl, centerMdl+N));
    
    const float nbr1Obs[N]   = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
    const float nbr2Obs[N]   = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };
    mDax->addNeighbor(1, series_t(nbr1Obs, nbr1Obs+N));
    mDax->addNeighbor(2, series_t(nbr2Obs, nbr2Obs+N));

    const miutil::miTime t0(2012, 3, 27, 16, 0);
    Interpolator::ValuesWithQualities_t interpolated  = mCNI->interpolate(Instrument(1234, 211, 0, 302, 0),
                                                                          TimeRange(t0, Helpers::plusHour(t0, Ngap-1)));
    EXPECT_EQ(Ngap, interpolated.size());
    for(unsigned int i=0; i<interpolated.size(); ++i) {
        EXPECT_FLOAT_EQ(1.0f, interpolated[i].value);
    }
}

// ------------------------------------------------------------------------

TEST_F(CorrelatedNeighborInterpolatorTest, Linear)
{
    const int Ngap = 4, N = Ngap + 2 + 4;
    const float centerObs[N] = { 0, 1, 2, -32767, -32767, -32767, -32767, 7, 8,  9 };
    const float centerMdl[N] = { 1, 2, 3,      4,      5,      6,      7, 8, 9, 10 };
    mDax->setCenter(1234, series_t(centerObs, centerObs+N), series_t(centerMdl, centerMdl+N));
    
    const float nbr1Obs[N]   = { 1, 1, 1, 2, 2, 2, 2, 1, 1, 1 };
    const float nbr2Obs[N]   = { 3, 3, 3, 4, 4, 4, 4, 3, 3, 3 };
    mDax->addNeighbor(1, series_t(nbr1Obs, nbr1Obs+N));
    mDax->addNeighbor(2, series_t(nbr2Obs, nbr2Obs+N));

    const miutil::miTime t0(2012, 3, 27, 16, 0);
    Interpolator::ValuesWithQualities_t interpolated  = mCNI->interpolate(Instrument(1234, 211, 0, 302, 0),
                                                                          TimeRange(t0, Helpers::plusHour(t0, Ngap-1)));
    EXPECT_EQ(Ngap, interpolated.size());
    for(unsigned int i=0; i<interpolated.size(); ++i) {
        EXPECT_FLOAT_EQ(i+4, interpolated[i].value);
    }
}

// ------------------------------------------------------------------------

TEST_F(CorrelatedNeighborInterpolatorTest, Triangel)
{
    const int Ngap = 3, N = Ngap + 2 + 4;
    const float centerObs[N] = { 0, 1, 2, -32767, -32767, -32767, 2, 1,  0 };
    const float centerMdl[N] = { 1, 2, 3,      4,      6,      7, 8, 9, 10 };
    mDax->setCenter(1234, series_t(centerObs, centerObs+N), series_t(centerMdl, centerMdl+N));
    
    const float nbr1Obs[N]   = { 1, 1, 1, 2, 3, 2, 1, 1, 1 };
    const float nbr2Obs[N]   = { 0, 1, 2, 3, 4, 3, 2, 1, 0 };
    mDax->addNeighbor(1, series_t(nbr1Obs, nbr1Obs+N));
    mDax->addNeighbor(2, series_t(nbr2Obs, nbr2Obs+N));

    const miutil::miTime t0(2012, 3, 27, 16, 0);
    Interpolator::ValuesWithQualities_t interpolated  = mCNI->interpolate(Instrument(1234, 211, 0, 302, 0),
                                                                          TimeRange(t0, Helpers::plusHour(t0, Ngap-1)));
    EXPECT_EQ(Ngap, interpolated.size());
    for(unsigned int i=0; i<interpolated.size(); ++i) {
        const float expected = (i==1) ? 4 : 3;
        EXPECT_FLOAT_EQ(expected, interpolated[i].value) << "i=" << i;
    }
}
