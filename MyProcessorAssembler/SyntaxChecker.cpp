#include "SyntaxChecker.hpp"
#include "BASIC.hpp"


SyntaxChecker::SyntaxChecker()
: tokenizer_(),
  errorDetected_(false),
  errorsCount_(0),
  globalSymbolTable_(),
  currentScopeSymbolTable_(NULL),
  offset_(0)
{
   startOfStatementList_.append(Tokenizer::const_);
   startOfStatementList_.append(Tokenizer::enum_);
   startOfStatementList_.append(Tokenizer::struct_);
   //startOfStatementList_.append(Tokenizer::endOfLine_);
   startOfStatementList_.append(Tokenizer::while_);
   //startOfStatementList_.append(Tokenizer::identifier_);
   startOfStatementList_.append(Tokenizer::dim_);
   startOfStatementList_.append(Tokenizer::let_);

   startingSubroutine_.append(Tokenizer::subroutine_);
   startingSubroutine_.append(Tokenizer::declare_);
   //startingSubroutine_.append(Tokenizer::dim_);
}

SyntaxChecker::~SyntaxChecker()
{
}

void SyntaxChecker::Parse(QString fileName)
{
   errorsCount_ = 0;
   errorDetected_ = false;
   if (tokenizer_.OpenFile(fileName))
   {
      GetNextToken();
      ParseProgram();
      tokenizer_.SetCurrentCharacterLocation(0);
      tokenizer_.SetLineNumber(0);
      tokenizer_.CloseFile(fileName);
   }
   else
   {
      printf("Cannot find the file specified");
      exit(0);
   }
}

void SyntaxChecker::GetNextToken()
{
   token_ = tokenizer_.GetNextToken();
}

void SyntaxChecker::ErrorMessage(QString errorMessage)
{
   printf("Line %d : Syntax Error : %s\n", tokenizer_.GetLineNumber(), errorMessage.toUtf8().data());
   errorDetected_ = true;
   errorsCount_++;
   emit Error("Line " + QString::number(tokenizer_.GetLineNumber()) + " : Syntax Error : " + errorMessage);
   Synchronize();
}


unsigned int SyntaxChecker::ErrorCount()
{
   return errorsCount_;
}


bool SyntaxChecker::ErrorDetected()
{
   return errorDetected_;
}


void SyntaxChecker::ParseProgram()
{
   while (token_ != Tokenizer::endOfFile_)
   {
      while(token_ == Tokenizer::endOfLine_ && (token_ != Tokenizer::endOfFile_))//skip any extra unneeded lines
      {
         GetNextToken();
      }
      if (token_ == Tokenizer::program_)
      {
         offset_ = 0;
         GetNextToken();
         while(token_ == Tokenizer::endOfLine_ && (token_ != Tokenizer::endOfFile_))
         {
            GetNextToken();
         }
         if (token_ == Tokenizer::identifier_)
         {
            QString programName = tokenizer_.String();
            globalSymbolTable_.EnterSymbolToTable(programName);
            currentScopeSymbolTable_ = globalSymbolTable_.GetSymbolTable(programName);
            GetNextToken();
            if(token_ == Tokenizer::endOfLine_)
            {
               QString programName = tokenizer_.String();
               GetNextToken();
               while ((token_ != Tokenizer::endOfFile_) && (token_ != Tokenizer::end_) && !TokenIn(startingSubroutine_))
               {
                  ParseStatement();
               }
               if (token_ == Tokenizer::end_)
               {
                  GetNextToken();
               }
               else
               {
                  ErrorMessage("Expected 'End' at the end of program");
               }
            }
            else
            {
               GetNextToken();
               ErrorMessage("Expected end of Line");
            }
         }
         else
         {
            GetNextToken();
            ErrorMessage("Expected  'PROGRAM' or PROCEDURE name");
         }
         while(token_ != Tokenizer::subroutine_ && token_ != Tokenizer::endOfFile_)
         {
            GetNextToken();
         }
      }
      else if(token_ == Tokenizer::subroutine_)
      {
         ParseSubroutine();
      }
      else if(token_ == Tokenizer::declare_)
      {
         ParseDeclare();
      }
      else
      {
         ErrorMessage("Expected Function/Procedure or main declaration/ definition.");
         while (token_ != Tokenizer::endOfLine_)
         {
            GetNextToken();
         }
      }
   }
}


