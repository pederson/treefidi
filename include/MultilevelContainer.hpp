#ifndef _MULTILEVELCONTAINER_H
#define _MULTILEVELCONTAINER_H

#include "NestedContainer.hpp"

#include <map>



namespace treefidi{

template <class LevelContainer>
struct MultilevelContainerTypedef{
	typedef NestedContainer<std::size_t, LevelContainer> type;
};
template <class LevelContainer>
using MultilevelContainer = typename MultilevelContainerTypedef<LevelContainer>::type;


} // end namespace treefidi

#endif