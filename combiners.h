#ifndef COMBINERS_H
#define COMBINERS_H

#include "searcheng.h"
#include <set>

/*** Define Derived WebPageSetCombiners (for AND, OR, DIFF) classes below ***/
class AndWebPageSetCombiner : public WebPageSetCombiner {
public:
    virtual WebPageSet combine(const WebPageSet& s1, const WebPageSet& s2) override;
};

class OrWebPageSetCombiner : public WebPageSetCombiner {
public:
    virtual WebPageSet combine(const WebPageSet& s1, const WebPageSet& s2) override;
};

class DiffWebPageSetCombiner : public WebPageSetCombiner {
public:
    virtual WebPageSet combine(const WebPageSet& s1, const WebPageSet& s2) override;
};
#endif