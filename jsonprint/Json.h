//
//  Json.h
//  jsonprint
//
//  Created by Shaun Scaling on 14/10/2012.
//  Copyright (c) 2012 Shaun Scaling. All rights reserved.
//

#ifndef __jsonprint__Json__
#define __jsonprint__Json__

#include <iostream>
#include "JsonValue.h"


class Json {
    
private:
    const char *m_pszInput;
    const int m_iSize;
    
    std::string m_strLastErr;
    int m_iLastErrPos;
    
    int m_iPos;
    JsonValue *m_pJson;
    
    
private:
    //
    // don't allow blank initialization
    Json();

    void eatWhiteSpace();
    char thisChar() const;
    char nextChar() const;
    void skipChar();
    void skipChars(int count);
    bool getNextChar(char &ch);

    void setError(std::string error);
    bool hasError() const;
    
    void parse();
    JsonValue* parseJsonObject();
    JsonValue* parseJsonArray();
    bool parseKeyValue(JsonValue::object_t *parentObject);
    JsonValue* parseJsonValue();
    bool readString(std::string &str);
    bool isBoolean() const;
    bool readBoolean(bool &boolean);
    bool readNumber(std::string &num);
    bool readNull();
    

    
public:
    Json( const char * pszJson, const int size );
    ~Json();
    
    const char const * toString() const;
    bool isValid() const;
};


#endif /* defined(__jsonprint__Json__) */
