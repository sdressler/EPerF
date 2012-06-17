#ifndef IKVSERIALIZABLE_H
#define IKVSERIALIZABLE_H

#include <string>

namespace ENHANCE {
class IKVSerializable {
public:
	virtual std::string serializeToKeyValue() const = 0;
};
}

#endif /* IKVSERIALIZABLE_H */
