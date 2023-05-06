Derleme Şekli:
lsServer için
gcc -c lsServer.c
gcc lsServer.o -o lsServer -pthread(thread fonksiyonları için)
./lsServer
tellMeMORE için
gcc -c tellMeMORE.c
gcc tellMeMORE.o -o tellMeMORE -phread(thread fonksiyonları için)
./tellMeMore pathName
^C ->bitirmek için
