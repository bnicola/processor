#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP
#include <string>
#include "InputFile.hpp"

class Tokenizer
{

public :
   typedef enum
   {
      unknownToken_,
      program_,
      subroutine_,
      endSubroutine_,
      let_,
      do_,
      loop_,
      for_,
      while_,
      if_,
      then_,
      elseif_,
      endif_,
      wend_,
      else_,
      sub_,
      end_,
      endOfFile_,
      endOfLine_,
      identifier_,
      number_,
      equal_,
      notEqual_,
      biggerThan_,
      biggerThanEqual_,
      smallerThan_,
      smallerThanEqual_,
      and_,
      or_,
      not_,
      semiColon_,
      dot_,
      openSquareBracket_,
      closeSquareBracket_,
      openBrackets_,
      closeBrackets_,
      operation_,
      comma_,
      colon_,
      plus_,
      plusPlus_,
      minus_,
      minusMinus_,
      plusEqual_,
      minusEqual_,
      starEqual_,
      forwordSlashEqual_,
      star_,
      forwordSlash_,
      dim_,
      as_,
      integer_,
      string_,
      boolean_,
      true_,
      false_,
      stringLit_,
      declare_,
      const_,
      enum_,
      endEnum_,
      struct_,
      write_,
      delay_
   }Token;
   
   Tokenizer();

   ~Tokenizer();

   Token GetNextToken();

   unsigned int GetLineNumber();

   QString String();

   double GetValue();

   bool OpenFile(QString fileName);

   void CloseFile(QString fileName);

   unsigned int GetCurrentCharacterLocation();

   void SetCurrentCharacterLocation(unsigned int address);

   void SetLineNumber(unsigned int lineNumber);

private:

   Token CheckToken();

   bool isCharacter(char c);

   bool isNumber(char c);

   bool isUnderScore(char c);

   bool isColon(char c);

   bool isComma(char c);

   bool isPlus(char c);
   
   bool isMinus(char c);

   bool isStar(char c);
   
   bool isForwordSlash(char c);

   bool isSemiColon(char c);

   bool isDot(char c);

   bool isOpenSquareBracket(char c);

   bool isCloseSquareBracket(char c);

   bool isEqual(char c);

   bool isBiggerThan(char c);

   bool isSmallerThan(char c);

   bool isSingleQuote(char c);

   bool isOpenBrackets(char c);

   bool isCloseBrackets(char c);

   bool isOperation(char c);

   bool isQuotation(char c);

   bool isEndOfLine(char c);

   bool isEndOfFile(char c);

   void  SkipWhiteSpace();

private:

   Token currentToken_;

   unsigned int lineNumber_;
   
   char  tokenString_[255];
   
   unsigned int tokenLength_;
   
   double value_;
   
   InputFile inputFile_;
};
#endif