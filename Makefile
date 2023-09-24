CFLAGS = -Iinclude -Wall -Wextra -Wpedantic

all: nxp-mfclassic-brute nxp-chip
	echo 'Done!'

nxp-mfclassic-brute: obj/keys_parser.o obj/mfclassic-nxp-brute.o obj/mfclassic.o obj/bruter.o
	gcc $(CFLAGS) -o nxp-mfclassic-brute $^ -lnfc_nci_linux

nxp-chip: src/nxp-chip.c
	gcc -Iinclude/ncihal $(CFLAGS) -o $@ $^ -lnfc_nci_linux

obj/mfclassic.o: src/mfclassic.c
	gcc $(CFLAGS) -o $@ -c $^

obj/keys_parser.o: src/keys_parser.c
	gcc $(CFLAGS) -o $@ -c $^

obj/bruter.o: src/bruter.c
	gcc $(CFLAGS) -o $@ -c $^

obj/mfclassic-nxp-brute.o: src/mfclassic-nxp-brute.c
	gcc $(CFLAGS) -o $@ -c $^

clean:
	rm -f obj/*.o
	rm -f nxp-mfclassic-brute
	rm -f nxp-chip
