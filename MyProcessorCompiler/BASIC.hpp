#include <ui_BASIC.h>
#include "SyntaxChecker.hpp"
#include "Parser.hpp"
#include "SyntaxHighLighter.h"


class BASIC:public QMainWindow, private Ui::MainWindow
{
   Q_OBJECT
public:
   BASIC::BASIC(QString fileName, SyntaxChecker& syntaxChecker, Parser& assemblyGenerator);

public slots:

   void CompileFile();

   void BuildProject();

   void Error(QString errorMessage);

   void SaveChanges();
   
   void OpenFile();

   void GoToFile(QListWidgetItem* item);

signals:

   void Compile(QString filename);

   

private:
   void CreateConnections();

private :
   QString filename_;

   SyntaxChecker& syntaxChecker_;
   
   Parser& assemblyGenerator_;

   SyntaxHighLighter *highlighter_;

   unsigned int numberOfFilesOpened_;

   QString currentWorkingDirectory_;

   QString machineCodeFilePath_;

};