ALG: 7z.o
	g++ 7z.o -lssl -lcrypto -o ALG
7z.o: 7z.cpp
	g++ -D "DEBUG" -c 7z.cpp
	
clean:
	rm *.o ALG