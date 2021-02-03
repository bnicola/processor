#include "Parser.hpp"
#include "BASIC.hpp"
#include <QFileInfo>
#include <qdir.h>

#include <QDate>
#include <QTime>


Parser::Parser()
: tokenizer_(),
  errorDetected_(false),
  labelIndex_(0),
  globalSymbolTable_(),
  offset_(0),
  registerIndex_(1),
  machineCodeFilePath_("memory.list")
{
   startOfStatementList_.append(Tokenizer::const_);
   startOfStatementList_.append(Tokenizer::enum_);
   startOfStatementList_.append(Tokenizer::struct_);
   startOfStatementList_.append(Tokenizer::while_);
   startOfStatementList_.append(Tokenizer::dim_);
   startOfStatementList_.append(Tokenizer::let_);
   startingSubroutine_.append(Tokenizer::subroutine_);
   startingSubroutine_.append(Tokenizer::declare_);
   runtimeStack_ = new Stack();
}


Parser::~Parser()
{
   delete runtimeStack_;
}


void Parser::SetOutputFile(QString fileName)
{
   objectFile_.setFileName(fileName);
   objectFileStream_.reset();
   if (objectFile_.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      objectFileStream_.setDevice(&objectFile_);
   }
   else
   {
      printf("Error opening object file for writting object code.\n");
   }
}

void Parser::SetMachineCodeFile(QString machineCodeFilePath)
{
   machineCodeFilePath_ = machineCodeFilePath;
}


void Parser::Parse(QString fileName)
{
   errorDetected_ = false;
   labelIndex_ = 0;
   QDate date;
   QTime time;
   QString timeString = time.currentTime().toString("hh:mm:ss");
   QString dateString = date.currentDate().toString("ddd MMMM d yy");
   offset_ = 0;
   if (tokenizer_.OpenFile(fileName))
   {
      GetNextToken();
      ParseProgram();
      tokenizer_.SetCurrentCharacterLocation(0);
      tokenizer_.SetLineNumber(1);
      tokenizer_.CloseFile(fileName);
      objectFile_.close();
      QFileInfo assembledFile(fileName);
      QString assembledFileExt = assembledFile.baseName();
      QString assembleFile = assembledFile.absoluteDir().absolutePath() + "/"  + assembledFileExt + ".asm";
      assembler_.Assemble(assembleFile, "../../../" + machineCodeFilePath_);
   }
   else
   {
      printf("Cannot find the file specified");
      exit(0);
   }
}


void Parser::GetNextToken()
{
   token_ = tokenizer_.GetNextToken();
}


void Parser::ErrorMessage(std::string errorMessage)
{
   printf("Syntax Error:: Line %d ::%s", tokenizer_.GetLineNumber(), errorMessage.c_str());
   errorDetected_ = true;
}


bool Parser::ErrorDetected()
{
   return errorDetected_;
}


void Parser::ParseProgram()
{
   objectFileStream_.device()->reset();
   assemblyLineList_.clear();
   while (token_ != Tokenizer::endOfFile_)
   {
      while(token_ == Tokenizer::endOfLine_ && (token_ != Tokenizer::endOfFile_))
      {
         GetNextToken();
      }
      if (token_ == Tokenizer::program_)
      {
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
               objectFileStream_ << ".text:\n\t;PROGRAM_" << programName << "\n";
               GetNextToken();
               while ((token_ != Tokenizer::endOfFile_) && (token_ != Tokenizer::end_) && (errorDetected_ == false))
               {
                  ParseStatement();
               }
               if (token_ == Tokenizer::end_)
               {
                  GetNextToken();
               }
               if (errorDetected_ == false)
               {
                  //objectFileStream_ << ";END \tPROGRAM_" << programName << "\n";
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
         GetNextToken();
      }
   }

   OptimizeGeneratedCode();
}

void Parser::OptimizeGeneratedCode()
{
   //optimization phase
   AssemblyLineList temp;
   QString labelPrev;
   bool labelFound = false;
   for (int i = 0; i < assemblyLineList_.size(); i++)
   {
      if (assemblyLineList_.at(i)->label.isEmpty() == false)
      {
         labelPrev = assemblyLineList_.at(i)->label;
         labelFound = true;
      }
      else
      {
         if (labelFound == true)
         {
            assemblyLineList_.at(i)->label = labelPrev;
            labelFound = false;
         }
         temp.append(assemblyLineList_.at(i));
      }
   }
   temp.append(assemblyLineList_.at(assemblyLineList_.size() - 1));
   assemblyLineList_.clear();
   assemblyLineList_ = temp;
   AssemblyLineList optimizedAssemblyLineList_;
   for (int i = 0; i < assemblyLineList_.size() - 1; i++)
   {
      QString label1 = assemblyLineList_.at(i)->label;
      QString opcode1 = assemblyLineList_.at(i)->opcode;
      QString dest1 = assemblyLineList_.at(i)->dest;
      QString src1_1 = assemblyLineList_.at(i)->src1;
      QString src2_1 = assemblyLineList_.at(i)->src2;
      QString literal1 = assemblyLineList_.at(i)->literal;

      QString label2 = assemblyLineList_.at(i + 1)->label;
      QString opcode2 = assemblyLineList_.at(i + 1)->opcode;
      QString dest2 = assemblyLineList_.at(i + 1)->dest;
      QString src1_2 = assemblyLineList_.at(i + 1)->src1;
      QString src2_2 = assemblyLineList_.at(i + 1)->src2;
      QString literal2 = assemblyLineList_.at(i + 1)->literal;

      if (opcode1 == "li" && dest1 == src2_2 && dest2 == src1_2)
      {
         
         AssemblyLine* newAssemblyLine = new AssemblyLine;
         newAssemblyLine->label = (label1 == "") ? label2 : label1;
         newAssemblyLine->opcode = opcode2 + "i";
         newAssemblyLine->dest = dest2;
         newAssemblyLine->src1 = src1_2;
         newAssemblyLine->src2 = "";
         newAssemblyLine->literal = literal1;
         optimizedAssemblyLineList_.append(newAssemblyLine);
         i++;
      }
      else if (opcode1 == "lod" && opcode2 == "mov" && dest1 == src1_2)
      {
         AssemblyLine* newAssemblyLine = new AssemblyLine;
         newAssemblyLine->label = (label1 == "") ? label2 : label1;
         newAssemblyLine->opcode = "lod";
         newAssemblyLine->dest = dest2;
         newAssemblyLine->src1 = "";
         newAssemblyLine->src2 = "";
         newAssemblyLine->literal = literal1;
         optimizedAssemblyLineList_.append(newAssemblyLine);
         i++;
      }
      else if (opcode1 == "li" && opcode2 == "mov" && dest1 == src1_2)
      {
         AssemblyLine* newAssemblyLine = new AssemblyLine;
         newAssemblyLine->label = (label1 == "") ? label2 : label1;
         newAssemblyLine->opcode = "li";
         newAssemblyLine->dest = dest2;
         newAssemblyLine->src1 = "";
         newAssemblyLine->src2 = "";
         newAssemblyLine->literal = literal1;
         optimizedAssemblyLineList_.append(newAssemblyLine);
         i++;
      }
      else if (opcode1 == "str" && opcode2 == "lod" && dest1 == dest2 && src2_1 == literal2)
      {
         AssemblyLine* newAssemblyLine = new AssemblyLine;
         newAssemblyLine->label = (label1 == "") ? label2 : label1;
         newAssemblyLine->opcode = "str";
         newAssemblyLine->dest = dest2;
         newAssemblyLine->src1 = src2_1;
         newAssemblyLine->src2 = src2_2;
         newAssemblyLine->literal = "";
         optimizedAssemblyLineList_.append(newAssemblyLine);
         i++;
      }
      else
      {
         optimizedAssemblyLineList_.append(assemblyLineList_.at(i));
      }
   }
   optimizedAssemblyLineList_.append(assemblyLineList_.at(assemblyLineList_.size() - 1));

   QFile optimzedAssembly("test.asm");
   QTextStream optimizedOut(&optimzedAssembly);
   if (optimzedAssembly.open(QIODevice::Text | QIODevice::WriteOnly))
   {
      for (int i = 0; i < optimizedAssemblyLineList_.size(); i++)
      {
         QString label = optimizedAssemblyLineList_.at(i)->label;
         QString opcode1 = optimizedAssemblyLineList_.at(i)->opcode;
         QString dest1 = optimizedAssemblyLineList_.at(i)->dest;
         QString src1 = optimizedAssemblyLineList_.at(i)->src1;
         QString src2 = optimizedAssemblyLineList_.at(i)->src2;
         QString literal = optimizedAssemblyLineList_.at(i)->literal;
         if (label.isEmpty() == false)
         {
            optimizedOut << label << ":";
         }
         if (opcode1.isEmpty() == false)
         {
            if (label.isEmpty() == false)
               optimizedOut << "\t" << opcode1 << " ";
            else
               optimizedOut << "\t\t" << opcode1 << " ";
         }
         if (dest1.isEmpty() == false)
         {
            optimizedOut << dest1 << ", ";
         }
         if (src1.isEmpty() == false)
         {
            optimizedOut << src1 << ", ";
         }
         if (src2.isEmpty() == false)
         {
            optimizedOut << src2;
         }
         if (literal.isEmpty() == false)
         {
            bool ok = false;
            quint32 literalVal = literal.toUInt(&ok);
            if (ok)
            {
               optimizedOut << "0x" << QString::number(literalVal, 16);
            }
            else
            {
               optimizedOut << literal;
            }
            
         }
         optimizedOut << "\n";
      }
   }
   printf("optimized code\n");
   for (int i = 0; i < optimizedAssemblyLineList_.size(); i++)
   {
      printf("%s %s %s %s %s %s\n", optimizedAssemblyLineList_.at(i)->label.toUtf8().data(), optimizedAssemblyLineList_.at(i)->opcode.toUtf8().data(), optimizedAssemblyLineList_.at(i)->dest.toUtf8().data(),
         optimizedAssemblyLineList_.at(i)->src1.toUtf8().data(), optimizedAssemblyLineList_.at(i)->src2.toUtf8().data(), optimizedAssemblyLineList_.at(i)->literal.toUtf8().data());
   }
}

void Parser::ParseStatement()
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
   else if (token_ == Tokenizer::endOfLine_) // endOfLine is a valid(empty) statement
   {
      GetNextToken();
   }
   else 
   {
      ErrorMessage("UnExpected BASIC statement");
      GetNextToken();
   }
}


