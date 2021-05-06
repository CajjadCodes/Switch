CC=g++
CF=-pedantic -W

main: main.cpp main.hpp 
	$(CC) $(CF) main.cpp

System: System.cpp System.hpp 
	$(CC) $(CF) System.cpp

Switch: Switch.cpp Switch.hpp 
	$(CC) $(CF) Switch.cpp

.PHONY: clean
clean:
	rm -rf main System Switch &> /dev/nu