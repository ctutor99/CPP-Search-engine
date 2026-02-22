#include "searcheng.h"
#include "combiners.h"

// Complete the necessary code

WebPageSet AndWebPageSetCombiner::combine(const WebPageSet& s1, const WebPageSet& s2) {
    WebPageSet result;
    const WebPageSet &small = (s1.size() < s2.size()) ? s1 : s2;
    const WebPageSet &big   = (&small == &s1) ? s2 : s1;
    for (WebPage* p : small) {
        if (big.find(p) != big.end()) result.insert(p);
    }
    return result;
}
WebPageSet OrWebPageSetCombiner::combine(const WebPageSet& s1, const WebPageSet& s2) {
    WebPageSet result = s1;
    for (WebPage* p : s2) result.insert(p);
    return result;
}
WebPageSet DiffWebPageSetCombiner::combine(const WebPageSet& s1, const WebPageSet& s2) {
    WebPageSet result;
    for (WebPage* p : s1) {
        if (s2.find(p) == s2.end()) result.insert(p);
    }
    return result;
}