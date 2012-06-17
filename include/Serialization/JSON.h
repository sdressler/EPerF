#ifndef JSON_H
#define JSON_H

#include <string>
#include <fstream>

#include "ISerializer.h"

namespace ENHANCE {
template <typename T>
class JSON : public ISerializer<T> {
private:
	std::string path;

public:
	JSON(std::string p) : path(p) {	}

	virtual void serialize(const T &o) {
		std::string jString = o.serializeToJSONString();

		std::ofstream f(path.c_str());

		f << jString;

		f.close();
	}
/*
	virtual T deserialize(const std::string &s) {

	}
*/
};
}

#endif /* JSON_H */
