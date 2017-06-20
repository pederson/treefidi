#include <treefidi.h>

#include <iostream>
#include <vector>

// compile with:
// clang++-3.8 -std=c++14 -I../ -O2 test.cpp -o test





enum class Key1 : int 
{
	one=1,
	two,
	three,
	four,
	five
};

enum class Key2 : int 
{
	one=1,
	two,
	three,
	four,
	five
};

enum class Key3 : int 
{
	one=1,
	two,
	three,
	four,
	five
};


int main(int argc, char * argv[]){

	std::cout << "hello" << std::endl;

	// typedef treefidi::NestedContainer<std::vector<double>> 		vec2d;
	// vec2d		myvec;
	// myvec.resize(5);
	// for (auto it=myvec.begin(); it!=myvec.end(); it++){
	// 	it->resize(6);
	// }


	// int ct=0;
	// typedef vec2d::nested_iterator<typename std::vector<double>::iterator> vnested_iterator;
	// for (vnested_iterator it = myvec.begin(); it!=myvec.end(); it++){
	// 	ct++;
	// }
	// std::cout << "numelements: " << ct << std::endl;

	typedef std::map<int, double>	inner_map;
	typedef treefidi::NestedContainer<Key2, inner_map>		mapmap;
	typedef treefidi::NestedContainer<Key1, mapmap> 		mapmapmap;
	mapmap m;
	mapmapmap mm;
	
	m[Key2::one][10]=3;
	m[Key2::one][11]=5;
	m[Key2::one][12]=4;

	m[Key2::two][13]=3;
	m[Key2::two][14]=1;
	m[Key2::two][15]=2;


	auto it = m.find(14);
	std::cout << "found key 14 : " << int(it->first) << " --> " << it->second << std::endl;

	it = m.find(14, Key2::two);
	std::cout << "found 14 with hint Key2::two : " << int(it->first) << " --> " << it->second << std::endl;

	int ct=0;
	for (auto it=m.outer_begin(); it!=m.outer_end(); it++){
		ct++;
	}
	std::cout << "level count: " << ct << std::endl;

	
	ct=0;
	for (auto it=m.begin(); it!=m.end(); it++){
		std::cout << it->second << std::endl;
		ct++;
	}
	std::cout << "total count: " << ct << std::endl;
















	mm[Key1::one][Key2::one][20]=9;
	mm[Key1::one][Key2::one][21]=4;

	mm[Key1::one][Key2::two][22]=8;
	mm[Key1::one][Key2::two][23]=3;

////////////////////////////////////////////////////////

	mm[Key1::two][Key2::one][24]=7;
	mm[Key1::two][Key2::one][25]=2;

////////////////////////////////////////////////////////

	mm[Key1::three][Key2::one][26]=6;
	mm[Key1::three][Key2::one][27]=1;
	mm[Key1::three][Key2::one][28]=5;

	mm[Key1::three][Key2::two][29]=0;

///////////////////////////////////////////////////////

	// mm[Key1::four][Key2::two][30] = 1;
	// mm[Key1::four][Key2::two][31] = 2;

	// mm[Key1::four][Key2::three][32] = 1;

	auto suc = mm.insert(std::make_pair(30, 1), Key1::four, Key2::two);
	if (suc.second == false) std::cout << "insert was unsuccessful" << std::endl;
	suc = mm.insert(std::make_pair(31, 2), Key1::four, Key2::two);
	if (suc.second == false) std::cout << "insert was unsuccessful" << std::endl;
	suc = mm.insert(std::make_pair(32, 1), Key1::four, Key2::three);
	if (suc.second == false) std::cout << "insert was unsuccessful" << std::endl;

///////////////////////////////////////////////////////

	// auto itmm;
	auto itmm = mm.find(28);
	std::cout << "found key 28 with no hint: " << int(itmm->first) << " --> " << itmm->second << std::endl;

	itmm = mm.find(28, Key1::three);
	std::cout << "found key 28 with one hint: " << int(itmm->first) << " --> " << itmm->second << std::endl;

	itmm = mm.find(28, Key1::three, Key2::one);
	std::cout << "found key 28 with two hints: " << int(itmm->first) << " --> " << itmm->second << std::endl;


	// test the find for a failed key
	itmm = mm.find(35);
	if (itmm == mm.end()) std::cout << "didn't find key 35" << std::endl;

	itmm = mm.find(35, Key1::three);
	if (itmm == mm.end(Key1::three)) std::cout << "didn't find key 35" << std::endl;

	// std::cout << "\n\n\n";
	itmm = mm.find(35, Key1::three, Key2::two);
	if (itmm == mm.end(Key1::three, Key2::two)) std::cout << "didn't find key 35" << std::endl;
	// std::cout << "\n\n\n";

	// itmm = mm[Key1::three][Key2::one].find(35);
	// if (itmm == mm[Key1::three][Key2::one].end()) std::cout << "didn't find key 35" << std::endl;



	ct=0;
	std::cout << "in whole container...." << std::endl;
	for (auto it=mm.begin(); it!=mm.end(); it++){
		std::cout << it->second << std::endl;
		ct++;
	}
	std::cout << "total count: " << ct << std::endl;

	ct=0;
	std::cout << "in container[Key1::four]...." << std::endl;
	for (auto it=mm.begin(Key1::four); it!=mm.end(Key1::four); it++){
		std::cout << it->second << std::endl;
		ct++;
	}
	std::cout << "total count: " << ct << std::endl;


	ct=0;
	std::cout << "in container[Key1::four][Key2::two]...." << std::endl;
	for (auto it=mm.begin(Key1::four, Key2::two); it!=mm.end(Key1::four, Key2::two); it++){
		std::cout << it->second << std::endl;
		ct++;
	}
	std::cout << "total count: " << ct << std::endl;

	return 0;
}