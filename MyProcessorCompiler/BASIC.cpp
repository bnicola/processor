//!----------------------------------------------------
//!Auto generated file from the Qt Project Builder tool
//!----------------------------------------------------
#include <BASIC.hpp>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QProcess>

BASIC::BASIC(QString fileName, SyntaxChecker& syntaxChecker, Parser& assemblyGenerator) 
: filename_(fileName),
  syntaxChecker_(syntaxChecker),
  assemblyGenerator_(assemblyGenerator),
  numberOfFilesOpened_(0),
  currentWorkingDirectory_("."),
  machineCodeFilePath_("memory.list")
 {
   setupUi(this);
   CreateConnections();
}


void BASIC::CreateConnections()
{
   connect(actionCompile_Current_File_, SIGNAL(triggered()), this, SLOT(CompileFile()));
   connect(actionBuild_, SIGNAL(triggered()), this, SLOT(BuildProject()));
   connect(actionOpenFile_, SIGNAL(triggered()), this, SLOT(OpenFile()));
   connect(&syntaxChecker_, SIGNAL(Error(QString)), this, SLOT(Error(QString)));
   connect(filelistWidget_, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(GoToFile(QListWidgetItem*)));
 }

void BASIC::CompileFile()
{
   compileResult_->clear();
   QString currentVisibleFile = tabWidget_->tabText(tabWidget_->currentIndex());
   compileResult_->insertPlainText("Compiling '" + currentVisibleFile + "'\n");
   syntaxChecker_.Parse(currentVisibleFile);
   bool error = syntaxChecker_.ErrorDetected();
   unsigned errorCount = syntaxChecker_.ErrorCount();
   if (error == false)
   {
      QString generatedAssemblyFileName = currentVisibleFile.remove(".bas") + ".asm";
      compileResult_->insertPlainText("Compiled file " + currentVisibleFile + " Successfully with 0 errors.\n\n");
      compileResult_->insertPlainText("Generating Code " + generatedAssemblyFileName + "\n\n");
      assemblyGenerator_.SetOutputFile(generatedAssemblyFileName);
      assemblyGenerator_.Parse(tabWidget_->tabText(tabWidget_->currentIndex()));
      assemblyGenerator_.SetMachineCodeFile(machineCodeFilePath_);
      QProcess::startDetached("explorer " + currentVisibleFile);
   }
   else
   {
      compileResult_->insertPlainText("Compilation Errors detected in " + currentVisibleFile + ".\n");
      compileResult_->insertPlainText(QString::number(errorCount) + " errors detected.\nAborting translation process.");
   }
}


void BASIC::BuildProject()
{
   compileResult_->clear();
   int numOfFilesToCompile = filelistWidget_->count();
   for (int i = 0; i < numOfFilesToCompile; i++)
   {
      QString fileName = filelistWidget_->item(i)->text();
      compileResult_->insertPlainText("Compiling '" + fileName + "'\n");
      syntaxChecker_.Parse(fileName);
      bool error = syntaxChecker_.ErrorDetected();
      unsigned errorCount = syntaxChecker_.ErrorCount();
      if (error == 0)
      {
         QString generatedAssemblyFileName = fileName.remove(".bas") + ".asm";
         compileResult_->insertPlainText("Compiled file " + fileName + " Successfully with 0 errors.\n\n");
         compileResult_->insertPlainText("Generating Code " + generatedAssemblyFileName + ".\n\n");
         assemblyGenerator_.SetOutputFile(generatedAssemblyFileName);
         assemblyGenerator_.Parse(filelistWidget_->item(i)->text());
      }
      else
      {
         compileResult_->insertPlainText("Compilation Errors detected in " + fileName + ".\n");
         compileResult_->insertPlainText(QString::number(errorCount) + " errors detected.\nAborting translation process.");
      }
   }
}

void BASIC::Error(QString errorMessage)
{
   compileResult_->insertPlainText(errorMessage);
   compileResult_->insertPlainText("\n");
}


void BASIC::SaveChanges()
{
   QString currentVisibleFile = tabWidget_->tabText(tabWidget_->currentIndex());
   QFile file(currentVisibleFile);
   file.remove(currentVisibleFile);
   file.open(QIODevice::WriteOnly | QIODevice::Text);
   QTextStream out(&file);
   QTextEdit* currentWidget = dynamic_cast<QTextEdit*>(tabWidget_->currentWidget());
   if (currentWidget != NULL)
   {
      out << currentWidget->toPlainText();
   }
}

void BASIC::OpenFile()
{
   QString filename = QFileDialog::getOpenFileName(this, tr("Open Basic File"), currentWorkingDirectory_, tr("Basic Files (*.bas)"));
   currentWorkingDirectory_ = filename;
   if (filename.isEmpty() == false)
   {
      bool alreadyOpened = false;
      for (int i = 0; i < filelistWidget_->count(); i++)
      {
         QListWidgetItem* item = filelistWidget_->item(i);
         if (filename == item->text())
         {
             alreadyOpened = true;
         }
      }
      if (!alreadyOpened)
      {
         QTextEdit* newFile = new QTextEdit(filename, this);
         
         QPalette palette = newFile->palette();
         palette.setBrush(QPalette::Active, QPalette::Base, QBrush(QColor(Qt::black)));
         palette.setBrush(QPalette::Inactive, QPalette::Base, QBrush(QColor(Qt::black)));
         palette.setBrush(QPalette::Active, QPalette::Text, QBrush(QColor(Qt::green)));
         palette.setBrush(QPalette::Inactive, QPalette::Text, QBrush(QColor(Qt::green)));
         newFile->setPalette(palette);

         connect(newFile, SIGNAL(textChanged()), this, SLOT(SaveChanges()));
         tabWidget_->insertTab(numberOfFilesOpened_, newFile, filename);
         tabWidget_->setCurrentIndex(numberOfFilesOpened_);
         QFile file(filename);
         file.open(QIODevice::ReadOnly | QIODevice::Text);
         while (!file.atEnd()) 
         {
            newFile->setPlainText(file.readAll());
         }
         newFile->setAcceptRichText(true);
         newFile->setFont(QFont("Courrier", 12));
         SyntaxHighLighter* highlighter = new SyntaxHighLighter(newFile->document());
         filelistWidget_->insertItem(numberOfFilesOpened_, new QListWidgetItem(filename));
         tabWidget_->setTabText(numberOfFilesOpened_++, filename);
      }
   }
}


void BASIC::GoToFile(QListWidgetItem* item)
{
   bool foundFile = false;

   if (item != NULL)
   {
      QString fileName = item->text();
      for (int i = 0; i < tabWidget_->count() && !foundFile; i++)
      {
         if (tabWidget_->tabText(i) == fileName)
         {
            foundFile = true;
            tabWidget_->setCurrentIndex(i);
         }
      }
   }
}

