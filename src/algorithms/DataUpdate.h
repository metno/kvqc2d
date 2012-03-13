// -*- c++ -*-

#ifndef DataUpdate_H
#define DataUpdate_H 1

#include <kvalobs/kvData.h>
#include <kvalobs/kvDataFlag.h>
#include <puTools/miTime.h>
#include <iosfwd>

// ########################################################################

class DataUpdate {
private:
    enum eForced { FORCED_NOTHING = 0,
                   FORCED_WRITE = 1,
                   FORCED_NOWRITE = 2 };

public:
    DataUpdate();

    DataUpdate(const kvalobs::kvData& data);

    DataUpdate(const kvalobs::kvData& templt, const miutil::miTime& obstime, const miutil::miTime& tbtime,
               float original, float corrected, const std::string& controlinfo);

    bool needsWrite() const;

    bool isNew() const
        { return mNew; }

    kvalobs::kvData& data()
        { return mData; }

    const kvalobs::kvData& data() const
        { return mData; }

    float original() const
        { return mData.original(); }

    float corrected() const
        { return mData.corrected(); }

    miutil::miTime obstime() const
        { return mData.obstime(); }

    kvalobs::kvControlInfo controlinfo() const
        { return mData.controlinfo(); }

    DataUpdate& corrected(float c)
        { mData.corrected(c); return *this; }

    DataUpdate& controlinfo(const kvalobs::kvControlInfo& ci);

    DataUpdate& cfailed(const std::string& cf, const std::string& extra="");

    DataUpdate& forceWrite()
        { mForced = FORCED_WRITE; return *this; }

    DataUpdate& forceNoWrite()
        { mForced = FORCED_NOWRITE; return *this; }

    bool operator<(const DataUpdate& other) const
        { return obstime() < other.obstime(); }

    std::string text(int hoursBefore=0, bool modified=true) const;
    std::string text(const miutil::miTime& start, bool modified=true) const;

private:
    kvalobs::kvData mData;
    bool mNew;
    eForced mForced;
    kvalobs::kvControlInfo mOrigControl;
    float mOrigCorrected;
    std::string mOrigCfailed;
};

std::ostream& operator<<(std::ostream& out, const DataUpdate& du);

// ########################################################################

#endif /* DataUpdate */
