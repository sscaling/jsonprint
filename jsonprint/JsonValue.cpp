//
//  JsonValue.cpp
//  jsonprint
//
//  Created by Shaun Scaling on 14/10/2012.
//  Copyright (c) 2012 Shaun Scaling. All rights reserved.
//

#include "JsonValue.h"
#include <sstream>


JsonValue::~JsonValue()
{
    // FIXME: delete objects in map/vector
    if ( m_pObjectValue )
    {
        if ( kObject == m_type )
        {
            delete (object_t*)m_pObjectValue;
        }
        else if ( kArray == m_type )
        {
            delete (array_t*)m_pObjectValue;
        }
    }
}

JsonValue::JsonValue()
{
    m_stringValue = "";
    m_pObjectValue = 0;
}

// http://www.cplusplus.com/articles/D9j2Nwbp/
template <typename T>
std::string NumberToString ( T Number )
{
    std::ostringstream ss;
    ss << Number;
    return ss.str();
}

std::string
JsonValue::toString() const
{
    switch (m_type) {
        case kNull:
        case kNumber:
        case kBoolean:
        case kString:
            return m_stringValue;
            
        case kObject:
            {
                // FIXME: stack will be lost!
                std::string rv("{\n");
                object_t * map = getObject();
                object_t::iterator it = map->begin();
                int count = 0;
                for ( ; it != map->end(); ++it )
                {
                    std::string key   = it->first;
                    JsonValue * value = it->second;
                    
                    if ( count > 0 )
                    {
                        rv.append(",\n");
                    }
                    
                    rv.append("\"");
                    rv.append(key);
                    rv.append("\" : ");
                    
                    rv.append(value->toString());

                    ++count;
                }
                rv.append("\n}\n");
                return rv;
            }
            break;
            
        case kArray:
            {
                std::string rv("[\n");
                array_t * array = getArray();
                array_t::iterator it = array->begin();
                int count = 0;
                for ( ; it != array->end() ; ++it )
                {
                    if ( count > 0 )
                    {
                        rv.append(",\n");
                    }
                    rv.append( NumberToString(count) );
                    rv.append(" => ");
                    rv.append( (*it)->toString() );
                    
                    ++count;
                }
                rv.append("\n]");
                return rv;
            }
            
        default:
            break;
    }
    
    return "";
}


void
JsonValue::pipeJson(std::ostream &os, std::string parentPath) const
{
    switch (m_type) {
            
        case kNull:
        case kNumber:
        case kBoolean:
            os << parentPath << "\t" << m_stringValue << std::endl;
            break;
        case kString:
            os << parentPath << "\t\"" << m_stringValue << "\"" << std::endl;
            break;
            
        case kObject:
            {
                if ( parentPath.empty() )
                {
                    os << m_pathSeparator;
                }
                
                os << parentPath << "\t{}" << std::endl;
                
                object_t * map = getObject();
                object_t::iterator it = map->begin();
                
                for ( ; it != map->end(); ++it )
                {
                    std::string childPath(parentPath);
                    childPath.append(m_pathSeparator).append(it->first);
                    
                    it->second->pipeJson(os, childPath);
                }
            }
            break;

        case kArray:
            {
                os << parentPath << m_pathSeparator <<  "\t[]" << std::endl;
                
                array_t * array = getArray();
                array_t::iterator it = array->begin();
                int count = 0;
                for ( ; it != array->end() ; ++it )
                {
                    std::string childPath(parentPath);
                    childPath.append(m_pathSeparator).append(NumberToString(count));
                    
                    (*it)->pipeJson(os, childPath);
                    
                    ++count;
                }
            }
            break;

            
        default:
            std::cerr << "Couldn't print JSON contents correctly" << std::endl;
            break;
    }
}