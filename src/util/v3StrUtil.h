/****************************************************************************
  FileName     [ v3StrUtil.h ]
  PackageName  [ v3/src/util ]
  Synopsis     [ String Related Utility Functions. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_STR_UTIL_H
#define V3_STR_UTIL_H

#include <string>
#include <cstring>

using namespace std;

const int v3StrNCmp(const string&, const string&, const unsigned&);
const size_t v3StrGetTok(const string&, string&, const size_t& = 0, const char = ' ');
const bool v3Str2Int(const string&, int&);
const string v3Int2Str(const int&);

#endif