void Parser::ParseLetStatement()
{
   if (token_ == Tokenizer::identifier_)
   {
      QString variable = tokenizer_.String();

      GetNextToken();
      if(token_ == Tokenizer::equal_)
      {
         GetNextToken();
         if (token_ != Tokenizer::stringLit_)
         {
            ParseExpression();
            registerIndex_--;
            objectFileStream_ << "\t\tstr  $r" << registerIndex_ << ", " << currentScopeSymbolTable_->GetSymbolOffset(variable) << " ($r29)\n";
            AddAssemblyLine("", "str", "$r" + QString::number(registerIndex_), "", QString::number(currentScopeSymbolTable_->GetSymbolOffset(variable)) + " ($r29)", "");
         }
         else
         {
            QString literalVal = tokenizer_.String();
            offset_ += literalVal.size() + 1;
            quint32 offset = currentScopeSymbolTable_->GetSymbolOffset(variable);
            for (int i = 0; i < literalVal.size(); i++)
            {
               objectFileStream_ << "\t\tli  $r" << registerIndex_ << "," << QString::number(literalVal.at(i).toLatin1()) << "\n";
               AddAssemblyLine("", "li", "$r" + QString::number(registerIndex_), "", "", QString::number(literalVal.at(i).toLatin1()));
               objectFileStream_ << "\t\tstr  $r" << registerIndex_ << "," << QString::number(offset) << " ($r29)\n";
               AddAssemblyLine("", "str", "$r" + QString::number(registerIndex_), "", "", QString::number(literalVal.at(i).toLatin1()));
               offset++;
            }
            objectFileStream_ << "\t\tli  $r" << registerIndex_ << "," << "0\n";
            AddAssemblyLine("", "li", "$r" + QString::number(registerIndex_), "", "", "0");
            objectFileStream_ << "\t\tstr  $r" << registerIndex_ << "," << QString::number(offset) << " ($r29)\n";
            AddAssemblyLine("", "str", "$r" + QString::number(registerIndex_), "", "", QString::number(offset) + " ($r29)");
            GetNextToken();
         }
      }
      else if (token_ == Tokenizer::openBrackets_)
      {
         GetNextToken();
         ParseExpression();
         registerIndex_--;   // this register will contain the index of the array resulting from the expression.
         objectFileStream_ << "\t\tmov  $r28, $r" << registerIndex_  << "\n";
         AddAssemblyLine("", "mov", "$r28", "$r" + QString::number(registerIndex_), "", "");
         if (token_ == Tokenizer::closeBrackets_)
         {
            GetNextToken();
            if (token_ == Tokenizer::equal_)
            {
               GetNextToken();
               ParseExpression();
               registerIndex_--;
               objectFileStream_ << "\t\tstr  $r" << registerIndex_ << ", " << currentScopeSymbolTable_->GetSymbolOffset(variable) << " ($r28)\n";
               AddAssemblyLine("", "str", "$r" + QString::number(registerIndex_), QString::number(currentScopeSymbolTable_->GetSymbolOffset(variable)) + " ($r28)", "", "");
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
   }
   else
   {
      GetNextToken();
      ErrorMessage("Expected  a VARIABLE definition after LET.");
   }

}


void Parser::ParseVarDeclaration()
{
   variableIsArray_   = false;
   variableArraySize_ = 0;
   QList<int> dimensions;
   if (token_ == Tokenizer::identifier_)
   {
      variable_ = tokenizer_.String();
      GetNextToken();
      if (token_ == Tokenizer::openBrackets_)
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
               //offset_ += dimensions.at(0);
               if (token_ == Tokenizer::comma_)
               {
                  GetNextToken();
                  goto startOver1;
               }
            }
            else if (token_ == Tokenizer::identifier_)
            {
               QString dimVar = tokenizer_.String();
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
   }
}



void Parser::ParseType(QList<int> dimensions)
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
               //currentScopeSymbolTable_->AddLocalItem(variable_, SymbolTable::integer);
               currentScopeSymbolTable_->AddSymbolItem(variable_ , offset_++, value, SymbolTable::integer, variableIsArray_, variableArraySize_, dimensions, SymbolTable::Type::integer);
               objectFileStream_ << "\t\tmov  [$r7 + " << currentScopeSymbolTable_->GetSymbolOffset(variable_) <<"]," << value << "\n";
               AddAssemblyLine("", "mov", "[$r7" + QString::number(currentScopeSymbolTable_->GetSymbolOffset(variable_)) + "]", "$r" + QString::number(value), "$r" + QString::number(value), "");
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
               currentScopeSymbolTable_->AddSymbolItem(variable_, offset_, 0, SymbolTable::string, true, (stringVal.size() + 1), dimensions, SymbolTable::Type::string);
               offset_ += stringVal.size() + 1;
            }
            else
            {
               GetNextToken();
               //ErrorMessage("Variable " + variable_ + "' Already defined in the current scope.");
            }
            GetNextToken();
         }
      }
      else
      {
         if (!currentScopeSymbolTable_->SymbolExists(variable_))
         {
            //currentScopeSymbolTable_->AddLocalItem(variable_, SymbolTable::string);
            currentScopeSymbolTable_->AddSymbolItem(variable_, offset_++, 0, SymbolTable::string, variableIsArray_, variableArraySize_, dimensions, SymbolTable::Type::string);
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
               //currentScopeSymbolTable_->AddLocalItem(variable_, SymbolTable::boolean);
               currentScopeSymbolTable_->AddSymbolItem(variable_, offset_++, 1, SymbolTable::boolean, variableIsArray_, variableArraySize_, dimensions, SymbolTable::Type::boolean);
               objectFileStream_ << "\t\t$mov  [r7 + " << currentScopeSymbolTable_->GetSymbolOffset(variable_) <<"], 1\n";
               AddAssemblyLine("", "mov", "[$r7" + QString::number(currentScopeSymbolTable_->GetSymbolOffset(variable_)) + "]", "1", "1", "");
            }
         }
         else if (token_ == Tokenizer::false_)
         {
            GetNextToken();
            if (!currentScopeSymbolTable_->SymbolExists(variable_))
            {
               //currentScopeSymbolTable_->AddLocalItem(variable_, SymbolTable::boolean);
               currentScopeSymbolTable_->AddSymbolItem(variable_, offset_++, 0, SymbolTable::boolean, variableIsArray_, variableArraySize_, dimensions, SymbolTable::Type::boolean);
               objectFileStream_ << "\t\tmov  [$r7 + " << currentScopeSymbolTable_->GetSymbolOffset(variable_) <<"], 0\n";
               AddAssemblyLine("", "mov", "[$r7" + QString::number(currentScopeSymbolTable_->GetSymbolOffset(variable_)) + "]", "0", "0", "");
            }
         }
      }
     else
      {
        currentScopeSymbolTable_->AddSymbolItem(variable_, offset_++, 0, SymbolTable::boolean, variableIsArray_, variableArraySize_, dimensions, SymbolTable::Type::boolean);
      }
   }
}
  

