////////////////////////////////////////////////////////////////////////////////
//                         PARSER OBJECT - INTERFACE                          //
////////////////////////////////////////////////////////////////////////////////
//    DATE      PROG.                                                         //
// DD-MMM-YYYY INIT. SIR    MODIFICATION DESCRIPTION                          //
// ---------- ----- ------ -------------------------------------------------- //
// 04/02/2024  E.MF  Creation File                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef FOOTPRINTAGENT_PARSEROBJECT_H
#define FOOTPRINTAGENT_PARSEROBJECT_H

#include <minwindef.h>
#include <list>
#include <cstdio>
#include <cstring>

// Defined identifier
#define FIELD_SEPARATOR_CONST       '|'
#define MAX_SWAP_BUFFER             1024
#define MAX_TAMPON_SIZE             (2 * MAX_PATH)


//============================================================================//
// CLASS       :                     IStringParser                            //
// DESCRIPTION : Classe permettant la construction d'une chaine de caracteres //
//               par concatenation de valeurs issues de champs d'une structure//
//               donnee. Elle permet aussi l'extraction de ces champs separes //
//               par un separateur.                                           //
//============================================================================//
class IStringParser
{
public:
             IStringParser(char* argData, int argSize, const char fieldSeparator=FIELD_SEPARATOR_CONST);
    virtual ~IStringParser()
    {
      m_data = nullptr;
    };

    // ------------------------ CLASS PUBLICS METHODS ------------------------
    void reset();

    IStringParser& operator << (char* argVal);
    IStringParser& operator << (char  argVal);
    IStringParser& operator << (ULONG argVal);
    IStringParser& operator << (bool  argVal);
    IStringParser& operator << (int   argVal);
    IStringParser& operator << (UINT  argVal);
    IStringParser& operator << (short argVal);
    IStringParser& operator << (long  argVal);
    IStringParser& operator << (float argVal);
    IStringParser& operator << (double argVal);

    IStringParser& operator >> (char* argVal);
    IStringParser& operator >> (char&  argVal);
    IStringParser& operator >> (ULONG& argVal);
    IStringParser& operator >> (ULONGLONG& argVal);
    IStringParser& operator >> (bool&  argVal);
    IStringParser& operator >> (int&   argVal);
    IStringParser& operator >> (UINT&  argVal);
    IStringParser& operator >> (short& argVal);
    IStringParser& operator >> (long&  argVal);
    IStringParser& operator >> (float& argVal);
    IStringParser& operator >> (double& argVal);

private:
    IStringParser& add(char* argVal);
    char* extract();
    void normalizeFloat(char* argFloat);

    // private properties
    char* m_data;
    int   m_size;
    int   m_iterator;
    char  m_tampon[MAX_TAMPON_SIZE]; // Local Buffer Used by Extract
    const char m_fieldSeparator;
};

//############################################################################//
//                ISTRINGPARSER EXTRACT DATA INLINE FUNCTIONS                 //
//############################################################################//
// Reset Data
inline void IStringParser::reset()
{
   std::memset(m_data, 0, m_size);
   m_iterator = 0;
};

inline IStringParser& IStringParser::operator << (char* argVal)
{
   return add(argVal);
};

inline IStringParser& IStringParser::operator << (char argVal)
{
   char Buffer[2];
   Buffer[0] = argVal;
   Buffer[1] = 0x0;
   return add(Buffer);
};

inline IStringParser& IStringParser::operator << (ULONG argVal)
{
   char Buffer[MAX_PATH];
   memset(Buffer, 0, MAX_PATH);
   sprintf(Buffer, "%lu", argVal);
   return add(Buffer);
};

inline IStringParser& IStringParser::operator << (bool argVal)
{
   char Buffer[2];
   Buffer[0] = ((argVal) ? '1' : '0');
   Buffer[1] = 0x0;
   return add(Buffer);
};

inline IStringParser& IStringParser::operator << (int argVal)
{
   char Buffer[MAX_PATH];
   memset(Buffer, 0, MAX_PATH);
   sprintf(Buffer, "%d", argVal);
   return add(Buffer);
};

inline IStringParser& IStringParser::operator << (UINT argVal)
{
   char Buffer[MAX_PATH];
   memset(Buffer, 0, MAX_PATH);
   sprintf(Buffer, "%d", argVal);
   return add(Buffer);
};

inline IStringParser& IStringParser::operator << (long argVal)
{
   char Buffer[MAX_PATH];
   memset(Buffer, 0, MAX_PATH);
   sprintf(Buffer, "%d", argVal);
   return add(Buffer);
};

inline IStringParser& IStringParser::operator << (short argVal)
{
   char Buffer[MAX_PATH];
   memset(Buffer, 0, MAX_PATH);
   sprintf(Buffer, "%d", argVal);
   return add(Buffer);
};

inline IStringParser& IStringParser::operator << (float argVal)
{
   char Buffer[MAX_PATH];
   memset(Buffer, 0, MAX_PATH);
   sprintf(Buffer, "%f", argVal);
   normalizeFloat(Buffer);
   return add(Buffer);
};

inline IStringParser& IStringParser::operator << (double argVal)
{
   char Buffer[MAX_PATH];
   memset(Buffer, 0, MAX_PATH);
   sprintf(Buffer, "%f", argVal);
   normalizeFloat(Buffer);
   return add(Buffer);
};

/*
 * Indirection methods
 */
inline IStringParser& IStringParser::operator >> (char* argVal)
{
   extract();
   strcpy(argVal, m_tampon);
   return *this;
};

inline IStringParser& IStringParser::operator >> (char& argVal)
{
   extract();
   argVal = m_tampon[0];
   return *this;
};

inline IStringParser& IStringParser::operator >> (ULONG& argVal)
{
   extract();
   argVal = strtol(m_tampon, nullptr, 10);
   return *this;
};

inline IStringParser& IStringParser::operator >> (ULONGLONG& argVal)
{
    extract();
    argVal = strtol(m_tampon, nullptr, 10);
    return *this;
};

inline IStringParser& IStringParser::operator >> (bool& argVal)
{
   extract();
   argVal = (m_tampon[0] == '1');
   return *this;
};

inline IStringParser& IStringParser::operator >> (int& argVal)
{
   extract();
   argVal = strtol(m_tampon, nullptr, 10);
   return *this;
};

inline IStringParser& IStringParser::operator >> (UINT& argVal)
{
   extract();
   argVal = (UINT)strtol(m_tampon, nullptr, 10);
   return *this;
};

inline IStringParser& IStringParser::operator >> (short& argVal)
{
   extract();
   argVal = (short)strtol(m_tampon, nullptr, 10);
   return *this;
};

inline IStringParser& IStringParser::operator >> (long& argVal)
{
   extract();
   argVal = std::strtol(m_tampon, nullptr, 10);
   return *this;
};

inline IStringParser& IStringParser::operator >> (float& argVal)
{
   extract();
   argVal = strtof(m_tampon, nullptr);
   return *this;
};

inline IStringParser& IStringParser::operator >> (double& argVal)
{
   extract();
   argVal = strtod(m_tampon, nullptr);
   return *this;
};

#endif

