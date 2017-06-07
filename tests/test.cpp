#include "../include/NestedContainer.hpp"

#include <iostream>
#include <vector>

// compile with:
// clang++-3.8 -std=c++14 -O2 test.cpp -o test


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

	typedef std::map<std::size_t, double>		inner_map;
	typedef treefidi::NestedMappedContainer<std::size_t, inner_map>		mapmap;
	typedef treefidi::NestedMappedContainer<std::size_t, mapmap> 		mapmapmap;
	mapmap m;
	mapmapmap mm;
	m[0][0]=1;
	m[0][0]=2;
	m[1][0]=3;
	m[2][0]=4;
	m[1][1]=5;

	int ct=0;
	for (mapmap::iterator it=m.begin(); it!=m.end(); it++){
		ct++;
	}
	std::cout << "count: " << ct << std::endl;


	// throw -1;



	typedef typename inner_map::iterator  				imit;
	typedef typename mapmap::nested_iterator<imit> 		mnit;
	typedef typename mapmapmap::nested_iterator<mnit> 	wholeiterator;
	
	
	ct=0;
	for (mnit it=m.begin(); it!=m.end(); it++){
		ct++;
		if (ct > 8) throw -1;
	}
	std::cout << "count: " << ct << std::endl;

	return 0;
}