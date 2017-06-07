#ifndef _MULTILEVELCONTAINER_H
#define _MULTILEVELCONTAINER_H


#include <map>


template <class LevelContainerT
		  class ContainerT = std::map>
struct MultilevelContainer{
	typedef MultilevelContainer<LevelContainerT>	SelfT;
	typedef std::map 							 	ContainerT;
	typedef std::size_t 							KeyT;

	// these map an integer LEVEL (starting from 0) to
	// a level container
	ContainerT<KeyT, LevelContainerT> 		mLevels; 		

	// LevelContainerT is required to have the followsing:
	// * 						- ::iterator, begin(), end()
	// *						- find() function that returns an iterator
	// MultilevelContainer is required to have the following:
	// *						- LevelContainerT & operator[](std::size_t level)
	// *						- void insert(std::size_t lvl, LevelContainerT lc)
	// *						- void erase(std::size_t lvl)
	// *						- ::iterator, begin(), end()
	// *						- iterator find(std::size_t lvl)



	friend class multilevel_iterator;
	// iterate over key/node pairs for all nodes 
	class multilevel_iterator{
	public:
		typedef multilevel_iterator self_type;
		typedef std::ptrdiff_t difference_type;
	    typedef std::pair<const KeyT, LevelContainerT> value_type;
	    typedef std::pair<const KeyT, LevelContainerT> & reference;
	    typedef std::pair<const KeyT, LevelContainerT> * pointer;
	    typedef std::forward_iterator_tag iterator_category;


	    friend class LevelContainerT::iterator;

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