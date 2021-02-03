#include "Tokenizer.hpp"
#include <math.h>




Tokenizer::Tokenizer()
: inputFile_(),
tokenLength_(0),
lineNumber_(1)
{
}


Tokenizer::~Tokenizer()
{

}

bool Tokenizer::OpenFile(QString fileName)
{
   return inputFile_.OpenFile(fileName);
}


void Tokenizer::CloseFile(QString fileName)
{
   inputFile_.CloseFile(fileName);
}


unsigned int Tokenizer::GetLineNumber()
{
   return lineNumber_;
}

QString Tokenizer::String()
{
   return tokenString_;
}

double Tokenizer::GetValue()
{
   return value_;
}

Tokenizer::Token Tokenizer::GetNextToken()
{
   currentToken_ = Tokenizer::endOfFile_;
   char c;
start:
   SkipWhiteSpace();
   c = inputFile_.GetCurrentCharacter();
   
   if(isCharacter(c))
   {
      tokenLength_ = 0;
      while (isCharacter(c) || isNumber(c) || isUnderScore(c))
      {
         tokenString_[tokenLength_++] = c;
         c = inputFile_.GetNextCharacter();
      }
      tokenString_[tokenLength_] = 0;
      currentToken_ = identifier_;
   }

   else if(isNumber(c))
   {
      value_ = 0;
      unsigned int decimal = 0;
      while (isNumber(c))
      {
         value_ = (value_ * 10) + (c - '0');
         c = inputFile_.GetNextCharacter();
      }
      if (isDot(c))
      {
         c = inputFile_.GetNextCharacter();
         while (isNumber(c))
         {
            value_ = (value_ * 10) + (c - '0');
            decimal++;
            c = inputFile_.GetNextCharacter();
         }
         value_ /= powf(10, decimal);
      }
      else if (isCharacter(c) && c == 'x')
      {
         QString hex;
         c = inputFile_.GetNextCharacter();
         while (isNumber(c))
         {
            hex = hex + QString(c);
            c = inputFile_.GetNextCharacter();
         }
         bool ok = false;
         value_= hex.toUInt(&ok, 16);
      }
      currentToken_ = number_;
   }
   else if(isSingleQuote(c))
   {
      while (!isEndOfLine(c) && !isEndOfFile(c))
      {
         c = inputFile_.GetNextCharacter();
      }
      c = inputFile_.GetNextCharacter();
      goto start;
   }
   else if(isColon(c))
   {
      currentToken_ = colon_;
      c = inputFile_.GetNextCharacter();
   }

   else if(isComma(c))
   {
      currentToken_ = comma_;
      c = inputFile_.GetNextCharacter();
   }

   else if(isDot(c))
   {
      currentToken_ = dot_;
      c = inputFile_.GetNextCharacter();
   }
   else if(isPlus(c))
   {
      currentToken_ = plus_;
      c = inputFile_.GetNextCharacter();
      if (isPlus(c))
      {
         currentToken_ = plusPlus_;
         c = inputFile_.GetNextCharacter();
      }
      if (isEqual(c))
      {
         currentToken_ = plusEqual_;
         c = inputFile_.GetNextCharacter();
      }
   }
   else if(isMinus(c))
   {
      currentToken_ = minus_;
      c = inputFile_.GetNextCharacter();
      if (isMinus(c))
      {
         currentToken_ = minusMinus_;
         c = inputFile_.GetNextCharacter();
      }
      if (isEqual(c))
      {
         currentToken_ = minusEqual_;
         c = inputFile_.GetNextCharacter();
      }
   }
   else if(isStar(c))
   {
      currentToken_ = star_;
      c = inputFile_.GetNextCharacter();
      if (isEqual(c))
      {
         currentToken_ = starEqual_;
         c = inputFile_.GetNextCharacter();
      }
   }
   else if(isForwordSlash(c))
   {
      currentToken_ = forwordSlash_;
      c = inputFile_.GetNextCharacter();
      if (isEqual(c))
      {
         currentToken_ = forwordSlashEqual_;
         c = inputFile_.GetNextCharacter();
      }
   }


   else if(isSemiColon(c))
   {
      currentToken_ = semiColon_;
      c = inputFile_.GetNextCharacter();
   }

   else if(isEqual(c))
   {
      currentToken_ = equal_;
      c = inputFile_.GetNextCharacter();
   }

   else if (isBiggerThan(c))
   {
      currentToken_ = biggerThan_;
      c = inputFile_.GetNextCharacter();
      if (isEqual(c))
      {
         currentToken_ = biggerThanEqual_;
         c = inputFile_.GetNextCharacter();
      }
   }

   else if (isSmallerThan(c))
   {
      currentToken_ = smallerThan_;
      c = inputFile_.GetNextCharacter();
      if (isEqual(c))
      {
         currentToken_ = smallerThanEqual_;
         c = inputFile_.GetNextCharacter();
      }
      if (isBiggerThan(c))
      {
         currentToken_ = notEqual_;
         c = inputFile_.GetNextCharacter();
      }
   }

   else if(isOpenSquareBracket(c))
   {
      currentToken_ = openSquareBracket_;
      c = inputFile_.GetNextCharacter();
   }

   else if(isCloseSquareBracket(c))
   {
      currentToken_ = closeSquareBracket_;
      c = inputFile_.GetNextCharacter();
   }
   else if(isOpenBrackets(c))
   {
      currentToken_ = openBrackets_;
      c = inputFile_.GetNextCharacter();
   }
   else if(isCloseBrackets(c))
   {
      currentToken_ = closeBrackets_;
      c = inputFile_.GetNextCharacter();
   }
   else if(isOperation(c))
   {
      currentToken_ = operation_;
      c = inputFile_.GetNextCharacter();
   }
   else if(isQuotation(c))
   {
      currentToken_ = stringLit_;
      c = inputFile_.GetNextCharacter();
      
      tokenLength_ = 0;
      while (!isQuotation(c) && (!isEndOfLine(c)))
      {
         tokenString_[tokenLength_++] = c;
         c = inputFile_.GetNextCharacter();
      }
      if (isEndOfLine(c))
      {
         currentToken_ = unknownToken_;
      }
      else
      {
         tokenString_[tokenLength_] = 0;
         currentToken_ = stringLit_;
      }

      c = inputFile_.GetNextCharacter();
   }

   else if(isEndOfLine(c))
   {
      currentToken_ = endOfLine_;
      c = inputFile_.GetNextCharacter();
   }
   else if (isEndOfFile(c))
   {
      currentToken_ = endOfFile_;
      c = inputFile_.GetNextCharacter();
   }

   if (currentToken_ == Tokenizer::identifier_)
   {
      currentToken_ = CheckToken();  
   }
  
   return currentToken_;
}

