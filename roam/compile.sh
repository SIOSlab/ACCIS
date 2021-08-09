f77 -c *.f -g -Wall -pedantic -O3

ar rcs libroam.a *.o

rm *.o
