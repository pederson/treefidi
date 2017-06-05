#ifndef _DOMAINCONTAINER_H
#define _DOMAINCONTAINER_H



// a homogeneous container class that stores different
// subdomains and contains iterators to them
template <class KeyT, class MappedT, 
		  class SubdomainLabelT = std::size_t>
struct DomainContainer{
	typedef DomainContainer<KeyT, MappedT, SubdomainLabelT> SelfT;
	typedef std::unordered_map<KeyT, MappedT>				SubdomainMapT;
	typedef std::map<SubdomainLabelT, SubdomainMapT> 		DomainMapT;
	typedef std::pair<const KeyT, MappedT> 					ValueT;

	// these map a SUBDOMAIN to another map
	// that maps a KEY to a NODE
	DomainMapT 		mKeyMaps; 		

 // is required to have the following:
 // *							- MappedT & operator[](KeyT key)
 // *							- void insert(KeyT key, MappedT node)
 // *							- void erase(KeyT key)
 // *							- ::iterator, begin(), end()
 // *							- iterator find(KeyT key)









	friend class subdomain_iterator;
	// iterate over key/node pairs for nodes in a specified subdomain 
	class subdomain_iterator{
	public:
		typedef subdomain_iterator self_type;
		typedef std::ptrdiff_t difference_type;
	    typedef std::pair<const KeyT, MappedT> value_type;
	    typedef std::pair<const KeyT, MappedT> & reference;
	    typedef std::pair<const KeyT, MappedT> * pointer;
	    typedef std::forward_iterator_tag iterator_category;

		// construction
		subdomain_iterator(DomainContainer & t, SubdomainLabelT subd)
		: cont(t)
		, sit(t.mKeyMaps.find(subd))
		, it(((t.mKeyMaps.find(subd))->second.begin()){};

		subdomain_iterator(DomainContainer & t, SubdomainLabelT subd, typename SubdomainMapT::iterator iter)
		: cont(t)
		, sit(t.mKeyMaps.find(subd))
		, it(iter) {};

		// dereferencing
		reference operator*(){ return *it;};

		// preincrement 
		self_type operator++(){
			it++;
			if (it != sit->second.end()) return *this;

			// reached end of subdomain
			return cont.subdomain_end(sit->first);
		}

		// postincrement 
		self_type operator++(int blah){
			it++;
			if (it != sit->second.end()) return *this;

			// reached end of subdomain
			return cont.subdomain_end(sit->first);
		}

		// pointer
		pointer operator->() {return it.operator->();};

		// inequality
		bool operator!=(const self_type & leaf) const {return it != leaf.it;};

		// equality
		bool operator==(const self_type & leaf) const {return it == leaf.it;};


	private:
		typename DomainMapT::iterator sit;
		typename SubdomainMapT::iterator it;
		DomainContainer & cont;
	};

	
	subdomain_iterator subdomain_begin(SubdomainLabelT subd){return subdomain_iterator(*this, subd);};
	subdomain_iterator subdomain_end(SubdomainLabelT subd){return subdomain_iterator(*this, subd, mKeyMaps.end());};












	friend class iterator;
	// iterate over key/node pairs for all nodes 
	class iterator{
	public:
		typedef iterator self_type;
		typedef std::ptrdiff_t difference_type;
	    typedef std::pair<const KeyT, MappedT> value_type;
	    typedef std::pair<const KeyT, MappedT> & reference;
	    typedef std::pair<const KeyT, MappedT> * pointer;
	    typedef std::forward_iterator_tag iterator_category;


	    friend class SelfT::subdomain_iterator;

		// construction
		iterator(DomainContainer & t)
		: cont(t)
		, sit(t.mKeyMaps.begin())
		, it((t.mKeyMaps.begin())->second.begin()){};

		iterator(DomainContainer & t, SubdomainLabelT subd, typename SubdomainMapT::iterator iter)
		: cont(t)
		, sit(t.mKeyMaps.find(subd))
		, it(iter) {};


		iterator(const subdomain_iterator & bit)
		: cont(bit.t)
		, sit(bit.sit)
		, it(bit.it) {};

		// dereferencing
		reference operator*(){ return *it;};

		// preincrement 
		self_type operator++(){
			it++;
			while (sit != lit->second.end()){
				if (it != sit->second.end(){
					return *this;
				}

				// reached end of subdomain
				sit++;	

				if (sit == cont.mKeyMaps.end()) break;
				it = sit->second.begin();		
			}
			// have reached the end of all the cells
			return cont.end();
		}

		// postincrement 
		self_type operator++(int blah){
			it++;
			while (sit != lit->second.end()){
				if (it != sit->second.end(){
					return *this;
				}

				// reached end of subdomain
				sit++;	

				if (sit == cont.mKeyMaps.end()) break;
				it = sit->second.begin();		
			}
			// have reached the end of all the cells
			return cont.end();
		}

		// pointer
		pointer operator->() {return it.operator->();};

		// inequality
		bool operator!=(const self_type & leaf) const {return it != leaf.it;};

		// equality
		bool operator==(const self_type & leaf) const {return it == leaf.it;};


	private:
		typename DomainMapT::iterator sit;		// iterator over all subdomains
		typename SubdomainMapT::iterator it;	// iterator to a key/value pair
		DomainContainer & cont;
	};

	
	iterator begin(){return iterator(*this);};
	iterator end(){auto q=mKeyMaps.end(); q--; return iterator(*this, q->first, q->second.end());};





	// find with zero information
	iterator find(const KeyT & key){
		auto sit = mKeyMaps.begin();
		while (sit != mKeyMaps.end()){
			auto it = sit.find(key);
			if (it != sit.end()) return iterator(*this, sit->first, it);
			sit++;
		}
		return end();
	}

	// find with information on the subdomain
	subdomain_iterator find(const KeyT & key, SubdomainLabelT subd){
		return subdomain_iterator(*this, subd, mKeyMaps[subd].find(key));
	}


	// MappedT & operator[](const KeyT & key){};

	// void insert(const KeyT & key, SubdomainLabelT subd, MappedT & val){
	// 	mKeyMaps[subd][key] = val;
	// };

	// iterator insert(const KeyT & key, SubdomainLabelT subd, MappedT & val){
	// 	mKeyMaps[subd][key] = val;
	// };

	std::pair<iterator,bool> insert(const ValueT & val){

	};

	// void erase(const KeyT & key, std::size_t lvl){mKeyMaps[lvl].erase(key);};

	// void erase(iterator position, std::size_t lvl){mKeyMaps[lvl].erase(position);};




	// void changeSubdomain(const KeyT & key, std::size_t lvl, SubdomainLabelT formerSub, SubomdainLabelT newSub){
	// 	mKeyMaps[lvl][newSub][key] = std::move(mKeyMaps[lvl][formerSub][key]);
	// 	mKeyMaps[lvl][formerSub].erase(key);
	// }


};



#endif