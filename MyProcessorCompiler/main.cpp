#include "Parser.hpp"
#include "SyntaxChecker.hpp"
#include "Parser.hpp"
#include <QApplication>
#include "Basic.hpp"

int main( int argc, char** argv )
{
   QApplication app(argc, argv);
   
   SyntaxChecker* syntaxChecker = new SyntaxChecker();
   Parser* assemblyGenerator = new Parser();
   BASIC basicDialog(argv[1], *syntaxChecker, *assemblyGenerator);
   basicDialog.showMaximized();
   basicDialog.show();
   app.exec();


   /*QApplication app(argc, argv);
   
   SyntaxChecker* syntaxChecker = new SyntaxChecker();
   syntaxChecker->Parse(argv[1]);
   
   bool error = syntaxChecker->ErrorDetected();
   unsigned erroeCount = syntaxChecker->ErrorCount();
   delete syntaxChecker;

   if (error == false)
   {
      Parser basicProgram;
      QString outputFile(argv[1]); 
      outputFile.remove(".bas");
      basicProgram.SetOutputFile(outputFile.append(".asm"));
      basicProgram.Parse(argv[1]);
   }
   else
   {
      printf("Aborting after: %4d errors.\n", erroeCount);
   }*/
}