#include <treefidi.h>

#include <iostream>
#include <vector>

// compile with:
// clang++-3.8 -std=c++14 -I../ -O2 test.cpp -o test



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

	typedef std::map<std::size_t, double>	inner_map;
	typedef treefidi::NestedContainer<std::size_t, inner_map>		mapmap;
	typedef treefidi::NestedContainer<std::size_t, mapmap> 		mapmapmap;
	mapmap m;
	mapmapmap mm;
	
	m[1][0]=3;
	m[1][1]=5;
	m[2][2]=4;
	m[3][3]=1;
	m[3][4]=2;


	auto it = m.find(4);
	std::cout << "found key 4: " << it->first << " --> " << it->second << std::endl;

	it = m.find(4, 3);
	std::cout << "found key 4 with hint: " << it->first << " --> " << it->second << std::endl;


	mm[1][0][0]=9;
	mm[1][0][1]=4;
	mm[1][1][0]=8;
	mm[1][1][1]=3;

	mm[2][0][0]=7;
	mm[2][0][1]=2;

	mm[3][0][0]=6;
	mm[3][0][1]=1;
	mm[3][1][0]=5;
	mm[3][1][1]=0;


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




	ct=0;
	for (auto it=mm.begin(); it!=mm.end(); it++){
		std::cout << it->second << std::endl;
		ct++;
	}
	std::cout << "total count: " << ct << std::endl;





	ct=0;
	for (auto it=mm.begin(2); it!=mm.end(2); it++){
		std::cout << it->second << std::endl;
		ct++;
	}
	std::cout << "total count: " << ct << std::endl;

	return 0;
}