void Parser::ParseDoStatement()
{
   int doEntry = labelIndex_++;
   int doExit  = labelIndex_++;
   objectFileStream_ << "\t\t;Do Loop Statement Line : " << tokenizer_.GetLineNumber();
   objectFileStream_ << "LABEL_" << doEntry << ":\n";
   AddAssemblyLine("LABEL_" + QString::number(doEntry), "", "", "", "", "");
   if (token_ == Tokenizer::endOfLine_)
   {
      GetNextToken();
      while (token_ != Tokenizer::loop_)
      {
         ParseStatement();
      }
      if (token_ == Tokenizer::loop_)
      {
         GetNextToken();
         ParseExpression();
         registerIndex_--;
         objectFileStream_ << "\t\tli  $r20, 1\n";
         AddAssemblyLine("" , "li", "$r20", "1", "", "");
         objectFileStream_ << "\t\tjeq  $r" << registerIndex_ << ", $r20, LABEL_" << doExit << "\n";
         AddAssemblyLine("", "jeq", "$r" + QString::number(registerIndex_), "$r20", "", "LABEL_" + QString::number(doExit));
         objectFileStream_ << "\t\tjmp  LABEL_" << doEntry << "\n";
         AddAssemblyLine("", "jmp", "", "", "", "LABEL_" + QString::number(doEntry));
         objectFileStream_ << "LABEL_" << doExit << ":";
         AddAssemblyLine("LABEL_" + QString::number(doExit), "", "", "", "", "");
      }
      else
      {
         GetNextToken();
         ErrorMessage("Expected 'LOOP' after statement block.\n");
      }
     
   }
   else
   {
      GetNextToken();
      ErrorMessage("Expected end of line after DO.");
   }
}


