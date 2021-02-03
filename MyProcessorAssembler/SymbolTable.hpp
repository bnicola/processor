#ifndef SYMBOLTABLE_HPP
#define SYMBOLTABLE_HPP

#include <QMap>
#include <QHash>
#include <QString>


class SymbolTable
{
public:

   typedef enum 
   {
      constant,
      integer,
      string,
      boolean,
      enumd,
      array,
      structure,
      procedure
   }Type;

   
   typedef struct 
   {
      Type symbolType;
      unsigned int symbolValue;
      unsigned int symbolAddress;
      QList<int> arrayDimensionsLength;
      unsigned int arraySize;
      unsigned int numOfDimensions;
      Type symbolArrayType;
      char* string;
      QHash<QString, int> structureOffset;
   }symbolItem;

public :
   
   SymbolTable();

   ~SymbolTable();

   void AddSymbolItem(QString symbol, unsigned int offset, unsigned int address, Type type, bool variableIsArray, unsigned int variableArraySize, QList<int> dimensionsList, SymbolTable::Type symbolArrayType);

   void AddLocalItem(QString symbol, Type type);

   void AddParameterItem(QString symbol, Type type);

   bool SymbolExists(QString symbol);

   unsigned int GetSymbolOffset(QString symbol);

   SymbolTable::Type GetSymbolArrayType(QString symbol);

   void SetSymbolArrayType(QString symbol, SymbolTable::Type arrayType);

   unsigned int GetSymbolValue(QString symbol);

   unsigned int GetNumOfLocalVars();

   unsigned int GetNumOfParameters();

   SymbolTable::Type GetSymbolType(QString symbol);

   SymbolTable::symbolItem* GetSymbol(QString symbol);

   void SetSymbol(QString symbol, SymbolTable::symbolItem symbolItm);

   void EmptySymbolTable();

   void DumpTable();

   void PrintVariables();

private:


   void AddDimension(symbolItem symbolItem,  QList<int> dimensions);

   typedef QMap<QString, symbolItem*> ProgramSymbolTable;
   ProgramSymbolTable symbolTable_;

   unsigned int numOfLocalVars_;
   unsigned int numOfParameters_;
   unsigned int stackOffset_;
};




#endif