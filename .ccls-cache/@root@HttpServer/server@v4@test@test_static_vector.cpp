/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-13 13:21
#
# Filename: test_static_vector.cpp
#
# Description: 
#
=============================================================================*/
#include <vector>
#include <iostream>

struct Item
{
	int i = 1;
};

struct Demo
{
	std::vector<Item> items;
	
	Demo() 
	{
		static Item item;
		items.push_back(item);
	}

	std::vector<Item>& getItems() {
		return items;
	}
};

int main() 
{
	Demo d;
	std::vector<Item> items = d.getItems();

	std::cout << items.at(0).i << std::endl;

	return 0;

}