void Parser::ParseIfStatement()
{
   int ifExitLabel   = labelIndex_++;

   objectFileStream_ << "\t\t;If/Elseif/Else Statement Line : " << tokenizer_.GetLineNumber() << "\n";
   ParseExpression();
   int ifTrueLabel   = labelIndex_++;
   int ifFalseLabel  = labelIndex_++;
   //result of the if expression sits on top of the stack
   registerIndex_--;
   objectFileStream_ << "\t\tli  $r20, 1\n";
   AddAssemblyLine("", "li", "$r20", "1", "", "");
   objectFileStream_ << "\t\tjeq  $r" << registerIndex_ << ", $r20, LABEL_" << ifTrueLabel << "\n";
   AddAssemblyLine("", "jeq", "$r" + QString::number(registerIndex_), "$r20", "", "LABEL_" + QString::number(ifTrueLabel));
   objectFileStream_ << "\t\tjmp  IF_FLASE_LABEL_" << ifFalseLabel << "\n";
   AddAssemblyLine("", "jmp", "", "", "", "IF_FLASE_LABEL_" + QString::number(ifFalseLabel));
   objectFileStream_ << "LABEL_" << ifTrueLabel << ":";
   AddAssemblyLine("LABEL_" + QString::number(ifTrueLabel), "", "", "", "", "");

   if (token_ == Tokenizer::then_)
   {
      GetNextToken();
      while (token_ != Tokenizer::endif_ && token_ != Tokenizer::else_ && token_ != Tokenizer::elseif_ && token_ != Tokenizer::endOfFile_)
      {
         ParseStatement();
      }
      objectFileStream_ << "\t\tjmp  IF_EXIT_LABEL_" << ifExitLabel << "\n";
      AddAssemblyLine("", "jmp", "", "", "", "IF_EXIT_LABEL_" + QString::number(ifExitLabel));
      objectFileStream_ << "IF_FLASE_LABEL_" << ifFalseLabel << ":";
      AddAssemblyLine("IF_FLASE_LABEL_" + QString::number(ifFalseLabel), "", "", "", "", "");
      
      if (token_ == Tokenizer::elseif_)
      {
         objectFileStream_ << "\t\t;ELSE_IF Statement Line : " << tokenizer_.GetLineNumber() << "\n";
         while(token_ == Tokenizer::elseif_)
         {
            GetNextToken();
            
            ParseExpression();
            int ifTrueLabel   = labelIndex_++;
            int ifFalseLabel  = labelIndex_++;
            
            registerIndex_--;
            objectFileStream_ << "\t\tli  $r20, 1\n";
            AddAssemblyLine("", "li", "$r20", "1", "", "");
            objectFileStream_ << "\t\tjeq  $r" << registerIndex_ << ", $r20, LABEL_" << ifTrueLabel << "\n";
            AddAssemblyLine("", "jeq", "$r" + QString::number(registerIndex_), "$r20", "", "LABEL_" + QString::number(ifTrueLabel));
            objectFileStream_ << "\t\tjmp  LABEL_" << ifFalseLabel << "\n";
            AddAssemblyLine("", "jmp", "", "", "", "IF_FLASE_LABEL_" + QString::number(ifFalseLabel));
            objectFileStream_ << "LABEL_" << ifTrueLabel << ":";
            AddAssemblyLine("LABEL_" + QString::number(ifTrueLabel), "", "", "", "", "");


            if (token_ == Tokenizer::then_)
            {
               GetNextToken();
               while (token_ != Tokenizer::endif_ && token_ != Tokenizer::else_ && token_ != Tokenizer::elseif_ && token_ != Tokenizer::endOfFile_)
               {
                  ParseStatement();
               }
               objectFileStream_ << "\t\tjmp  LABEL_" << ifExitLabel << "\n";
               AddAssemblyLine("", "jmp", "", "", "", "LABEL_" + QString::number(ifExitLabel));
               objectFileStream_ << "LABEL_" << ifFalseLabel << ":";
               AddAssemblyLine("LABEL_" + QString::number(ifFalseLabel), "", "", "", "", "");
            }
            else
            {
               GetNextToken();
               ErrorMessage("Expected 'THEN' after 'ELSEIF' condition.");
            }
         }
      }
      if (token_ == Tokenizer::else_)
      {
         objectFileStream_ << "\t\t;ELSE  Statement Line : " << tokenizer_.GetLineNumber() << "\n";
         GetNextToken();
         while (token_ != Tokenizer::endif_  && token_ != Tokenizer::endOfFile_)
         {
            ParseStatement();
         }
      }
      objectFileStream_ << "IF_EXIT_LABEL_" << ifExitLabel << ":";
      AddAssemblyLine("IF_EXIT_LABEL_" + QString::number(ifExitLabel), "", "", "", "", "");
      if (token_ == Tokenizer::endif_)
      {
         GetNextToken();
      }
      else
      {
         GetNextToken();
         ErrorMessage("Missing 'ENDIF' .");
      }
   }
   else
   {
      GetNextToken();
      ErrorMessage("Expected THEN after IF condition.");
   }
}


void Parser::ParseWhileStatement()
{
   int whileEntry = labelIndex_++;
   int whileExit  = labelIndex_++;
   int whileBody = labelIndex_++;
   objectFileStream_ << "\t\t;While Statement Line : " << tokenizer_.GetLineNumber() << "\n";
   objectFileStream_ << "LABEL_" << whileEntry << ":";
   AddAssemblyLine("LABEL_" + QString::number(whileEntry), "", "", "", "", "");
   ParseExpression();

   registerIndex_--;
   objectFileStream_ << "\t\tli  $r20, 1\n";
   AddAssemblyLine("", "li", "$r20", "1", "", "");
   objectFileStream_ << "\t\tjeq  $r" << registerIndex_ << ", $r20, LABEL_" << whileBody << "\n";
   AddAssemblyLine("", "jeq", "$r" + QString::number(registerIndex_), "$r20", "", "LABEL_" + QString::number(whileBody));
   objectFileStream_ << "\t\tjmp  LABEL_" << whileExit << "\n";
   AddAssemblyLine("", "jmp", "", "", "", "LABEL_" + QString::number(whileExit));
   
   objectFileStream_ << "LABEL_" << whileBody << ":";
   AddAssemblyLine("LABEL_" + QString::number(whileBody), "", "", "", "", "");
   while(token_ != Tokenizer::wend_ && token_ != Tokenizer::endOfFile_)
   {
      ParseStatement();
   }
   if (token_ == Tokenizer::wend_)
   {
      GetNextToken();

      objectFileStream_ << "\t\tjmp LABEL_" << whileEntry << "\n";
      AddAssemblyLine("", "jmp", "", "", "", "LABEL_" + QString::number(whileEntry));
      objectFileStream_ << "LABEL_" << whileExit << ":";
      AddAssemblyLine("LABEL_" + QString::number(whileExit), "", "", "", "", "");
   }
}


