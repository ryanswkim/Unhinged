#include "MemberDatabase.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

bool MemberDatabase::LoadDatabase(std::string filename) {
	std::ifstream members(filename);
	if (!members)
		return false;

	std::string name;
	std::string email;
	std::string stringCount;
	std::string pair;
	std::string attribute;
	std::string value;
	for (;;) {
		if (!getline(members, name)) // set name
			break;
		if (name == "")
			continue;

		// read all of the lines and set our variables
		getline(members, email);
		getline(members, stringCount);
		int count = std::stoi(stringCount);
		
		// check for duplicate email, return false if so
		if (emailToProfile.search(email) != nullptr)
			return false;

		PersonProfile* p = new PersonProfile(name, email);

		for (int i = 0; i < count; i++) { // go through each attribute value pair and add it to our PersonProfile
			getline(members, pair);
			std::istringstream iss(pair);
			getline(iss, attribute, ',');
			getline(iss, value);
			AttValPair av(attribute, value);
			p->AddAttValPair(av); // PersonProfile.cpp already checks that this does not insert duplicates

			std::string key = attribute + value;
			std::vector<std::string>* pairEmails = pairToEmails.search(key);
			if (pairEmails != nullptr) { // if we already have a vector for our current attribute, then just add our email to this existing vector
				// this already accounts for duplicates, because we will return false above if we find a duplicate email
				(*pairEmails).push_back(email);
			}
			else { // otherwise, make a new vector with our single corresponding email
				std::vector<std::string> pairEmail;
				pairEmail.push_back(email);
				pairToEmails.insert(key, pairEmail);
			}
		}
		emailToProfile.insert(email, p); // insert our PersonProfile into our RadixTree (duplicates are checked for above)
		dynamicAllocations.push_back(p); // insert a reference into our dynamicAllocations vector as well to delete later
	}

	return true;
}

std::vector<std::string> MemberDatabase::FindMatchingMembers(const AttValPair& input) const {
	std::vector<std::string> empty; // default return

	std::string key = input.attribute + input.value;
	std::vector<std::string>* pairEmails = pairToEmails.search(key); // see if our radixtree contains the corresponding key and return if so
	if (pairEmails != nullptr)
		return *pairEmails;

	return empty;
}

const PersonProfile* MemberDatabase::GetMemberByEmail(std::string email) const {
	PersonProfile** profilePointer = emailToProfile.search(email); // our radixtree.search will return a pointer to a pointer
	if (profilePointer != nullptr)
		return *profilePointer;

	return nullptr;
}

MemberDatabase::~MemberDatabase() { // delete our pointers using our auxiliary vector
	for (int i = 0; i < dynamicAllocations.size(); i++) {
		delete dynamicAllocations[i];
	}
}