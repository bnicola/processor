#include "MyProcessorAssembler.hpp"
#include <QFile>
#include <QTextStream>

#include <QFileInfo>
#include <qprocess.h>

MyProcessorAssembler::MyProcessorAssembler()
{
   opCodes_["nop"] = 0;
   opCodes_["add"] = 1;
   opCodes_["sub"] = 2;
   opCodes_["and"] = 3;
   opCodes_["or"]  = 4;
   opCodes_["xor"] = 5;
   opCodes_["not"] = 6;
   opCodes_["mul"] = 7;
   opCodes_["div"] = 8;
   opCodes_["shl"] = 9;
   opCodes_["shr"] = 10;
   opCodes_["pas"] = 11;

   opCodes_["jal"] = 12;
   opCodes_["ret"] = 13;
   opCodes_["jmp"] = 14;
   opCodes_["lui"] = 15;
   opCodes_["mov"] = 16;

   opCodes_["addi"] = 17;
   opCodes_["subi"] = 18;
   opCodes_["andi"] = 19;
   opCodes_["ori"]  = 20;
   opCodes_["xori"] = 21;
   opCodes_["noti"] = 22;
   opCodes_["muli"] = 23;
   opCodes_["divi"] = 24;
   opCodes_["shli"] = 25;
   opCodes_["shri"] = 26;

   opCodes_["lod"]  = 27;
   opCodes_["str"]  = 28;
   opCodes_["movi"] = 29;

   opCodes_["jeq"]  = 30;
   opCodes_["jneq"] = 31;
   opCodes_["jg"]   = 32;
   opCodes_["jl"]   = 33;
   opCodes_["jge"] = 34;
   opCodes_["jle"] = 35;

   opCodes_["spc"] = 36;

   opCodes_["push"] = -1;
   opCodes_["pop"]  = -1;

   opCodes_["li"] = -1;

   registers_ << "$r0" << "$r1" << "$r2" << "$r3" << "$r4" << "$r5" << "$r6" << "$r7";
   registers_ << "$r8" << "$r9" << "$r10" << "$r11" << "$r12" << "$r13" << "$r14" << "$r15";
   registers_ << "$r16" << "$r17" << "$r18" << "$r19" << "$r20" << "$r21" << "$r22" << "$r23";
   registers_ << "$r24" << "$r25" << "$r26" << "$r27" << "$r28" << "$r29" << "$r30" << "$r31";

   
}


MyProcessorAssembler::~MyProcessorAssembler()
{
}


void MyProcessorAssembler::Assemble(QString file, QString outputPath)
{
   QFile assemblyFile(file);
   QTextStream in(&assemblyFile);

   QFileInfo sourceFile(file);
   QFileInfo outFile(outputPath);
   outputPath_ = outFile.absoluteFilePath();
   
   QStringList lines;
   if (assemblyFile.exists() == true)
   {
      if (assemblyFile.open(QIODevice::ReadOnly | QIODevice::Text) == true)
      {
         while (assemblyFile.atEnd() == false)
         {
            QString code = assemblyFile.readAll();
            lines = code.split("\n");
         }
      }
   }
   
   Tokenize(lines);
   error_ = false;
   if (error_ == false)
   {
      ParseProgram(sourceFile.absoluteFilePath(), 1);
   }
   if (error_ == false)
   {
      ParseProgram(sourceFile.absoluteFilePath(), 2);
   }
   if (error_ == false)
   {
      GenerateMachineCode();
      printf("\n\nAssembled the file : \"%s\"\n", sourceFile.absoluteFilePath().toUtf8().data());
      printf("Output file        : \"%s\"\n", outputPath_.toUtf8().data());
      printf("Dumped Tokens file : \"Dump.text\"", outputPath_.toUtf8().data());
   }  
}



