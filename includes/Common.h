//
// Created by admin on 05/02/2024.
//

#ifndef FOOTPRINTAGENT_COMMON_H
#define FOOTPRINTAGENT_COMMON_H

#include <string>
#include <list>
#include <sstream>

#define INT32_LEN  32
#define INT64_LEN  64

class Common {
public:
    Common() = default;
    virtual ~Common()
    = default;

    static std::string getLastErrorAsString(const char* text, int dwError=-1);
    static void safeCopy(char* argDest, char* argSrc, int argDestLen);

    static std::list<std::string> split(const std::string& chaine, char delimiter = ' ')
    {
        std::list<std::string> result;
        std::stringstream strm(chaine);
        std::string word;
        while (std::getline(strm, word, delimiter)) {
            result.push_back(word);
        }
        return result;
    }
};

#endif //FOOTPRINTAGENT_COMMON_H
