#ifndef _LEVELSUBDOMAINCONTAINER_H
#define _LEVELSUBDOMAINCONTAINER_H




template <class KeyT, class MappedT, 
		  class SubdomainLabelT = std::size_t>
struct LevelSubdomainContainer{
	typedef LevelSubdomainContainer<KeyT, MappedT, SubdomainLabelT> SelfT;

	// these map an integer LEVEL (starting from 0) to another map
	// that maps a SUBDOMAIN to another map
	// that maps a KEY to a NODE
	std::map<std::size_t, std::map<SubdomainLabelT, std::unordered_map<KeyT, MappedT>>> 		mKeyMaps; 		

	// is required to have the following:
 // *							- MappedT & operator[](KeyT key)
 // *							- void insert(KeyT key, MappedT node)
 // *							- void erase(KeyT key)
 // *							- ::iterator, begin(), end()
 // *							- iterator find(KeyT key)









	friend class subdomain_iterator;
	// iterate over key/node pairs for all nodes 
	class subdomain_iterator{
	public:
		typedef subdomain_iterator self_type;
		typedef std::ptrdiff_t difference_type;
	    typedef std::pair<const KeyT, MappedT> value_type;
	    typedef std::pair<const KeyT, MappedT> & reference;
	    typedef std::pair<const KeyT, MappedT> * pointer;
	    typedef std::forward_iterator_tag iterator_category;

		// construction
		subdomain_iterator(LevelSubdomainContainer & t, std::size_t lvl, SubdomainLabelT subd)
		: cont(t)
		, lit(t.mKeyMaps.find(lvl))
		, sit((t.mKeyMaps.find(lvl))->second.find(subd))
		, it(((t.mKeyMaps.find(lvl))->second.find(subd))->second.begin()){};

		subdomain_iterator(LevelSubdomainContainer & t, std::size_t lvl, SubdomainLabelT subd, typename std::unordered_map<KeyT, MappedT>::subdomain_iterator iter)
		: cont(t)
		, lit(t.mKeyMaps.find(lvl))
		, sit(t.mKeyMaps[lvl].find(subd))
		, it(iter) {};

		// dereferencing
		reference operator*(){ return *it;};

		// preincrement 
		self_type operator++(){
			it++;
			if (it != sit->second.end()) return *this;

			// reached end of subdomain
			return cont.subdomain_end(lit->first, sit->first);
		}

		// postincrement 
		self_type operator++(int blah){
			it++;
			if (it != sit->second.end()) return *this;

			// reached end of subdomain
			return cont.subdomain_end(lit->first, sit->first);
		}

		// pointer
		pointer operator->() {return it.operator->();};

		// inequality
		bool operator!=(const self_type & leaf) const {return it != leaf.it;};

		// equality
		bool operator==(const self_type & leaf) const {return it == leaf.it;};


	private:
		typename std::map<std::size_t, std::map<SubdomainLabelT, std::unordered_map<KeyT, MappedT>>>::subdomain_iterator lit;
		typename std::map<SubdomainLabelT, <std::unordered_map<KeyT, MappedT>>::subdomain_iterator sit;
		typename std::unordered_map<KeyT, MappedT>::subdomain_iterator it;
		LevelSubdomainContainer & cont;
	};

	
	subdomain_iterator subdomain_begin(std::size_t lvl, SubdomainLabelT subd){return subdomain_iterator(*this, lvl, subd);};
	subdomain_iterator subdomain_end(std::size_t lvl, SubdomainLabelT subd){return subdomain_iterator(*this, lvl, subd, mKeyMaps[lvl][subd].end());};


















	



	friend class level_iterator;
	// iterate over key/node pairs for all nodes 
	class level_iterator{
	public:
		typedef level_iterator self_type;
		typedef std::ptrdiff_t difference_type;
	    typedef std::pair<const KeyT, MappedT> value_type;
	    typedef std::pair<const KeyT, MappedT> & reference;
	    typedef std::pair<const KeyT, MappedT> * pointer;
	    typedef std::forward_iterator_tag iterator_category;

	    friend class SelfT::subdomain_iterator;

		// construction
		level_iterator(LevelSubdomainContainer & t, std::size_t lvl)
		: cont(t)
		, lit(t.mKeyMaps.find(lvl))
		, sit((t.mKeyMaps.find(lvl))->second.begin())
		, it(((t.mKeyMaps.find(lvl))->second.begin())->second.begin()){};

		level_iterator(LevelSubdomainContainer & t, std::size_t lvl, SubdomainLabelT subd, typename std::unordered_map<KeyT, MappedT>::level_iterator iter)
		: cont(t)
		, lit(t.mKeyMaps.find(lvl))
		, sit(t.mKeyMaps[lvl].find(subd))
		, it(iter) {};

		level_iterator(const subdomain_iterator & bit)
		: cont(bit.t)
		, lit(bit.lit)
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
				if (sit == lit->second.end()) break;
				it = sit->second.begin();		
			}