void SyntaxChecker::ParseStatement()
{ 
   if (token_ == Tokenizer::let_)
   {
      GetNextToken();
      ParseLetStatement();
   }
   else if (token_ == Tokenizer::const_)
   {
      GetNextToken();
      ParseConstantDeclaration();
   }
   else if (token_ == Tokenizer::enum_)
   {
      GetNextToken();
      ParseEnumDeclaration();
   }
   else if (token_ == Tokenizer::struct_)
   {
      GetNextToken();
      ParseStructureDeclaration();
   }
   else if (token_ == Tokenizer::do_)
   {
      GetNextToken();
      ParseDoStatement();
   }
   else if (token_ == Tokenizer::if_)
   {
      GetNextToken();
      ParseIfStatement();
   }
   else if (token_ == Tokenizer::endOfLine_)
   {
      GetNextToken();
   }
   else if (token_ == Tokenizer::while_)
   {
      GetNextToken();
      ParseWhileStatement();
   }
  /* else if (token_ == Tokenizer::identifier_)
   {
      GetNextToken();
      ParseAssignment();
   }*/
   else if (token_ == Tokenizer::dim_)
   {
      GetNextToken();
      ParseVarDeclaration();
   }
   else if (token_ == Tokenizer::write_)
   {
      GetNextToken();
      ParseWriteMemory();
   }
   else if (token_ == Tokenizer::delay_)
   {
      GetNextToken();
      ParseDelay();
   }
   else 
   {
      GetNextToken();
      ErrorMessage("Unknown BASIC statement '" + tokenizer_.String() + "'");
   }
}


void SyntaxChecker::ParseLetStatement()
{
   if (token_ == Tokenizer::identifier_)
   {
      QString variable = tokenizer_.String();
      if (currentScopeSymbolTable_->SymbolExists(variable))
      {
         GetNextToken();
         if(token_ == Tokenizer::equal_)
         {
            GetNextToken();
            if (token_ != Tokenizer::stringLit_)
            {
               ParseExpression();
            }
            else
            {
               QString literalVal = tokenizer_.String();
               offset_ += literalVal.size() + 1;
               GetNextToken();
            }
         }
         else if (token_ == Tokenizer::openBrackets_)
         {
            GetNextToken();
            ParseExpression();
            if (token_ == Tokenizer::closeBrackets_)
            {
               GetNextToken();
               if (token_ == Tokenizer::equal_)
               {
                  GetNextToken();
                  ParseExpression();
               }
            }
            if (token_ == Tokenizer::identifier_)
            {
               GetNextToken();
               ParseExpression();
               if (token_ == Tokenizer::closeBrackets_)
               {
                  GetNextToken();
                  if (token_ == Tokenizer::equal_)
                  {
                     GetNextToken();
                     ParseExpression();
                  }
               }
            }
         }
         else
         {
            GetNextToken();
            ErrorMessage("Expected '=' after identifier '" + variable + "' definition.");
         }
      }
      else
      {
         QString message = "Undefined variable '" + variable + "'";
         ErrorMessage(message);
         GetNextToken();
      }
   }
   else
   {
      GetNextToken();
      ErrorMessage("Expected  a VARIABLE definition after LET.");
   }
}


void SyntaxChecker::ParseVarDeclaration()
{
   variableIsArray_   = false;
   variableArraySize_ = 0;
   QList<int> dimensions;
   if (token_ == Tokenizer::identifier_)
   {
      variable_ = tokenizer_.String();
      GetNextToken();
      if (token_ == Tokenizer::openBrackets_) // if brackets then it is an array definition
      {
         variableIsArray_ = true;
         GetNextToken();
         startOver2:
         while (token_ != Tokenizer::closeBrackets_ && token_ != Tokenizer::as_ && token_ != Tokenizer::integer_ && token_ != Tokenizer::boolean_ && token_ != Tokenizer::string_)
         {
            startOver1:
            if (token_ == Tokenizer::number_)
            {
               dimensions.append(tokenizer_.GetValue());
               GetNextToken();
               if (token_ == Tokenizer::comma_)
               {
                  GetNextToken();
                  goto startOver1;
               }
            }
            else if (token_ == Tokenizer::identifier_)
            {
               QString dimVar = tokenizer_.String();
               if (currentScopeSymbolTable_->SymbolExists(dimVar))
               {
                  SymbolTable::Type varType = currentScopeSymbolTable_->GetSymbolType(dimVar);
                  if (varType == SymbolTable::Type::constant)
                  {
                     dimensions.append(currentScopeSymbolTable_->GetSymbolValue(dimVar));
                     
                  }
                  else
                  {
                     ErrorMessage("Should Specify a number/constant in Array Declaration.");
                  }
                  GetNextToken();
                  if (token_ == Tokenizer::comma_)
                  {
                     GetNextToken();
                     goto startOver2;
                  }
               }
               else
               {
                  ErrorMessage("Undefined Constant " + tokenizer_.String() + " in array definition");
                  GetNextToken();
                  if (token_ == Tokenizer::comma_)
                  {
                     GetNextToken();
                     goto startOver2;
                  }
               }
            }
            else 
            {
               ErrorMessage("Expected array subscript.");
            }
         }
         if (token_ == Tokenizer::closeBrackets_)
         {
            GetNextToken(); 
         }
         else
         {
            GetNextToken();
            ErrorMessage("Missing ')' in array type definition.");
         }
      }
      if (token_ == Tokenizer::as_)
      {
         GetNextToken();
         ParseType(dimensions);
      }
      else
      {
         GetNextToken();
         ErrorMessage("Expected 'AS' in type definition.");
      }
   }
   else
   {
      GetNextToken();
      ErrorMessage("Expected variable identifier after DIM.");
   }
}


