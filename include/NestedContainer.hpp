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
  typedef typename T::value_type 	type;

  static typename std::add_rvalue_reference<type>::type get(I & t) {
    return std::move(*t);
  };
};

// if is a map or unordered map.... has a mapped_type
template <typename T, typename I>
struct value_type<T, I, typename tovoid<typename T::mapped_type>::type> {
  typedef typename T::mapped_type 	type;

  static typename std::add_rvalue_reference<type>::type get(I & t) {
    return std::move(t->second);
  };
};


template <typename Container, typename Iterator>
inline typename std::add_rvalue_reference<typename value_type<Container, Iterator>::type>::type getIteratorValue(const Container & c, Iterator & it){
	return std::move(value_type<Container, Iterator>::get(it));
};




template <typename T, typename = void>
struct isNestedContainer{
	static const bool value = false;
};

template <typename T>
struct isNestedContainer<T, typename tovoid<typename T::SubcontainerT>::type>{
	static const bool value = true;
};




// // some SFINAE to get the key from a container iterator
// // if is a vector, list, deque, etc... where there is no key_type and mapped_type, just a value
// template <typename T, typename I, typename = void>
// struct key_type {
//   typedef std::size_t	 			type;

//   static type getKey(T & c, I & t) {
//     return t - c.begin();
//   };
// };


// // if is a NestedContainer.... has a KeyT
// template <typename T, typename I>
// struct key_type<T, I, typename tovoid<typename T::KeyT>::type> {
//   typedef typename T::KeyT	 	type;

//   static type getKey(T & c, I & t) {
//     return t->first;
//   };
// };


// // if is a map or unordered map.... has a mapped_type
// template <typename T, typename I>
// struct key_type<T, I, typename tovoid<typename T::mapped_type>::type> {
//   typedef typename T::key_type	 	type;

//   static type getKey(T & c, I & t) {
//     return t->first;
//   };
// };



// template <typename Container, typename Iterator>
// inline typename key_type<Container, Iterator>::type getIteratorKey(const Container & c, Iterator & it){
// 	return key_type<Container, Iterator>::getKey(c, it);
// };















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
	typedef typename SubcontainerT::key_type 						key_type;

private:
	SubcontainerT    mEndSubcont;

