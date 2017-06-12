#ifndef _NESTEDCONTAINER_H
#define _NESTEDCONTAINER_H


#include <map>
#include <vector>
#include <iostream>
#include <type_traits>
#include <algorithm>


namespace treefidi{


// typedef std::map as defaultmap;
template <class K, class V>
struct DefMapTypedef{
	typedef std::map<K, V> type;
};
template <class K, class V>
using DefMap = typename DefMapTypedef<K,V>::type;

template <class V>
struct IntMapTypedef{
	typedef std::map<std::size_t, V> type;
};
template <class V>
using IntMap = typename IntMapTypedef<V>::type;


template <class V>
struct DefVecTypedef{
	typedef std::vector<V> type;
};
template <class V>
using DefVec = typename DefVecTypedef<V>::type;











// use some SFINAE magic to get mapped containers value 
template <typename T>
struct tovoid {
  typedef void type;
};

// if is a vector, list, deque, etc... where there is no key_type and mapped_type, just a value
template <typename T, typename I, typename = void>
struct value_type {
  typedef typename T::value_type type;
  static typename std::add_lvalue_reference<type>::type get(I & t) {
    return *t;
  };
};

// if is a map or unordered map.... has a mapped_type
template <typename T, typename I>
struct value_type<T, I, typename tovoid<typename T::mapped_type>::type> {
  typedef typename T::mapped_type type;
  static typename std::add_lvalue_reference<type>::type get(I & t) {
    return t->second;
  };
};


template <typename Container, typename Iterator>
inline typename std::add_lvalue_reference<typename value_type<Container, Iterator>::type>::type getIteratorValue(const Container & c, Iterator & it){
	return value_type<Container, Iterator>::get(it);
};



















// NestedContainer is a container that contains another container.
//* You could possible contain another nested container... container-ception
//*
//* Of course you can also make containers containing containers in the
//* usual stl container types. What makes this container different is that
//* it implements not just ::iterator, but also ::nested_iterator, which
//* allows you to iterate over all subcontainer elements in each subcontainer
//* with a single iterator. For example, you might have a map<int, map<int, double>>. 
//* You could iterate over all the doubles by using 
//* a ::nested_iterator<map<int,double>::iterator>
//*
//***********************************************************/

// SubcontainerT is required to have the followsing:
// * 						- ::iterator, begin(), end()
// *						- a trivial constructor
// NestedContainer inherits from its ContainerT :
// *						- NestedContainer & operator[](std::size_t level)
// *						- void insert(std::size_t lvl, NestedContainer lc)
// *						- void erase(std::size_t lvl)
// *						- ::iterator, begin(), end()
// *			optional:	- iterator at(std::size_t lvl)

template <class Key, 
		  class Subcontainer,
		  template <class K, class V> class ContainerT = DefMap >
struct NestedContainer : public ContainerT<Key, Subcontainer>{
public:
	// class typedefs
	typedef Key 													KeyT;
	typedef Subcontainer 											SubcontainerT;
	typedef NestedContainer<KeyT, SubcontainerT, ContainerT>		SelfT;
	typedef ContainerT<KeyT, SubcontainerT> 						base_container;
	typedef typename ContainerT<KeyT, SubcontainerT>::iterator 		base_iterator;
	typedef typename SubcontainerT::value_type						value_type;

private:
	SubcontainerT    mEndSubcont;

public:

	class iterator;
	friend class iterator;
	


	// basic iterator over the outer container
	class outer_iterator{
	public:
		typedef outer_iterator 								self_type;
		typedef typename base_iterator::difference_type		difference_type;
	    typedef typename base_iterator::value_type			value_type;
	    typedef typename base_iterator::reference			reference;
	    typedef typename base_iterator::pointer				pointer;
	    typedef typename base_iterator::iterator_category 	iterator_category;


	    friend class iterator;

		// construction
		outer_iterator(NestedContainer & t, const base_iterator & bit)
		: cont(&t)
		, it(bit){};


		// copy construction
		outer_iterator(const outer_iterator & itr): cont(itr.cont), it(itr.it) {};

		// copy assignment
		outer_iterator & operator=(const outer_iterator & itr){
			outer_iterator i(itr);
			// std::swap(i,*this);
			cont = i.cont;
			it = i.it;
			return *this;
		}

		// dereferencing
		reference operator*(){ return *it;};

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
		pointer operator->() {return it.operator->();};

		// inequality
		bool operator!=(const self_type & leaf) const {return it != leaf.it;};

		// equality
		bool operator==(const self_type & leaf) const {return it == leaf.it;};


	private:
		base_iterator 			it;
		NestedContainer * 		cont;
	};


	outer_iterator outer_begin(){return outer_iterator(*this, base_container::begin());};
	outer_iterator outer_end(){return outer_iterator(*this, base_container::end());};


	





