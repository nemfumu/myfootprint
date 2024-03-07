////////////////////////////////////////////////////////////////////////////////
//                          CONFIGURATION PARSING                             //
////////////////////////////////////////////////////////////////////////////////
//    DATE      PROG.                                                         //
// DD-MMM-YYYY INIT. SIR    MODIFICATION DESCRIPTION                          //
// ---------- ----- ------ -------------------------------------------------- //
// 13/02/2024  E.MF  Creation File                                            //
//                  Manage .ini files                                         //
////////////////////////////////////////////////////////////////////////////////

#ifndef FOOTPRINTAGENT_CONFIGPARSER_H
#define FOOTPRINTAGENT_CONFIGPARSER_H

#pragma once

#include <string>
#include <vector>
#include <map>

typedef std::map<std::string, std::string> Section;
typedef std::map<std::string, Section> Config;

class ConfigParser
{
public:
    ConfigParser(const std::string& pathname);
    virtual ~ConfigParser()
    {}

    Section getSection(const std::string& sectionName) const {
        return m_sections.at(sectionName);
    }

    std::string get(const std::string& sectionName, const std::string& key) const {
        return m_sections.at(sectionName).at(key);
    }

    std::vector<std::string> getSections() const;

private:
    Config parseFile(const std::string& pathname);
    void parseLine(const std::string& line, Config& sections);
    void addSection(const std::string& line, Config& sections);
    void addKeyValuePair(const std::string& line, Config& sections) const;

    std::string m_currentSection;
    const Config m_sections;
};

#endif //FOOTPRINTAGENT_CONFIGPARSER_H
