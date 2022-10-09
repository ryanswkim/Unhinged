#ifndef _PERSONPROFILE_H_
#define _PERSONPROFILE_H_

#include <string>
#include <vector>
#include "provided.h"
#include "RadixTree.h"
#include <unordered_set>

class PersonProfile {
public:
	PersonProfile(std::string name, std::string email) : m_name(name), m_email(email) {}
	std::string GetName() const { return m_name; }
	std::string GetEmail() const { return m_email; }
	void AddAttValPair(const AttValPair& attval);
	int GetNumAttValPairs() const { return (int)references.size(); }
	bool GetAttVal(int attribute_num, AttValPair& attval) const;
private:
	std::string m_name;
	std::string m_email;
	std::vector<AttValPair> references;
	RadixTree<std::unordered_set<std::string>> m_pairs;
};

#endif