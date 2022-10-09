#ifndef RADIXTREE_H
#define RADIXTREE_H

#include <iostream>

template <typename ValueType>
class RadixTree {
public:
	RadixTree() {
		root = new Node;	
	}
	
	~RadixTree() {
		free(root);
	}

	void insert(std::string key, const ValueType& value) {
		if (key == "")
			return;

		ValueType copy = value; // make a copy of value, as specified by the spec
		int index = 0; // index to traverse our key
		Node* last = root; // eventually points to the last node that matches the most characters with key

		// we now want to traverse all nodes to find last
		for (;;) { // for each relevant node
			std::string cur = (last->word); // get our current node's word
			std::string overlap = ""; // string that represents what characters overlap
			std::string newWord = ""; // placeholder string for when we want to find the tail of a string

			// go through our current node's word to see if any characters match with key
			int i = 0;
			
			for (; i < cur.length(); i++) {
				if (index >= key.length() || cur[i] != key[index]) { // if we reached the end of our key or if cur does not match, break
					break;
				}
				else
					overlap += key[index++];
			}

			if (index >= key.length()) { 
				if (i >= cur.length()) { // we have a duplicate
					last->complete = true; 
					last->value = copy; // update our original value
				}
				else { // cur contains key (e.g. cur == "ballistic" and key == "ball")
					// we want to turn last into an intermediate node whose word == "ball"
					int charToInt = cur[i];
					
					i++;
					newWord = "";
					for (; i < cur.length(); i++) {
						newWord += cur[i];
					}

					// create a new node to replace last
					Node* n = new Node;
					n->word = newWord;
					n->complete = last->complete;
					n->value = last->value;

					for (int j = 0; j < 128; j++) { // this makes insertion O(128K) which is still O(K)
						n->children[j] = last->children[j]; // this new node is now the new parent of all of last's children
						last->children[j] = nullptr;
					}

					// last now becomes an intermediate node
					last->word = overlap;
					last->complete = true;
					last->value = copy;
					last->children[charToInt] = n; // last's only child will now be this new node

				}
				return;
			}


			int charToInt = key[index]; 
			index++;

			if (last->children[charToInt] == nullptr) { // no more branches left to traverse, meaning we create a new leaf node													
				// get the remaining letters of key to define the word of our new node
				newWord = "";
				for (; index < key.length(); index++) { // append all remaining characters in key to our word
					newWord += key[index];
				}

				// create our leaf node
				Node* n1 = new Node;
				n1->complete = true;
				n1->value = copy;
				n1->word = newWord;

				if (i < cur.length()) { // we must make another intermediate node that corresponds to our original word
					int charToInt2 = cur[i]; // charToInt for cur
					i++;
					newWord = ""; // reset new word
					for (; i < cur.length(); i++) { // append all remaining characters in cur to our word
						newWord += cur[i];
					}

					// create our second node
					Node* n2 = new Node;
					n2->word = newWord;
					n2->complete = last->complete;
					n2->value = last->value;

					// we want the intermediate node's children to only contain n1 and n2 (and n2 now contains all of the original children)
					for (int j = 0; j < 128; j++) { // this makes insertion O(128K) which is still O(K)
						n2->children[j] = last->children[j];
						last->children[j] = nullptr;
					}

					// reset our current node to become an incomplete node (that branches out into the two new complete nodes)
					last->word = overlap;
					last->complete = false;
					last->value = root->value; // let us call root->value as the default value;
					last->children[charToInt2] = n2;
				}

				last->children[charToInt] = n1; // n1 is always  added as a child to last's children
				return;
			}
			else { // there is a next branch to traverse
				if (i < cur.length()) { // more to compare with current word
				// this means we want to create an intermediate node and split off here
					newWord = "";
					for (; index < key.length(); index++) {
						newWord += key[index];
					}

					// create these new leaf nodes 
					Node* n1 = new Node;
					n1->word = newWord;
					n1->complete = true;
					n1->value = copy;

					// create an intermediate leaf node that parents our new node and the original node that contained cur
					int charToInt2 = cur[i];
					i++;
					newWord = "";
					for (; i < cur.length(); i++) {
						newWord += cur[i];
					}

					Node* n2 = new Node;
					n2->word = newWord;
					n2->complete = last->complete;
					n2->value = last->value;

					// we want the intermediate node's children to only contain n1 and n2 (and n2 now contains all of the original children)
					for (int j = 0; j < 128; j++) { // this makes insertion O(128K) which is still O(K)
						n2->children[j] = last->children[j];
						last->children[j] = nullptr;
					}

					last->word = overlap;
					last->complete = false;
					last->value = root->value; // let us call root->value as the default value;
					last->children[charToInt2] = n2;
					last->children[charToInt] = n1;

					return;
				}

				last = last->children[charToInt]; // simply set our traversing pointer to its child to continue traversing
			}
		}
	}

	ValueType* search(std::string key) const {
		int index = 0;
		Node* last = root;
		for (;;) { // traverse through our key
			std::string cur = last->word; // for each node we go through, key must contain its word
			for (int i = 0; i < cur.length(); i++) { // we need to go through the entire word (otherwise, the tree can never contain key)
				if (index >= key.length() || cur[i] != key[index]) { // if key does not extend for our entire word or there is a mismatch, return nullptr
					return nullptr;
				}
				index++; // otherwise, keep traversing
			}

			// once we reach the end of our current word, see if our key also reached its end. return it if it does
			if (index == key.length()) {
				if (last->complete) {
					ValueType* value = &(last->value);
					return value;
				}
				else
					return nullptr;
			}

			// otherwise, move on to the next node if it exists
			int charToInt = key[index++];
			if (last->children[charToInt] == nullptr) {
				return nullptr; // if no continued node exists, then our tree does not contain key
			}
			last = last->children[charToInt];
		}
	}

private:
	struct Node {
		std::string word = "";
		bool complete = false;
		Node* children[128] = { nullptr };
		ValueType value;
	};
	Node* root;
	void free(Node* &p) {
		for (int i = 0; i < 128; i++) { // go through each child and recursively call free on them
			if (p->children[i] != nullptr) {
				free(p->children[i]);
			}
		}
		
		// delete whatever we point to and become a nullptr
		delete p;
		p = nullptr;
	}
};

#endif