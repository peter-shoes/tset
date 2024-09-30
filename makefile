SRC = src
INC = inc
BIN = bin
TEST = testsuite
DEFTEST = $(TEST)/def.tests
MATHDEFTEST = $(TEST)/mathdef.tests
TABLETEST = $(TEST)/table.tests
MATHSPLITTEST = $(TEST)/mathsplit.tests


tset: $(SRC) $(INC) $(BIN)
	gcc -g $(SRC)/* -I $(INC) -o $(BIN)/tset

run:
	./bin/tset $(TEST)/test1.t

clean:
	rm -rf $(BIN)/*

$(BIN):
	mkdir -p $@

check-def: tset
	echo "\033[34mRunning def tests ...\033[0m"
	./testsuite/testrunner.sh $(DEFTEST)

check-mathdef: tset
	echo "\033[34mRunning mathdef tests ...\033[0m"
	./testsuite/testrunner.sh $(MATHDEFTEST)

check-table: tset
	echo "\033[34mRunning table tests ...\033[0m"
	./testsuite/testrunner.sh $(TABLETEST)

check-mathsplit: tset
	echo "\033[34mRunning mathsplit tests ...\033[0m"
	./testsuite/testrunner.sh $(MATHSPLITTEST)

check: tset
	echo "\033[34mRunning all tests ...\033[0m\n"
	make -s check-def
	make -s check-mathdef
	make -s check-table
	make -s check-mathsplit