void Parser::ParseAssignment()
{
   objectFileStream_ << "\t\t;Assignment Statement Line : " << tokenizer_.GetLineNumber() << "\n";
   bool variableIsArray = false;
   QString variable = tokenizer_.String();
   if (currentScopeSymbolTable_->GetSymbolType(variable) == SymbolTable::Type::array)
   {
      variableIsArray = true;
      ParseSubScript(variable, variableIsArray);
   }

   if (token_ == Tokenizer::equal_             || 
       token_ == Tokenizer::plusEqual_         ||
       token_ == Tokenizer::minusEqual_        ||
       token_ == Tokenizer::starEqual_         ||
       token_ == Tokenizer::forwordSlashEqual_ )
   {
      Tokenizer::Token token = token_;
      GetNextToken();
      ParseExpression();
      switch(token)
      {
         case Tokenizer::equal_:
            if (variableIsArray)
            {
               registerIndex_--;
               registerIndex_--;
               objectFileStream_ << "\t\tmov  [$r" << registerIndex_  << "], $r" << registerIndex_ + 1 << "\n";
            }
            else
            {
               registerIndex_--;
               objectFileStream_ << "\t\tstr $r" << registerIndex_ << ", "<< currentScopeSymbolTable_->GetSymbolOffset(variable) << " ($r29)\n";
            }
            break;
         case Tokenizer::plusEqual_:
            if (variableIsArray)
            {
               registerIndex_--;
               registerIndex_--;
               objectFileStream_ << "\t\tadd  [$r" << registerIndex_  << "], $r" << registerIndex_ + 1 << "\n";
            }
            else
            {
               registerIndex_--;
               objectFileStream_ << "\t\tadd  [$r7 + " << currentScopeSymbolTable_->GetSymbolOffset(variable)  << "], $r" << registerIndex_  << "\n";
            }
            break;
         case Tokenizer::minusEqual_:
            if (variableIsArray)
            {
               registerIndex_--;
               registerIndex_--;
               objectFileStream_ << "\t\tsub  [$r" << registerIndex_  << "], $r" << registerIndex_ + 1 << "\n";
            }
            else
            {
               registerIndex_--;
               objectFileStream_ << "\t\tsub  [$r7 + " << currentScopeSymbolTable_->GetSymbolOffset(variable)  << "], $r" << registerIndex_  << "\n";
            }
               break;
         case Tokenizer::starEqual_:
            if (variableIsArray)
            {
               registerIndex_--;
               registerIndex_--;
               objectFileStream_ << "\t\tmul  [$r" << registerIndex_  << "], $r" << registerIndex_ + 1 << "\n";
            }
            else
            {
               registerIndex_--;
               objectFileStream_ << "\t\tmul  [$r7 + " << currentScopeSymbolTable_->GetSymbolOffset(variable)  << "], $r" << registerIndex_  << "\n";
            }
            break;
         case Tokenizer::forwordSlashEqual_:
            if (variableIsArray)
            {
               registerIndex_--;
               registerIndex_--;
               objectFileStream_ << "\t\tdiv  [$r" << registerIndex_  << "], $r" << registerIndex_ + 1 << "\n";
            }
            else
            {
               registerIndex_--;
               objectFileStream_ << "\t\tdiv  [$r7 + " << currentScopeSymbolTable_->GetSymbolOffset(variable)  << "], $r" << registerIndex_  << "\n";
            }
            break;
      }
   }
   if (token_ == Tokenizer::plusPlus_)
   {
      if (variableIsArray)
      {
         registerIndex_--;
         objectFileStream_ << "\t\tlod $r" << registerIndex_ << ", " << currentScopeSymbolTable_->GetSymbolOffset(variable) << " ($r29)\n";
         objectFileStream_ << "\t\taddi  $r" << registerIndex_ + 1 << ", $r" << registerIndex_ + 1 << ", 1\n";
         objectFileStream_ << "\t\tmov  [" << variable << " + $r" << registerIndex_  << "], $r" << registerIndex_ + 1 << "\n";
         GetNextToken();
      }
      else
      {
         objectFileStream_ << "\t\tlod $r" << registerIndex_ << ", " << currentScopeSymbolTable_->GetSymbolOffset(variable) << " ($r29)\n";
         objectFileStream_ << "\t\taddi  $r" << registerIndex_ << ", $r" << registerIndex_ << ", 1\n";
         objectFileStream_ << "\t\tstr $r" << registerIndex_ << ", " << currentScopeSymbolTable_->GetSymbolOffset(variable) << " ($r29)\n";
         GetNextToken();
      }
   }
   if (token_ == Tokenizer::minusMinus_)
   {
      if (variableIsArray)
      {
         registerIndex_--;
         objectFileStream_ << "\t\tlod $r" << registerIndex_ << ", " << currentScopeSymbolTable_->GetSymbolOffset(variable) << " ($r29)\n";
         objectFileStream_ << "\t\tsubi  $r" << registerIndex_ + 1 << ", $r" << registerIndex_ + 1 << ", 1\n";
         objectFileStream_ << "\t\tmov  [" << variable << " + $r" << registerIndex_  << "], $r" << registerIndex_ + 1 << "\n";
         GetNextToken();
      }
      else
      {
         objectFileStream_ << "\t\tlod $r" << registerIndex_ << ", " << currentScopeSymbolTable_->GetSymbolOffset(variable) << " ($r29)\n";
         objectFileStream_ << "\t\tsubi  $r" << registerIndex_ + 1 << ", $r" << registerIndex_ + 1 << ", 1\n";
         objectFileStream_ << "\t\tstr $r" << registerIndex_ << ", " << currentScopeSymbolTable_->GetSymbolOffset(variable) << " ($r29)\n";
         GetNextToken();
      }
   }
}



