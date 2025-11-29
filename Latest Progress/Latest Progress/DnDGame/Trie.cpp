#include "Trie.hpp"

Trie::Trie():
	m_pRootNode(std::make_unique<TrieNode>())
{
	
}

void Trie::Insert(const std::string& str)
{
	// Iterate over TRIE tree
	TrieNode* pCurrentNode = m_pRootNode.get();
	for (int i = 0; i < str.size(); ++i)
	{
		// If char not found, create next branch
		if (pCurrentNode->m_pNextNode.find(str[i]) == pCurrentNode->m_pNextNode.end())
		{
			pCurrentNode->m_pNextNode[str[i]] = std::make_unique<TrieNode>(str[i], false);
		}
		
		pCurrentNode = pCurrentNode->m_pNextNode.at(str[i]).get(); // Move on to next branch
	}

	pCurrentNode->m_bIsEndOfWord = true;
}


std::string Trie::Search(const std::string& str)
{
	// Iterate over TRIE tree
	TrieNode* pCurrentNode = m_pRootNode.get();
	for (int i = 0; i < str.size(); ++i)
	{
		// If char not found, return false
		if (pCurrentNode->m_pNextNode.find(str[i]) == pCurrentNode->m_pNextNode.end())
		{
			return str.substr(0, i); // return substring(prefix) and let the user know if thats a complete word
		}

		pCurrentNode = pCurrentNode->m_pNextNode.at(str[i]).get(); // Move on to next branch
	}

	return str; // String exist
}


