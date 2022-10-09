#include "AttributeTranslator.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

bool AttributeTranslator::Load(std::string filename) {
	std::ifstream translator(filename);
	if (!translator)
		return false;

	// initialize variables
	std::string current;
	std::string sourceAttribute;
	std::string sourceValue;
	std::string compatibleAttribute;
	std::string compatibleValue;
	while (getline(translator, current)) {
		if (current != "") { // skip empty lines
			std::istringstream iss(current); // use istringstream to get our four components per line
			getline(iss, sourceAttribute, ',');
			getline(iss, sourceValue, ',');
			getline(iss, compatibleAttribute, ',');
			getline(iss, compatibleValue);

			std::string key = sourceAttribute + sourceValue; // create a key that concatenates our source pair's strings
			AttValPair av(compatibleAttribute, compatibleValue); 
			std::vector<AttValPair>* pair = m_pairs.search(key); // this is O(K) where K is the length of key, we can consider it as a constant
			if (pair == nullptr) { // if we do not have a corresponding vector for the source pair in our RadixTree, make a new one
				std::vector<AttValPair> newPair; 
				newPair.push_back(av); // this is O(1)
				m_pairs.insert(key, newPair); // insert our new vector into our RadixTree
			}
			else { // if we do have a corresponding vector for the source pair in our RadixTree...
				bool contains = false;
				std::vector<AttValPair>::iterator it = (*pair).begin();

				// go through our entire vector of compatible pairs to see if there is a duplicate 
				// this is still O(constant), because we can treat the average number of compatible pairs as a constant
				while (it != (*pair).end()) { 
					if (*it == av) {
						contains = true;
						break;
					}
					it++;
				}

				if(!contains)
					(*pair).push_back(av); // if there is no duplicate, we can add it to our vector
			}
		}
	}
	return true;
}

std::vector<AttValPair> AttributeTranslator::FindCompatibleAttValPairs(const AttValPair& source) const {
	std::string key = source.attribute + source.value; // concatenate source to get our key again
	std::vector<AttValPair>* pair = m_pairs.search(key); 
	if (pair != nullptr) // return a corresponding vector if it exists
		return *pair;

	std::vector<AttValPair> empty;
	return empty; // otherwise, make an empty vector and return
}