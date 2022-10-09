#include "PersonProfile.h"
#include <iostream>

void PersonProfile::AddAttValPair(const AttValPair& attval) {
	std::string key = attval.attribute;
	std::string value = attval.value;
	std::unordered_set<std::string>* attribute = m_pairs.search(key); // see if RadixTree contains key already
	if (attribute != nullptr) { // if RadixTree contains our key
		if ((*attribute).find(value) != (*attribute).end()) { // if we already have a duplicate, do nothing. also, Big O of find here should average O(1)
			return;
		}
		else { // otherwise, insert our new value into the existing attribute's values
			(*attribute).insert(value); // big O here will be O(1) average
		}
	}
	else { // if RadixTree does not contain our key, we want to create a new attribute-value unordered_set that corresponds to this key
		std::unordered_set<std::string> newValue;
		newValue.insert(value); // big O of inserting here should be O(1) average
		m_pairs.insert(key, newValue); // big O of inserting here should be O(K) where K = key.length()
	}
	references.push_back(attval); // finally, we want to index it in our vector as well so we can easily return any index number. this is O(1)
}

bool PersonProfile::GetAttVal(int attribute_num, AttValPair& attval) const {
	if (attribute_num < 0 || attribute_num >= GetNumAttValPairs()) { // if index out of bounds, return false
		return false;
	}
	attval = references[attribute_num]; // otherwise, we can just return the index in our vector
	return true;
}