SRC = src
INC = inc
BIN = bin
SCR = pkk_scripts

pkk: $(SRC) $(INC)
	gcc -g $(SRC)/* -I $(INC) -o $(BIN)/pkk

clean:
	rm -rf $(BIN)/*

test:
	./bin/pkk $(SCR)/test.pkk