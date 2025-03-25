# OS_Project1

## Operating Systems Project 1:
encryption logger progam in c++ using vigenere

### To run:

open cslinux1 server (or any linux server)
upload/create 3 files:
- driver.cpp
- encryption.cpp
- logger.cpp

compile all 3 files:
$ g++ driver.cpp -o driver -std=c++11
$ g++ encryption.cpp -o encryption -std=c++11
$ g++ logger.cpp -o logger -std=c++11

run:
$ ./driver logfile.txt

"logfile.txt: --> fileName

view results:
$ cat logfile.txt
    OR
$ vi logfile.txt
