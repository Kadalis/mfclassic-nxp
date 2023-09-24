### mfclassic-nxp tools

This is tools for interact with Mifare Classic cards via linux_libnfc-nci stack and using NXP chips
Currently, implemented mifare keys bruter and NXP chip checker.

### Bruter usage

~~~
        nxp-mfclassic-brute - tool for bruting mifare classic keys using NXP chips via linux_libnfc-nci.
        Common usage:
        nxp-mfclassic-brute <options>

        Mandatory parameters:
                -f/--keys_file <filepath> - use this file as keys dictionary

        Options:
                -b/--block <block_num> - brute <block_num> block
                -s/--sector <sector_num> - brute first block of <sector_num> sector

        Flags:
                        -h/--help - print this help message and exit
                --keys_a - try all keys only as A key
                --keys_b - try all keys only as B key
                --keys_both - try all keys as both A and B key
~~~

### Keys dictionary format (bruter)

Key dictionary must be LF (not CR or CRLF).
Any line that starts with hex digit (0-9, A-F or a-f) is treated as a key. If line starts with other symbol, then it's ignored.
Line with key can be 12 or 14 symbols long.
If there is no suffix (12 symbols), key will be treated as A&B.
If Key is 14 symbols, it must be suffixed with :A or :B, idicating the key that must be used only as A or B key.
If it's suffixed with other symbols, key will be ignored.

If there is symbol other than hex digit inside key, program will exit.

Key dict example is inside example-keys.txt file

### TODO:
- [ ] Ability to stop bruter using Ctrl+C
- [ ] Dumper/Writer


### About other Mifare Classic attacks

Actually, I started to write those programs with desire to get mifare attacks (such as darkside or even mfkey32) having only android phone with NFC chip by NXP and now I haven't anything to say better then "I don't know".
Maybe it's not possible with NXP chips at all. Maybe this is possible and there will be someone more experienced than me, who can implement those things. Maybe someday in future I will be experienced enough with NFC, Mifare, C programming and drivers to get something like this.

### About libnfc

There is driver in libnfc, that uses linux_libnfc-nci to send bytes. But looks like any mifare tools, based on libnfc can't work with this driver - there is some differences in authentication commands between linux_libnfc-nci and libnfc, for example. Also, there is a number of functions that not implemented, and IDK how to make it work.
