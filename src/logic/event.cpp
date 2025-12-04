#include "event.h"
#include <algorithm>
#include <cctype>
using std::string;

Event::Event(string name, string note): name(name), note(note){}

string Event::getName() const{
    return name;
}

string Event::getNote() const{
    return note;
}

void Event::setName(const string &updName){
    name = updName;
}

void Event::setNote(const string &updNote){
    note = updNote;
}

bool Event::matchesSearch(const string& searchText) const {
    if (searchText.empty()) return true;
    
    string lowerTitle = name;
    string lowerSearch = searchText;
    
    // convert both strings to lowercase for case-insensitive comparison
    std::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);
    std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);
    
    return lowerTitle.find(lowerSearch) != string::npos;
}