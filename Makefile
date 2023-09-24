all: nxp-mfclassic-brute nxp-chip
	echo 'Done!'

nxp-mfclassic-brute: obj/keys_parser.o obj/mfclassic-nxp-brute.o obj/mfclassic.o obj/bruter.o
	gcc -Iinclude -Wall -Wextra -Wpedantic -o nxp-mfclassic-brute $^ -lnfc_nci_linux

nxp-chip: src/nxp-chip.c
	gcc -Iinclude -Iinclude/ncihal -Wall -Wextra -Wpedantic -o $@ $^ -lnfc_nci_linux

obj/mfclassic.o: src/mfclassic.c
	gcc -Iinclude -Wall -Wextra -Wpedantic -o $@ -c $^

obj/keys_parser.o: src/keys_parser.c
	gcc -Iinclude -Wall -Wextra -Wpedantic -o $@ -c $^

obj/bruter.o: src/bruter.c
	gcc -Iinclude -Wall -Wextra -Wpedantic -o $@ -c $^

obj/mfclassic-nxp-brute.o: src/mfclassic-nxp-brute.c
	gcc -Iinclude -Wall -Wextra -Wpedantic -o $@ -c $^

clean:
	rm -f obj/*.o
	rm -f nxp-mfclassic-brute
	rm -f nxp-chip
