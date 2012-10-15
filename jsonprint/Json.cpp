//
//  Json.cpp
//  jsonprint
//
//  Created by Shaun Scaling on 14/10/2012.
//  Copyright (c) 2012 Shaun Scaling. All rights reserved.
//

#include "Json.h"
#include <string.h>

using namespace std;

int MAX_STRING_SIZE = 256;


Json::Json( const char * pszJson, const int size )
    : m_iSize(size),
      m_iPos(0),
      m_pJson(NULL),
      m_strLastErr(""),
      m_iLastErrPos(-1)
{
    char* pszTmp = new char[size];
    if ( pszTmp )
    {
        strncpy(pszTmp, pszJson, (size_t)size);
        
        if ( pszTmp )
        {
            m_pszInput = pszTmp;
            
            this->parse();
        }
    }
}

Json::~Json()
{
    delete [] m_pszInput;
    delete m_pJson;
}


const char const *
Json::toString() const
{
    if ( isValid() )
    {
        return m_pszInput;
    }
    else
    {
        return "";
    }
}

bool
Json::isValid() const
{
    return NULL != m_pszInput;
}


//
// from current character pos
void
Json::eatWhiteSpace()
{
    char ch = thisChar();
    
    do {
        if ( isspace(ch) )
        {
//            cout << "eating '" << ch << "' @ " << m_iPos << endl;
        }
        else
        {
            break;
        }
    } while ( getNextChar(ch) );
}

char
Json::thisChar() const
{
//    cout << "thisChar() '" << m_pszInput[m_iPos] << "' @ pos " << m_iPos << endl;
    
    return m_pszInput[m_iPos];
}

char
Json::nextChar() const
{
//    cout << "nextChar() '" << m_pszInput[m_iPos + 1] << "' @ pos " << (m_iPos + 1) << endl;
    
    if ( m_iPos + 1 <= m_iSize )
    {
        return m_pszInput[m_iPos + 1];
    }
    else
    {
        return 0;
    }
}

bool
Json::getNextChar(char &ch)
{
    if ( m_iPos + 1 <= m_iSize )
    {
        skipChar();
        ch = thisChar();
        return true;
    }
    else
    {
        ch = 0;
        return false;
    }
}

void
Json::skipChar()
{
    m_iPos++;
    if (m_iPos >= m_iSize) {
        m_iPos = m_iSize;
    }
}

void
Json::skipChars(int count)
{
    m_iPos += count;
    if ( m_iPos >= m_iSize )
    {
        m_iPos = m_iSize;
    }
}



void
Json::setError(std::string error)
{
    if ( m_strLastErr.empty() )
    {
        m_strLastErr = error;
        m_iLastErrPos = m_iPos;
    }
}

bool
Json::hasError() const
{
    return !m_strLastErr.empty();
}

void
Json::parse()
{
    m_pJson = parseJsonValue();
    
// printf( "Parsed JSON @ 0x%x  :: error ? %s\n", m_pJson, (m_strLastErr.length() > 0?"true" : "false") );
}

void
Json::pipe(std::ostream &os)
{
    if ( m_strLastErr.length() > 0 )
    {
        os << "[ERROR] " << m_strLastErr << " around character " << m_iLastErrPos << endl;
        const char * p = (m_pszInput + m_iLastErrPos - 10);
        if ( p < 0 )
        {
            p = m_pszInput;
        }
        
        os << "around :" << endl << "------------" << endl;
        for ( int i = 0; i < 15; ++i )
        {
            printf("%c", *(p + i));
        }
        os << endl << "-------------" << endl;
        
    }
    else if ( m_pJson )
    {
        //cout << "[SUCCESS] " << endl; //<< root->toString() << endl;
        
        m_pJson->pipeJson(os);
    }
    else
    {
        os << "[Unknown ERROR] no JSON parsed" << endl;
    }

}

JsonValue *
Json::parseJsonArray()
{
    eatWhiteSpace();
    
    if ( '[' == thisChar() )
    {
        skipChar();
        
        JsonValue *parent = JsonValue::withArray();
        
        JsonValue *child;
        
        while ( (NULL != (child = parseJsonValue())) )
        {
            JsonValue::array_t * array = parent->getArray();
            if ( array )
            {
//                cout << "adding " << child->toString() << " to array" << endl;
                array->push_back(child);
            }
            else
            {
                setError("Couldn't locate array in parent");
                break;
            }
            
            eatWhiteSpace();
            
            if ( ',' == thisChar() )
            {
                skipChar();
            }
            else
            {
                break;
            }
        }
        
        eatWhiteSpace();
        
        if ( !hasError() && ']' == thisChar() )
        {
            skipChar();
            return parent;
        }
        else
        {
            delete parent;
        }
    }
    
    return NULL;
}

JsonValue *
Json::parseJsonObject()
{
    eatWhiteSpace();
    
    if ( '{' == thisChar() )
    {
        skipChar();
        
        JsonValue *parent = JsonValue::withObject();

        while ( parseKeyValue(parent->getObject()) )
        {            
            eatWhiteSpace();
            
            if ( ',' == thisChar() )
            {
                skipChar();
            }
            else
            {
                break;
            }
        }
        
        eatWhiteSpace();

        if ( '}' == thisChar() )
        {
            // FIXME: error checking, delete bad object
            //  - space at end of json?
            skipChar();
            return parent;
        }
        else
        {
            setError("Couldn't find end of object");
            delete parent;
        }
        
    }
    return NULL;
}


