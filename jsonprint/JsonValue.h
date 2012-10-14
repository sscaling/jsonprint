//
//  JsonValue.h
//  jsonprint
//
//  Created by Shaun Scaling on 14/10/2012.
//  Copyright (c) 2012 Shaun Scaling. All rights reserved.
//

#ifndef __jsonprint__JsonValue__
#define __jsonprint__JsonValue__

#include <iostream>
#include <map>
#include <vector>

class JsonValue {
    
public:
    
    typedef std::map<std::string, JsonValue*> object_t;
    typedef std::vector<JsonValue*> array_t;
    typedef std::pair<std::string, JsonValue*> pair_t;
    
    // legal JSON values
    enum value_t {
        kString,
        kNumber,
        kObject,
        kArray,
        kBoolean,
        kNull
    };
   
private:
    
    value_t m_type;

    void *m_pObjectValue;
    std::string m_stringValue;
    
private:
    JsonValue();
    
public:
    // factory methods
    static
    JsonValue * withObject()
    {
        JsonValue *value = new JsonValue();
        value->m_type = kObject;
        
        object_t* map = new object_t();
        value->m_pObjectValue = (void*)map;
        
        return value;
    }
    
    static
    JsonValue * withArray()
    {
        JsonValue * value = new JsonValue();
        value->m_type = kArray;
        
        array_t * array = new array_t();
        value->m_pObjectValue = (void*)array;
        
        return value;
    }
    
    static
    JsonValue * withBoolean(bool isTrue)
    {
        JsonValue * value = new JsonValue();
        value->m_type = kBoolean;
        value->m_stringValue = isTrue ? "true" : "false";
        return value;
    }
    
    static
    JsonValue * withString(std::string str)
    {
        JsonValue * value = new JsonValue();
        value->m_type = kString;
        value->m_stringValue = str;
        return value;
    }
    
    static
    JsonValue * withNumber(std::string number)
    {
        JsonValue *value = new JsonValue();
        value->m_type = kNumber;
        value->m_stringValue = number;
        return value;
    }
    
    static
    JsonValue * withNull()
    {
        JsonValue * value = new JsonValue();
        value->m_type = kNull;
        value->m_stringValue = "null";
        return value;
    }
    
    
    value_t getType() const
    {
        return m_type;
    }
    
    object_t*
    getObject() const
    {
        if ( kObject == m_type )
        {
            return (object_t*)m_pObjectValue;
        }
        else
        {
            return NULL;
        }
    }
    
    array_t*
    getArray() const
    {
        if ( kArray == m_type )
        {
            return (array_t*)m_pObjectValue;
        }
        else
        {
            return NULL;
        }
    }
    
    bool
    isPrimitive() const
    {
        return (kObject != m_type && kArray != m_type);
    }
    
    ~JsonValue();
    
    std::string toString() const;
};

#endif /* defined(__jsonprint__JsonValue__) */
