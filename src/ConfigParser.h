// -*- c++ -*-

#ifndef ConfigParser_H
#define ConfigParser_H 1

#include <boost/shared_ptr.hpp>
#include <map>
#include <string>
#include <sstream>
#include <vector>

class ErrorList {
public:
    typedef std::vector<std::string> List_t;

    class Push {
    public:
        Push(List_t& list)
            : mList(list), mStream(new std::ostringstream) { }

        ~Push()
            { mList.push_back(mStream->str()); }

        template<class T>
        std::ostream& operator <<(const T& t)
            { return *mStream << t; }

    private:
        List_t& mList;
        boost::shared_ptr<std::ostringstream> mStream;
    };

public:
    void clear()
        { mErrors.clear(); }

    int size() const
        { return mErrors.size(); }

    bool empty() const
        { return mErrors.empty(); }

    const std::string& operator[](int idx) const
        { return get(idx); }

    const std::string& get(int idx) const
        { return mErrors[idx]; }

    Push add()
        { return Push(mErrors); }

    operator bool() const
        { return mErrors.empty(); }

    std::string format(const std::string& separator) const;

private:
    std::vector<std::string> mErrors;
};

// ########################################################################

class ConfigParser {
public:
    class Item {
        friend class ConfigParser;

    public:
        Item() : mRequested(0) { }

        int count() const
            { return mValues.size(); }

        int requestCount() const
            { return mRequested; }

        template<class T>
        T convert(unsigned int idx) const;

        template<class T>
        T convert(unsigned int idx, const T& dflt) const;

        template<class T>
        std::vector<T> convert() const;

    private:
        const std::string& value(unsigned int idx) const;

        const std::string& value(unsigned int idx, const std::string& dflt) const;

        const std::vector<std::string>& values() const
            { return mValues; }

    private:
        std::vector<std::string> mValues;
        mutable int mRequested;
    };

    ConfigParser();
    ~ConfigParser();

    bool load(std::istream& input);

    bool load(const std::string& filename);

    bool has(const std::string& key) const
        { return mItems.find(key) != mItems.end(); }

    const Item& get(const std::string& key) const;

    const ErrorList& errors() const
        { return mErrors; }

    ErrorList checkUnrequested() const;

private:
    typedef std::map<std::string, Item> mItems_t;
    mItems_t mItems;
    ErrorList mErrors;
};

// ########################################################################

template<class T>
T ConfigParser::Item::convert(unsigned int idx) const
{
    mRequested += 1;
    T t;
    std::istringstream i(mValues[idx]);
    i >> t;
    return t;
}

template<class T>
T ConfigParser::Item::convert(unsigned int idx, const T& dflt) const
{
    mRequested += 1;
    if( idx < mValues.size() ) {
        T t;
        const std::string v = mValues[idx];
        std::istringstream i(v);
        i >> t;
        if( !i.fail() && (int)i.tellg() == (int)v.size() )
            return t;
    }
    return dflt;
}

template<class T>
std::vector<T> ConfigParser::Item::convert() const
{
    mRequested += 1;
    std::vector<T> out;
    for(unsigned int i=0; i<mValues.size(); ++i ) {
        T t;
        const std::string v = mValues[i];
        std::istringstream iv(v);
        iv >> t;
        if( iv.fail() || (int)iv.tellg() != (int)v.size() )
            return std::vector<T>();
        out.push_back(t);
    }
    return out;
}

template<>
inline std::string ConfigParser::Item::convert<std::string>(unsigned int idx) const
{
    mRequested += 1;
    return mValues[idx];
}

template<>
inline std::string ConfigParser::Item::convert(unsigned int idx, const std::string& dflt) const
{
    mRequested += 1;
    return ( idx < mValues.size() ) ? mValues[idx] : dflt;
}

template<>
inline std::vector<std::string> ConfigParser::Item::convert<std::string>() const
{
    mRequested += 1;
    return mValues;
}

#endif // ConfigParser_H
