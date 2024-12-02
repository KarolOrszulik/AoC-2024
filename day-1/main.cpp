#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <cmath>

struct Lists
{
	std::vector<int> l1, l2;
};

Lists loadLists(const char* path)
{
	std::ifstream file(path);

	if(!file)
	{
		std::cerr << "Could not open file" << std::endl;
		return {};
	}

	Lists lists;
	int el1, el2;

	while (file >> el1 >> el2)
	{
		lists.l1.push_back(el1);
		lists.l2.push_back(el2);
	}

	std::sort(lists.l1.begin(), lists.l1.end());
	std::sort(lists.l2.begin(), lists.l2.end());

	return lists;
}

int main()
{
	Lists lists = loadLists("input.txt");
	
	int sum = 0;

	for (int i = 0; i < lists.l1.size(); i++)
	{
		sum += std::abs(lists.l1[i] - lists.l2[i]);
	}

	std::cout << sum << std::endl;	

}