void SyntaxChecker::ParseType(QList<int> dimensions)
{
   if (token_ == Tokenizer::integer_)
   {
      GetNextToken();
      if(token_ == Tokenizer::equal_)
      {
         GetNextToken();
         if (token_ == Tokenizer::number_)
         {
            double value = tokenizer_.GetValue();
            if (!currentScopeSymbolTable_->SymbolExists(variable_))
            {
               SymbolTable::symbolItem* symbol = currentScopeSymbolTable_->GetSymbol(variable_);
               currentScopeSymbolTable_->AddSymbolItem(variable_, offset_++, value, SymbolTable::integer, variableIsArray_, variableArraySize_, dimensions, SymbolTable::Type::integer);
            }
            else
            {
               GetNextToken();
               ErrorMessage("Variable '" + variable_ + "' Already defined in the current scope.");
            }
            GetNextToken();
         }
      }
      else
      {
         if (!currentScopeSymbolTable_->SymbolExists(variable_))
         {
            quint32 arraySize = 1;

            for (int i = 0; i < dimensions.size(); i++)
            {
               arraySize *= dimensions.at(i);
            }
            currentScopeSymbolTable_->AddSymbolItem(variable_, offset_, 0, SymbolTable::integer, variableIsArray_, arraySize, dimensions, SymbolTable::Type::integer);
            offset_ += arraySize;
            //currentScopeSymbolTable_->AddSymbolItem(variable_, offset_++, 0, SymbolTable::integer, variableIsArray_, variableArraySize_, dimensions, SymbolTable::Type::integer);
         }
         else
         {
            GetNextToken();
            ErrorMessage("Variable '" + variable_ + "' Already defined in the current scope.");
         }
      }
   }
   else if (token_ == Tokenizer::string_)
   {
      GetNextToken();
      if(token_ == Tokenizer::equal_)
      {
         GetNextToken();
         if (token_ == Tokenizer::stringLit_)
         {
            QString stringVal = tokenizer_.String();
            GetNextToken();
            if (!currentScopeSymbolTable_->SymbolExists(variable_))
            {
               dimensions.append(1);
               currentScopeSymbolTable_->AddSymbolItem(variable_, offset_++, 0, SymbolTable::string, true, (stringVal.size() + 1), dimensions, SymbolTable::Type::string);
               offset_ += stringVal.size() + 1;
            }
            else
            {
               GetNextToken();
               ErrorMessage("Variable '" + variable_ + "' Already defined in the current scope.");
            }
            GetNextToken();
         }
         else
         {
            GetNextToken();
            ErrorMessage("Expected String literal Initialization.");
         }
      }
      else
      {
         if (!currentScopeSymbolTable_->SymbolExists(variable_))
         {
            //!?????!!! Wrong
            //QString literalVal = tokenizer_.String();
            //dimensions.append(1);
            currentScopeSymbolTable_->AddSymbolItem(variable_, offset_++, 0, SymbolTable::string, true, 0, dimensions, SymbolTable::Type::integer);
            //offset_ += literalVal.size() + 1;
         }
         else
         {
            GetNextToken();
            ErrorMessage("Variable '" + variable_ + "' Already defined in the current scope.");
         }
      }
   }
   else if (token_ == Tokenizer::boolean_)
   {
      GetNextToken();
      if(token_ == Tokenizer::equal_)
      {
         GetNextToken();
         if (token_ == Tokenizer::true_)
         {
            GetNextToken();
            if (!currentScopeSymbolTable_->SymbolExists(variable_))
            {
               currentScopeSymbolTable_->AddSymbolItem(variable_, offset_++, 0, SymbolTable::boolean, variableIsArray_, variableArraySize_, dimensions, SymbolTable::Type::boolean);
            }
            else
            {
               GetNextToken();
               ErrorMessage("Variable '" + variable_ + "' Already defined in the current scope.");
            }
         }
         else if (token_ == Tokenizer::false_)
         {
            GetNextToken();
            if (!currentScopeSymbolTable_->SymbolExists(variable_))
            {
               currentScopeSymbolTable_->AddSymbolItem(variable_, offset_++, 0, SymbolTable::boolean, variableIsArray_, variableArraySize_, dimensions, SymbolTable::Type::boolean);
            }
            else
            {
               GetNextToken();
               ErrorMessage("Variable '" + variable_ + "' Already defined in the current scope.");
            }
         }
         else
         {
            GetNextToken();
            ErrorMessage("Invalid Boolean Initializer " + tokenizer_.String() + ".");
         }
      }
      else
      {
         currentScopeSymbolTable_->AddSymbolItem(variable_, offset_++, 0, SymbolTable::boolean, variableIsArray_, variableArraySize_, dimensions, SymbolTable::Type::boolean);
      }
   }
   else
   {
      GetNextToken();
      ErrorMessage("Illegal type specifier '" + tokenizer_.String() + "'.");
   }
}
  
   

