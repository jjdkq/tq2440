/* -*- Mode: C++ -*-
 * Worldvisions Weaver Software:
 *   Copyright (C) 1997-2002 Net Integration Technologies, Inc.
 * 
 * A UniConfGen for returning only a particular subtree of a given generator.
 */

#ifndef __UNISUBTREEGEN_H
#define __UNISUBTREEGEN_H

#include "unifiltergen.h"

/**
 * A UniConfGen that returns only a particular subtree of a given generator.
 * 
 * Requests going to this generator have the given prefix added to all keys.
 */
class UniSubtreeGen : public UniFilterGen
{
    UniConfKey subkey;
    
public:
    UniSubtreeGen(IUniConfGen *gen, const UniConfKey &_subkey);
    
    virtual UniConfKey keymap(const UniConfKey &key);
    virtual UniConfKey reversekeymap(const UniConfKey &key);
};


#endif // __UNISUBTREEGEN_H
