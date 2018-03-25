#ifndef _MULTISETMAP_H
#define _MULTISETMAP_H


#include <unordered_map>
#include <vector>
#include <iostream>
#include <type_traits>
#include <algorithm>
#include <functional>

namespace treefidi{


// typedef std::map as defaultmap;
template <class K, class V>
struct DefUMapTypedef{
	typedef std::unordered_map<K, V> type;
};
template <class K, class V>
using DefUMap = typename DefUMapTypedef<K,V>::type;








// MultiSetMap is a mapped container type that organizes its elements into sets.
//* Each key-value pair could possibly be in multiple sets (i.e. the intersection
//* of two sets can be nonzero). 
//*
//* Ordinary iteration over all elements is available with the ::iterator type and
//* can be accessed using the begin() and end() functions. 
//* 
//* The unique aspect of this container is its ability to iterate over sets. 
//* Iteration over sets is available with the ::set_iterator type, and can be 
//* conveniently accessed using the begin(SetType s) and end(SetType s) functions
//*
//* Furthermore, iteration through the available SetType values can be done
//* via the ::set_enumerator type
//*
//***********************************************************/
template <class Key, 
		  class Value,
		  class Set,
		  template <class K, class V> class ContainerT = DefUMap >
struct MultiSetMap : public ContainerT<Key, Value>{
public:
	// class typedefs
	typedef Key 													KeyT;
	typedef Value 													ValueT;
	typedef Set 													SetT;
	typedef MultiSetMap<KeyT, ValueT, SetT, ContainerT>				SelfT;

	typedef ContainerT<KeyT, ValueT> 								base_container;
	typedef typename ContainerT<KeyT, ValueT>::iterator 			iterator;

	typedef std::unordered_multimap<KeyT, SetT> 					MultimapT;		// this manages the subdomains
	typedef std::unordered_map<KeyT, std::reference_wrapper<ValueT>>						SetContainerT;	// this is the set container
	typedef std::unordered_map<SetT, SetContainerT>					SetmapT;		// this is the map to set containers

protected:
	// member data
	MultimapT     				mMultiMap;
	SetmapT 					mSetMap;

public:


	// add an existing element to a set "s"
	void add_to_set(iterator & it, SetT s){
		mMultiMap.emplace(it->first, s);		// create entry in the multimap
		mSetMap[s].insert(std::make_pair(it->first, std::ref(it->second)));	// create entry in the set container
	}

	// remove an existing element from set "s"
	void remove_from_set(iterator & it, SetT s){
		// erase from the multimap
		auto mit = mMultiMap.find(it->first);
		while (mit->second != s && mit->first == it->first){
			mit++;
		}
		if (mit->second == s && mit->first == it->first){
			mMultiMap.erase(mit);
			// erase from set container
			auto sit = mSetMap[s].find(it->first);
			mSetMap[s].erase(sit);
			// remove the set if it is now empty
			if (mSetMap[s].empty()){
				auto rit = mSetMap.find(s);
				mSetMap.erase(rit);
			}
		}
	}



	// basic iterator over the set types
	class set_enumerator{
	public:
		typedef set_enumerator 									self_type;
		typedef typename SetmapT::iterator::difference_type		difference_type;
	    typedef const SetT 										value_type;
	    typedef value_type &									reference;
	    typedef value_type *									pointer;
	    typedef typename SetmapT::iterator::iterator_category 	iterator_category;


		// construction
		set_enumerator(const typename SetmapT::iterator & bit)
		: it(bit){};


		// copy construction
		set_enumerator(const set_enumerator & itr): it(itr.it) {};

		// copy assignment
		set_enumerator & operator=(const set_enumerator & itr){
			set_enumerator i(itr);
			std::swap(i.it, it);
			return *this;
		}

		// dereferencing
		reference operator*(){ return it->first;};

		// preincrement 
		self_type operator++(){
			it++;	
			return *this;
		};