	class iterator{
	public:
		typedef iterator 											self_type;
		typedef typename SubcontainerT::iterator::difference_type 	difference_type;
	    typedef typename SubcontainerT::iterator::value_type		value_type;
	    typedef typename SubcontainerT::iterator::reference			reference;
	    typedef typename SubcontainerT::iterator::pointer			pointer;
	    typedef typename SubcontainerT::iterator::iterator_category iterator_category;

		// construction from a subiterator
		iterator(NestedContainer & c, outer_iterator oit, typename SubcontainerT::iterator sit)
		: cont(&c)
		, subit(sit)
		, it(oit) {};

		// nested iterator has explicit conversion from outer_iterator to iterator
		iterator(const outer_iterator & bit)
		: cont(bit.cont)
		, subit(bit.cont->mEndSubcont.begin())
		, it(bit) {
			if (it != cont->outer_end())
			{
				subit = getIteratorValue(*cont, it).begin();
			}
			else{
				subit = cont->mEndSubcont.end();
			}
		};


		// copy construction
		iterator(const iterator & bit)
		: cont(bit.cont)
		, subit(bit.subit)
		, it(bit.it) {};


		// copy assignment
		iterator & operator=(const iterator & bit)
		{
			iterator nit(bit);
			// std::swap(nit, *this);
			cont = nit.cont;
			subit = nit.subit;
			it = nit.it;
			return *this;
		};

		// dereferencing
		reference operator*(){ return *subit;};

		// preincrement 
		self_type operator++(){
			subit++;
			while (it != cont->outer_end()){
				if (subit != getIteratorValue(*cont, it).end()){
					return *this;
				}

				// reached end of subcontainer, iterate to next one
				it++;

				if (it == cont->outer_end()) break;
				subit = getIteratorValue(*cont, it).begin();	
			}

			// have reached the end of all the cells
			it = cont->outer_end();
			subit = cont->mEndSubcont.end();
			return *this;
		}

		// postincrement 
		self_type operator++(int blah){
			subit++;
			while (it != cont->outer_end()){
				if (subit != getIteratorValue(*cont, it).end()){
					// subit++;
					return *this;
				}

				// reached end of subcontainer, iterate to next one
				it++;

				if (it == cont->outer_end()) break;
				subit = getIteratorValue(*cont, it).begin();	
			}

			// have reached the end of all the cells
			it = cont->outer_end();
			subit = cont->mEndSubcont.end();
			return *this;
		}

		// pointer
		pointer operator->() {return subit.operator->();};

		// inequality
		bool operator!=(const self_type & leaf) const {return subit != leaf.subit;};

		// equality
		bool operator==(const self_type & leaf) const {return subit == leaf.subit;};

	private:
		typename SubcontainerT::iterator 	subit;
		outer_iterator 						it;
		NestedContainer * 					cont;
	};


	// begin with the first argument specified
	template <typename Arg1, typename... Args>
	iterator begin(Arg1 arg1, Args... args){
		outer_iterator it = outer_iterator(*this, base_container::find(arg1));
		return iterator(*this, it, this->operator[](arg1).begin(args...));
	}

	// end with the first argument specified
	template <typename Arg1, typename... Args>
	iterator end(Arg1 arg1, Args... args){
		outer_iterator it = outer_iterator(*this, base_container::find(arg1));
		it++;
		if (it == outer_end()) return iterator(*this, it, mEndSubcont.end());
		// return iterator(*this, it, this->operator[](arg1).end(args...));
		return iterator(*this, it, getIteratorValue(*this, it).begin(args...));
	}


	iterator begin(){return outer_iterator(*this, base_container::begin());};
	
	iterator end(){return outer_iterator(*this, base_container::end());};



	// do a find with the first argument specified
	template <typename Arg1, typename... Args>
	iterator find(const KeyT & key, Arg1 arg1, Args... args){
		outer_iterator it = outer_iterator(*this, base_container::find(arg1));
		return iterator(*this, it, this->operator[](arg1).find(key, args...));
	}

	// do a find by searching through all the subcontainers
	iterator find(const KeyT & key){
		for (auto it=outer_begin(); it!=outer_end(); it++){
			auto sit = this->operator[](it->first).find(key);
			if (sit != this->operator[](it->first).end()){
				return iterator(*this, it, sit);
			}
		}
		return end();
	}




	template <typename Arg1, typename... Args>
	std::pair<iterator, bool> insert(const value_type & val, Arg1 arg1, Args... args){
		std::pair<typename SubcontainerT::iterator, bool> sitpair = this->operator[](arg1).insert(val, args...);
		outer_iterator it = outer_iterator(*this, base_container::find(arg1));
		return std::make_pair(iterator(*this, it, sitpair.first), sitpair.second);
	}

};



}// end namespace treefidi

#endif