Tokenizer::Token Tokenizer::CheckToken()
{
   QString token = String();
   Token thisToken = Tokenizer::identifier_;
   if (token == "IF"|| token == "if"  || token == "If")
   {
      thisToken = Tokenizer::if_;
   }
   else if (token == "ELSEIF" || token == "elseif"  || token == "Elseif")
   {
      thisToken = Tokenizer::elseif_;
   }
   else if (token == "ELSE" || token == "else"  || token == "Else")
   {
      thisToken = Tokenizer::else_;
   }
   if (token == "ENDIF" || token == "endif"  || token == "Endif")
   {
      thisToken = Tokenizer::endif_;
   }
   else if (token == "THEN" || token == "then"  || token == "Then")
   {
      thisToken = Tokenizer::then_;
   }
   else if (token == "DO" || token == "do"  || token == "Do")
   {
      thisToken = Tokenizer::do_;
   }
   else if (token == "WHILE" || token == "while" || token == "While")
   {
      thisToken = Tokenizer::while_;
   }
   if (token == "WEND" || token == "wend"  || token == "Wend")
   {
      thisToken = Tokenizer::wend_;
   }
   else if (token == "LOOP" || token == "loop" || token == "Loop")
   {
      thisToken = Tokenizer::loop_;
   }
    else if (token == "FOR" || token == "for" || token == "For")
   {
      thisToken = Tokenizer::for_;
   }
   else if (token == "LET" || token == "let"  || token == "Let")
   {
      thisToken = Tokenizer::let_;
   }
   else if (token == "END" || token == "end"  || token == "End")
   {
      thisToken = Tokenizer::end_;
   }
   else if (token == "DIM" || token == "dim"  || token == "Dim")
   {
      thisToken = Tokenizer::dim_;
   }
   else if (token == "AND" || token == "and" || token == "And")
   {
      thisToken = Tokenizer::and_;
   }
   else if (token == "NOT" || token == "not" || token == "Not")
   {
      thisToken = Tokenizer::not_;
   }
   else if (token == "OR" || token == "or" || token == "Or")
   {
      thisToken = Tokenizer::or_;
   }
   else if (token == "AS" || token == "as" || token == "As")
   {
      thisToken = Tokenizer::as_;
   }
   else if (token == "SUB" || token == "sub"  || token == "Sub")
   {
      thisToken = Tokenizer::subroutine_;
   }
   else if (token == "INTEGER" || token == "integer" || token == "Integer")
   {
      thisToken = Tokenizer::integer_;
   }
   else if (token == "STRING" || token == "string" || token == "String")
   {
      thisToken = Tokenizer::string_;
   }
   else if (token == "BOOLEAN" || token == "boolean" || token == "Boolean")
   {
      thisToken = Tokenizer::boolean_;
   }
   else if (token == "TRUE" || token == "true" || token == "True")
   {
      thisToken = Tokenizer::true_;
   }
   else if (token == "FALSE" || token == "false" || token == "False")
   {
      thisToken = Tokenizer::false_;
   }
   else if (token == "PROGRAM" || token == "program" || token == "Program")
   {
      thisToken = Tokenizer::program_;
   }
   else if (token == "ENDSUB" || token == "endsub" || token == "EndSub")
   {
      thisToken = Tokenizer::endSubroutine_;
   }
   else if (token == "DECLARE" || token == "declare" || token == "Declare")
   {
      thisToken = Tokenizer::declare_;
   }
   else if (token == "CONST" || token == "const" || token == "Const")
   {
      thisToken = Tokenizer::const_;
   }
   else if (token == "ENUM" || token == "enum" || token == "Enum")
   {
      thisToken = Tokenizer::enum_;
   }
   else if (token == "ENDENUM" || token == "endenum" || token == "EndEnum")
   {
      thisToken = Tokenizer::endEnum_;
   }
   else if (token == "STRUCTURE" || token == "structure" || token == "Structure")
   {
      thisToken = Tokenizer::struct_;
   }

   else if (token == "WRITE" || token == "write" || token == "Write")
   {
      thisToken = Tokenizer::write_;
   }
   else if (token == "DELAY" || token == "delay" || token == "Delay")
   {
      thisToken = Tokenizer::delay_;
   }
   

   return thisToken;
}


