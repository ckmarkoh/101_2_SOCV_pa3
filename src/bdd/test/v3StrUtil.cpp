/****************************************************************************
  FileName     [ v3StrUtil.cpp ]
  PackageName  [ v3/src/util ]
  Synopsis     [ String Related Utility Functions. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_STR_UTIL_C
#define V3_STR_UTIL_C

#include <sstream>
#include <cassert>
#include "v3StrUtil.h"

const int
v3StrNCmp(const string& s1, const string& s2, const unsigned& n) {
   assert(n > 0);
   if (s2.size() == 0) return -1;
   unsigned l = s1.size();
   assert(l >= n);
   for (unsigned i = 0; i < l; ++i) {
      if (!s2[i])
         return (i < n)? 1 : 0;
      char ch1 = (isupper(s1[i]))? tolower(s1[i]) : s1[i];
      char ch2 = (isupper(s2[i]))? tolower(s2[i]) : s2[i];
      if (ch1 != ch2)
         return (ch1 - ch2);
   }
   return (l - s2.size());
}

const size_t
v3StrGetTok(const string& str, string& tok, const size_t& pos, const char del) {
   size_t begin = str.find_first_not_of(del, pos);
   if (begin == string::npos) { tok = ""; return begin; }
   size_t end = str.find_first_of(del, begin);
   tok = str.substr(begin, end - begin);
   return end;
}

const bool
v3Str2Int(const string& str, int& num) {
   num = 0;
   size_t i = 0;
   int sign = 1;
   if (str[0] == '-') { sign = -1; i = 1; }
   bool valid = false;
   for (; i < str.size(); ++i) {
      if (isdigit(str[i])) {
         num *= 10;
         num += int(str[i] - '0');
         valid = true;
      }
      else return false;
   }
   num *= sign;
   return valid;
}

const string
v3Int2Str(const int& num) {
   stringstream ss;
   ss << num;
   return ss.str();
}

#endif

