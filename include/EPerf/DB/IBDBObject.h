#ifndef IBDBOBJECT_H
#define IBDBOBJECT_H

#include <db_cxx.h>
#include <map>

namespace ENHANCE {

typedef std::map<std::string, Dbt> tBDBObjectMap;

class IBDBObject {

public:
	virtual std::vector<char> convertToByteVector() const = 0;
//i	virtual tBDBObjectMap convertToBDBObjectMap(std::string prefix = std::string()) = 0;

};
}

#endif /* IBDBOBJECT_H */
