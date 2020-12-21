# task_for_pid
A PoC demonstrating how to use task_for_pid in modern MacOS. Verified on El Capitan (10.11) but should work up to Catalina (10.15) since the API has not changed.

## Overview
task_for_pid allows you to retrieve a pid for user-land processes that are not signed by Apple. If you are trying to hook the kernel, or applications like Safari or Pages, task_for_pid will fail. task_for_pid's definition is as follows:
```
kern_return_t task_for_pid(  
                mach_port_t parent, 
                int pid,
                mach_port_t *task_out ); 
```

* parent should be the calling task and can be satisfied by `mach_task_self()`.
* pid is the task you want to attach to.
* task_out is a pointer to a mach_port struct that will hold the task port id.
* task_for_pid returns a kernel code. The two you will generally see are KERN_SUCCESS( 0 ) and (os/kern) failure( 5 ).

An example call:
```
kern_return_t kern_return = 0;

mach_port_t task = 0;

long int pid = _some_pid;

kern_return = task_for_pid( mach_task_self(), pid, &task );  
if( kern_return != KERN_SUCCESS )  
{
    printf( "task_for_pid failed: %s", mach_error_string( kern_return ) );
    return 0;
}
```

You do not need to disable SIP to call task_for_pid. You also do not need to sign your code, but it doesn't hurt.

## Compiling
For the kernel to allow us to use `task_for_pid`, we need to give our application the `SecTaskAccess` permission. To do this, create a file called `Info.plist` with the following content:

```
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
```

When compiling, use -sectcreate to create a section for the plist:
`gcc task_for_pid.c -sectcreate __TEXT __info_plist ./Info.plist -o task_for_pid`

## Usage
Execute the target, find its process id with `ps`, and then run `sudo ./task_for_pid 1234` where `1234` is the pid found by `ps`.

## (os/kern) failure
If you get an (os/kern) failure, make sure you are running with root. If it still fails, create a new system certificate in the keychain and use it to sign task_for-pid via `codesign -s hook-cert ./task_for_pid`.

## Why?
With a mach task, you can do pretty much anything, including reading memory (mach_vm_read), writing memory (mach_vm_write), mapping memory (mach_vm_map), and scanning memory regions (mach_vm_region and mach_vm_region_recurse). 

## Caveats
Due to security features in Mac OS X El Capitan, `task_for_pid` will correctly fill in `task` but will always output `3331`.

## Resources
1. A [blog post](http://os-tres.net/blog/2010/02/17/mac-os-x-and-task-for-pid-mach-call/) for 10.5 (Leopard) from 2010, which I used a fair amount.
2. A [blog post](https://www.spaceflint.com/?p=150) from 2016 which basically parrots the aforementioned post with some refinements.
3. The [Bit Slicer](https://github.com/zorgiepoo/Bit-Slicer) source code.
4. An [article](http://uninformed.org/index.cgi?v=4&a=3&p=14) from 2006 about ptrace.
