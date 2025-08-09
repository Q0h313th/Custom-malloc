#include <iostream>
#include <unistd.h>

with namespace std;

int main(){ cout << "Hello world" << endl; write(1, "Hello world\n", 12); }