void SyntaxChecker::ParseDoStatement()
{
   if (token_ == Tokenizer::endOfLine_)
   {
      GetNextToken();
   }
   else
   {
      GetNextToken();
      ErrorMessage("Expected end of line after DO.");
   }
   while (token_ != Tokenizer::loop_ && token_ != Tokenizer::endOfFile_)
   {
      ParseStatement();
   }
   if (token_ == Tokenizer::loop_)
   {
      
   }
   else
   {
      ErrorMessage("Expected 'LOOP' after statement block.");
   }
   GetNextToken();
   ParseExpression();
}


void SyntaxChecker::ParseIfStatement()
{
   SymbolTable::Type resultType;

   resultType = ParseExpression();
   if (resultType != SymbolTable::Type::boolean)
   {
      ErrorMessage("Non Boolean result in 'If' expression");
   }
   if (token_ == Tokenizer::then_)
   {
      GetNextToken();
   }
   else
   {
      GetNextToken();
      ErrorMessage("Expected 'THEN' after 'IF' condition.");
   }
   while (token_ != Tokenizer::endif_ && token_ != Tokenizer::else_ && token_ != Tokenizer::elseif_ && token_ != Tokenizer::endOfFile_)
   {
      ParseStatement();
   }
   if (token_ == Tokenizer::elseif_)
   {
      while(token_ == Tokenizer::elseif_)
      {
         GetNextToken();

         ParseExpression();
         if (token_ == Tokenizer::then_)
         {
            GetNextToken();
         }
         else
         {
            GetNextToken();
            ErrorMessage("Expected 'THEN' after 'ELSEIF' condition.");
         }
         while (token_ != Tokenizer::endif_ && token_ != Tokenizer::else_ && token_ != Tokenizer::elseif_ && token_ != Tokenizer::endOfFile_ && token_ != Tokenizer::end_ && token_ != Tokenizer::subroutine_ && token_ != Tokenizer::endSubroutine_)
         {
            ParseStatement();
         }
      }
   }
   if (token_ == Tokenizer::else_)
   {
      GetNextToken();
      while (token_ != Tokenizer::endif_  && token_ != Tokenizer::endOfFile_)
      {
         ParseStatement();
      }
   }
   if (token_ == Tokenizer::endif_)
   {
      GetNextToken();
   }
   else
   {
      GetNextToken();
      ErrorMessage("Expected 'ENDIF' after Statement Block.");
   }
}


void SyntaxChecker::ParseWhileStatement()
{
   ParseExpression();
   while(token_ != Tokenizer::wend_ && token_ != Tokenizer::endOfFile_)
   {   
      ParseStatement();
   }
  
   if (token_ == Tokenizer::wend_)
   {
      GetNextToken();
   }
   else
   {
      GetNextToken();
      ErrorMessage("Expected 'WEND' statement.");
   }
}


