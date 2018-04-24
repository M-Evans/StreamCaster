#include <stdio.h>

#include "http.h"

// maybe cl-args in future?
//int main(int argc, char** argv) {
int main() {
    //char** channels = get_channels()
    
    fetch("somafm.com");
    //fetch("example.com");
    
    return 0;
}

