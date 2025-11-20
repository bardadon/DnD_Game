#ifndef __COMMAND_TRIE_HPP__
#define __COMMAND_TRIE_HPP__

#include <unordered_map>
#include <memory>
#include <string>

/*
This class will be used to store and search all commands available to the player.
If a player writes a command: "Look"

we'll traverse the tree and search for a match.
When the system will launch(or load from disk) we will build this tree in the DnDGame.cpp file
with all the commands currently available to the player.

After verifying that a string exist, we have 100% certainty that the command the user wrote exist
and we can activate the dispatcher to launch the appropriate function.

This is also beneficial for screening commands such as: "Look at Table"
where "Table" can be anything instead.
Such format is not really suitable for a dispacher.

Having said that, the Trie will be a generic data structure for searching strings. It wont have
anything directly to do with command functions.

*/





//Each node in the Trie tree, has a char and a map which connects chars to the next nodes
//For example, Node1 value is "c" and by inserting "a" to its map we get the pointer to Node2
//	Node1("c")
//	/
//	a
//   /
//Node2("a")

struct TrieNode
{
	TrieNode(char val = ' ', bool bIsEndOfWord = false) :
		m_val(val), m_bIsEndOfWord(bIsEndOfWord){ }

	char m_val;
	std::unordered_map<char, std::unique_ptr<TrieNode>> m_pNextNode;
	bool m_bIsEndOfWord;
}; // TrieNode


class Trie
{
public:
	explicit Trie();
	void Insert(const std::string& str);
	bool Search(const std::string& str);

private:
	std::unique_ptr<TrieNode> m_pRootNode;
}; // Trie


 
#endif // !__COMMAND_TRIE_HPP__