void SyntaxChecker::ParseAssignment()
{
   SymbolTable::Type resultType;
   SymbolTable::Type destType;

   QString variable = tokenizer_.String();
   if (currentScopeSymbolTable_->SymbolExists(variable) == true)
   {
      destType = currentScopeSymbolTable_->GetSymbolType(variable);
      if (currentScopeSymbolTable_->GetSymbolType(variable) == SymbolTable::Type::array)
      {
         destType = ParseSubScript(variable);
      }
   }

   if (currentScopeSymbolTable_->SymbolExists(variable) == false)
   {
      QString message = "Undefined variable '" + variable + "'";
      ErrorMessage(message);
   }
   else if (currentScopeSymbolTable_->GetSymbolType(variable) == SymbolTable::Type::constant 
      || currentScopeSymbolTable_->GetSymbolType(variable) == SymbolTable::Type::enumd)
   {
      ErrorMessage("Cannot assign values to constant.");
   }
   
   if (token_ == Tokenizer::equal_            || 
      token_ == Tokenizer::plusEqual_         ||
      token_ == Tokenizer::minusEqual_        ||
      token_ == Tokenizer::starEqual_         ||
      token_ == Tokenizer::forwordSlashEqual_ )
   {
      GetNextToken();
      resultType = ParseExpression();
      if ((destType == SymbolTable::Type::integer) &&
          (resultType == SymbolTable::Type::integer || resultType == SymbolTable::Type::constant || resultType == SymbolTable::Type::enumd))
      {
      }
      else if (destType == SymbolTable::Type::boolean && resultType == SymbolTable::Type::boolean)
      {

      }
      else

      {
         ErrorMessage("Invalid Asignmennt Performed (Incompatible types)");
      }

   }
   else if (token_ == Tokenizer::plusPlus_)
   {
      if (destType != SymbolTable::Type::integer)
      {
         ErrorMessage("Invalid Asignmennt Performed (Incompatible types)");
      }
      GetNextToken();
   }
   else if (token_ == Tokenizer::minusMinus_)
   {
      if (destType != SymbolTable::Type::integer)
      {
         ErrorMessage("Invalid Asignmennt Performed (Incompatible types)");
      }
      GetNextToken();
   }
   else
   {
      GetNextToken();
      ErrorMessage("Bad Assignment statement.");
   }
}
   

void SyntaxChecker::ParseWriteMemory()
{
   if (token_ == Tokenizer::openBrackets_)
   {
      GetNextToken();
      ParseExpression();
      //registerIndex_--
      if (token_ == Tokenizer::comma_)
      {
         GetNextToken();
         ParseExpression();
         //registerIndex_--
         if (token_ == Tokenizer::closeBrackets_)
         {
            GetNextToken();
         }
         else
         {
            ErrorMessage("Expected ')' after value expression in write system Call.");
         }
      }
      else
      {
         ErrorMessage("Expected , after address expression in write system Call.");
      }
   }
   else
   {
      ErrorMessage("Expected ( after write system Call.");
   }
}


void SyntaxChecker::ParseDelay()
{
   if (token_ == Tokenizer::openBrackets_)
   {
      GetNextToken();
      ParseExpression();
      if (token_ == Tokenizer::closeBrackets_)
      {
         GetNextToken();
      }
      else
      {
         ErrorMessage("Expected ')' after value expression in write system Call.");
      }
   }
   else
   {
      ErrorMessage("Expected ( after write system Call.");
   }
}

SymbolTable::Type SyntaxChecker::ParseExpression()
{
   SymbolTable::Type resultType;
   SymbolTable::Type operandType;

   resultType = ParseSimpleExpression();
   if (token_ == Tokenizer::biggerThan_      ||
       token_ == Tokenizer::biggerThanEqual_ ||
       token_ == Tokenizer::smallerThan_     ||
       token_ == Tokenizer::smallerThanEqual_|| 
       token_ == Tokenizer::equal_           ||
       token_ == Tokenizer::notEqual_        )
   {
      GetNextToken();
      operandType = ParseSimpleExpression();
      resultType = SymbolTable::Type::boolean;
   }
   if (token_ == Tokenizer::and_ || token_ == Tokenizer::or_)
   {
      GetNextToken();
      ParseExpression();
   }

   return resultType;
}


SymbolTable::Type SyntaxChecker::ParseSimpleExpression()
{
   SymbolTable::Type resultType;
   SymbolTable::Type operandType;

   if (token_ == Tokenizer::plus_)
   {
      GetNextToken();
   }
   else if (token_ == Tokenizer::minus_)
   {
      GetNextToken();
   }

   resultType = ParseTerm();
  
   while(token_ == Tokenizer::plus_ || token_ == Tokenizer::minus_ && (token_ != Tokenizer::endOfFile_))
   {
      GetNextToken();
      operandType = ParseTerm();
      if ((operandType == SymbolTable::Type::integer || operandType == SymbolTable::Type::constant || operandType == SymbolTable::Type::enumd) &&
          (resultType == SymbolTable::Type::integer || resultType == SymbolTable::Type::constant || resultType == SymbolTable::Type::enumd))
      {
      }
      else
      {
         ErrorMessage("Non Integer operation performed");
      }
   }

   return resultType;
}