			// reached end of level
			return cont.level_end(lit->first);
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
				if (sit == lit->second.end()) break;
				it = sit->second.begin();		
			}

			// reached end of level
			return cont.level_end(lit->first);
		}

		// pointer
		pointer operator->() {return it.operator->();};

		// inequality
		bool operator!=(const self_type & leaf) const {return it != leaf.it;};

		// equality
		bool operator==(const self_type & leaf) const {return it == leaf.it;};


	private:
		typename std::map<std::size_t, std::map<SubdomainLabelT, std::unordered_map<KeyT, MappedT>>>::level_iterator lit;
		typename std::map<SubdomainLabelT, <std::unordered_map<KeyT, MappedT>>::level_iterator sit;
		typename std::unordered_map<KeyT, MappedT>::level_iterator it;
		LevelSubdomainContainer & cont;
	};

	
	level_iterator level_begin(std::size_t lvl){return level_iterator(*this);};
	level_iterator level_end(std::size_t lvl){auto p=mKeyMaps.find(lvl); auto q=p.end(); q--; return level_iterator(*this, p->first, q->first, q->second.end());};

















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


	    friend class SelfT::level_iterator;
	    friend class SeltT::subdomain_iterator;

		// construction
		iterator(LevelSubdomainContainer & t)
		: cont(t)
		, lit(t.mKeyMaps.begin())
		, sit((t.mKeyMaps.begin())->second.begin())
		, it(((t.mKeyMaps.begin())->second.begin())->second.begin()){};

		iterator(LevelSubdomainContainer & t, std::size_t lvl, SubdomainLabelT subd, typename std::unordered_map<KeyT, MappedT>::iterator iter)
		: cont(t)
		, lit(t.mKeyMaps.find(lvl))
		, sit(t.mKeyMaps[lvl].find(subd))
		, it(iter) {};


		iterator(const subdomain_iterator & bit)
		: cont(bit.t)
		, lit(bit.lit)
		, sit(bit.sit)
		, it(bit.it) {};


		iterator(const level_iterator & bit)
		: cont(bit.t)
		, lit(bit.lit)
		, sit(bit.sit)
		, it(bit.it) {};

		// dereferencing
		reference operator*(){ return *it;};

		// preincrement 
		self_type operator++(){
			it++;
			while (lit != cont.mKeyMaps.end()){
				while (sit != lit->second.end()){
					if (it != sit->second.end(){
						return *this;
					}

					// reached end of subdomain
					sit++;			
				}

				// reached end of level
				lit++;

				if (lit == cont.mKeyMaps.end()) break;
				sit = lit->second.begin();
				it = sit->second.begin();
			}
			// have reached the end of all the cells
			return cont.end();
		}

		// postincrement 
		self_type operator++(int blah){
			it++;
			while (lit != cont.mKeyMaps.end()){
				while (sit != lit->second.end()){
					if (it != sit->second.end(){
						return *this;
					}

					// reached end of subdomain
					sit++;	
					if (sit == lit->second.end()) break;
					it = sit->second.begin();		
				}

				// reached end of level
				lit++;

				if (lit == cont.mKeyMaps.end()) break;
				sit = lit->second.begin();
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
		typename std::map<std::size_t, std::map<SubdomainLabelT, std::unordered_map<KeyT, MappedT>>>::iterator lit;
		typename std::map<SubdomainLabelT, <std::unordered_map<KeyT, MappedT>>::iterator sit;
		typename std::unordered_map<KeyT, MappedT>::iterator it;
		LevelSubdomainContainer & cont;
	};

	
	iterator begin(){return iterator(*this);};
	iterator end(){auto p=mKeyMaps.end(); p--; auto q=p.end(); q--; return iterator(*this, p->first, q->first, q->second.end());};






	iterator find(const KeyT & key){
		auto lit = mKeyMaps.begin();
		while (lit != mKeyMaps.end()){
			auto sit = lit->second.begin();
			while (sit != lit->second.end()){
				auto it = sit.find(key);
				if (it != sit.end()) return iterator(*this, lit->first, sit->first, it);
				sit++;
			}
		}
		return end();
	}


	level_iterator find(const KeyT & key, std::size_t lvl){
		auto sit = mKeyMaps[lvl].begin();
		while (sit!=mKeyMaps[lvl].end()){
			auto it = sit.find(key);
			if (it != sit.end()) return level_iterator(*this, lvl, sit->first, it);
			sit++;
		}
		return level_end(lvl);
	}


	subdomain_iterator find(const KeyT & key, std::size_t lvl, SubdomainLabelT subd){
		return subdomain_iterator(*this, lvl, subd, mKeyMaps[lvl][subd].find(key));
	}


	// MappedT & operator[](const KeyT & key){};

	void insert(const KeyT & key, std::size_t lvl, SubdomainLabelT subd, MappedT & val){
		mKeyMaps[lvl][subd][key] = val;
	};

	// void erase(const KeyT & key, std::size_t lvl){mKeyMaps[lvl].erase(key);};

	// void erase(iterator position, std::size_t lvl){mKeyMaps[lvl].erase(position);};




	void changeSubdomain(const KeyT & key, std::size_t lvl, SubdomainLabelT formerSub, SubomdainLabelT newSub){
		mKeyMaps[lvl][newSub][key] = std::move(mKeyMaps[lvl][formerSub][key]);
		mKeyMaps[lvl][formerSub].erase(key);
	}


};



#endif