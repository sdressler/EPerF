#ifndef IBDBOBJECT_H
#define IBDBOBJECT_H

#include <db_cxx.h>
#include <map>

namespace ENHANCE {

typedef std::map<std::string, Dbt> tBDBObjectMap;
typedef std::map<std::string, std::vector<char> > tByteVectorMap;

class IBDBObject {

protected:
    ~IBDBObject() {
        
    }

private:
	virtual tByteVectorMap convertToByteVectorMap() const = 0;

};
}

#endif /* IBDBOBJECT_H */
