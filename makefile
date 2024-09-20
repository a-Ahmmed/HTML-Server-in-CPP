build: src/main.cpp
	mkdir -p bin
	g++ src/main.cpp -o bin/server.exe -lwsock32