		// postincrement 
		self_type operator++(int blah){
			it++;
			return *this;
		};

		// pointer
		pointer operator->() {return &it->first;};

		// inequality
		bool operator!=(const self_type & leaf) const {return it != leaf.it;};

		// equality
		bool operator==(const self_type & leaf) const {return it == leaf.it;};

	private:
		typename SetmapT::iterator 		it;
	};


	set_enumerator set_enumerator_begin(){return set_enumerator(mSetMap.begin());};
	set_enumerator set_enumerator_end(){return set_enumerator(mSetMap.end());};




	// classic iterator over the base container
	iterator begin(){
		return base_container::begin();
	}

	iterator end(){
		return base_container::end();
	}


	// iterator over a single set
	class set_iterator{
	public:
		typedef set_iterator 										self_type;
		typedef typename SetContainerT::iterator::difference_type 	difference_type;
	    typedef typename SetContainerT::iterator::value_type		value_type;
	    typedef typename SetContainerT::iterator::reference			reference;
	    typedef typename SetContainerT::iterator::pointer			pointer;
	    typedef typename SetContainerT::iterator::iterator_category iterator_category;

	    friend class MultiSetMap;

		// construction from a subset_iterator
		set_iterator(MultiSetMap & c, const typename SetContainerT::iterator & sit, SetT s)
		: cont(&c)
		, it(sit)
		, st(s) {};

		// copy construction
		set_iterator(const set_iterator & bit)
		: cont(bit.cont)
		, it(bit.it)
		, st(bit.st) {};

		// copy assignment
		set_iterator & operator=(const set_iterator & bit)
		{
			set_iterator nit(bit);
			cont = nit.cont;
			st = nit.st;
			std::swap(nit.it, it);
			return *this;
		};

		// destructor
		~set_iterator(){};

		// move constructor
		set_iterator(set_iterator && bit) = default;

		// move assigment
		set_iterator & operator=(set_iterator && bit) = default;

		// dereferencing
		reference operator*(){
			return it.operator*();
		};

		// preincrement 
		self_type & operator++(){
			it++;
			return *this;
		}

		// postincrement 
		self_type operator++(int blah){
			it++;
			return *this;
		}

		// pointer
		pointer operator->() {
			return it.operator->();
		};

		// inequality
		bool operator!=(const self_type & leaf) const {return it != leaf.it;};

		// equality
		bool operator==(const self_type & leaf) const {return it == leaf.it;};

		SetT set() {return st;};
	private:
		typename SetContainerT::iterator 		it;
		MultiSetMap * 							cont;
		SetT 									st;
	};

	set_iterator begin(SetT s){
		return set_iterator(*this, mSetMap[s].begin(), s);
	}

	set_iterator end(SetT s){
		return set_iterator(*this, mSetMap[s].end(), s);
	}

	// operator[] with KeyT as argument
	ValueT & operator[](KeyT key) {return base_container::operator[](key);};

	// operator[] with SetT as argument
	SetContainerT & operator[](SetT s){return mSetMap[s];};

	// find with no set specified
	iterator find(KeyT key) {return base_container::find(key);};

	// find with the set specified
	set_iterator find(KeyT key, SetT s) {return set_iterator(*this, mSetMap[s].find(key), s);};

	// insert

	// erase by iterator


};












template <class NContainer, class SetT, class UnaryFunction>
void for_each_set(NContainer & c, std::vector<SetT> & sets, UnaryFunction f){
	for (auto it = sets.begin(); it!=sets.end(); it++){
		std::for_each(c[*it].begin(), c[*it].end(), f);
	}
}


template <class NContainer, class SetT, class UnaryFunction>
void for_each_set(NContainer & c, std::vector<SetT> & sets, std::map<SetT, UnaryFunction> & fmap){
	for (auto it = sets.begin(); it!=sets.end(); it++){
		std::for_each(c[*it].begin(), c[*it].end(), fmap.at(*it));
	}
}



}// end namespace treefidi

#endif