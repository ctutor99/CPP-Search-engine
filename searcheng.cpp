#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <fstream>
#include "searcheng.h"
#include "webpage.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <sstream>

using namespace std;

// Helper function that will extract the extension of a filename
std::string extract_extension(const std::string& filename);

std::string extract_extension(const std::string& filename)
{
    size_t idx = filename.rfind(".");
    if (idx == std::string::npos) {
        return std::string();
    }
    return filename.substr(idx + 1);
}

static std::string normalize_term_local(const std::string &s) {
    std::string out;
    out.reserve(s.size());
    for (unsigned char c : s) out.push_back(std::tolower(c));
    return out;
}
// To be updated as needed 
SearchEng::SearchEng()
{

}

// To be completed
SearchEng::~SearchEng()
{
    for (auto &kv : parsers_) {
        delete kv.second;
    }
    parsers_.clear();
    for (auto &kv : pages_) {
        delete kv.second;
    }
    pages_.clear();
    index_.clear();
}

// Complete
void SearchEng::register_parser(const std::string& extension, PageParser* parser)
{
    if (parsers_.find(extension) != parsers_.end())
    {
        throw std::invalid_argument("parser for provided extension already exists");
    }
    parsers_.insert(make_pair(extension, parser));
}

// Complete
void SearchEng::read_pages_from_index(const std::string& index_file)
{
    ifstream ifile(index_file.c_str());
    if(ifile.fail()) {
        cerr << "Unable to open index file: " << index_file << endl;
    }

    // Parse all the files
    string filename;
    while(ifile >> filename) {
#ifdef DEBUG
        cout << "Reading " << filename << endl;
#endif
        read_page(filename);
    }
    ifile.close();
}

// To be completed
void SearchEng::read_page(const std::string& filename)
{
    if (parsed_pages_.find(filename) != parsed_pages_.end()) return;

    std::string ext = extract_extension(filename);
    if (!ext.empty() && parsers_.find(ext) == parsers_.end()) {
        throw std::logic_error("parser for provided extension does not exist: " + ext);
    }

    std::ifstream ifs(filename.c_str());
    if (ifs.fail()) {
        throw std::invalid_argument("file does not exist: " + filename);
    }

    WebPage* page = nullptr;
    auto pit = pages_.find(filename);
    if (pit == pages_.end()) {
        page = new WebPage(filename);
        pages_.insert(std::make_pair(filename, page));
    } else {
        page = pit->second;
    }

    PageParser* parser = nullptr;
    if (!ext.empty()) {
        auto parser_it = parsers_.find(ext);
        if (parser_it != parsers_.end()) parser = parser_it->second;
    }

    std::set<std::string> terms;
    std::set<std::string> outgoing;
    if (parser) {
        parser->parse(ifs, terms, outgoing);
    }
    page->all_terms(terms);
    for (const std::string &raw : terms) {
        std::string term = normalize_term_local(raw);
        index_[term].insert(page);
    }

    for (const std::string &linkname : outgoing) {
        WebPage* linked = nullptr;
        auto it2 = pages_.find(linkname);
        if (it2 == pages_.end()) {
            linked = new WebPage(linkname);
            pages_.insert(std::make_pair(linkname, linked));
        } else {
            linked = it2->second;
        }
        page->add_outgoing_link(linked);
        linked->add_incoming_link(page);
    }

    parsed_pages_.insert(filename);
    ifs.close();
}

// To be completed
WebPage* SearchEng::retrieve_page(const std::string& page_name) const
{
    auto it = pages_.find(page_name);
    if (it == pages_.end()) return nullptr;
    return it->second;
}

// To be completed
void SearchEng::display_page(std::ostream& ostr, const std::string& page_name) const
{
    auto pit = pages_.find(page_name);
    if (pit == pages_.end()) {
        throw std::invalid_argument("page not found: " + page_name);
    }

    std::string ext = extract_extension(page_name);
    if (!ext.empty() && parsers_.find(ext) == parsers_.end()) {
        throw std::logic_error("no parser registered for extension: " + ext);
    }

    PageParser* parser = nullptr;
    if (!ext.empty()) parser = parsers_.at(ext);

    std::ifstream ifs(page_name.c_str());
    if (ifs.fail()) {
        throw std::runtime_error("unable to open page file: " + page_name);
    }

    if (parser) {
        std::string display = parser->display_text(ifs);
        ostr << display;
    } else {
        // No parser: fallback to raw file output
        std::string line;
        while (std::getline(ifs, line)) {
            ostr << line;
            if (!ifs.eof()) ostr << "\n";
        }
    }
    ifs.close();
}

// To be completed
WebPageSet SearchEng::search(const std::vector<std::string>& terms, WebPageSetCombiner* combiner) const
{
    WebPageSet result;
    if (terms.empty()) return result;

    bool first = true;
    for (const std::string &raw : terms) {
        std::string term = normalize_term_local(raw);

        WebPageSet termSet;
        auto it = index_.find(term);
        if (it != index_.end()) termSet = it->second; // copy

        if (first) {
            result = termSet;
            first = false;
        } else {
            if (combiner) {
                result = combiner->combine(result, termSet);
            } else {
                // default to intersection if no combiner provided
                WebPageSet tmp;
                const WebPageSet &small = (result.size() < termSet.size()) ? result : termSet;
                const WebPageSet &big   = (&small == &result) ? termSet : result;
                for (WebPage* p : small) {
                    if (big.find(p) != big.end()) tmp.insert(p);
                }
                result.swap(tmp);
            }
        }

        if (result.empty()) break; // early exit
    }

    return result;
}

// Add private helper function implementations here

