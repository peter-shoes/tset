SRC = src
INC = inc
BIN = bin
TEST = test/t_scripts

pkk: $(SRC) $(INC)
	gcc -g $(SRC)/* -I $(INC) -o $(BIN)/tset

run:
	./bin/tset $(TEST)/test1.t

clean:
	rm -rf $(BIN)/*

