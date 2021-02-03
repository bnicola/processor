#ifndef __MY_PROCESSOR_ASSEMBLER_HPP__
#define __MY_PROCESSOR_ASSEMBLER_HPP__
#include <QString>
#include <QStringList>
#include <QMap>
#include "AssemblerElement.hpp"

class MyProcessorAssembler
{
public:
   typedef struct
   {
      QString tokenType;
      QString tokenString;
      QString tokenLineCode;
      quint32 tokenLineNumber;
      quint32 tokenIndex;
      quint32 tokenLineAddress;
   }TokenStruct;

   public:
      MyProcessorAssembler();
      ~MyProcessorAssembler();

      void Assemble(QString file, QString outputPath);

   private:
      bool IsLiteral(QString literal);
      bool IsLabel(QString literal);
      bool IsHex(QChar ch);
      bool IsOpCode(QString opCode);
      bool IsRegister(QString reg);
      void Tokenize(QStringList lines);
      void ShowCodeError(TokenStruct token);
      TokenStruct NextToken();
      QString ClassifyOpCode(QString currentOpCode);

      void ParseRegisterOpCode(QString file, quint32 pass);
      void ParsePushPopOpCode(QString file, quint32 pass);
      void ParseSpcOpCode(QString file, quint32 pass);
      void ParseMoveOpCode(QString file, quint32 pass);
      void ParseLiOpCode(QString file, quint32 pass);
      void ParseRegisterImmediateOpCode(QString file, quint32 pass);
      void ParseJumpImmediateOpCode(QString file, quint32 pass);
      void ParseLoadStoreOpCode(QString file, quint32 pass);
      void ParseReturnOpCode(QString file, quint32 pass);
      void ParseLoadImmediateOpCode(QString file, quint32 pass);

      void ParseProgram(QString file, quint8 pass);
      void GenerateMachineCode();
      void CreateAssemberHexFile(QString dumpFile);
      QStringList  Parse(QString code);

      void Error(QString file, QString error, quint32 line);

   private:

      TokenStruct currentToken_;

      QStringList tokens_;

      QMap<QString, quint8> opCodes_;

      QStringList registers_;

      QList<AssemblerElement*> assembly_;

      bool detectedErrors_;

      QMap<QString, quint8> symbolTable_;

      QList<TokenStruct> finalTokens_;

      quint32 currentTokenIndex_;

      QList<TokenStruct> tokensList_;

      bool error_;

      QString outputPath_;

      quint32 currentInstrAddress_;


};

#endif //__MY_PROCESSOR_ASSEMBLER_HPP__