bool Tokenizer::isCharacter(char c)
{
   return ((c >= 'a' && c <='z') || (c >= 'A' && c <='Z'));
}


bool Tokenizer::isNumber(char c)
{
   return (c >= '0' && c <='9');
}


bool Tokenizer::isUnderScore(char c)
{
   return (c == '_');
}

bool Tokenizer::isColon(char c)
{
   return (c == ':');
}

bool Tokenizer::isComma(char c)
{
   return (c == ',');
}

bool Tokenizer::isPlus(char c)
{
   return (c =='+');
}

bool Tokenizer::isMinus(char c)
{
   return (c =='-');
}

bool Tokenizer::isStar(char c)
{
   return (c == '*');
}

bool Tokenizer::isForwordSlash(char c)
{
   return (c == '/');
}

bool Tokenizer::isSemiColon(char c)
{
   return (c == ';');
}

bool Tokenizer::isDot(char c)
{
   return (c == '.');
}

bool Tokenizer::isOpenSquareBracket(char c)
{
   return (c == '[');
}

bool Tokenizer::isCloseSquareBracket(char c)
{
   return (c == ']');
}

bool Tokenizer::isEqual(char c)
{
   return (c == '=');
}

bool Tokenizer::isBiggerThan(char c)
{
   return (c == '>');
}

bool Tokenizer::isSmallerThan(char c)
{
   return (c == '<');
}

bool Tokenizer::isSingleQuote(char c)
{
   return (c == '\'');
}

bool Tokenizer::isEndOfLine(char c)
{
   bool retVal = false;
   if (c == '\n' || c == 10 || c == 13)
   {
      lineNumber_++;
      retVal = true;
   }
   return retVal;
}

bool Tokenizer::isEndOfFile(char c)
{
   return (c == -17 /*0xEF*/);
}

bool Tokenizer::isOpenBrackets(char c)
{
   return (c == '(');
}

bool Tokenizer::isCloseBrackets(char c)
{
   return (c == ')');
}

bool Tokenizer::isOperation(char c)
{
   return (c == '+'  || c == '-' || c == '*' || c == '/');
}

bool Tokenizer::isQuotation(char c)
{
   return (c =='\"');
}

void  Tokenizer::SkipWhiteSpace()
{
   char c;
   c = inputFile_.GetCurrentCharacter();
   while ((c == ' ') || /*(c == 10) || (c == '\n') ||*/ (c =='\t') /*|| (c == 13)||(c == -51)*/)
   {
       c = inputFile_.GetNextCharacter();
   }
}

unsigned int Tokenizer::GetCurrentCharacterLocation()
{
    return inputFile_.GetCurrentCharacterLocation();
}

void Tokenizer::SetCurrentCharacterLocation(unsigned int address)
{
   inputFile_.SetCurrentCharacterLocation(address);
}

void Tokenizer::SetLineNumber(unsigned int lineNumber)
{
   lineNumber_ = lineNumber;
}