SymbolTable::Type SyntaxChecker::ParseTerm()
{
   SymbolTable::Type resultType;
   SymbolTable::Type operandType;

   resultType = ParseFactor();
   while(token_ == Tokenizer::star_ || token_ == Tokenizer::forwordSlash_ && (token_ != Tokenizer::endOfFile_))
   {
      GetNextToken();
      operandType = ParseFactor();
      resultType = SymbolTable::Type::integer;
      if (operandType != SymbolTable::Type::integer && operandType != SymbolTable::Type::constant && operandType != SymbolTable::Type::enumd &&
          resultType != SymbolTable::Type::integer && resultType != SymbolTable::Type::constant && resultType != SymbolTable::Type::enumd)
      {
         ErrorMessage("Non Integer operation performed");
      }
   }

   return resultType;
}



void SyntaxChecker::ParseLiteral(QString literalValue)
{
   for (int i = 0; i < literalValue.size(); i++)
   {

   }
}

SymbolTable::Type SyntaxChecker::ParseFactor()
{
   SymbolTable::Type resultType;

   if(token_ == Tokenizer::identifier_)
   {
      QString variable = tokenizer_.String();
      if (currentScopeSymbolTable_->SymbolExists(variable))
      {
         if (currentScopeSymbolTable_->GetSymbolType(variable) == SymbolTable::Type::procedure)
         {
            GetNextToken();
            resultType = ParseProcedure();
         }
         else if (currentScopeSymbolTable_->GetSymbolType(variable) == SymbolTable::Type::array)
         {
            GetNextToken();
            resultType = ParseSubScript(variable);
         }
         else
         {
            resultType = currentScopeSymbolTable_->GetSymbolType(variable);
            GetNextToken();
         }
      }
      else
      {
         QString message = "Undefined variable '" + variable + "'";
         ErrorMessage(message);
         GetNextToken();
      }
   }
   else if (token_ == Tokenizer::number_)
   {
      GetNextToken();
      resultType = SymbolTable::Type::integer;
   }
   else if (token_ == Tokenizer::not_)
   {
      GetNextToken();
      resultType = ParseFactor();
   }
   else if (token_ == Tokenizer::stringLit_)
   {
      QString literalVal = tokenizer_.String();
      offset_ += literalVal.size() + 1;
      ParseLiteral(literalVal);
      GetNextToken();   
   }
   else if (token_ == Tokenizer::openBrackets_)
   {
      GetNextToken();
      resultType = ParseExpression();
      if (token_ == Tokenizer::closeBrackets_)
      {
          GetNextToken();
      }
      else
      {
         GetNextToken();
         ErrorMessage("Expected ')' after expression.");
      }
   }
   else if (token_ == Tokenizer::true_ || token_ == Tokenizer::false_)
   {
      GetNextToken();
   }
   else
   {
      GetNextToken();
      ErrorMessage("Invalid expression.");
   }
   return resultType;
}


SymbolTable::Type SyntaxChecker::ParseProcedure()
{
   SymbolTable::Type resultType;

   return resultType;
}


SymbolTable::Type SyntaxChecker::ParseSubScript(QString symbolName)
{
   SymbolTable::Type resultType;

   int dimension = 1;
   SymbolTable::symbolItem* symbol = currentScopeSymbolTable_->GetSymbol(symbolName);
   QList<int> dimensionList = symbol->arrayDimensionsLength;
   int numOfDimensions = symbol->numOfDimensions;

   if (token_ == Tokenizer::openBrackets_)
   {
      GetNextToken();
      getNextDim:
      //ParseFactor();//currentScopeSymbolTable_->GetSymbolType(symbolName);
      ParseExpression();
      if (token_ == Tokenizer::comma_)
      {
         dimension++;
         GetNextToken();
         goto getNextDim;
      }
      if (token_ == Tokenizer::closeBrackets_)
      {
         if (numOfDimensions != dimension)
         {
            ErrorMessage("Incompatibe dimension referencing");
         }
         resultType = currentScopeSymbolTable_->GetSymbolArrayType(symbolName);
         GetNextToken();
      }
      else
      {
         ErrorMessage("UnExpected token in Array expression.");
      }
   }
   else
   {
      ErrorMessage("Expected array subscripts following the array name.");
   }

    return resultType;
}


