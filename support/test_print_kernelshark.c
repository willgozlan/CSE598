#include <stdio.h>

int main(){

    //open file as normal

    FILE* fd = fopen( "/sys/kernel/debug/tracing/trace_marker", "a" );
    if( fd == NULL ){
        perror("Could not open trace marker");
        return -1;
    }   

    //Print like to a file. Always must flush buffer between writes.

    fprintf( fd, "my trace marker" );
    fflush( fd );
    fprintf( fd, "my trace marker 2" );
    fflush( fd );
    fprintf( fd, "my trace marker3 " );
    fflush( fd );
    fprintf( fd, "my trace marker 4" );
    fflush( fd );
    fprintf( fd, "my trace marker 5" );
    fflush( fd );

    return 0;
}