public:

	// class iterator;
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
			// std::swap(i.cont, cont);
			cont = i.cont;
			std::swap(i.it, it);
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




		base_iterator get_base() {return it;};
		NestedContainer * get_cont() {return cont;};
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


	    friend class NestedContainer;

		// construction from a subiterator
		iterator(NestedContainer & c, const outer_iterator & oit, const typename SubcontainerT::iterator & sit)
		: cont(&c)
		, subit(sit)
		, it(oit) {};

		// copy construction
		iterator(const iterator & bit)
		: cont(bit.cont)
		, subit(bit.subit)
		, it(bit.it) {};


		// copy assignment
		iterator & operator=(const iterator & bit)
		{
			iterator nit(bit);
			// std::swap(nit.cont, cont);
			cont = nit.cont;
			std::swap(nit.subit, subit);
			std::swap(nit.it, it);
			return *this;
		};

		// destructor
		~iterator(){};

		// move constructor
		iterator(iterator && bit) = default;

		// move assigment
		iterator & operator=(iterator && bit) = default;

		// dereferencing
		reference operator*(){ return subit.operator*();};

		// preincrement 
		self_type & operator++(){
			subit++;
			while (it != cont->outer_end()){
				if (subit != getIteratorValue(*cont,it).end()) return *this;

				// reached end of subcontainer, iterate to next one
				it++;

				if (it == cont->outer_end()) break;
				subit = getIteratorValue(*cont, it).begin();	
			}

			// have reached the end of all the cells
			subit = cont->mEndSubcont.end();
			return *this;
		}

		// postincrement 
		self_type operator++(int blah){
			auto tmp(*this);
			subit++;
			while (it != cont->outer_end()){
				if (subit != getIteratorValue(*cont,it).end()){
					return tmp;
				}

				// reached end of subcontainer, iterate to next one
				it++;

				if (it == cont->outer_end()) break;
				subit = getIteratorValue(*cont, it).begin();
			}

			// have reached the end of all the cells
			subit = cont->mEndSubcont.end();
			return tmp;
		}

		// pointer
		pointer operator->() {
			return subit.operator->();
		};

		// inequality
		bool operator!=(const self_type & leaf) const {return subit != leaf.subit;};

		// equality
		bool operator==(const self_type & leaf) const {return subit == leaf.subit;};

		outer_iterator get_outer() {return it;};
		NestedContainer * get_cont() {return cont;};
	private:
		typename SubcontainerT::iterator 	subit;
		outer_iterator 						it;
		NestedContainer * 					cont;


		void summarize(){
			std::cout << "cont: " << cont << " outer_it: " << &it << " subit: " << &subit << std::endl;
		}
	};


	// begin with the first argument specified
	template <typename Arg1, typename... Args>
	iterator begin(Arg1 arg1, Args... args){
		typename SubcontainerT::iterator sit = this->operator[](arg1).begin(args...);
		outer_iterator it = outer_iterator(*this, base_container::find(arg1));
		
		if (sit != getIteratorValue(*this, it).end(args...)) return iterator(*this, it, sit);
		return end(arg1, args...);
	}

	// end with the first argument specified
	template <typename Arg1, typename... Args>
	iterator end(Arg1 arg1, Args... args){
		outer_iterator it = outer_iterator(*this, base_container::find(arg1));
		typename SubcontainerT::iterator sit = getIteratorValue(*this, it).end(args...);

		if (sit == getIteratorValue(*this, it).end()){
			it++;
			if (it == outer_end()) return iterator(*this, it, mEndSubcont.end());
			sit = getIteratorValue(*this, it).begin();
		} 
		
		return iterator(*this, it, sit);
	}



	iterator begin(){
		base_iterator bit = base_container::begin();
		outer_iterator it(*this, bit);
		typename SubcontainerT::iterator sit = getIteratorValue(*this, it).begin();//it->second.begin();
		return iterator(*this, it, sit);
	};
	
	iterator end(){
		return iterator(*this, outer_end(), mEndSubcont.end());
	};





	// get a tuple of keys for a given iterator. this overload is for
	// a SubcontainerT that is NOT another NestedContainer
	template <typename Iter, typename std::enable_if<!isNestedContainer<SubcontainerT>::value && 
							  std::is_same<Iter, iterator>::value, void>::type * dummy = nullptr>
	auto key_tuple(Iter it){
		return std::make_tuple(it.it->first, it->first);
	}

	// get a tuple of keys for a given iterator. this overload is for
	// a SubcontainerT that is another NestedContainer
	template <typename Iter, typename std::enable_if<isNestedContainer<SubcontainerT>::value && 
							std::is_same<Iter, iterator>::value, void>::type * dummy = nullptr>
	auto key_tuple(Iter it){
		SubcontainerT & subc = this->operator[](it.it->first);
		auto subt = subc.key_tuple(it.subit);
		return std::tuple_cat(std::make_tuple(it.it->first), subt);
	}













	// do a find with the first argument specified
	// if not found, the result is end(arg1, args...)
	template <typename Arg1, typename... Args>
	iterator find(const key_type & key, Arg1 arg1, Args... args){
		// std::cout << "finding subit \n";
		typename SubcontainerT::iterator subit = this->operator[](arg1).find(key, args...);
		outer_iterator it = outer_iterator(*this, base_container::find(arg1));
		
		if (subit != getIteratorValue(*this, it).end(args...)){
			// std::cout << "possible success of sub finder" << std::endl;
			return iterator(*this, it, subit);
		}
		// std::cout << "reached end of sub finder" << std::endl;
		return end(arg1, args...);
		
	}

	// do a find by searching through all the subcontainers
	// if not found, the result is end() with no arguments
	iterator find(const key_type & key){
		for (auto it=outer_begin(); it!=outer_end(); it++){
			auto sit = this->operator[](it->first).find(key);
			if (sit != this->operator[](it->first).end()){
				return iterator(*this, it, sit);
			}
		}
		return end();
	}



	// this MUST be a fully specified insert call... all args must be present, otherwise compilation error
	template <typename Arg1, typename... Args>
	std::pair<iterator, bool> insert(const value_type & val, Arg1 arg1, Args... args){
		std::pair<typename SubcontainerT::iterator, bool> sitpair = this->operator[](arg1).insert(val, args...);
		outer_iterator it = outer_iterator(*this, base_container::find(arg1));
		return std::make_pair(iterator(*this, it, sitpair.first), sitpair.second);
	}



	// this MUST be a fully specified erase call... all args must be present, otherwise compilation error
	// erase by iterator
	// template <typename Arg1, typename... Args>
	iterator erase(iterator position){
		auto it = position.it;
		typename SubcontainerT::iterator sit = getIteratorValue(*this, it).erase(position.subit);
		
		if (sit == getIteratorValue(*this, it).end()){
			it++;
			if (it == outer_end()) return iterator(*this, it, mEndSubcont.end());
			sit = getIteratorValue(*this, it).begin();
		} 
		
		return iterator(*this, it, sit);
	}

};












template <class NContainer, class UnaryFunction>
void nested_for_each(NContainer & c, std::vector<typename NContainer::KeyT> & keys, UnaryFunction f){
	for (auto it = keys.begin(); it!=keys.end(); it++){
		std::for_each(c[*it].begin(), c[*it].end(), f);
	}
}


template <class NContainer, class UnaryFunction>
void nested_for_each(NContainer & c, std::vector<typename NContainer::KeyT> & keys, std::map<typename NContainer::KeyT, UnaryFunction> & fmap){
	for (auto it = keys.begin(); it!=keys.end(); it++){
		std::for_each(c[*it].begin(), c[*it].end(), fmap.at(*it));
	}
}



}// end namespace treefidi

#endif