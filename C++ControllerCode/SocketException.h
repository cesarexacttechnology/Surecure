//
//  SocketException.h
//  SimpleClient
//
//  Created by Mark Davis on 8/7/15.
//  Copyright (c) 2015 Products Engineering. All rights reserved.
//

#ifndef __SimpleClient__SocketException__
#define __SimpleClient__SocketException__

#include <stdio.h>
#include <string>

class SocketException
{
public:
    SocketException ( std::string s ) : m_s ( s ) {};
    ~SocketException (){};
    
    std::string description() { return m_s; }
    
private:
    
    std::string m_s;
    
};

#endif /* defined(__SimpleClient__SocketException__) */
