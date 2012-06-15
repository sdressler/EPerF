#ifndef IJSONSERIALIZABLE_H
#define IJSONSERIALIZABLE_H

#include <string>

namespace ENHANCE {
class IJSONSerializable {
public:
	virtual std::string serializeToJSONString() const = 0;
};
}

#endif /* IJSONSERIALIZABLE_H */
