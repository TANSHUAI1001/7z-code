CKP: main.o raf.o
	g++ raf.o main.o -lssl -lcrypto -o CKP
main.o: CheckPassword.cpp
	g++ -D "DEBUG" -c verification.cpp -o main.o
raf.o: ReadArchiveFile.cpp
	g++ -D "DEBUG" -c ReadArchiveFile.cpp -o raf.o
clean:
	rm *.o CKP
