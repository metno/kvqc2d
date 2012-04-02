// -*- c++ -*-

#ifndef KvalobsElemExtract_h
#define KvalobsElemExtract_h 1

#include "KvalobsDbGate.h"

template<class Elem, class Iterator>
class KvalobsElemExtract : public KvalobsDbExtract {
public:
    KvalobsElemExtract(Iterator push)
        : mPush(push) { }
    
    void extractFromRow(const dnmi::db::DRow& row)
        { *mPush++ = Elem(row); }

private:
    Iterator mPush;
};

template<class E, class I>
KvalobsElemExtract<E, I>* makeElementExtract(I i)
{
    return new KvalobsElemExtract<E, I>(i);
}

#endif /* KvalobsElemExtract_h */
