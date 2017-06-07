#ifndef _NESTEDCONTAINER_H
#define _NESTEDCONTAINER_H


#include <map>
#include <vector>
#include <iostream>
#include <type_traits>


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
  }
};

// if is a map or unordered map.... has a mapped_type
template <typename T, typename I>
struct value_type<T, I, typename tovoid<typename T::mapped_type>::type> {
  typedef typename T::mapped_type type;
  static typename std::add_lvalue_reference<type>::type get(I & t) {
    return t->second;
  }
};


template <typename Container, typename Iterator>
inline typename std::add_lvalue_reference<typename value_type<Container, Iterator>::type>::type getIteratorValue(const Container & c, Iterator & it){
	return value_type<Container, Iterator>::get(it);
}























template <class KeyT, 
		  class SubcontainerT,
		  template <class K, class V> class ContainerT = DefMap >
struct NestedMappedContainer : public ContainerT<KeyT, SubcontainerT>{
	typedef NestedMappedContainer<KeyT, SubcontainerT, ContainerT>	SelfT;
	// typedef typename ContainerT<KeyT, SubcontainerT>::iterator 		iterator;
	typedef ContainerT<KeyT, SubcontainerT> 					base_container;
	typedef typename ContainerT<KeyT, SubcontainerT>::iterator 		base_iterator;


	typedef typename base_container::mapped_type 		mapped_type;

	// SubcontainerT is required to have the followsing:
	// * 						- ::iterator, begin(), end()
	// NestedMappedContainer inherits from its ContainerT :
	// *						- NestedMappedContainer & operator[](std::size_t level)
	// *						- void insert(std::size_t lvl, NestedMappedContainer lc)
	// *						- void erase(std::size_t lvl)
	// *						- ::iterator, begin(), end()
	// *			optional:	- iterator at(std::size_t lvl)


	// NestedMappedContainer() : ContainerT<KeyT, SubcontainerT>() {};

	template <class SubiteratorT> class nested_iterator;

	







	class iterator{
	public:
		typedef iterator 						self_type;
		typedef std::ptrdiff_t 					difference_type;
	    typedef typename base_iterator::value_type		value_type;
	    typedef typename base_iterator::reference		reference;
	    typedef typename base_iterator::pointer			pointer;
	    typedef std::forward_iterator_tag 		iterator_category;


	    template <class SubiteratorT> friend class nested_iterator;

		// construction
		iterator(NestedMappedContainer & t, const base_iterator & bit)
		: cont(t)
		, it(bit){};


		iterator(const iterator & itr): cont(itr.cont), it(itr.it) {};


		iterator operator=(const iterator & itr){
			iterator i(itr);
			return i;
		}
		// iterator(NestedMappedContainer & t, typename ContainerT::iterator iter)
		// : cont(t)
		// , sit(t.mKeyMaps[lvl].find(subd))
		// , it(iter) {};

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
		NestedMappedContainer & 		cont;
	};



	iterator begin(){return iterator(*this, ContainerT<KeyT, SubcontainerT>::begin());};
	iterator end(){return iterator(*this, ContainerT<KeyT, SubcontainerT>::end());};

	// template <class IteratorT = typename ContainerT<KeyT, SubcontainerT>::iterator>
	// IteratorT begin(){return ContainerT<KeyT, SubcontainerT>::begin();};

	// template <class SubiteratorT> begin<nested_iterator<SubiteratorT>>(){return nested_iterator<SubiteratorT>(*this);};
	



	// template <class SubiteratorT = typename SubcontainerT::iterator>
	// nested_iterator<SubiteratorT> begin(){return nested_iterator<SubiteratorT>(*this);};


	// template <class IteratorT>
	// IteratorT begin(){};


	// nested_iterator begin(){return nested_iterator(*this);};




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


	    // friend class SubiteratorT;

		// construction
		// nested_iterator(NestedMappedContainer & t)
		// : cont(t)
		// , subit((t.mKeyMaps.begin())->second.begin())
		// , it(((t.mKeyMaps.begin())->second.begin())->second.begin()){std::cout << "reg const" << std::endl;};

		// nested_iterator(NestedMappedContainer & t, typename ContainerT::nested_iterator iter)
		// : cont(t)
		// , sit(t.mKeyMaps[lvl].find(subd))
		// , it(iter) {};

		// nested iterator has explicit conversion from iterator to nested_iterator
		nested_iterator(const iterator & bit)
		: cont(bit.cont)
		, it(bit) {
			std::cout << "copy const" << std::endl; 
			if (it != cont.end()){
				subit = getIteratorValue(cont, it).begin();
				std::cout << "setting subit" << std::endl;
			}
			else{
				std::cout << "nulling subit" << std::endl;
				subit = *subitend;
			}
		};
		// dereferencing
		reference operator*(){ return *subit;};

		// preincrement 
		self_type operator++(){
			subit++;
			std::cout << "subit++" << std::endl;
			while (it != cont.end()){
				if (subit != getIteratorValue(cont, it).end()){
					std::cout << subit->first << "->" << subit->second << std::endl;
					std::cout << "return *this" << std::endl;
					return *this;
				}

				// reached end of subiterator
				it++;
				std::cout << "it++" << std::endl;

				if (it == cont.end()) break;
				subit = getIteratorValue(cont, it).begin();	
			}

			std::cout << "returning" << std::endl;

			// have reached the end of all the cells
			it = cont.end();
			subit = *subitend;
			return *this;
		}

		// postincrement 
		self_type operator++(int blah){
			subit++;
			std::cout << "subit++" << std::endl;
			while (it != cont.end()){
				if (subit != getIteratorValue(cont, it).end()){
					std::cout << subit->first << "->" << subit->second << std::endl;
					std::cout << "return *this" << std::endl;
					return *this;
				}

				// reached end of subiterator
				it++;
				std::cout << "it++" << std::endl;

				if (it == cont.end()) break;
				subit = getIteratorValue(cont, it).begin();	
			}

			std::cout << "returninggg" << std::endl;

			// have reached the end of all the cells
			it = cont.end();
			subit = *subitend;
			// auto p = it;
			// subit = getIteratorValue(--p).end();
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
		NestedMappedContainer & 			cont;



		const SubiteratorT * 				subitend=nullptr;
		// template <typename Container>
		// typename value_type<Container>::type getIteratorValue(const typename Container::iterator & it){
		// 	return value_type<Container>::get(it);
		// }

		// SubcontainerT & getSubcontainer(base_iterator & it){
		// 	if ()
		// }
	};

};



































}// end namespace treefidi

#endif