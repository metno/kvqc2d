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

private:
    std::vector<std::string> mErrors;
};

// ########################################################################

class ConfigParser {
public:
    class Item {
        friend class ConfigParser;

    public:
        int count() const
            { return mValues.size(); }

        int requestCount() const
            { return mRequested; }

        const std::string& value(unsigned int idx) const;

        const std::string& value(unsigned int idx, const std::string& dflt) const;

        const std::vector<std::string>& values() const
            { return mValues; }

        template<class T>
        T convert(unsigned int idx) const;

        template<class T>
        T convert(unsigned int idx, const T& dflt) const;

        template<class T>
        std::vector<T> convert() const;

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

private:
    typedef std::map<std::string, Item> mItems_t;
    mItems_t mItems;
    ErrorList mErrors;
};

// ########################################################################

template<class T>
T ConfigParser::Item::convert(unsigned int idx) const
{
    T t;
    std::istringstream i(mValues[idx]);
    i >> t;
    return t;
}

template<class T>
T ConfigParser::Item::convert(unsigned int idx, const T& dflt) const
{
    if( idx < mValues.size() ) {
        T t;
        const std::string v = mValues[idx];
        std::istringstream i(v);
        i >> t;
        if( !i.fail() && i.tellg() == v.size() )
            return t;
    }
    return dflt;
}

template<class T>
std::vector<T> ConfigParser::Item::convert() const
{
    std::vector<T> out;
    for(unsigned int i=0; i<mValues.size(); ++i ) {
        T t;
        const std::string v = mValues[i];
        std::istringstream i(v);
        i >> t;
        if( i.fail() || i.tellg() != v.size() )
            return std::vector<T>();
        out.push_back(t);
    }
    return out;
}

#endif // ConfigParser_H