bool
Json::parseKeyValue(JsonValue::object_t *parentObject)
{    
    eatWhiteSpace();
    
    string key;
    if ( readString(key) )
    {
//        cout << "KEY :: '" << key << "'" << endl;
        
        eatWhiteSpace();
        
        if ( ':' == thisChar() )
        {
            skipChar();
            
//            cout << "looking for " << key << " value" << endl;
            
            JsonValue *value = parseJsonValue();
            if ( value )
            {
//                cout << "inserting " << key << ", " << value->toString() << endl;
                
                JsonValue::pair_t pair(key, value);
                parentObject->insert(pair);
                
                return true;
            }
        }
        setError("Invalid value format");
    }
    else
    {
        setError("Invalid key format");
    }

    return false;
}

JsonValue *
Json::parseJsonValue()
{
    eatWhiteSpace();
    
    // Object / Array / Value
    
    char ch = thisChar();
    switch (ch) {
        case '{':
            return parseJsonObject();
            
        case '[':
            return parseJsonArray();
            break;
            
        default:
            {
                if ( '"' == ch )
                {
                    string str;
                    if ( readString(str) )
                    {
//                        cout << "found string" << endl;
                        return JsonValue::withString(str);
                    }
                }
                else if ( '-' == ch || isdigit(ch) )
                {
                    string num;
                    if ( readNumber(num) )
                    {
//                        cout << "found number" << endl;
                        return JsonValue::withNumber(num);
                    }
                }
                else if ( isBoolean() )
                {
                    bool bValue;
                    if ( readBoolean(bValue) )
                    {
//                        cout << "found boolean" << endl;
                        return JsonValue::withBoolean(bValue);
                    }
                }
                else if ( 'n' == thisChar() )
                {
                    if ( readNull() )
                    {
//                        cout << "found null" << endl;
                        return JsonValue::withNull();
                    }
                }
            }
            break;
    }
    
    return NULL;
}

bool
Json::readString(string &str)
{
    eatWhiteSpace();
    
    //
    // any character until an un-escaped double-quote
    
    char ch = thisChar();
    if ( '"' == ch )
    {
        char pszTmp[MAX_STRING_SIZE];
        memset(pszTmp, 0, MAX_STRING_SIZE);
        
        char* pTmp = pszTmp;
        
        while ( getNextChar(ch) )
        {
            // FIXME: unicode? anything else...
            if ( '"' == ch ) //|| ('\\' == ch && '"' == nextChar()) )
            {
                break;
            }
            
            *pTmp = ch;
            pTmp++;
        }
        
        if ( '"' == ch )
        {
            skipChar();
            // FIXME: ... correct assignment?
            str = string(pszTmp);
            return true;
        }
    }
    
    return false;
}


bool
Json::isBoolean() const
{
    const char * p = (m_pszInput + m_iPos);
    
    if (   ('t' == *p || 'T' == *p)
        && ('r' == *(p+1) || 'R' == *(p+1))
        && ('u' == *(p+2) || 'U' == *(p+2))
        && ('e' == *(p+3) || 'E' == *(p+3)) )
    {
        return true;
    }
    else if (   ('f' == *p || 'F' == *p)
             && ('a' == *(p+1) || 'A' == *(p+1))
             && ('l' == *(p+2) || 'L' == *(p+2))
             && ('s' == *(p+3) || 'S' == *(p+3))
             && ('e' == *(p+4) || 'E' == *(p+4)) )
    {
        return true;
    }
    
    return false;
}

bool
Json::readBoolean(bool &boolean)
{
    eatWhiteSpace();
    
    if ( !isBoolean() )
    {
        setError("trying to parse a boolean that isn't a boolean!");
        return false;
    }
    
    if ( 't' == thisChar() || 'T' == thisChar() )
    {
        skipChars(4);
        boolean = true;
    }
    else
    {
        skipChars(5);
        boolean = false;
    }
    
    return true;
}

#define PUSH_CHAR(x)    *pTmp = (x); \
                        pTmp++;

bool
Json::readNumber(string &num)
{
    eatWhiteSpace();
    
    // json.org spec
    // [-]?[0-9]+(\.[0-9]*)?[eE](-|+)?[0-9]
    
    char ch = thisChar();
    bool bGoodState = false;
    if ( '-' == ch || isdigit(ch) )
    {   
        if ( !isdigit(ch) && !isdigit(nextChar()) )
        {
            setError("Unexpected number format");
            return false;
        }
        
        char pszTmp[MAX_STRING_SIZE];
        memset(pszTmp, 0, MAX_STRING_SIZE);
        char* pTmp = pszTmp;
        
        PUSH_CHAR(ch);
        
        bGoodState = true;
        
        while (getNextChar(ch) && isdigit(ch) )
        {
            PUSH_CHAR(ch);
        }
        
        
        if ( '.' == ch )
        {
            bGoodState = false;
            
            PUSH_CHAR(ch);
            
            while (getNextChar(ch) && isdigit(ch) )
            {
                bGoodState = true;
                PUSH_CHAR(ch);
            }
        }
        
        if ( bGoodState && ('e' == ch || 'E' == ch) )
        {
            bGoodState = false;
            
            getNextChar(ch);
            PUSH_CHAR('e');
            
            if ( '-' == ch || '+' == ch )
            {
                skipChar();
                PUSH_CHAR(ch);
            }
            
            // don't want to overshoot where we are
            while (isdigit(ch))
            {
                bGoodState = true;
                
                PUSH_CHAR(ch);
                
                getNextChar(ch);
            }
        }
        
        if ( bGoodState )
        {
            num = string(pszTmp);
            return true;
        }
    }
    
    setError("Unexpected number format :: invalid start");
    
    return false;
}

bool
Json::readNull()
{
    eatWhiteSpace();
    
    const char * p = m_pszInput + m_iPos;

    if ('n' == p[0] && 'u' == p[1] && 'l' == p[2] && 'l' == p[3])
    {
        skipChars(4);
        return true;
    }
    
    return false;
}
