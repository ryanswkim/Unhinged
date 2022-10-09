#include "MatchMaker.h"
#include "RadixTree.h"
#include "provided.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_set>
#include <set>
#include <vector>
#include <unordered_map>

struct comp {
	bool operator ()(const EmailCount& lhs, const EmailCount& rhs) const {
		if (lhs.count == rhs.count)
			return lhs.email < rhs.email;
		return lhs.count > rhs.count;
	}
};

std::vector<EmailCount> MatchMaker::IdentifyRankedMatches(std::string email, int threshold) const {
	if (threshold <= 0) // negative or zero threshold should only return members with 1 or more compatible pairs
		threshold = 1; // i don't include 0 matches to save space and time

	std::vector<EmailCount> toReturn;
	const PersonProfile* p = m_mdb.GetMemberByEmail(email);
	std::unordered_set<std::string> allCompatiblePairs;
	for (int k = 0; k != p->GetNumAttValPairs(); k++) { // go through each attvalpair in personprofile
		AttValPair av;
		p->GetAttVal(k, av);
		std::vector<AttValPair> compatibles = m_at.FindCompatibleAttValPairs(av); // for each attvalpair, get all compatible attvalpairs
		std::vector<AttValPair>::iterator it = compatibles.begin();
		while (it != compatibles.end()) {
			allCompatiblePairs.insert(it->attribute + "," + it->value); // accumulate our unordered_set of unique compatible pairs. we use an unordered_set to remove duplicates
			it++;
		}
	}

	// make an ordered set to store our emailcounts
	// I decide to make a sorted set before making a vector because vector.sort takes O(NlogN), while
	// set.insert takes O(logN) and then O(N) to move everything to the vector. Basicaly, I'm sacrificing
	// storage to get overall O(N) in this step as opposed to O(NlogN)
	std::set<EmailCount, comp> sortedEC;
	std::unordered_map<std::string, int> unsortedEC;

	// iterate through our unordered set
	std::unordered_set<std::string>::iterator it = allCompatiblePairs.begin();
	std::string attribute;
	std::string value;
	while (it != allCompatiblePairs.end()) {
		std::istringstream iss(*it);
		getline(iss, attribute, ',');
		getline(iss, value);
		AttValPair av(attribute, value); // create an attvalpair and get the emails of all members with said attribute
		std::vector<std::string> emails = m_mdb.FindMatchingMembers(av);
		std::vector<std::string>::iterator it2 = emails.begin();
		while (it2 != emails.end()) { // go through each email
			std::string curEmail = *it2;
			std::unordered_map<std::string, int>::iterator it3 = unsortedEC.find(curEmail);
			if (it3 != unsortedEC.end())  // if the map already contains the key
				(*it3).second++; // increment the count
			else {
				unsortedEC.insert({ curEmail, 1 }); // otherwise, insert a new pair
			}
			it2++;
		}
		it++;
	}
	
	std::unordered_map<std::string, int>::iterator i = unsortedEC.begin();
	while (i != unsortedEC.end()) {
		if ((*i).second >= threshold) {
			EmailCount e((*i).first, (*i).second);
			sortedEC.insert(e);
		}
		i++;
	}


	std::set<EmailCount>::iterator j = sortedEC.begin();
	while (j != sortedEC.end()) {
		toReturn.push_back(*j);
		j++;
	}

	return toReturn;
}