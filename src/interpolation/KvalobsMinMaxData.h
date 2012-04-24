// -*- c++ -*-

#ifndef KVALOBSDATA_H_
#define KVALOBSDATA_H_

#include "FlagPatterns.h"
#include "KvalobsNeighborData.h"
#include "MinMaxInterpolator.h"

class KvalobsMinMaxData : public Interpolation::MinMaxInterpolator::Data {
public:
    KvalobsMinMaxData(KvalobsNeighborData& nd);

    virtual Interpolation::SeriesData minimum(int time);
    virtual Interpolation::SeriesData maximum(int time);
    virtual void setMinimum(int time, Interpolation::Quality q, float value);
    virtual void setMaximum(int time, Interpolation::Quality q, float value);
    virtual float fluctuationLevel() const;

    const Interpolation::SimpleResultVector& getInterpolatedMin()
        { return interpolationsMin; }

    const Interpolation::SimpleResultVector& getInterpolatedMax()
        { return interpolationsMax; }

private:
    miutil::miTime timeAtOffset(int time) const
        { return neighborData().timeAtOffset(time); }

    DBInterface* database()
        { return neighborData().getDatabase(); }

    KvalobsNeighborData& neighborData()
        { return static_cast<KvalobsNeighborData&>(centerData()); }

    const KvalobsNeighborData& neighborData() const
        { return static_cast<const KvalobsNeighborData&>(centerData()); }

protected:
    FlagSetCU mNeighborFlags;
    DBInterface::DataList minimumData, maximumData;
    Interpolation::SimpleResultVector interpolationsMin, interpolationsMax;
};

#endif /* KVALOBSDATA_H_ */
