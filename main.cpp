#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

bool dfs(unsigned u, std::vector<std::vector<bool>>& matrix, std::vector<unsigned>& color)
// Returns `true` iff no cycle has been found
{
	// Mark as `grey`
	color[u] = 1;
	
	// Visit all neighbours
	for (unsigned index = 0, limit = matrix.size(); index != limit; ++index) {
		// No edge?
		if (!matrix[u][index]) continue;
		
		// Subtree already considered?
		if (color[index] == 2) continue;
		
		// Already `grey`, i.e. cycle?
		if (color[index] == 1)
			return false;
		
		// Cycle in our subtree?
		if (!dfs(index, matrix, color))
			return false;
	}
	
	// Mark as `black`
	color[u] = 2;
	return true;
}

int main(int argc, char** argv) {
	// Check usage
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <history file>" << std::endl;
		exit(-1);
	}
	
	// Check input file
	std::ifstream input(argv[1]);
	if (!input.is_open()) {
		std::cerr << "Couln't open '" << argv[1] << "'!" << std::endl;
		exit(-1);
	}
	
	// Read input file
	std::map<unsigned, std::vector<std::pair<std::pair<bool, std::string>, unsigned>>> transactions;
	std::string a, b, c;
	unsigned index = 0;
	while (input >> a >> b >> c) {
		assert(a.size() == 1);
		assert(b.size() == 1);
		bool type = (a[0] == 'w');
		unsigned id = (b[0] - '0');
		
		// Append the operation to its transaction
		transactions[id].push_back({{type, c}, index++});
	}
	
#if 0
	const auto debug = [&](std::pair<std::pair<bool, std::string>, unsigned> pair) -> std::string {
		return "((" + std::to_string(pair.first.first) + ", " + pair.first.second + "), " + std::to_string(pair.second) + ")";
	};

	for (auto it: transactions) {
		std::cerr << "key: " << it.first << std::endl;
		for (auto elem: it.second) {
			std::cerr << "elem=" << debug(elem) << std::endl;
		}
		std::cerr << std::endl;
	}
#endif

	// Create the adjacency matrix
	std::vector<std::vector<bool>> matrix(1 + transactions.size(), std::vector<bool>(1 + transactions.size(), false));
	for (auto it1: transactions) {
		for (auto it2: transactions) {
			// Same transaction?
			if (it1.first == it2.first)
				continue;
			
			// Find conflicting operations
			for (auto elem1: it1.second) {
				for (auto elem2: it2.second) {
					// Conflicting operation?
					if ((elem1.first.first || elem2.first.first) && (elem1.first.second == elem2.first.second)) {
						// Decide the orientation of the edge
						if (elem1.second < elem2.second)
							matrix[it1.first][it2.first] = true;
						else
							matrix[it2.first][it1.first] = true;
					}
				}
			}
		}
	}
	
	// Analyze each connected component
	const auto analyzeConnexComponents = [&]() -> bool {
		// Mark all nodes as `white`
		std::vector<unsigned> color(1 + transactions.size(), 0);
		for (auto it1: transactions)
			if (!color[it1.first])
				if (!dfs(it1.first, matrix, color))
					return false;
		return true;
	};
	
	// And return the result
	std::cout << (analyzeConnexComponents() ? "true" : "false") << std::endl;
	return 0;
}