void MyProcessorAssembler::Tokenize(QStringList lines)
{
   quint32 lineNumber = 0;
   quint32 lineMarker = 0;
   quint32 address    = 0;

   currentInstrAddress_ = 0;
   QString currentToken;
   QString currentTokenType;

   QString line;

   quint32 index = 0;

   detectedErrors_ = false;
   QStringList tokens;
   QString code;
   
   QFile dump("dump.txt");
   QTextStream inDump(&dump);
   dump.open(QIODevice::WriteOnly | QIODevice::Text);
   for (int j = 0; j < lines.size(); j++)
   {
      QString line = lines.at(j);
      line = line.replace("\t", " ");
      line = line.append("\n");
      lineNumber++;
      inDump << "\n";
      for (int i = 0; i < line.size(); i++)
      {
         currentTokenType = "";
         currentToken = "";
         if (line.at(i).isLetter() == true)
         {
            while (line.at(i).isLetter() == true || line.at(i).isNumber() == true || line.at(i) == "_")
            {
               currentToken.append(line.at(i));
               i++;
               currentTokenType = "identifier";
               if (opCodes_.contains(currentToken) == true)
               {
                  currentTokenType = ClassifyOpCode(currentToken);
               }
            }
            i--;
         }
         else if (line.at(i).isNumber() == true || line.at(i) == "-" || line.at(i) == "+")
         {
            currentToken.append(line.at(i));
            currentTokenType = "number";
            i++;
            if (line.at(i).isNumber())
            {
               do
               {
                  currentToken.append(line.at(i));
                  i++;
               } while (line.at(i).isNumber());
            }
            else if (line.at(i) == 'x')
            {
               do
               {
                  currentToken.append(line.at(i));
                  i++;
               } while (IsHex(line.at(i)));
            }
            
            i--;
         }
         else if (line.at(i) == ',')
         {
            currentTokenType = "comma";
         }
         else if (line.at(i) == ':')
         {
            currentTokenType = "colon";
         }
         else if (line.at(i) == '.')
         {
            currentTokenType = "dot";
         }
         else if (line.at(i) == '(')
         {
            currentTokenType = "openbracket";
         }
         else if (line.at(i) == ')')
         {
            currentTokenType = "closebracket";
         }
         else if (line.at(i) == ';')
         {
            while (line.at(i) != '\n')
            {
               currentToken.append(line.at(i));
               i++;
            }
            i--;
            currentTokenType = "comment";
         }
         else if (line.at(i) == '\n')
         {
           currentTokenType = "newline";
         }
         else if (line.at(i) == '#')
         {
            while (line.at(i) != '\n')
            {
               currentToken.append(line.at(i));
               i++;
            }
            i--;
            currentTokenType = "comment";
         }

         if ((line.at(i) == ' ') || (line.at(i) == '\t'))
         {
            while (line.at(i) == ' ' || line.at(i) == '\t')
            {
               i++;
            }
            i--;
         }
         if (line.at(i) == '$')
         {
            while (line.at(i) == '$' || line.at(i).isLetter() == true || line.at(i).isNumber() == true)
            {
               currentToken.append(line.at(i));
               i++;
               currentTokenType = "register";
            }
            i--;
            if (registers_.contains(currentToken) == false)
            {
               currentTokenType = "identifier";
            }
         }

         if (currentTokenType.isEmpty() == false)
         {
            tokens.append(currentTokenType);
            
            TokenStruct newToken;
            newToken.tokenString = currentToken;
            newToken.tokenType = currentTokenType;
            newToken.tokenLineNumber = lineNumber;
            newToken.tokenIndex = i - currentToken.size();
            newToken.tokenLineCode = line;
            newToken.tokenLineAddress = currentInstrAddress_;
            tokensList_.append(newToken);
         }
      }
   }
  

   for (int i = 0; i < tokensList_.size() - 1; i++)
   {
      TokenStruct thisToken = tokensList_.at(i);
      if (tokensList_.at(i).tokenType == "identifier" && tokensList_.at(i + 1).tokenType == "colon")
      {
         thisToken.tokenType = "label";
         finalTokens_.append(thisToken);
         i++;
      }
      else if (tokensList_.at(i).tokenType == "dot" && tokensList_.at(i + 1).tokenType == "identifier")
      {
         if (tokensList_.at(i + 1).tokenString.toLower() == "text")
         {
            thisToken.tokenType = "dotText";
            finalTokens_.append(thisToken);
            i++;
         }
         else if (tokensList_.at(i + 1).tokenString.toLower() == "data")
         {
            thisToken.tokenType = "dotData";
            finalTokens_.append(thisToken);
            i++;
         }
         else if (tokensList_.at(i + 1).tokenString.toLower() == "byte")
         {
            thisToken.tokenType = "dotByte";
            finalTokens_.append(thisToken);
            i++;
         }
         else if (tokensList_.at(i + 1).tokenString.toLower() == "word")
         {
            thisToken.tokenType = "dotWord";
            finalTokens_.append(thisToken);
            i++;
         }
         else if (tokensList_.at(i + 1).tokenString.toLower() == "short")
         {
            thisToken.tokenType = "dotShort";
            finalTokens_.append(thisToken);
            i++;
         }
         else if (tokensList_.at(i + 1).tokenString.toLower() == "space")
         {
            thisToken.tokenType = "dotSpace";
            finalTokens_.append(thisToken);
            i++;
         }
      }
      else
      {
         finalTokens_.append(thisToken);
      }
   }
   finalTokens_.append(tokensList_.at(tokensList_.size() - 1));
 

   TokenStruct finalToken;
   finalToken.tokenType = "endoffile";
   finalTokens_.append(finalToken);
   for (int i = 0; i < finalTokens_.size(); i++)
   {
      QString tokenType = finalTokens_.at(i).tokenType;
      QString tokenString = finalTokens_.at(i).tokenString;
      inDump << tokenType << " (" << tokenString << ")\n";
   }

   TokenStruct newToken;
   newToken.tokenString = "endoffile";
   newToken.tokenType = "endoffile";
   newToken.tokenLineNumber = lines.size();
   tokensList_.append(newToken);
   dump.close();
}


QString MyProcessorAssembler::ClassifyOpCode(QString currentOpCode)
{
   QString opCodeCalss;
   if (currentOpCode == "nop" )
   {
      opCodeCalss = "nopOpCode";
   }
   else if (currentOpCode == "add" ||
      currentOpCode == "sub" ||
      currentOpCode == "and" ||
      currentOpCode == "or" ||
      currentOpCode == "xor" ||
      currentOpCode == "not" ||
      currentOpCode == "mul" ||
      currentOpCode == "div" ||
      currentOpCode == "shl" ||
      currentOpCode == "shr")
   {
      opCodeCalss = "registerOpCode";
   }
   else if (currentOpCode == "addi" ||
      currentOpCode == "subi" ||
      currentOpCode == "andi" ||
      currentOpCode == "ori"  ||
      currentOpCode == "xori" ||
      currentOpCode == "noti" ||
      currentOpCode == "muli" ||
      currentOpCode == "divi" ||
      currentOpCode == "shli" ||
      currentOpCode == "shri" ||
      currentOpCode == "jeq"  || 
      currentOpCode == "jneq" || 
      currentOpCode == "jg"   || 
      currentOpCode == "jl"   ||
      currentOpCode == "jge"  ||
      currentOpCode == "jle")
   {
      opCodeCalss = "registerImmediateOpCode";
   }
   else if (currentOpCode == "jal" ||
            currentOpCode == "jmp")
   {
      opCodeCalss = "jmpImmediateOpCode";
   }
   else if (currentOpCode == "lod" || currentOpCode == "str")
   {
      opCodeCalss = "lodStrOpCode";
   }
   else if (currentOpCode == "movi" ||
      currentOpCode == "lui")
   {
      opCodeCalss = "lodImmOpCode";
   }
   else if (currentOpCode == "li")
   {
      opCodeCalss = "liOpCode";
   }
   else if (currentOpCode == "push" ||
            currentOpCode == "pop")
   {
      opCodeCalss = "pushPopOpCode";
   }
   else if (currentOpCode == "spc")
   {
      opCodeCalss = "spcOpCode";
   }
   else if (currentOpCode == "mov")
   {
      opCodeCalss = "moveOpCode";
   }

   else if (currentOpCode == "ret")
   {
      opCodeCalss = "retOpCode";
   }
   
   currentInstrAddress_++;
   return opCodeCalss;
}


