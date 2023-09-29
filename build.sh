cd includes
g++ -Wall -Wextra -O2 -g -c *.cpp
cd ..
g++ -Wall -Wextra -g -O2 includes/*.o main.cpp -o main.out
