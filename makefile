all: guava

guava: Symbol.o GuavaSymTable.o GuavaTree.o GuavaDriver.o GuavaParser.o GuavaLexer.o Guava.o

	g++ -o guava Symbol.o GuavaSymTable.o GuavaTree.o GuavaDriver.o GuavaParser.o GuavaLexer.o Guava.o

GuavaDriver.o: GuavaDriver.cc GuavaDriver.hh GuavaTree.hh GuavaParser.hh

	g++ -c  GuavaDriver.cc

GuavaParser.o: GuavaParser.cc GuavaParser.hh GuavaTree.hh GuavaDriver.hh

	g++ -c GuavaParser.cc 

GuavaParser.cc GuavaParser.hh: GuavaParser.yy

	bison --defines=GuavaParser.hh -o GuavaParser.cc GuavaParser.yy

GuavaLexer.o: GuavaLexer.cc GuavaParser.hh GuavaDriver.hh

	g++ -c GuavaLexer.cc

GuavaLexer.cc: GuavaLexer.l

	flex -o GuavaLexer.cc GuavaLexer.l

GuavaSymTable.o: GuavaSymTable.cc GuavaSymTable.hh Symbol.hh GuavaTree.hh

	g++ -c GuavaSymTable.cc

Symbol.o: Symbol.cc Symbol.hh

	g++ -c Symbol.cc

GuavaTree.o: GuavaTree.cc GuavaTree.hh GuavaSymTable.hh

	g++ -c GuavaTree.cc

Guava.o: Guava.cc

	g++ -c Guava.cc

.PHONY: clean

clean:

	-rm *.o GuavaParser.hh GuavaParser.cc location.hh position.hh stack.hh guava GuavaLexer.cc


