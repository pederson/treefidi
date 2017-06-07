#ifndef _NESTEDCONTAINER_H
#define _NESTEDCONTAINER_H


#include <map>
#include <vector>
#include <iostream>
#include <type_traits>
#include <algorithm>


namespace treefidi{


// typedef std::mapdefaultmap;
template<class K, class V>
struct DefMapTypedef{
	typedef std::map<K, V> type;
};
template<class K, class V>
using DefMap = typename DefMapTypedef<K,V>::type;

template <class V>
struct IntMapTypedef{
	typedef std::map<std::size_t, V> type;
};
template <class V>
using IntMap = typename IntMapTypedef<V>::type;


template<class V>
struct DefVecTypedef{
	typedef std::vector<V> type;
};
template<class V>
using DefVec = typename DefVecTypedef<V>::type;











// use some magic to get mapped containers value 
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
}



















// NestedContainer is a container that contains another container.
/* You could possible contain another nested container... container-ception
/*
/* Of course you can also make containers containing containers in the
/* usual stl container types. What makes this container different is that
/* it implements not just ::iterator, but also ::nested_iterator, which
/* allows you to iterate over all subcontainer elements in each subcontainer
/* with a single iterator. For example, you might have a map<int, map<int, double>>. 
/* You could iterate over all the doubles by using 
/* a ::nested_iterator<map<int,double>::iterator>
/*
/***********************************************************/

// SubcontainerT is required to have the followsing:
// * 						- ::iterator, begin(), end()
// *						- a trivial constructor
// NestedContainer inherits from its ContainerT :
// *						- NestedContainer & operator[](std::size_t level)
// *						- void insert(std::size_t lvl, NestedContainer lc)
// *						- void erase(std::size_t lvl)
// *						- ::iterator, begin(), end()
// *			optional:	- iterator at(std::size_t lvl)

template <class KeyT, 
		  class SubcontainerT,
		  template <class K, class V> class ContainerT = DefMap >
struct NestedContainer : public ContainerT<KeyT, SubcontainerT>{
public:
	typedef NestedContainer<KeyT, SubcontainerT, ContainerT>	SelfT;
	typedef ContainerT<KeyT, SubcontainerT> 						base_container;
	typedef typename ContainerT<KeyT, SubcontainerT>::iterator 		base_iterator;


private:
	SubcontainerT    mEndSubcont;

public:

	template <class SubiteratorT> class nested_iterator;
	template <class SubiteratorT> friend class nested_iterator;
	



	class iterator{
	public:
		typedef iterator 								self_type;
		typedef std::ptrdiff_t 							difference_type;
	    typedef typename base_iterator::value_type		value_type;
	    typedef typename base_iterator::reference		reference;
	    typedef typename base_iterator::pointer			pointer;
	    typedef std::forward_iterator_tag 				iterator_category;


	    template <class SubiteratorT> friend class nested_iterator;

		// construction
		iterator(NestedContainer & t, const base_iterator & bit)
		: cont(&t)
		, it(bit){};


		// copy construction
		iterator(const iterator & itr): cont(itr.cont), it(itr.it) {};

		// copy assignment
		iterator & operator=(const iterator & itr){
			iterator i(itr);
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
		base_iterator 					it;
		NestedContainer * 		cont;
	};



	iterator begin(){return iterator(*this, ContainerT<KeyT, SubcontainerT>::begin());};
	iterator end(){return iterator(*this, ContainerT<KeyT, SubcontainerT>::end());};











	template <class SubiteratorT> friend class nested_iterator;
	// iterate over key/value pairs for all nested containers at once
	template <class SubiteratorT = typename SubcontainerT::iterator>
	class nested_iterator{
	public:
		typedef nested_iterator<SubiteratorT> 			self_type;
		typedef std::ptrdiff_t 							difference_type;
	    typedef typename SubiteratorT::value_type		value_type;
	    typedef typename SubiteratorT::reference		reference;
	    typedef typename SubiteratorT::pointer			pointer;
	    typedef std::forward_iterator_tag 				iterator_category;

		// nested iterator has explicit conversion from iterator to nested_iterator
		nested_iterator(const iterator & bit)
		: cont(bit.cont)
		, subit(bit.cont->mEndSubcont.begin())
		, it(bit) {
			if (it != cont->end())
			{
				subit = getIteratorValue(*cont, it).begin();
			}
			else{
				subit = cont->mEndSubcont.end();
			}
		};


		// copy construction
		nested_iterator(const nested_iterator & bit)
		: cont(bit.cont)
		, subit(bit.subit)
		, it(bit.it) {};


		// copy assignment
		nested_iterator & operator=(const nested_iterator & bit)
		{
			nested_iterator nit(bit);
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
			while (it != cont->end()){
				if (subit != getIteratorValue(*cont, it).end()){
					return *this;
				}

				// reached end of subcontainer, iterate to next one
				it++;

				if (it == cont->end()) break;
				subit = getIteratorValue(*cont, it).begin();	
			}

			// have reached the end of all the cells
			it = cont->end();
			subit = cont->mEndSubcont.end();
			return *this;
		}

		// postincrement 
		self_type operator++(int blah){
			subit++;
			while (it != cont->end()){
				if (subit != getIteratorValue(*cont, it).end()){
					return *this;
				}

				// reached end of subcontainer, iterate to next one
				it++;

				if (it == cont->end()) break;
				subit = getIteratorValue(*cont, it).begin();	
			}

			// have reached the end of all the cells
			it = cont->end();
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
		SubiteratorT 						subit;
		iterator 							it;
		NestedContainer * 			cont;
	};

};



}// end namespace treefidi

#endif