bool MyProcessorAssembler::IsHex(QChar ch)
{
   return (ch.isNumber() || ch == 'a' || ch == 'b' || ch == 'c' || ch == 'd' || ch == 'e' || ch == 'f' ||
                            ch == 'A' || ch == 'B' || ch == 'C' || ch == 'D' || ch == 'E' || ch == 'F');
}

void MyProcessorAssembler::ParseProgram(QString file, quint8 pass)
{
   currentTokenIndex_ = 0;
   error_ = false;
   currentInstrAddress_ = 0; // initial value should be taken out from .txt section.
   //currentToken_ = NextToken();
   while (currentToken_.tokenType != "endoffile" && error_ == false)
   {
      currentToken_ = NextToken(); 
      if (currentToken_.tokenType == "label" && error_ == false)
      {
         QString label = currentToken_.tokenString;
         if (pass == 1)
         {
            symbolTable_.insert(label, currentInstrAddress_);
         }
      }


      if (currentToken_.tokenType == "registerOpCode" && error_ == false)
      {
         ParseRegisterOpCode(file, pass);
      }
      else if (currentToken_.tokenType == "nopOpCode"&& error_ == false)
      {
         QString opcode = currentToken_.tokenString;
         if (pass == 2)
         {
            AssemblerElement* newElement = new AssemblerElement;
            newElement->SetLineOpCode(opcode);
            newElement->SetLineNumber(currentToken_.tokenLineNumber);
            newElement->SetLineCode(currentToken_.tokenLineCode.trimmed());
            newElement->SetLineAddress(currentInstrAddress_++);
            assembly_.append(newElement);
         }
         else
         {
            currentInstrAddress_++;
         }
      }
      else if (currentToken_.tokenType == "label" && error_ == false)
      {
         QString label = currentToken_.tokenString;
         if (pass == 1)
         {
            symbolTable_.insert(label, currentInstrAddress_);
         }
      }
      else if (currentToken_.tokenType == "pushPopOpCode" && error_ == false)
      {
         ParsePushPopOpCode(file, pass);
      }
      else if (currentToken_.tokenType == "spcOpCode"&& error_ == false)
      {
         ParseSpcOpCode(file, pass);
      }
      else if (currentToken_.tokenType == "moveOpCode"&& error_ == false)
      {
         ParseMoveOpCode(file, pass);
      }
      else if (currentToken_.tokenType == "liOpCode" && error_ == false)
      {
         ParseLiOpCode(file, pass);
      }
      else if (currentToken_.tokenType == "registerImmediateOpCode" && error_ == false)
      {
         ParseRegisterImmediateOpCode(file, pass);
      }
      else if (currentToken_.tokenType == "jmpImmediateOpCode")
      {
         ParseJumpImmediateOpCode(file, pass);
      }
      else if (currentToken_.tokenType == "lodStrOpCode" && error_ == false)
      {
         ParseLoadStoreOpCode(file, pass);
      }
      else if (currentToken_.tokenType == "retOpCode" && error_ == false)
      {
         ParseReturnOpCode(file, pass);
      }
      else if (currentToken_.tokenType == "lodImmOpCode" && error_ == false)
      {
         ParseLoadImmediateOpCode(file, pass);
      }
      else if (currentToken_.tokenType == "dotData" && error_ == false)
      {
         //Start of a data section.
         currentToken_ = NextToken();
         if (currentToken_.tokenType == "label" && error_ == false)
         {
            QString label = currentToken_.tokenString;
            currentToken_ = NextToken();
            if (currentToken_.tokenType == "dotWord" && error_ == false)
            {
               currentToken_ = NextToken();
               if ((currentToken_.tokenType == "number" || currentToken_.tokenType == "identifier" ) && error_ == false)
               {
                  QString var = currentToken_.tokenString;
                  
                  do
                  {
                     bool ok = false;
                     quint32 number = var.toInt(&ok);
                     if (ok == false)
                     {
                        // it not a number , it is a variable assigned to another
                        number = var.toInt(&ok);
                        if (symbolTable_.contains(var) == true)
                        {

                        }
                        else
                        {

                        }
                     }
                     else
                     {
                        if (pass == 1)
                        {
                           symbolTable_.insert(label, number);
                        }
                     }
                  } while (currentToken_.tokenType == "comma");
               }
               else
               {
                  // expected a variable declaration in .data section
                  printf("Error in file:%s, at line %d, Expected a variable declaration in .data section\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
                  ShowCodeError(currentToken_);
                  error_ = true;
               }
            }
            else if (currentToken_.tokenType == "dotShort" && error_ == false)
            {
               currentToken_ = NextToken();
               if (pass == 1)
               {
                  symbolTable_.insert(label, currentInstrAddress_);
               }
            }
            else if (currentToken_.tokenType == "dotByte" && error_ == false)
            {
               currentToken_ = NextToken();
               if (pass == 1)
               {
                  symbolTable_.insert(label, currentInstrAddress_);
               }
            }
            else
            {
               // expected a variable type declaration in .data section
               Error(file, "Expected  a variable type declaration in .data section\n", currentToken_.tokenLineNumber);
               printf("Error in file:%s, at line %d, Expected  a variable type declaration in .data section\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
               ShowCodeError(currentToken_);
               error_ = true;
            }
         }
         else
         {
            // expected a variable declaration in .data section
            printf("Error in file:%s, at line %d, Expected a variable declaration in .data section\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
            ShowCodeError(currentToken_);
            error_ = true;
         }
      }
      else if (currentToken_.tokenType == "number" || currentToken_.tokenType == "identifier" && error_ == false)
      {
         printf("Error in file:%s, at line %d, Un-Expected token\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
         ShowCodeError(currentToken_);
         error_ = true;
      }
   }
}


void MyProcessorAssembler::ParseRegisterOpCode(QString file, quint32 pass)
{
   QString destReg;
   QString src1;
   QString src2;
   quint32 lineNumer;
   QString opcode = currentToken_.tokenString;
   currentToken_ = NextToken(); 
   if (currentToken_.tokenType == "register" && error_ == false) //dest reg
   {
      destReg = currentToken_.tokenString;
      lineNumer = currentToken_.tokenLineNumber;
      currentToken_ = NextToken(); 
      if (currentToken_.tokenType == "comma" && error_ == false)
      {
         currentToken_ = NextToken(); 
         if (currentToken_.tokenType == "register" && error_ == false) //src1 reg
         {
            src1 = currentToken_.tokenString;
            currentToken_ = NextToken(); 
            if (currentToken_.tokenType == "comma" && error_ == false)
            {
               currentToken_ = NextToken(); 
               if (currentToken_.tokenType == "register" && error_ == false) //src2 reg
               {
                  src2 = currentToken_.tokenString;
                  if (pass == 2)
                  {
                     AssemblerElement* newElement = new AssemblerElement;
                     newElement->SetLineOpCode(opcode);
                     newElement->SetDest(destReg);
                     newElement->SetReg1(src1);
                     newElement->SetReg2(src2);
                     newElement->SetLineNumber(lineNumer);
                     newElement->SetLineCode(currentToken_.tokenLineCode.trimmed());
                     newElement->SetLineAddress(currentInstrAddress_++);
                     assembly_.append(newElement);
                  }
                  else
                  {
                     currentInstrAddress_++;
                  }
               }
               else
               {
                  // expected a src2 after comma
                  printf("Error in file:%s, at line %d, Expected a src2 register after comma\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
                  ShowCodeError(currentToken_);
                  error_ = true;
               }
            }
            else
            {
               //expected a comma after src1 register
               printf("Error in file:%s, at line %d, Expected a comma after src1 register\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
               ShowCodeError(currentToken_);
               error_ = true;
            }
         }
         else
         {
            //expected a src1 after comma
            printf("Error in file:%s, at line %d, Expected a src1 register after comma\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
            ShowCodeError(currentToken_);
            error_ = true;
         }
      }
      else
      {
         //expected a comma after destination register
         printf("Error in file:%s, at line %d, Expected a comma after destination register\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
         ShowCodeError(currentToken_);
         error_ = true;
      }
   }
   else
   {
      //expected a destination register
      printf("Error in file:%s, at line %d, Expected a destination register\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
      ShowCodeError(currentToken_);
      error_ = true;
   }
}


void MyProcessorAssembler::ParsePushPopOpCode(QString file, quint32 pass)
{
   QString destReg;
   QString src1;
   QString literal;
   QString literalUpper;
   QString literalLower;
   quint32 lineNumer;
   QString opcode = currentToken_.tokenString;
   currentToken_ = NextToken(); 
   if (currentToken_.tokenType == "register" && error_ == false) //dest reg
   {
      destReg = currentToken_.tokenString;
      lineNumer = currentToken_.tokenLineNumber;
      if (pass == 2)
      {
         if (opcode == "push")
         {
            AssemblerElement* newElement1 = new AssemblerElement;
            newElement1->SetLineOpCode("str");
            newElement1->SetDest(destReg);
            newElement1->SetReg1("$r30");
            newElement1->SetLineNumber(lineNumer);
            newElement1->SetLineAddress(currentInstrAddress_++);
            newElement1->SetLineCode(currentToken_.tokenLineCode.trimmed());
            assembly_.append(newElement1);

            AssemblerElement* newElement2 = new AssemblerElement;
            newElement2->SetLineOpCode("addi");
            newElement2->SetDest("$r30");
            newElement2->SetReg1("$r30");
            newElement2->SetLiteral("1");
            newElement2->SetLineNumber(lineNumer);
            newElement2->SetLineCode(currentToken_.tokenLineCode.trimmed());
            newElement2->SetLineAddress(currentInstrAddress_++);
            assembly_.append(newElement2);
         }
         else if (opcode == "pop")
         {
            AssemblerElement* newElement1 = new AssemblerElement;
            newElement1->SetLineOpCode("subi");
            newElement1->SetDest("$r30");
            newElement1->SetReg1("$r30");
            newElement1->SetLiteral("1");
            newElement1->SetLineNumber(lineNumer);
            newElement1->SetLineCode(currentToken_.tokenLineCode.trimmed());
            newElement1->SetLineAddress(currentInstrAddress_++);
            assembly_.append(newElement1);

            AssemblerElement* newElement2 = new AssemblerElement;
            newElement2->SetLineOpCode("lod");
            newElement2->SetDest(destReg);
            newElement2->SetReg1("$r30");
            newElement2->SetLineNumber(lineNumer);
            newElement2->SetLineCode(currentToken_.tokenLineCode.trimmed());
            newElement2->SetLineAddress(currentInstrAddress_++);
            assembly_.append(newElement2);
         }
        
      }
      else
      {
         currentInstrAddress_++;
         currentInstrAddress_++;
      }
   }
   else
   {
      //expected a register
      printf("Error in file:%s, at line %d, Expected a register argument for push/popc operations\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
      ShowCodeError(currentToken_);
      error_ = true;
   }
}


void MyProcessorAssembler::ParseSpcOpCode(QString file, quint32 pass)
{
   QString destReg;
   QString src1;
   QString literal;
   QString literalUpper;
   QString literalLower;
   quint32 lineNumer;
   QString opcode = currentToken_.tokenString;
   currentToken_ = NextToken();
   if (currentToken_.tokenType == "register" && error_ == false) //dest reg
   {
      destReg = currentToken_.tokenString;
      lineNumer = currentToken_.tokenLineNumber;
      if (pass == 2)
      {
         AssemblerElement* newElement1 = new AssemblerElement;
         newElement1->SetLineOpCode(opcode);
         newElement1->SetDest(destReg);
         newElement1->SetLineNumber(lineNumer);
         newElement1->SetLineCode(currentToken_.tokenLineCode.trimmed());
         newElement1->SetLineAddress(currentInstrAddress_++);
         assembly_.append(newElement1);
      }
      else
      {
         currentInstrAddress_++;
      }
   }
   else
   {
      printf("Error in file:%s, at line %d, Expected a register argument for spc operations\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
      ShowCodeError(currentToken_);
      error_ = true;
   }

}


void MyProcessorAssembler::ParseMoveOpCode(QString file, quint32 pass)
{
   QString destReg;
   QString src1;
   QString src2;
   quint32 lineNumer;
   QString opcode = currentToken_.tokenString;
   currentToken_ = NextToken();
   if (currentToken_.tokenType == "register" && error_ == false) //dest reg
   {
      destReg = currentToken_.tokenString;
      lineNumer = currentToken_.tokenLineNumber;
      currentToken_ = NextToken();
      if (currentToken_.tokenType == "comma" && error_ == false)
      {
         currentToken_ = NextToken();
         if (currentToken_.tokenType == "register" && error_ == false) //src1 reg
         {
            src1 = currentToken_.tokenString;
            if (pass == 2)
            {
               AssemblerElement* newElement = new AssemblerElement;
               newElement->SetLineOpCode(opcode);
               newElement->SetDest(destReg);
               newElement->SetReg1(src1);
               newElement->SetReg2(src2);
               newElement->SetLineNumber(lineNumer);
               newElement->SetLineCode(currentToken_.tokenLineCode.trimmed());
               newElement->SetLineAddress(currentInstrAddress_++);
               assembly_.append(newElement);
            }
            else
            {
               currentInstrAddress_++;
            }
         }
         else
         {
            //expected a src1 after comma
            printf("Error in file:%s, at line %d, Expected a src1 register after comma\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
            ShowCodeError(currentToken_);
            error_ = true;
         }
      }
      else
      {
         //expected a comma after destination register
         printf("Error in file:%s, at line %d, Expected a comma after destination register\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
         ShowCodeError(currentToken_);
         error_ = true;
      }
   }
   else
   {
      //expected a destination register
      printf("Error in file:%s, at line %d, Expected a destination register\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
      ShowCodeError(currentToken_);
      error_ = true;
   }
}


void MyProcessorAssembler::ParseLiOpCode(QString file, quint32 pass)
{
   QString destReg;
   QString src1;
   QString literal;
   QString literalUpper;
   QString literalLower;
   quint32 lineNumer;
   QString opcode = currentToken_.tokenString;
   currentToken_ = NextToken(); // finalTokens_.at(currentTokenIndex_++);
   if (currentToken_.tokenType == "register" && error_ == false) //dest reg
   {
      destReg = currentToken_.tokenString;
      lineNumer = currentToken_.tokenLineNumber;
      currentToken_ = NextToken(); 
      if (currentToken_.tokenType == "comma" && error_ == false)
      {
         currentToken_ = NextToken(); 
         if (currentToken_.tokenType == "number" && error_ == false) //3ed is a literal (number)
         {
            bool ok = false;
            literal = currentToken_.tokenString;
            quint32 literalVal = literal.toUInt(&ok, 10);
            if (!ok)
            {
               literalVal = literal.toUInt(&ok, 16);
            }
            literalUpper = QString::number((literalVal & 0xFFFF0000) >> 16);
            literalLower = QString::number(literalVal & 0x0000FFFF);
            if (pass == 2)
            {
               AssemblerElement* newElement1 = new AssemblerElement;
               newElement1->SetLineOpCode("lui");
               newElement1->SetDest(destReg);
               newElement1->SetReg1(src1);
               newElement1->SetLiteral(literalUpper);
               newElement1->SetLineNumber(lineNumer);
               newElement1->SetLineCode(currentToken_.tokenLineCode.trimmed());
               newElement1->SetLineAddress(currentInstrAddress_++);
               assembly_.append(newElement1);


               AssemblerElement* newElement2 = new AssemblerElement;
               newElement2->SetLineOpCode("ori");
               newElement2->SetDest(destReg);
               newElement2->SetReg1(destReg);
               newElement2->SetLiteral(literalLower);
               newElement2->SetLineNumber(lineNumer);
               newElement2->SetLineCode(currentToken_.tokenLineCode.trimmed());
               newElement2->SetLineAddress(currentInstrAddress_++);
               assembly_.append(newElement2);
            }
            else
            {
               currentInstrAddress_++;
               currentInstrAddress_++;
            }
         }
         else
         {
            //expected a comma after src1 register
            printf("Error in file:%s, at line %d, Expected a number after comma\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
            ShowCodeError(currentToken_);
            error_ = true;
         }
      }
      else
      {
         //expected a comma after destination register
         printf("Error in file:%s, at line %d, Expected a comma after destination register\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
         ShowCodeError(currentToken_);
         error_ = true;
      }
   }
   else
   {
      //expected a destination register
      printf("Error in file:%s, at line %d, Expected a a destination register\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
      ShowCodeError(currentToken_);
      error_ = true;
   }
}


void MyProcessorAssembler::ParseRegisterImmediateOpCode(QString file, quint32 pass)
{
   QString destReg;
   QString src1;
   QString var;
   quint32 lineNumer;
   QString opcode = currentToken_.tokenString;
   currentToken_ = NextToken(); 
   if (currentToken_.tokenType == "register" && error_ == false) //dest reg
   {
      destReg = currentToken_.tokenString;
      lineNumer = currentToken_.tokenLineNumber;
      currentToken_ = NextToken(); 
      if (currentToken_.tokenType == "comma" && error_ == false)
      {
         currentToken_ = NextToken(); 
         if (currentToken_.tokenType == "register" && error_ == false) //src1 reg
         {
            src1 = currentToken_.tokenString;
            currentToken_ = NextToken(); 
            if (currentToken_.tokenType == "comma" && error_ == false)
            {
               currentToken_ = NextToken(); 
               if (currentToken_.tokenType == "number" && error_ == false) //3ed is a literal (number)
               {
                  var = currentToken_.tokenString;
                  if (pass == 2)
                  {
                     AssemblerElement* newElement = new AssemblerElement;
                     newElement->SetLineOpCode(opcode);
                     newElement->SetDest(destReg);
                     newElement->SetReg1(src1);
                     newElement->SetLiteral(var);
                     newElement->SetLineNumber(lineNumer);
                     newElement->SetLineAddress(currentInstrAddress_++);
                     newElement->SetLineCode(currentToken_.tokenLineCode.trimmed());
                     assembly_.append(newElement);
                  }
                  else
                  {
                     currentInstrAddress_++;
                  }
               }
               else if (currentToken_.tokenType == "identifier" && error_ == false) // 3ed is a literal (value to look up in symbol table)
               {
                  var = currentToken_.tokenString;
                  if (pass == 2)
                  {
                     if (symbolTable_.contains(var) == true && error_ == false)
                     {
                        AssemblerElement* newElement = new AssemblerElement;
                        newElement->SetLineOpCode(opcode);
                        newElement->SetDest(destReg);
                        newElement->SetReg1(src1);
                        newElement->SetLiteral(var);
                        newElement->SetLineCode(currentToken_.tokenLineCode.trimmed());
                        newElement->SetLineNumber(symbolTable_.value(var));
                        newElement->SetLineAddress(currentInstrAddress_++);
                        assembly_.append(newElement);
                     }
                     else
                     {
                        //expected a comma after src1 register
                        printf("Error in file:%s, at line %d, Un-Identified variable %s as the third argument\n", file.toUtf8().data(), currentToken_.tokenLineNumber, currentToken_.tokenString.toUtf8().data());
                        ShowCodeError(currentToken_);
                        error_ = true;
                     }
                  }
                  else
                  {
                     currentInstrAddress_++;
                  }
               }
               else
               {
                  //expected a comma after src1 register
                  printf("Error in file:%s, at line %d, Expected a number or a variable after comma\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
                  ShowCodeError(currentToken_);
                  error_ = true;
               }
            }
            else
            {
               //expected a comma after src1 register
               printf("Error in file:%s, at line %d, Expected a comma after src1\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
               ShowCodeError(currentToken_);
               error_ = true;
            }
         }
         else
         {
            //expected a src1 after comma
            printf("Error in file:%s, at line %d, Expected a a src1 after comma\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
            ShowCodeError(currentToken_);
            error_ = true;
         }
      }
      else
      {
         //expected a comma after destination register
         printf("Error in file:%s, at line %d, Expected a comma after destination register\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
         ShowCodeError(currentToken_);
         error_ = true;
      }
   }
   else
   {
      //expected a destination register
      printf("Error in file:%s, at line %d, Expected a a destination register\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
      ShowCodeError(currentToken_);
      error_ = true;
   }

}

void MyProcessorAssembler::ParseJumpImmediateOpCode(QString file, quint32 pass)
{
   QString destReg;
   QString src1;
   QString var;
   quint32 lineNumer;
   QString opcode = currentToken_.tokenString;
   currentToken_ = NextToken(); 
   if (currentToken_.tokenType == "number" && error_ == false) //2nd argument is a literal (number)
   {
      var = currentToken_.tokenString;
      if (pass == 2)
      {
         AssemblerElement* newElement = new AssemblerElement;
         newElement->SetLineOpCode(opcode);
         newElement->SetDest(destReg);
         newElement->SetReg1(src1);
         newElement->SetLiteral(var);
         newElement->SetLineNumber(lineNumer);
         newElement->SetLineCode(currentToken_.tokenLineCode.trimmed());
         newElement->SetLineAddress(currentInstrAddress_++);
         assembly_.append(newElement);
      }
      else
      {
         currentInstrAddress_++;
      }
   }
   else if (currentToken_.tokenType == "identifier" && error_ == false) // 3ed is a literal (value to look up in symbol table)
   {
      var = currentToken_.tokenString;
      if (pass == 2)
      {
         if (symbolTable_.contains(var) == true)
         {
            AssemblerElement* newElement = new AssemblerElement;
            newElement->SetLineOpCode(opcode);
            newElement->SetDest(destReg);
            newElement->SetReg1(src1);
            newElement->SetLiteral(var);
            newElement->SetLineCode(currentToken_.tokenLineCode.trimmed());
            newElement->SetLineNumber(symbolTable_.value(var));
            newElement->SetLineAddress(currentInstrAddress_++);
            assembly_.append(newElement);
         }
         else
         {
            //expected a comma after src1 register
            printf("Error in file:%s, at line %d, Un-Identified variable %s as the second argument\n", file.toUtf8().data(), currentToken_.tokenLineNumber, currentToken_.tokenString.toUtf8().data());
            ShowCodeError(currentToken_);
            error_ = true;
         }
      }
      else
      {
         currentInstrAddress_++;
      }
   }
}

void MyProcessorAssembler::ParseLoadStoreOpCode(QString file, quint32 pass)
{
   QString destReg;
   QString src1;
   QString literal = "0";
   quint32 lineNumer;
   QString opcode = currentToken_.tokenString;
   currentToken_ = NextToken(); 
   if (currentToken_.tokenType == "register" && error_ == false) //dest reg
   {
      destReg = currentToken_.tokenString;
      lineNumer = currentToken_.tokenLineNumber;
      currentToken_ = NextToken(); 
      if (currentToken_.tokenType == "comma" && error_ == false)
      {
         currentToken_ = NextToken(); 
         if (currentToken_.tokenType == "number" && error_ == false)
         {
            literal = currentToken_.tokenString;
            currentToken_ = NextToken();  // take the literal value here
         }
         if (currentToken_.tokenType == "openbracket") //src1 reg
         {
            currentToken_ = NextToken(); 
            if (currentToken_.tokenType == "register" && error_ == false)
            {
               src1 = currentToken_.tokenString;
               currentToken_ = NextToken(); 
               if (currentToken_.tokenType == "closebracket")
               {
                  if (pass == 2)
                  {
                     AssemblerElement* newElement = new AssemblerElement;
                     newElement->SetLineOpCode(opcode);
                     newElement->SetDest(destReg);
                     newElement->SetReg1(src1);
                     newElement->SetLiteral(literal);
                     newElement->SetLineNumber(lineNumer);
                     newElement->SetLineCode(currentToken_.tokenLineCode.trimmed());
                     newElement->SetLineAddress(currentInstrAddress_++);
                     assembly_.append(newElement);
                  }
                  else
                  {
                     currentInstrAddress_++;
                  }
               }
               else
               {
                  //expected a close bracket after register
                  printf("Error in file:%s, at line %d, Expected closing bracket after register argument\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
                  ShowCodeError(currentToken_);
                  error_ = true;
               }
            }
            else
            {
               //expected an open bracket after register
               printf("Error in file:%s, at line %d, Expected a register argument\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
               ShowCodeError(currentToken_);
               error_ = true;
            }
         }
         else
         {
            //expected an open bracket after register
            printf("Error in file:%s, at line %d, Expected open bracket before register argument\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
            ShowCodeError(currentToken_);
            error_ = true;
         }
      }
      else
      {
         //expected a comma after destination register
         printf("Error in file:%s, at line %d, Expected a comma after register argument\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
         ShowCodeError(currentToken_);
         error_ = true;
      }
   }
   else
   {
      //expected a destination register
      printf("Error in file:%s, at line %d, Expected a register argument\n", file.toUtf8().data(), currentToken_.tokenLineNumber);
      ShowCodeError(currentToken_);
      error_ = true;
   }

}


void MyProcessorAssembler::ParseReturnOpCode(QString file, quint32 pass)
{
   if (pass == 2)
   {
      QString opcode;
      opcode = currentToken_.tokenString;
      AssemblerElement* newElement = new AssemblerElement;
      newElement->SetLineOpCode(opcode);
      newElement->SetLineAddress(currentInstrAddress_++);
      newElement->SetLineCode(currentToken_.tokenLineCode.trimmed());
      assembly_.append(newElement);
   }
   else
   {
      currentInstrAddress_++;
   }
}


void MyProcessorAssembler::ParseLoadImmediateOpCode(QString file, quint32 pass)
{
   QString destReg;
   quint32 lineNumer;
   QString var;
   QString opcode;
   opcode = currentToken_.tokenString;
   currentToken_ = NextToken(); 
   if (currentToken_.tokenType == "register" && error_ == false) //dest reg
   {
      destReg = currentToken_.tokenString;
      lineNumer = currentToken_.tokenLineNumber;
      currentToken_ = NextToken(); 
      if (currentToken_.tokenType == "comma" && error_ == false)
      {
         currentToken_ = NextToken(); 
         if (currentToken_.tokenType == "number" && error_ == false) //3ed is a literal (number)
         {
            var = currentToken_.tokenString;
            if (pass == 2)
            {
               AssemblerElement* newElement = new AssemblerElement;
               newElement->SetLineOpCode(opcode);
               newElement->SetDest(destReg);
               newElement->SetLiteral(var);
               newElement->SetLineNumber(lineNumer);
               newElement->SetLineCode(currentToken_.tokenLineCode.trimmed());
               newElement->SetLineAddress(currentInstrAddress_++);
               assembly_.append(newElement);
            }
            else
            {
               currentInstrAddress_++;
            }
         }
         else if (currentToken_.tokenType == "identifier" && error_ == false) // 3ed is a literal (value to look up in symbol table)
         {
            var = currentToken_.tokenString;
            if (pass == 2)
            {
               if (symbolTable_.contains(var) == true && error_ == false)
               {
                  AssemblerElement* newElement = new AssemblerElement;
                  newElement->SetLineOpCode(opcode);
                  newElement->SetDest(destReg);
                  newElement->SetLiteral(var);
                  newElement->SetLineNumber(symbolTable_.value(var));
                  newElement->SetLineAddress(currentInstrAddress_++);
                  newElement->SetLineCode(currentToken_.tokenLineCode.trimmed());
                  assembly_.append(newElement);
               }
               else
               {
                  //expected a comma after src1 register
                  printf("Error in file:%s, at line %d, Un-Identified variable %s as the second argument\n", file.toUtf8().data(), currentToken_.tokenLineNumber, currentToken_.tokenString.toUtf8().data());
                  ShowCodeError(currentToken_);
                  error_ = true;
               }
            }
            else
            {
               currentInstrAddress_++;
            }
         }
      }
   }
}

MyProcessorAssembler::TokenStruct MyProcessorAssembler::NextToken()
{
   return finalTokens_.at(currentTokenIndex_++);
}


void MyProcessorAssembler::ShowCodeError(TokenStruct token)
{
   printf("%s", token.tokenLineCode.toUtf8().data());
   for (int i = 0; i < token.tokenIndex + 1; i++)
   {
      printf(" ");
   }
   printf("^\n");
}

bool MyProcessorAssembler::IsLiteral(QString literal)
{
   bool ok = false;

   literal.toInt(&ok, 10); //is it a decimal?
   if (ok == false)
   {
      literal.toInt(&ok, 16); // is it a hexadecimal ?
   }

   return ok;
}


bool MyProcessorAssembler::IsLabel(QString literal)
{
   bool isLabel = literal.at(0).isLetter();

   if (isLabel == true)
   {
      for (int i = 1; i < literal.size() && (isLabel == true); i++)
      {
         isLabel = literal.at(i).isLetter() || literal.at(i).isNumber();
      }
   }

   return isLabel;
}


bool MyProcessorAssembler::IsOpCode(QString opCode)
{
   return opCodes_.contains(opCode);
}


bool MyProcessorAssembler::IsRegister(QString reg)
{
   return registers_.contains(reg);
}


QStringList  MyProcessorAssembler::Parse(QString code)
{
   QStringList tokens;

   while (code.startsWith(" ") || code.startsWith("\t"))
   {
      code = code.remove(0, 1);
   }

   QRegExp rx("[, \n]"); // match a comma or a space or a : for labels

   tokens = code.split(rx, QString::SkipEmptyParts);

   return tokens;
}


void MyProcessorAssembler::GenerateMachineCode()
{
   QFile assemblyFileOut("Out.hexout");
   QTextStream out(&assemblyFileOut);

   if (assemblyFileOut.open(QIODevice::WriteOnly | QIODevice::Text) == true)
   {
      for (int i = 0; i < assembly_.size(); i++)
      {
         AssemblerElement* element = assembly_.at(i);

         QString reg1Val = element->Register1().remove("$r");
         QString reg2Val = element->Register2().remove("$r");
         QString dest = element->Dest().remove("$r");
         QString literal = element->Literal();
         QString opCode = element->OpCode();
         QString code = element->LineCode();
         quint32 instrAddress = element->LineAddress();
         quint32 op = opCodes_.value(element->OpCode());

         quint32 instruction = op << 26;

         if (dest.isEmpty() == false)
         {
            instruction |= dest.toUInt() << 21;
         }
         if (reg1Val.isEmpty() == false)
         {
            instruction |= reg1Val.toUInt() << 16;
         }
         if (reg2Val.isEmpty() == false)
         {
            instruction |= reg2Val.toUInt() << 11;
         }
         if (literal.isEmpty() == false)
         {
            // if our literal is a number
            bool ok            = false;
            qint32 literalVal = literal.toInt(&ok, 10);
            if (ok == false)
            {
               literalVal = literal.toInt(&ok, 16);     // try if it is a hex number      
            }
            
            // if our literal is a variable
            if (ok == false) 
            {
               if (opCode == "jeq" || opCode == "jneq" || opCode == "jg" || opCode == "jge" || opCode == "jle" || opCode == "jl")
               {
                  //jump conditions takes relative addresses.
                  literalVal = symbolTable_.value(literal) - element->LineAddress();
                  if (literalVal < 0)
                  {
                     literalVal = std::abs(literalVal);
                     literalVal = literalVal | 0x00008000;
                  }
               }
               else if (opCode == "jal" || opCode == "jmp")
               {
                  //jal and jmp takes absolute addresses.
                  literalVal = symbolTable_.value(literal);
               }
               else
               {
                  literalVal = symbolTable_.value(literal);    
               }
            }
            
            if (ok == true && (opCode == "lod" || opCode == "str"))
            {
               if (literalVal < 0)
               {
                  literalVal = std::abs(literalVal);
                  literalVal = literalVal | 0x00008000;
               }
            }


            instruction |= literalVal;
         }
         QString opcodeHexString = QString::number(instruction, 16);
         while (opcodeHexString.size() < 8)
         {
            opcodeHexString = "0" + opcodeHexString;
         }
         QString opcodeString = QString::number(instruction, 2);
         while (opcodeString.size() < 32)
         {
            opcodeString = "0" + opcodeString;
         }
         printf("(0x%8.8X) : 0x%s (%s)\n", instrAddress, opcodeHexString.toUtf8().data(), code.toUtf8().data());
         out << opcodeString << "\n";
      }

      assemblyFileOut.close();

      CreateAssemberHexFile(outputPath_);
   }
   else
   {
      printf("Can't open dump file\n");
   }
}


void MyProcessorAssembler::CreateAssemberHexFile(QString dumpFile)
{
   QFile assemblyFile("Out.hexout");
   QString contents;

   if (assemblyFile.open(QIODevice::ReadOnly | QIODevice::Text) == true)
   {
      contents = assemblyFile.readAll();
   }

   QFile outHexMachineCode(dumpFile);
   QTextStream out(&outHexMachineCode);
   outHexMachineCode.open(QIODevice::WriteOnly | QIODevice::Text);

   QStringList machineCodelines = contents.split("\n", QString::SkipEmptyParts);

   for (int i = 0; i < machineCodelines.size(); i++)
   {
      bool ok = false;
      QString line = machineCodelines.at(i);
      quint32 code = line.toUInt(&ok, 2);
      QString machineCodeInHex = QString::number(code, 16);
      out << machineCodeInHex << "\n";
   }
}


void MyProcessorAssembler::Error(QString file, QString error, quint32 line)
{
   //QString command = "C:\\Program Files(x86)\\Notepad++\\notepad++.exe";// test.asm - n18"
   //command = "\"" + command + "\" " + file + " -n" + QString::number(line);
   //command = command.replace("\\", "\\\\");
   //QProcess::startDetached(command);
   //printf("\x1b[31;1mError in File %s at Line : %d\n", file.toUtf8().data(), line);
   //printf("\x1b[31;1m%s\n", error.toUtf8().data());
   //ShowCodeError(currentToken_);
   //error_ = true;
   
}

