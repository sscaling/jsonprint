//
//  JsonValue.cpp
//  jsonprint
//
//  Created by Shaun Scaling on 14/10/2012.
//  Copyright (c) 2012 Shaun Scaling. All rights reserved.
//

#include "JsonValue.h"


JsonValue::~JsonValue()
{
    if ( m_pObjectValue )
    {
        if ( kObject == m_type )
        {
            delete (object_t*)m_pObjectValue;
        }
        else if ( kArray == m_type )
        {
            // FIXME: delete array
        }
    }
}

JsonValue::JsonValue()
{
    m_stringValue = "";
    m_pObjectValue = 0;
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
            return "{ not implemented }";
            
        case kArray:
            return "[ not implemented ]";
            
        default:
            break;
    }
    
    return "";
}