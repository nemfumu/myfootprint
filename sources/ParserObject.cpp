//============================================================================//
////////////////////////////////////////////////////////////////////////////////
//                      PARSER OBJECT - IMPLEMENTATION                        //
////////////////////////////////////////////////////////////////////////////////
//    DATE      PROG.                                                         //
// DD-MMM-YYYY INIT. SIR    MODIFICATION DESCRIPTION                          //
// ---------- ----- ------ -------------------------------------------------- //
// 04/02/2024  E.MF  Creation File                                            //
////////////////////////////////////////////////////////////////////////////////
//============================================================================//

#include "../includes/ParserObject.h"

#include <algorithm>
#include <cstring>

#include "../includes/Common.h"

// Constructor
IStringParser::IStringParser(char* argData, int argSize, const char fieldSeparator)
: m_data(argData)
, m_size(argSize)
, m_iterator(0)
, m_tampon()
, m_fieldSeparator(fieldSeparator)
{
   memset(m_tampon, 0, MAX_TAMPON_SIZE);
};

// Building Fonction
IStringParser& IStringParser::add(char* argVal)
{
   if (m_iterator < m_size) {
      int nByte = std::min((int)std::strlen(argVal), m_size-m_iterator-2);
      if (nByte > 0)
         std::memcpy(m_data+m_iterator, argVal, nByte);
      m_iterator += nByte;
      m_data[m_iterator++] = m_fieldSeparator;
   }
   return *this;
};

// Extracting Function
char* IStringParser::extract()
{
   int nByte = 0;
   if (m_iterator < m_size) {
      int offset = m_iterator;
      while((m_iterator < m_size) && m_data[m_iterator] && (m_data[m_iterator] != m_fieldSeparator))
         m_iterator++;
       nByte = std::min(m_iterator-offset, MAX_TAMPON_SIZE-1);
      if (nByte > 0)
         memcpy(m_tampon, m_data+offset, nByte);
      m_iterator++;
   }
   m_tampon[nByte] = 0x0;
   return m_tampon;
};


// Trim zero decimal after dot.

void IStringParser::normalizeFloat(char* argFloat)
{
   try {
      if (std::string(argFloat).find('.') != std::string::npos) {
         size_t index = std::strlen(argFloat) - 1;
         while (index >= 0 && argFloat[index] == '0')
             argFloat[index--] = 0x0;

         if (argFloat[index] == '.')
            argFloat[index--] = 0x0;
      }
   }
   catch(...) {
      throw Common::getLastErrorAsString("StringParser::NormalizeFloat()");
   }
};
