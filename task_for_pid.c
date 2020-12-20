/*
    To compile, create a file called Info.plist with the following content:
    
    <?xml version="1.0" encoding="UTF-8"?>  
    <!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">  
    <plist version="1.0">  
    <dict>  
        <key>SecTaskAccess</key>
        <array>
            <string>allowed</string>
        </array>
    </dict>  
    </plist>  
    
    When compiling, use -sectcreate to create a section for the plist:
    
    gcc task_for_pid.c -sectcreate __TEXT __info_plist ./Info.plist -o task_for_pid  
    Run using sudo ./task_for_pid _some_pid
 */

/*!
*   task_for_pid.c: Given a pid in argv[ 1 ], return the mach task port.
*/
#include <stdio.h>
#include <stdlib.h>
#include <mach/mach.h>

int main( int argc, char** argv )
{
    kern_return_t kern_return = 0;

    mach_port_t task = 0;

    long int pid = 0;

    char *endptr = NULL;

    if( argc < 2 ) 
    {
        return 0;
    }

    pid = strtol( argv[ 1 ], &endptr, 10 );

    kern_return = task_for_pid( mach_task_self(), pid, &task );
    if( kern_return != KERN_SUCCESS ) 
    {
        printf( "task_for_pid failed: %s\n", mach_error_string( kern_return ) );
        return 0;
    }

    printf( "%u\n", task );

    return 0;
}
