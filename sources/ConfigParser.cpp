//
// Created by admin on 13/02/2024.
//

#include "../includes/ConfigParser.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>


// The function in this namespace will go into a custom string util library
namespace stringUtil
{
    std::vector<std::string> split(const std::string& chaine, char delimiter = ' ')
    {
        std::vector<std::string> result;
        std::stringstream strm(chaine);
        std::string word;
        while (std::getline(strm, word, delimiter)) {
            result.push_back(word);
        }
        return result;
    }
}

namespace cfg
{
    bool isComment(const std::string& line) {
        return line[0] == ';';
    }

    std::string extractSectionName(const std::string& line) {
        return std::string(line.begin() + 1, line.end() - 1);
    }

    bool isSectionHeading(const std::string& line)
    {
        if (line[0] != '[' || line[line.size() - 1] != ']')
            return false;
        const std::string sectionName = extractSectionName(line);
        return std::all_of(sectionName.begin(), sectionName.end(), [](char c){ return std::isalpha(c); });
    }

    bool isKeyValuePair(const std::string& line) {
        // Assume we have already checked if it's a comment or section header.
        return std::count(line.begin(), line.end(), '=') == 1;
    }

    void ensureSectionIsUnique(const std::string& sectionName, const Config& sections) {
        if (sections.count(sectionName) != 0)
            throw std::runtime_error(sectionName + " appears twice in config file");
    }

    void ensureKeyIsUnique(const std::string& key, const Section& section) {
        if (section.count(key) != 0)
            throw std::runtime_error(key + " appears twice in section");
    }

    void ensureCurrentSection(const std::string& line, const std::string& currentSection) {
        if (currentSection.empty())
            throw std::runtime_error(line + " does not occur within a section");
    }

    std::pair<std::string, std::string> parseKeyValuePair(const std::string& line) {
        std::vector<std::string> pair = stringUtil::split(line, '=');
        return std::pair<std::string, std::string>(pair[0], pair[1]);
    }
}

ConfigParser::ConfigParser(const std::string& pathname) :
m_currentSection(""),
m_sections(parseFile(pathname))
{}

std::vector<std::string> ConfigParser::getSections() const
{
    std::vector<std::string> sectionNames;
    for (auto it = m_sections.begin(); it != m_sections.end(); ++it) {
        sectionNames.push_back(it->first);
    }
    return sectionNames;
}

Config ConfigParser::parseFile(const std::string& pathname)
{
    Config sections;
    std::ifstream input(pathname);
    std::string line;
    while (std::getline(input, line)) {
        parseLine(line, sections);
    }
    return sections;
}

void ConfigParser::parseLine(const std::string& line, Config& sections)
{
    if (cfg::isComment(line))
        return;
    else if (cfg::isSectionHeading(line))
        addSection(line, sections);
    else if (cfg::isKeyValuePair(line))
        addKeyValuePair(line, sections);
}

void ConfigParser::addSection(const std::string& line, Config& sections)
{
    const std::string sectionName = cfg::extractSectionName(line);
    cfg::ensureSectionIsUnique(sectionName, sections);
    sections.insert(std::pair<std::string, Section>(sectionName, Section()));
    m_currentSection = sectionName;
}

void ConfigParser::addKeyValuePair(const std::string& line, Config& sections) const
{
    cfg::ensureCurrentSection(line, m_currentSection);
    const auto keyValuePair = cfg::parseKeyValuePair(line);
    cfg::ensureKeyIsUnique(keyValuePair.first, sections.at(m_currentSection));
    sections.at(m_currentSection).insert(keyValuePair);
}
