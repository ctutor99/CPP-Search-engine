#include "cmdhandler.h"
#include "handler.h"
#include "util.h"
using namespace std;

// Complete
QuitHandler::QuitHandler()
{

}

// Complete
QuitHandler::QuitHandler(Handler* next)
    : Handler(next)
{

}

// Complete
bool QuitHandler::canHandle(const std::string& cmd) const
{
    return cmd == "QUIT";

}

// Complete
Handler::HANDLER_STATUS_T QuitHandler::process(SearchEng* eng, std::istream& instr, std::ostream& ostr)
{
    return HANDLER_QUIT;
}

// Complete
PrintHandler::PrintHandler()
{

}

// Complete
PrintHandler::PrintHandler(Handler* next)
    : Handler(next)
{

}

// Complete
bool PrintHandler::canHandle(const std::string& cmd) const
{
    return cmd == "PRINT";

}

// Complete
Handler::HANDLER_STATUS_T PrintHandler::process(SearchEng* eng, std::istream& instr, std::ostream& ostr)
{
    string name;

    if (!(instr >> name)) {
        return HANDLER_ERROR;
    }
    try {
        eng->display_page(ostr, name);
    }
    catch (std::exception& e) {
        return HANDLER_ERROR;
    }
    return HANDLER_OK;
}

// Add code for other handler class implementations below

AndHandler::AndHandler() {}
AndHandler::AndHandler(Handler* next) : Handler(next) {}

bool AndHandler::canHandle(const std::string& cmd) const {
    return cmd == "AND";
}

Handler::HANDLER_STATUS_T AndHandler::process(SearchEng* eng, std::istream& instr, std::ostream& ostr) {
    std::vector<std::string> terms;
    std::string w;
    while (instr >> w) terms.push_back(w);

    AndWebPageSetCombiner comb;
    WebPageSet results = eng->search(terms, &comb);

    display_hits(results, ostr);
    return HANDLER_OK;
}

// OR Handler implementations
OrHandler::OrHandler() {}
OrHandler::OrHandler(Handler* next) : Handler(next) {}

bool OrHandler::canHandle(const std::string& cmd) const {
    return cmd == "OR";
}

Handler::HANDLER_STATUS_T OrHandler::process(SearchEng* eng, std::istream& instr, std::ostream& ostr) {
    std::vector<std::string> terms;
    std::string w;
    while (instr >> w) terms.push_back(w);

    OrWebPageSetCombiner comb;
    WebPageSet results = eng->search(terms, &comb);

    display_hits(results, ostr);
    return HANDLER_OK;
}

// DIFF Handler implementations
DiffHandler::DiffHandler() {}
DiffHandler::DiffHandler(Handler* next) : Handler(next) {}

bool DiffHandler::canHandle(const std::string& cmd) const {
    return cmd == "DIFF";
}

Handler::HANDLER_STATUS_T DiffHandler::process(SearchEng* eng, std::istream& instr, std::ostream& ostr) {
    std::vector<std::string> terms;
    std::string w;
    while (instr >> w) terms.push_back(w);

    DiffWebPageSetCombiner comb;
    WebPageSet results = eng->search(terms, &comb);

    display_hits(results, ostr);
    return HANDLER_OK;
}

// INCOMING Handler implementations
IncomingHandler::IncomingHandler() {}
IncomingHandler::IncomingHandler(Handler* next) : Handler(next) {}

bool IncomingHandler::canHandle(const std::string& cmd) const {
    return cmd == "INCOMING";
}

Handler::HANDLER_STATUS_T IncomingHandler::process(SearchEng* eng, std::istream& instr, std::ostream& ostr) {
    std::string page;
    if (!(instr >> page)) return HANDLER_ERROR;

    WebPage* wp = eng->retrieve_page(page);
    if (!wp) return HANDLER_ERROR;

    const WebPageSet &in = wp->incoming_links();
    ostr << in.size() << std::endl;
    for (WebPage* p : in) {
        ostr << p->filename() << std::endl;
    }
    return HANDLER_OK;
}

// OUTGOING Handler implementations
OutgoingHandler::OutgoingHandler() {}
OutgoingHandler::OutgoingHandler(Handler* next) : Handler(next) {}

bool OutgoingHandler::canHandle(const std::string& cmd) const {
    return cmd == "OUTGOING";
}

Handler::HANDLER_STATUS_T OutgoingHandler::process(SearchEng* eng, std::istream& instr, std::ostream& ostr) {
    std::string page;
    if (!(instr >> page)) return HANDLER_ERROR;

    WebPage* wp = eng->retrieve_page(page);
    if (!wp) return HANDLER_ERROR;

    const WebPageSet &out = wp->outgoing_links();
    ostr << out.size() << std::endl;
    for (WebPage* p : out) {
        ostr << p->filename() << std::endl;
    }
    return HANDLER_OK;
}