/****************************************************************
**
** Definition of Station Selection class
**
****************************************************************/

#ifndef _PARSE_PARVAL_FILE_
#define _PARSE_PARVAL_FILE_

#include <vector>
#include <list>
#include <map>
#include <milog/milog.h>



class ParseParValFile
{
public:

    ParseParValFile(std::string filename);
    ~ParseParValFile(){};
    std::map<int, float> ReturnMap();

protected:

private:
    std::map<int, float> ParValMap;

};


#endif
