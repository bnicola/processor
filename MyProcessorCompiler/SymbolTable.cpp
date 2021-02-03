#include "SymbolTable.hpp"




SymbolTable::SymbolTable()
: symbolTable_(),
  numOfLocalVars_(0),
  numOfParameters_(0),
  stackOffset_(0)
{

}

SymbolTable::~SymbolTable()
{

}


void SymbolTable::AddSymbolItem(QString symbol, unsigned int offset, unsigned int address, Type type, bool variableIsArray, unsigned int variableArraySize, QList<int> dimensionsList, SymbolTable::Type symbolArrayType)
{
   symbolItem* newSymbol = new symbolItem;
   if (variableIsArray)
   {
      newSymbol->symbolType = array;
      newSymbol->symbolValue = offset;
      newSymbol->symbolAddress = offset;
      newSymbol->symbolArrayType = symbolArrayType;
      newSymbol->arraySize = 1;
      newSymbol->numOfDimensions = 0;
      for (int i = 0; i < dimensionsList.size(); i++)
      {
         newSymbol->arraySize *= dimensionsList.at(i);
         newSymbol->numOfDimensions++;
      }
      int multiplier = 1;
      for (int i = 0; i < dimensionsList.size() - 1; i++)
      {
         for (int j = i + 1; j < dimensionsList.size(); j++)
         {
            multiplier *=  dimensionsList.at(j);
         }
         newSymbol->arrayDimensionsLength.append(multiplier);
         multiplier = 1;
      }
      newSymbol->arrayDimensionsLength.append(1);
      symbolTable_[symbol] = newSymbol; 
   }
   else
   {
      newSymbol->symbolType = type;
      newSymbol->symbolValue = address;
      newSymbol->symbolAddress = offset;

      symbolTable_[symbol] = newSymbol; 
   }
}


void SymbolTable::AddDimension(symbolItem symbolItem, QList<int> dimensions)
{
  
   int multiplier = 1;
   for (int i = 0; i < dimensions.size() - 1; i++)
   {
      for (int j = i + 1; j < dimensions.size(); j++)
      {
         multiplier *=  dimensions.at(j);
      }
      symbolItem.arrayDimensionsLength.append(multiplier);
      multiplier = 1;
   }
   symbolItem.arrayDimensionsLength.append(1);

   for (int i = 0; i < symbolItem.arrayDimensionsLength.size(); i++)
   {
      printf("%d\n", symbolItem.arrayDimensionsLength.at(i));
   }
}


SymbolTable::symbolItem* SymbolTable::GetSymbol(QString symbol)
{
   return symbolTable_.value(symbol);
}


SymbolTable::Type SymbolTable::GetSymbolArrayType(QString symbol)
{
   SymbolTable::Type type = SymbolTable::integer;
   symbolItem* symbolItem = symbolTable_.value(symbol);
   type = symbolItem->symbolArrayType;
   return type;
}


void SymbolTable::SetSymbolArrayType(QString symbol, SymbolTable::Type arrayType)
{
   symbolItem* symbolItem = symbolTable_.value(symbol);
   symbolItem->symbolArrayType = arrayType;
}


void SymbolTable::SetSymbol(QString symbol, SymbolTable::symbolItem symbolItm)
{
   SymbolTable::symbolItem* symbolItemOld = symbolTable_.value(symbol);
   memcpy(symbolItemOld, &symbolItm, sizeof(symbolItm));
}


void SymbolTable::AddLocalItem(QString symbol, Type type)
{
   symbolItem* local = new symbolItem;
   local->symbolValue = numOfLocalVars_;
   local->symbolType = type;
   local->symbolAddress = stackOffset_++;
   symbolTable_[symbol] = local; 
   numOfLocalVars_++;
}


void SymbolTable::AddParameterItem(QString symbol, Type type)
{
   symbolItem* parameter = new symbolItem;
   parameter->symbolValue = numOfParameters_;
   parameter->symbolType = type;
   parameter->symbolAddress = stackOffset_++;
   symbolTable_[symbol] = parameter; 
   numOfParameters_++;
}


unsigned int SymbolTable::GetNumOfLocalVars()
{
   return numOfLocalVars_;
}

unsigned int SymbolTable::GetNumOfParameters()
{
   return numOfParameters_;
}


unsigned int SymbolTable::GetSymbolOffset(QString symbol)
{
   symbolItem* symbolItem = symbolTable_.value(symbol);;
   return symbolItem->symbolAddress;
}


 unsigned int SymbolTable::GetSymbolValue(QString symbol)
 {
    symbolItem* symbolItem = symbolTable_.value(symbol);
    return symbolItem->symbolValue;
 }


 SymbolTable::Type SymbolTable::GetSymbolType(QString symbol)
 {
    SymbolTable::Type type = SymbolTable::integer;
    symbolItem* symbolItem = symbolTable_.value(symbol);
    type = symbolItem->symbolType;
    return type;
 }
 

 bool SymbolTable::SymbolExists(QString symbol)
 {
    bool symbolFound = false;
    if( symbolTable_.contains(symbol))
    {
       symbolFound = true;
    }

    return symbolFound;
}


void SymbolTable::EmptySymbolTable()
{
   symbolTable_.clear();
}


void SymbolTable::DumpTable()
{
   ProgramSymbolTable::iterator it = symbolTable_.begin();
   for(it = symbolTable_.begin(); it != symbolTable_.end(); it++)
   {
      printf("Symbol = %s : Value = %d\n", it.key().toUtf8().data(), symbolTable_.value(it.key()));
   }
}


void SymbolTable::PrintVariables()
{
   ProgramSymbolTable::iterator it = symbolTable_.begin();
   for(it = symbolTable_.begin(); it != symbolTable_.end(); it++)
   {
      printf("%15s\t\tDB\t%d\n", it.key().toUtf8().data(), symbolTable_.value(it.key()));
   }
}




