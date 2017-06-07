#ifndef _DOMAINCONTAINER_H
#define _DOMAINCONTAINER_H

#include "NestedContainer.hpp"



// a homogeneous container class that stores different
// subdomain containers
namespace treefidi{

template <class SubdomainLabelT, class SubdomainContainer>
struct DomainContainerTypedef{
	typedef NestedContainer<SubdomainLabelT, SubdomainContainer> type;
};

template <class SubdomainLabelT, class SubdomainContainer>
using DomainContainer = typename DomainContainerTypedef<SubdomainLabelT, SubdomainContainer>::type;



} // end namespace treefidi

#endif