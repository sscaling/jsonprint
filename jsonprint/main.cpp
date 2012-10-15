//
//  main.cpp
//  jsonprint
//
//  Created by Shaun Scaling on 14/10/2012.
//  Copyright (c) 2012 Shaun Scaling. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <unistd.h>

#include "Json.h"

const char const * kTestFile = "/Users/shaun/Development/jsonprint/jsonprint/test.json";

int main(int argc, const char * argv[])
{
//    using std::cout;
//    using std::ifstream;
//    using std::endl;
//    using std::ios;
//    
//    //
//    // get pwd
//    char buff[256];
//    getcwd( buff, 256 );
//    cout << "Starting in '" << buff << "'" << endl;
//    
//    ifstream::pos_type size = 0;
//    char * jsonStr;
//    
//    //
//    // read in test json
//    ifstream file(kTestFile, ios::in|ios::ate);
//    if ( file.is_open() )
//    {
//        size = file.tellg();
//        jsonStr = new char [size];
//        file.seekg (0, ios::beg);
//        file.read (jsonStr, size);
//        file.close();
//    } else {
//        cout << "Unabled to open '" << kTestFile << "'" << endl;
//    }
//
//    Json* j = new Json(jsonStr, (int)size);
//    
//    cout << "Json input has '" << (j->isValid()?j->toString():"") << "'" << endl;
//    
//    delete [] jsonStr;
//    
//    
//    cout << "exiting normally";
    
    std::string jsonInput;
    
    std::string line;
    while (std::getline(std::cin, line))
    {
        jsonInput.append(line);
    }
    
    Json * j = new Json(jsonInput.c_str(), (int)jsonInput.length());
    
    j->pipe(std::cout);
    
    return 0;
}
