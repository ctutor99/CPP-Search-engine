#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include "md_parser.h"
#include "util.h"

using namespace std;

typedef enum { NORMALTEXT, LINKTEXT, ISLINK, LINKURL } PARSE_STATE_T;

static std::string normalize_term(const std::string &s) {
    std::string out;
    out.reserve(s.size());
    for (unsigned char c : s) out.push_back(std::tolower(c));
    return out;
}

static void flush_term_to_set(std::set<std::string> &dest, std::string &term) {
    if (!term.empty()) {
        dest.insert(normalize_term(term));
        term.clear();
    }
}
// To be completed
void MDParser::parse(std::istream& istr, std::set<std::string>& allSearchableTerms, std::set<std::string>& allOutgoingLinks)
{

    // Remove any contents of the sets before starting to parse.
    allSearchableTerms.clear();
    allOutgoingLinks.clear();

    // Complete your code below

    std::ostringstream buf;
    buf << istr.rdbuf();
    std::string content = buf.str();
    const size_t n = content.size();
    PARSE_STATE_T state = NORMALTEXT;
    std::string term;
    std::string link;
    bool just_closed_bracket = false;

    

    for( size_t i = 0; i < n; ++i)
    {
        char c = content[i];
        bool is_alnum = std::isalnum(static_cast<unsigned char>(c));
        switch(state)
        {
            case NORMALTEXT:
                if(c == '[')
                {
                    flush_term_to_set(allSearchableTerms, term);
                    state = LINKTEXT;
                } else if (is_alnum) {
                    term.push_back(c);
                } else {
                    flush_term_to_set(allSearchableTerms, term);
                }
                break;
            case LINKTEXT:
                if (c == ']') {
                    size_t next_i = i + 1;
                    flush_term_to_set(allSearchableTerms, term);
                    if (next_i < n && content[next_i] == '(') {
                        
                        state = ISLINK;
                        just_closed_bracket = true;
                    } else {
                        state = NORMALTEXT;
                        just_closed_bracket = false;
                    }
                } else {
                    if (is_alnum) term.push_back(c);
                    else flush_term_to_set(allSearchableTerms, term);
                }
                break;
            case ISLINK:
                if (c == '(' && just_closed_bracket) {
                    link.clear();
                    state = LINKURL;
                } else {
                    just_closed_bracket = false;
                }
                break;
            case LINKURL:
                if(c==')')
                {
                    size_t start = 0;
                    while(start < link.size() && std::isspace(static_cast<unsigned char>(link[start]))) ++start;
                    size_t end = link.size();
                    while (end > start && std::isspace(static_cast<unsigned char>(link[end-1]))) --end;
                    if (end > start) {
                        std::string trimmed = link.substr(start, end - start);
                        allOutgoingLinks.insert(trimmed);
                    }
                    flush_term_to_set(allSearchableTerms, term);
                    state = NORMALTEXT;

                }
                else 
                {
                    link.push_back(c);
                }
                break;
                
        }
    }

    flush_term_to_set(allSearchableTerms, term);





}

// To be completed
std::string MDParser::display_text(std::istream& istr)
{
    std::ostringstream buf;
    buf << istr.rdbuf();
    std::string content = buf.str();
    const size_t n = content.size();
    PARSE_STATE_T state = NORMALTEXT;
    std::string out;
    out.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        char c = content[i];
        switch (state) {
            case NORMALTEXT:
                if (c == '[') {
                    out.push_back(c);
                    state = LINKTEXT;
                } else {
                    out.push_back(c);
                }
                break;
            case LINKTEXT:
                if (c == ']') {
                    size_t next_i = i + 1;
                    out.push_back(c); 
                    if (next_i < n && content[next_i] == '(') {
                        state = ISLINK;

                    } else {
                        state = NORMALTEXT;
                    }
                } else {
                    out.push_back(c);
                }
                break;
            case ISLINK:
                if (c == '(') {
                    state = LINKURL;
                } else {
                    state = NORMALTEXT;
                }
                break;
            case LINKURL:
                if(c == ')')
                {
                    state = NORMALTEXT;

                }
                else
                {

                }
                break;
        }
    }
        
    return out;

}
