#ifndef ATTRIBUTETRANSLATOR_H_
#define ATTRIBUTETRANSLATOR_H_

#include <string>
#include <vector>
#include "provided.h"
#include "RadixTree.h"

class AttributeTranslator {
public:
	AttributeTranslator() {}
	bool Load(std::string filename);
	std::vector<AttValPair> FindCompatibleAttValPairs(const AttValPair& source) const;
private:
	RadixTree<std::vector<AttValPair>> m_pairs;
};

#endif