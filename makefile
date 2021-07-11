uttt: UTTT.o
	gcc UTTT.o -o uttt -lncurses
UTTT.o: UltimateTicTacToe.c
	gcc -c UltitmateTicTacToe.c -o UTTT.o
