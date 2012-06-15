#ifndef ISERIALIZER_H
#define ISERIALIZER_H

#include <string>

namespace ENHANCE {

template <typename T>
class ISerializer {
public:
	virtual void serialize(const T &o) = 0;
//	virtual T deserialize(const std::string &o) = 0;
};
}

#endif /* ISERIALIZER_H */
