
#include <gtest/gtest.h>

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
    class TestDataAccess : public DataAccess {
    public:
        const series_t fetchObservations(int stationid, int paramid, const miutil::miTime& t0, const miutil::miTime& t1);
        const series_t fetchModelValues (int stationid, int paramid, const miutil::miTime& t0, const miutil::miTime& t1);
        const neighbors_t findNeighbors(int stationid, int paramid, double maxsigma);

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
    CorrelatedNeighborInterpolator* mCNI;
};

// ------------------------------------------------------------------------

void CorrelatedNeighborInterpolatorTest::SetUp()
{
    mDax = new TestDataAccess();
    mCNI = new CorrelatedNeighborInterpolator(mDax);
}

// ------------------------------------------------------------------------

void CorrelatedNeighborInterpolatorTest::TearDown()
{
    delete mCNI;
    delete mDax;
}

// ------------------------------------------------------------------------

const series_t CorrelatedNeighborInterpolatorTest::TestDataAccess::fetchObservations
    (int stationid, int paramid, const miutil::miTime& t0, const miutil::miTime& t1)
{
    if( miutil::miTime::hourDiff(t1, t0) != (int)mCenterObservations.size()-1 )
        throw std::runtime_error("bad series length");

    if( stationid == mCenterStationId )
        return mCenterObservations;
    
    mNeighborObservations_t::const_iterator it = mNeighborObservations.find(stationid);
    if( it != mNeighborObservations.end() )
        return it->second;

    throw std::runtime_error("bad station id for observation data");
}

// ------------------------------------------------------------------------

const series_t CorrelatedNeighborInterpolatorTest::TestDataAccess::fetchModelValues
    (int stationid, int paramid, const miutil::miTime& t0, const miutil::miTime& t1)
{
    if( miutil::miTime::hourDiff(t1, t0) != (int)mCenterObservations.size()-1 )
        throw std::runtime_error("bad series length");

    if( stationid == mCenterStationId )
        return mCenterModelvalues;
    
    throw std::runtime_error("bad station id for model data");
}

// ------------------------------------------------------------------------

const neighbors_t CorrelatedNeighborInterpolatorTest::TestDataAccess::findNeighbors(int stationid, int paramid, double maxsigma)
{
    if( stationid != mCenterStationId )
        throw std::runtime_error("bad station id for neighbor list");

    neighbors_t neighbors;
    foreach(mNeighborObservations_t::value_type& n, mNeighborObservations)
        neighbors.push_back(NeighborData(n.first, 0, 1, 2));
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

TEST_F(CorrelatedNeighborInterpolatorTest, Mini)
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

    Interpolator::ValuesWithQualities_t interpolated  = mCNI->interpolate(t0, Helpers::plusHour(t0, Ngap+1), 1234, 211);
    EXPECT_EQ(Ngap, interpolated.size());
    for(unsigned int i=0; i<interpolated.size(); ++i) {
        EXPECT_FLOAT_EQ(1.0f, interpolated[i].value);
    }
}