void SyntaxChecker::ParseArgumentList()
{
   offset_ = 1;
scan:
   if (token_ == Tokenizer::identifier_)
   {
      QString parameterName = tokenizer_.String();
      if (token_ == Tokenizer::identifier_)
      {
         GetNextToken();
      }
      else
      {
         ErrorMessage("Missing argument Name.");
      }
      if (token_ == Tokenizer::as_)
      {
            GetNextToken();
      }
      else
      {
         ErrorMessage("Missing 'As' in Function Argument List.");
      }
      SymbolTable::Type itemType = currentScopeSymbolTable_->GetSymbolType(parameterName);
      if (token_ == Tokenizer::integer_)
      {              
         currentScopeSymbolTable_->AddParameterItem(parameterName, SymbolTable::integer);
         GetNextToken();
         if (itemType != SymbolTable::Type::integer)
         {
            ErrorMessage("Incompatible '" + parameterName + "' Definition/Declaration type specified");
         }
         
      }
      else if (token_ == Tokenizer::string_)
      {
         currentScopeSymbolTable_->AddParameterItem(parameterName, SymbolTable::string);
         GetNextToken();
         if (itemType != SymbolTable::Type::string)
         {
            ErrorMessage("Incompatible '" + parameterName + "' Definition/Declaration type specified");
         }
         if (token_ == Tokenizer::comma_)
         {
            GetNextToken();
            goto scan;
         }
      }
      else if (token_ == Tokenizer::boolean_)
      {
         currentScopeSymbolTable_->AddParameterItem(parameterName, SymbolTable::boolean);
         GetNextToken();
         if (itemType != SymbolTable::Type::boolean)
         {
            ErrorMessage("Incompatible '" + parameterName + "' Definition/Declaration type specified");
         }
      }
      else
      {
         GetNextToken();
         ErrorMessage("Invalid Parameter Type.");
      }
      if (token_ == Tokenizer::comma_)
      {
         GetNextToken();
         goto scan;
      }
   }
}




void SyntaxChecker::Synchronize()
{
   while (token_ != Tokenizer::endOfLine_ && token_ != Tokenizer::endOfFile_)
   {
      GetNextToken();
   }
}

void SyntaxChecker::SynchronizeToNextStatement()
{
   while(token_ != Tokenizer::let_       &&
         token_ != Tokenizer::do_        &&
         token_ != Tokenizer::if_        &&
         token_ != Tokenizer::while_     &&
         token_ != Tokenizer::endOfLine_ &&
         token_ != Tokenizer::identifier_ )
   {
      GetNextToken();
   }
}



void SyntaxChecker::ParseDeclare()
{
   QString functionName;
   GetNextToken();
   QList<int> dimensionList;
   if (token_ == Tokenizer::identifier_)
   {
      functionName = tokenizer_.String();
      globalSymbolTable_.EnterSymbolToTable(functionName);
      currentScopeSymbolTable_ = globalSymbolTable_.GetSymbolTable(functionName);
      currentScopeSymbolTable_->AddSymbolItem(functionName, offset_++, 0, SymbolTable::Type::procedure, false, 0, dimensionList, SymbolTable::Type::procedure);
      GetNextToken();
   }
   else
   {
      ErrorMessage("Missing identifier name after 'Declare'");
   }

   if (token_ == Tokenizer::openBrackets_)
   {
      GetNextToken();
   }
   else
   {
      ErrorMessage("Missing '(' in Function Declaration");
   }

   start:
   if (token_ == Tokenizer::identifier_)
   {
      QString parameterName = tokenizer_.String();
      
      GetNextToken();
      if (token_ == Tokenizer::as_)
      {
         GetNextToken();
      }
      else
      {
         ErrorMessage("Missing 'As' in function declaration");
      }

      if (token_ == Tokenizer::integer_)
      {
         currentScopeSymbolTable_->AddParameterItem(parameterName, SymbolTable::integer);
         GetNextToken();
      }
      else if (token_ == Tokenizer::boolean_)
      {
         currentScopeSymbolTable_->AddParameterItem(parameterName, SymbolTable::boolean);
         GetNextToken();
      }
      else if (token_ == Tokenizer::string_)
      {
         currentScopeSymbolTable_->AddParameterItem(parameterName, SymbolTable::string);
         GetNextToken();
      }
      else
      {
         ErrorMessage("Illegal Type in function declaration");
      }

      if (token_ == Tokenizer::comma_)
      {
         GetNextToken();
         goto start;
      }

   }
   if (token_ == Tokenizer::closeBrackets_)
   {
      GetNextToken();
   }
   else
   {
      ErrorMessage("Missing ')' in function declaration");
   }
}


