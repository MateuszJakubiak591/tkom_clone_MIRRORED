#include "lexer/CharBasicLib.hpp"
#include <cctype>


bool isAlpha(char c) {
   return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isDigit(char c) {
   return c >= '0' && c <= '9';
}

bool isAlphaNumeric(char c) {
   return isAlpha(c) || isDigit(c);
}

bool isAlphaNumericOrUnderscore(char c) {
   return isAlpha(c) || isDigit(c) || c == '_';
}

bool isWhiteSpace(const char c){
   if (c == ' ' || c == '\t' || c == '\r'){
      return true;
   }
   return false;
}