#ifndef MEMBERDATABASE_H_
#define MEMBERDATABASE_H_

#include <string>
#include <vector>
#include <unordered_set>
#include "provided.h"
#include "PersonProfile.h"
#include "RadixTree.h"

class MemberDatabase {
public:
	MemberDatabase() {};
	~MemberDatabase();
	bool LoadDatabase(std::string filename);
	std::vector<std::string> FindMatchingMembers(const AttValPair& input) const;
	const PersonProfile* GetMemberByEmail(std::string email) const;
private:
	RadixTree<std::vector<std::string>> pairToEmails;
	RadixTree<PersonProfile*> emailToProfile;
	std::vector<PersonProfile*> dynamicAllocations; // this is purely to delete each PersonProfile in the destructor
};

#endif