void Parser::ParseWriteMemory()
{
   if (token_ == Tokenizer::openBrackets_)
   {
      GetNextToken();
      ParseExpression();
      registerIndex_--;
      objectFileStream_ << "\t\tmov $r27, $r" << registerIndex_<< "\n";
      AddAssemblyLine("", "mov", "$r27", "$r" + QString::number(registerIndex_), "", "");
      quint32 RegisterContainingaddress = registerIndex_;
      
      if (token_ == Tokenizer::comma_)
      {
         GetNextToken();
         ParseExpression();
         registerIndex_--;
         objectFileStream_ << "\t\tmov $r26, $r" << registerIndex_ << "\n";
         AddAssemblyLine("", "mov", "$r26", "$r" + QString::number(registerIndex_), "", "");
         quint32 RegisterContainingfata = registerIndex_;
         if (token_ == Tokenizer::closeBrackets_)
         {
            objectFileStream_ << "\t\tstr $r26, 0($r27)\n";
            AddAssemblyLine("", "str", "$r26", "0($r27)", "", "");
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


void Parser::ParseDelay()
{
   int delayLabel = labelIndex_++;
   
   if (token_ == Tokenizer::openBrackets_)
   {
      GetNextToken();   
      ParseExpression();
      registerIndex_--;
      int firstRegComp = registerIndex_ + 1;
      int secondRegComp = registerIndex_;
      objectFileStream_ << "\t\tli $r" << firstRegComp << ",0\n";
      AddAssemblyLine("", "li", "$r" + QString::number(firstRegComp), "", "", "0");
      objectFileStream_ << "LABEL_" << QString::number(delayLabel) << ":\n";
      AddAssemblyLine("LABEL_" + QString::number(delayLabel), "", "" , "" , "", "");

      objectFileStream_ << "" << "subi $r" << secondRegComp << ", $r" << secondRegComp << ", 1\n";
      AddAssemblyLine("", "subi", "$r" + QString::number(secondRegComp), "$r" + QString::number(secondRegComp), "", "1");

      objectFileStream_ << "" << "\tjge $r" << secondRegComp << ", $r" << firstRegComp << ", " << "LABEL_" << QString::number(delayLabel) << "\n";
      AddAssemblyLine("", "jge", "$r" + QString::number(secondRegComp), "$r" + QString::number(firstRegComp), "", "LABEL_" + QString::number(delayLabel));

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

void Parser::ParseExpression()
{
   unsigned int thenLabel    = labelIndex_++;
   unsigned int exitIfLabel = labelIndex_++;
   ParseSimpleExpression();
   if (token_ == Tokenizer::biggerThan_      ||
       token_ == Tokenizer::biggerThanEqual_ ||
       token_ == Tokenizer::smallerThan_     ||
       token_ == Tokenizer::smallerThanEqual_|| 
       token_ == Tokenizer::equal_           ||
       token_ == Tokenizer::notEqual_        )
   {
      Tokenizer::Token token = token_;

      GetNextToken();
      ParseSimpleExpression();
      registerIndex_--;
      registerIndex_--;

      switch(token)
      {
      case Tokenizer::biggerThan_:
         objectFileStream_ << "\t\tjg  $r" << registerIndex_ << ", $r" << registerIndex_ + 1 << ", LABEL_" << thenLabel << " \n";
         AddAssemblyLine("", "jg", "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_ + 1), "", "LABEL_" + QString::number(thenLabel));
            break;
      case Tokenizer::biggerThanEqual_:
         objectFileStream_ << "\t\tjge  $r" << registerIndex_ << ", $r" << registerIndex_ + 1 << ", LABEL_" << thenLabel << " \n";
         AddAssemblyLine("", "jge", "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_ + 1), "", "LABEL_" + QString::number(thenLabel));
            break;
      case Tokenizer::smallerThan_:
         objectFileStream_ << "\t\tjl  $r" << registerIndex_ << ", $r" << registerIndex_ + 1 << ", LABEL_" << thenLabel << " \n";
         AddAssemblyLine("", "jl", "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_ + 1), "", "LABEL_" + QString::number(thenLabel));
            break;
      case Tokenizer::smallerThanEqual_:
         objectFileStream_ << "\t\tjle  $r" << registerIndex_ << ", $r" << registerIndex_ + 1 << ", LABEL_" << thenLabel << " \n";
         AddAssemblyLine("", "jle", "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_ + 1), "", "LABEL_" + QString::number(thenLabel));
            break;
      case Tokenizer::equal_:
         objectFileStream_ << "\t\tjeq  $r" << registerIndex_ << ", $r" << registerIndex_ + 1 << ", LABEL_" << thenLabel << " \n";
         AddAssemblyLine("", "jeq", "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_ + 1), "", "LABEL_" + QString::number(thenLabel));
            break;
      case Tokenizer::notEqual_:
         objectFileStream_ << "\t\tjneq  $r" << registerIndex_ << ", $r" << registerIndex_ + 1 << ", LABEL_" << thenLabel << " \n";
         AddAssemblyLine("", "jneq", "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_ + 1), "", "LABEL_" + QString::number(thenLabel));
            break;
      }

      objectFileStream_ << "\t\tli  $r" << registerIndex_ << ", 0 \n";
      AddAssemblyLine("", "li", "$r" + QString::number(registerIndex_), "" , "", "0");
      objectFileStream_ << "\t\tjmp  LABEL_" << exitIfLabel << " \n";
      AddAssemblyLine("", "jmp", "" , "" , "", "LABEL_" + QString::number(exitIfLabel));
      objectFileStream_ << "LABEL_" << thenLabel << ":";
      AddAssemblyLine("LABEL_" + QString::number(thenLabel), "", "", "", "", "");
      objectFileStream_ << "\t\tli  $r" << registerIndex_ << ", 1 \n";
      AddAssemblyLine("", "li", "$r" + QString::number(registerIndex_), "" , "", "1" );
      objectFileStream_ << "LABEL_" << exitIfLabel << ":";
      AddAssemblyLine("LABEL_" + QString::number(exitIfLabel), "", "" , "", "", "");
      registerIndex_++;
      
   }
   if (token_ == Tokenizer::and_ || token_ == Tokenizer::or_)
   {
      Tokenizer::Token token = token_;
      GetNextToken();
      ParseExpression();
      registerIndex_--;
      registerIndex_--;
      switch(token)
      {
      case Tokenizer::and_:
         objectFileStream_ << "\t\tand  $r" << registerIndex_ << ", $r" << registerIndex_ << ",$r" << registerIndex_ + 1 << "\n";
         AddAssemblyLine("", "and", "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_ +  1), "");
         registerIndex_++;
         break;
      case Tokenizer::or_:
         objectFileStream_ << "\t\tor  $r" << registerIndex_ << ", $r" << registerIndex_ << ",$r" << registerIndex_ + 1 << "\n";
         AddAssemblyLine("", "or", "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_ + 1), "");
         registerIndex_++;
         break;
      }
   }
}


void Parser::ParseSimpleExpression()
{
   bool negative = false;
   if (token_ == Tokenizer::plus_)
   {
      GetNextToken();
   }
   else if (token_ == Tokenizer::minus_)
   {
      negative = true;
      GetNextToken();
   }

   ParseTerm();
   if (negative)
   {
      registerIndex_--;
      objectFileStream_ << "\t\tneg  $r" << registerIndex_ << ", $r" << registerIndex_ << ", $r" << registerIndex_ << "\n";
      AddAssemblyLine("", "neg", "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_), "");
      registerIndex_++;
   }
  
   while(token_ == Tokenizer::plus_ || token_ == Tokenizer::minus_ && (token_ != Tokenizer::endOfFile_))
   {
      Tokenizer::Token token = token_;
      
      GetNextToken();
      ParseTerm();
      registerIndex_--;
      registerIndex_--;
      switch(token)
      {
      case Tokenizer::plus_:
            objectFileStream_ << "\t\tadd  $r" << registerIndex_ << ", $r" << registerIndex_ << ", $r" << registerIndex_ + 1 << "\n";
            AddAssemblyLine("", "add", "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_ + 1), "");
            break;
      case Tokenizer::minus_:
            objectFileStream_ << "\t\tsub  $r" << registerIndex_ << ", $r" << registerIndex_ << ", $r" << registerIndex_ + 1 << "\n";
            AddAssemblyLine("", "sub", "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_ + 1), "");
            break;
      }
 
      registerIndex_++;
   }
}


void Parser::ParseTerm()
{
   ParseFactor();
   while(token_ == Tokenizer::star_ || token_ == Tokenizer::forwordSlash_ && (token_ != Tokenizer::endOfFile_))
   {
      Tokenizer::Token token = token_; 
      
      GetNextToken();
      ParseFactor();
      registerIndex_--;
      registerIndex_--;
      switch(token)
      {
         case Tokenizer::star_:
            objectFileStream_ << "\t\tmul  $r" << registerIndex_ << ", $r" << registerIndex_ << ",$r" << registerIndex_ + 1 << "\n";
            AddAssemblyLine("", "mul", "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_ + 1), "");
                       
            break;
         case Tokenizer::forwordSlash_:
            objectFileStream_ << "\t\tdiv  $r" << registerIndex_ << ", $r" << registerIndex_ << ",$r" << registerIndex_ + 1 << "\n";
            AddAssemblyLine("", "div", "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_ + 1), "");
            break;
      }
      registerIndex_++;
   }
}


void Parser::ParseFactor()
{
   if(token_ == Tokenizer::identifier_)
   {
      QString variable = tokenizer_.String();
      SymbolTable::Type type = currentScopeSymbolTable_->GetSymbolType(variable);

      switch(type)
      {
         case SymbolTable::Type::constant: 
            objectFileStream_ << "\t\tli  $r" << registerIndex_ << ", " << "0x" << QString::number(currentScopeSymbolTable_->GetSymbolValue(tokenizer_.String()), 16) << "\n";
            AddAssemblyLine("", "li", "$r" + QString::number(registerIndex_), "", "", QString::number(currentScopeSymbolTable_->GetSymbolValue(tokenizer_.String())));
            registerIndex_++;
            GetNextToken();
            break;
         case SymbolTable::Type::integer:
            objectFileStream_ << "\t\tlod  $r" << registerIndex_ << ", " << currentScopeSymbolTable_->GetSymbolOffset(tokenizer_.String()) << " ($r29)\n";
            AddAssemblyLine("", "lod", "$r" + QString::number(registerIndex_), "", "", QString::number(currentScopeSymbolTable_->GetSymbolOffset(tokenizer_.String())) + " ($r29)");
            registerIndex_++;
            GetNextToken();
            break;
         case SymbolTable::Type::enumd:
            objectFileStream_ << "\t\tli  $r" << registerIndex_ << ", " << "0x" << QString::number(currentScopeSymbolTable_->GetSymbolValue(tokenizer_.String()), 16) << "\n";
            AddAssemblyLine("", "li", "$r" + QString::number(registerIndex_), "", "", QString::number(currentScopeSymbolTable_->GetSymbolValue(tokenizer_.String())));
            registerIndex_++;
            GetNextToken();
            break;
         case SymbolTable::Type::boolean:
            objectFileStream_ << "\t\tlod  $r" << registerIndex_ << ", " << currentScopeSymbolTable_->GetSymbolOffset(tokenizer_.String()) << " ($r29)\n";
            AddAssemblyLine("", "lod", "$r" + QString::number(registerIndex_), "", "", QString::number(currentScopeSymbolTable_->GetSymbolOffset(tokenizer_.String())) + " ($r29)");
            registerIndex_++;
            GetNextToken();
            break;
         case SymbolTable::Type::array:
            //objectFileStream_ << "\t\tlod  $r" << registerIndex_ << ", " << currentScopeSymbolTable_->GetSymbolOffset(tokenizer_.String()) << "($r29)\n";
            GetNextToken();
            ParseSubScript(variable, false);
            break;
         case SymbolTable::Type::procedure:
            GetNextToken();
            ParseProcedure();
            break;
         default:
             GetNextToken();
      }
   }
   else if (token_ == Tokenizer::number_)
   {
      objectFileStream_ << "\t\tli $r" << registerIndex_ << ", " << "0x" << QString::number((uint)tokenizer_.GetValue(), 16) << "\n";
      AddAssemblyLine("", "li", "$r" + QString::number(registerIndex_), "", "", QString::number(tokenizer_.GetValue()));
      registerIndex_++;
      GetNextToken();
   }
   else if (token_ == Tokenizer::not_)
   {
      GetNextToken();
      ParseExpression();
      registerIndex_--;
      objectFileStream_ << "\t\tnot  $r" << registerIndex_ << ", $r" << registerIndex_ << ", $r" << registerIndex_ << "\n";
      AddAssemblyLine("", "not", "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_), "");
      registerIndex_++;
   }
   else if (token_ == Tokenizer::stringLit_)
   {
      QString literalVal = tokenizer_.String();
      offset_ += literalVal.size() + 1;
      quint32 offset = currentScopeSymbolTable_->GetSymbolOffset(tokenizer_.String());
      for (int i = 0; i < literalVal.size(); i++)
      {
         objectFileStream_ << "\t\tli  $r" << registerIndex_ << "," << literalVal.at(i).toLatin1();
         AddAssemblyLine("", "li", "$r" + QString::number(registerIndex_), "", "", QString::number(literalVal.at(i).toLatin1()));
         objectFileStream_ << "\t\tstr  $r" << registerIndex_ << "," << QString::number(offset);
         AddAssemblyLine("", "str", "$r" + QString::number(registerIndex_), "", "", QString::number(offset));
         offset++;
      }
      objectFileStream_ << "\t\tli  $r" << registerIndex_ << "," << QString::number(0);
      AddAssemblyLine("", "li", "$r" + QString::number(registerIndex_), "", "", QString::number(0));
      objectFileStream_ << "\t\tstr  $r" << registerIndex_ << "," << QString::number(offset);
      AddAssemblyLine("", "str", "$r" + QString::number(registerIndex_), "", "", QString::number(offset));
      GetNextToken();
   }
   else if (token_ == Tokenizer::true_ || token_ == Tokenizer::false_)
   {
      QString boolean = (token_ == Tokenizer::true_) ? "1" : "0";
      objectFileStream_ << "\t\tli $r" << registerIndex_ << ", " << boolean << "\n";
      AddAssemblyLine("", "li", "$r" + QString::number(registerIndex_), "", "", boolean);
      GetNextToken();
      registerIndex_++;
   }
   else if (token_ == Tokenizer::openBrackets_)
   {
      GetNextToken();
      ParseExpression();
      if (token_ == Tokenizer::closeBrackets_)
      {
          GetNextToken();
      }
   }
}


void Parser::ParseProcedure()
{

}


void Parser::ParseSubScript(QString symbolName, bool  isAnAddress)
{
   typedef QList<int> DimensionsCalculationList;
   int dimension = 1;
   SymbolTable::symbolItem* symbol = currentScopeSymbolTable_->GetSymbol(symbolName);
   DimensionsCalculationList dimensionsCalculationList = symbol->arrayDimensionsLength;
   for (int i = 0; i < symbol->arrayDimensionsLength.size(); i++)
   {
      dimensionsCalculationList.append(symbol->arrayDimensionsLength.at(i));
   }
   if (token_ == Tokenizer::openBrackets_)
   {
      GetNextToken();
      getNextDim:
      ParseExpression();
      registerIndex_--;
      objectFileStream_ << "\t\tmuli  $r" << registerIndex_ << ", $r" << registerIndex_ << "," << dimensionsCalculationList.at(dimension - 1) << "\n";
      AddAssemblyLine("", "muli", "$r" + QString::number(registerIndex_), "$r" + QString::number(registerIndex_), "", QString::number(dimensionsCalculationList.at(dimension - 1)));
      objectFileStream_ << "\t\tmov $r28, $r" << registerIndex_ << "\n";
      AddAssemblyLine("", "mov", "$r28" , "$r" + QString::number(registerIndex_), "", "");
      registerIndex_++;
      if (token_ == Tokenizer::comma_)
      {
         dimension++;
         GetNextToken();
         goto getNextDim;
      }
      if (token_ == Tokenizer::closeBrackets_)
      {
         GetNextToken();
         for (unsigned int i = 0; i < symbol->numOfDimensions - 1; i++)
         {
            registerIndex_--;
            registerIndex_--;
            objectFileStream_ << "\t\tlod  $r" << registerIndex_ << ", " << currentScopeSymbolTable_->GetSymbolOffset(symbolName) << " ($r28)\n";
            AddAssemblyLine("", "lod", "$r" + QString::number(registerIndex_), "", "", QString::number(currentScopeSymbolTable_->GetSymbolOffset(symbolName)) + " ($r28)");
            registerIndex_++;
         }
         registerIndex_--;
         if (isAnAddress)
         {
            objectFileStream_ << "\t\tlod  $r" << registerIndex_ << ", " << currentScopeSymbolTable_->GetSymbolOffset(symbolName) << " ($r28)\n";
            AddAssemblyLine("", "lod", "$r" + QString::number(registerIndex_), "", "", QString::number(currentScopeSymbolTable_->GetSymbolOffset(symbolName)) + " ($r28)");
         }
         else
         {
            objectFileStream_ << "\t\tlod  $r" << registerIndex_ << ", " << currentScopeSymbolTable_->GetSymbolOffset(symbolName) << " ($r28)\n";
            AddAssemblyLine("", "lod", "$r" + QString::number(registerIndex_), "", "", QString::number(currentScopeSymbolTable_->GetSymbolOffset(symbolName)) + " ($r28)");
            //objectFileStream_ << "\t\tmov  $r29, $r" << registerIndex_ << "\n";
         }
         registerIndex_++;
      }
   }
}


void Parser::ParseArgumentList()
{
   offset_ = 0;
   scan:
   if (token_ == Tokenizer::identifier_)
   {
      if (token_ == Tokenizer::identifier_)
      {
         QString parameterName = tokenizer_.String();
         GetNextToken();
         if (token_ == Tokenizer::as_)
         {
            GetNextToken();
            if (token_ == Tokenizer::integer_)
            {
               currentScopeSymbolTable_->AddParameterItem(parameterName, SymbolTable::integer);
               GetNextToken();
               if (token_ == Tokenizer::comma_)
               {
                  GetNextToken();
                  goto scan;
               }
            }
            else if (token_ == Tokenizer::string_)
            {
               currentScopeSymbolTable_->AddParameterItem(parameterName, SymbolTable::string);
               GetNextToken();
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
               if (token_ == Tokenizer::comma_)
               {
                  GetNextToken();
                  goto scan;
               }
            }
            else
            {
               GetNextToken();
               ErrorMessage("Invalid Parameter Type.");
            }
         }
         else
         {
            GetNextToken();
            ErrorMessage("Missing 'AS'.");
         }
      }
   }
}




void Parser::PrintVariables()
{
   currentScopeSymbolTable_->PrintVariables();
}


void Parser::DumpSymbolTable()
{
   currentScopeSymbolTable_->DumpTable();
}


void Parser::ParseDeclare()
{
   QString functionName;
   GetNextToken();
   if (token_ == Tokenizer::identifier_)
   {
      functionName = tokenizer_.String();
      globalSymbolTable_.EnterSymbolToTable(functionName);
      currentScopeSymbolTable_ = globalSymbolTable_.GetSymbolTable(functionName);
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
      ErrorMessage("Missing '(' in Function Declaration ");
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


void Parser::ParseSubroutine()
{
   GetNextToken();
   if(token_ == Tokenizer::identifier_)
   {
      GetNextToken();
      QString subroutineName = tokenizer_.String();
      if (!globalSymbolTable_.SymbolExist(subroutineName))
      {
         globalSymbolTable_.EnterSymbolToTable(subroutineName);
      }
      currentScopeSymbolTable_ = globalSymbolTable_.GetSymbolTable(subroutineName);
   
      objectFileStream_ << ";" << subroutineName <<" Subroutine\n";
      objectFileStream_ << "CSEG:\n" ;
      objectFileStream_ << subroutineName << ":\n" ;
      objectFileStream_ << "\t\tload  $r29, SP\n" ;
      if (token_ == Tokenizer::openBrackets_)
      {
         GetNextToken();
         ParseArgumentList();
         if(token_ == Tokenizer::closeBrackets_)
         {
            GetNextToken();
            while(token_ != Tokenizer::endSubroutine_ && token_ != Tokenizer::endOfFile_)
            {
               ParseStatement();
            }
            if (token_ == Tokenizer::endSubroutine_)
            {
               GetNextToken();
               objectFileStream_ << "\t\tret\n" ;
            }
            else
            {
               GetNextToken();
               ErrorMessage("Missing 'end Subroutine'");
            }
         }
      }
   }
   else
   {
      GetNextToken();
      ErrorMessage("Missing 'Subroutine name'");
   }
}


void Parser::ParseConstantDeclaration()
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
                     currentScopeSymbolTable_->AddSymbolItem(constant, 
                                                             value, offset_++,
                                                             identifierType, 
                                                             false, 0, dimensions, 
                                                             SymbolTable::Type::integer);
                     break;
                  case SymbolTable::Type::string:
                      //currentScopeSymbolTable_->AddSymbolItem(constant, 
                      break;
                  default:
                     ErrorMessage("");
                     break;
               }
            }
         }
         else if (token_ == Tokenizer::number_)
         {
            currentScopeSymbolTable_->AddSymbolItem(constant, 0, tokenizer_.GetValue(), SymbolTable::Type::constant, false, 0, dimensions, SymbolTable::Type::integer);
         }
         else if (token_ == Tokenizer::stringLit_)
         {
         }
         GetNextToken();
      }
   }
}


void Parser::ParseEnumDeclaration()
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
   while (token_ == Tokenizer::endOfLine_)
   {
      GetNextToken();
   }
   if (token_ == Tokenizer::identifier_)
   {
      while (token_ != Tokenizer::endEnum_ && token_ != Tokenizer::endOfFile_ && !TokenIn(startOfStatementList_))
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
 

void Parser::ParseStructureDeclaration()
{

}


void Parser::AddAssemblyLine(QString label, QString opCode, QString dest, QString src1, QString src2, QString literalVal)
{
   AssemblyLine* newLine = new AssemblyLine;
   newLine->label = label;
   newLine->opcode = opCode;
   newLine->dest = dest;
   newLine->src1 = src1;
   newLine->src2 = src2;
   newLine->literal = literalVal;

   assemblyLineList_.append(newLine);
}


bool Parser::TokenIn(QList<Tokenizer::Token> list)
{
   return list.contains(token_);
}


void Parser::EmitCode(QString label, QString opCode, QString dest, QString src1, QString src2, QString literalVal)
{
   AssemblyLine* newLine = new AssemblyLine;
   newLine->label = label;
   newLine->opcode = opCode;
   newLine->dest = dest;
   newLine->src1 = src1;
   newLine->src2 = src2;
   newLine->literal = literalVal;

   assemblyLineList_.append(newLine);
}
