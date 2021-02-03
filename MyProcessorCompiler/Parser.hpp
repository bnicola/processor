#ifndef PARESER_HPP
#define PARESER_HPP
#include "Tokenizer.hpp"
#include "GlobalSymbolTable.hpp"
#include "SymbolTable.hpp"
#include "ObjectFileGenerator.hpp"
#include "Stack.hpp"
#include "MyProcessorAssembler.hpp"
#include <string>

class MyAssembler;


class Parser
{

public :  

   enum Condition
   {
      equalCondition,
      smallerThanCondition,
      biggerThanCondition,
   };

   struct AssemblyLine
   {
      QString label;
      QString opcode;
      QString dest;
      QString src1;
      QString src2;
      QString literal;
   };

   Parser();

   ~Parser();

   void Parse(QString fileName);

   void DumpSymbolTable();

   bool ErrorDetected();

   void SetOutputFile(QString fileName);

   void SetMachineCodeFile(QString machineCodeFilePath);


private:


   void GetNextToken();

   void ErrorMessage(std::string errorMessage);
   
   void ParseProgram();

   void ParseStatement();

   void ParseComment();

   void ParseLetStatement();

   void ParseVarDeclaration();

   void ParseType(QList<int> dimensions);

   void ParseDoStatement();

   void ParseIfStatement();

   void ParseWhileStatement();

   void ParseAssignment();

   void ParseWriteMemory();

   void ParseDelay();

   void ParseExpression();

   void ParseSimpleExpression();

   void ParseFactor();

   void ParseProcedure();

   void ParseSubScript(QString symbolName, bool  isAnAddress);

   void ParseTerm();

   void ParseArgumentList();

   void ParseDeclare();

   void ParseConstantDeclaration();

   void ParseEnumDeclaration();

   void ParseStructureDeclaration();

   void ParseSubroutine();

   void PrintVariables();

   void AddAssemblyLine(QString label, QString opCode, QString dest, QString src1, QString src2, QString literalVal);

   bool TokenIn(QList<Tokenizer::Token> list);

   void OptimizeGeneratedCode();

   void EmitCode(QString label, QString opCode, QString dest, QString src1, QString src2, QString literalVal);

private:
   
   Tokenizer::Token token_;

   Tokenizer tokenizer_;

   GlobalSymbolTable globalSymbolTable_;

   SymbolTable* currentScopeSymbolTable_;

   ObjectFileGenerator* objectFileGenerator_;

   Stack* runtimeStack_;

   QFile objectFile_;

   QTextStream objectFileStream_;

   QString label_;

   bool errorDetected_;

   unsigned int offset_;

   unsigned int labelIndex_;

   QString variable_;
   bool variableIsArray_;
   unsigned int variableArraySize_;

   unsigned int registerIndex_;

   MyProcessorAssembler assembler_;

   QString machineCodeFilePath_;

   typedef QList<AssemblyLine*> AssemblyLineList;
   AssemblyLineList assemblyLineList_;

   QList<Tokenizer::Token> startOfStatementList_;
   QList<Tokenizer::Token> startingSubroutine_;
};



#endif