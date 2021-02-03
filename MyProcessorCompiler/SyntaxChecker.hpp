#ifndef SYNTAX_CHECKER_HPP
#define SYNTAX_CHECKER_HPP
#include "Tokenizer.hpp"
#include "GlobalSymbolTable.hpp"

#include <string>
#include <QObject>

class SyntaxChecker:public QObject
{

   Q_OBJECT

public :  

   SyntaxChecker();

   ~SyntaxChecker();

   void Parse(QString fileName);

   bool ErrorDetected();

   unsigned int ErrorCount();

signals:
    
   void Error(QString errorMessage);


private:


   void GetNextToken();

   void ErrorMessage(QString errorMessage);
   
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

   void ParseDelay();

   void ParseWriteMemory();

   SymbolTable::Type ParseExpression();

   SymbolTable::Type ParseSimpleExpression();

   SymbolTable::Type ParseFactor();

   void ParseLiteral(QString literalValue);

   SymbolTable::Type ParseProcedure();

   SymbolTable::Type ParseSubScript(QString symbolName);

   SymbolTable::Type ParseTerm();

   void ParseArgumentList();

   void ParseDeclare();

   void ParseConstantDeclaration();

   void ParseEnumDeclaration();

   void ParseStructureDeclaration();

   void ParseSubroutine();

   void Synchronize();

   void SynchronizeToNextStatement();

   bool TokenIn(QList<Tokenizer::Token> list);
   

private:
   
   Tokenizer::Token token_;

   Tokenizer tokenizer_;

   GlobalSymbolTable globalSymbolTable_;

   SymbolTable* currentScopeSymbolTable_;

   bool errorDetected_;

   unsigned int errorsCount_;

   unsigned int offset_;

   QString variable_;
   bool variableIsArray_;
   unsigned int variableArraySize_;

   QList<Tokenizer::Token> startOfStatementList_;
   QList<Tokenizer::Token> startingSubroutine_;

};



#endif