void SyntaxChecker::ParseSubroutine()
{
   GetNextToken();
   if(token_ == Tokenizer::identifier_)
   {
      GetNextToken();
   }
   else
   {
      ErrorMessage("Missing SubroutineName.");
   }
   QString subroutineName = tokenizer_.String();
   if (!globalSymbolTable_.SymbolExist(subroutineName))
   {
      ErrorMessage("Function \"" + subroutineName + "\" Undeclared");
      //exit(0);
      currentScopeSymbolTable_ = globalSymbolTable_.GetSymbolTable("main");
   }
   else
   {
      currentScopeSymbolTable_ = globalSymbolTable_.GetSymbolTable(subroutineName);
   }

   if (token_ == Tokenizer::openBrackets_)
   {
      GetNextToken();
   }
   else
   {
      GetNextToken();
      ErrorMessage("Missing '(' In Subroutine definition.");
   }
   ParseArgumentList();
   if(token_ == Tokenizer::closeBrackets_)
   {
      GetNextToken();
   }
   else
   {
      GetNextToken();
      ErrorMessage("Missing ')' In Subroutine definition.");
   }
   while(token_ != Tokenizer::endSubroutine_ && token_ != Tokenizer::endOfFile_)
   {
      ParseStatement();
   }
   if (token_ == Tokenizer::endSubroutine_)
   {
      GetNextToken();
   }
   else
   {
      GetNextToken();
      ErrorMessage("Missing 'end Subroutine'");
   }
}


void SyntaxChecker::ParseConstantDeclaration()
{
   QList<int> dimensions;
   if (token_ == Tokenizer::identifier_)
   {
      QString constant = tokenizer_.String();
      GetNextToken();
      if (token_ == Tokenizer::equal_)
      {
         GetNextToken();
         if (token_ == Tokenizer::identifier_)
         {
            if (currentScopeSymbolTable_->SymbolExists(tokenizer_.String()))
            {
               SymbolTable::Type identifierType = currentScopeSymbolTable_->GetSymbolType(tokenizer_.String());
               double value = currentScopeSymbolTable_->GetSymbolValue(tokenizer_.String());
               switch(identifierType)
               {
                  case SymbolTable::Type::constant:
                  case SymbolTable::Type::integer:
                     currentScopeSymbolTable_->AddSymbolItem(constant, offset_++, value, identifierType, false, 0, dimensions, SymbolTable::Type::integer);
                     break;
                  case SymbolTable::Type::string:
                      //currentScopeSymbolTable_->AddSymbolItem(constant, 
                      break;
                  default:
                     ErrorMessage("");
                     break;
               }
            }
            else
            {
               ErrorMessage("Undefined Constant '" + tokenizer_.String() + "'");
            }
         }
         else if (token_ == Tokenizer::number_)
         {
            currentScopeSymbolTable_->AddSymbolItem(constant, offset_++, tokenizer_.GetValue(), SymbolTable::Type::constant, false, 0, dimensions, SymbolTable::Type::integer);
         }
         else if (token_ == Tokenizer::stringLit_)
         {
            currentScopeSymbolTable_->AddSymbolItem(constant, offset_++, tokenizer_.GetValue(), SymbolTable::Type::constant, false, 0, dimensions, SymbolTable::Type::string);
         }
         else
         {
            ErrorMessage("Error initializing constant '" + constant + "'" );
         }
         GetNextToken();
      }
   }
}


void SyntaxChecker::ParseEnumDeclaration()
{
   int enumIndex = 0;
   QList<int> dimensions;
   if (token_ == Tokenizer::identifier_)
   {
      GetNextToken();
   }
   else
   {
      ErrorMessage("Missing Enum Name.");
   }
   while(token_ == Tokenizer::endOfLine_)
   {
      GetNextToken();
   }
   if (token_ == Tokenizer::identifier_)
   {
      while(token_ != Tokenizer::endEnum_ && token_ != Tokenizer::endOfFile_ && !TokenIn(startOfStatementList_))
      {
         QString enumIdent = tokenizer_.String();
         GetNextToken();
         if (token_ == Tokenizer::equal_)
         {
            GetNextToken();
            if (token_ == Tokenizer::number_)
            {
               enumIndex = (quint32)tokenizer_.GetValue();
               GetNextToken();
            }
         }
         currentScopeSymbolTable_->AddSymbolItem(enumIdent, 0, enumIndex++, SymbolTable::Type::enumd, false, 0, dimensions, SymbolTable::Type::integer);
         while (token_ == Tokenizer::endOfLine_)
         {
            GetNextToken();
         }
      }
   }
   if (token_ == Tokenizer::endEnum_)
   {
      GetNextToken();
   }
   else
   {
      ErrorMessage("Missing EndEnum.");
   }
}
 

void SyntaxChecker::ParseStructureDeclaration()
{

}


bool SyntaxChecker::TokenIn(QList<Tokenizer::Token> list)
{
   return list.contains(token_);
}
