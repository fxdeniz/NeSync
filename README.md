
# NeSync ![128px_version-control_5815495](https://github.com/fxdeniz/NeSync/assets/104620840/02a32910-d2c3-433b-adad-9e0a6ea6c86c)


_Pronounced as_ __Any Sync__.

__NeSync__ is a modern backup/sync software for desktop with a GUI.<br>
It monitors your filesytem for events (modification, create, delete and move) then adds detected changes to your files in historical order.
In each revision you can add description to changed file. It works with every file format and compatible with __Linux__, __Windows__ and __MacOS__.
<br><br>
NeSync uses intenal filesystem which is OS independent.
That is, you create your own folder tree inside the app then assign this folders to your OS file system. 
This allows you to transfer your folder tree (with historical changes) from computer to computer (even between different OSes).
Transfers are done via zip files (manually exported and imported as zip by the user).
<br><br>
__NeSync does not connect to the internet and works offline completely.__<br>
NeSync is __Free and open-source software (FOSS)__ and source code licensed under GPL v3 (completely open to the public).

__WARNING: NeSync is currently in pre-alpha stage and it's not complete yet.__

Executables are available in the [Releases](https://github.com/fxdeniz/NeSync/releases) page.


## Screenshots
### Folder Monitor
![monitor](https://github.com/fxdeniz/NeSync/assets/104620840/022d9798-bc30-4ace-8796-0f71d70f4936)
### File Explorer
![mainwindow](https://github.com/fxdeniz/NeSync/assets/104620840/3780549f-3a4b-474b-8696-a0214b7dfb4b)
### Backup Import Screen
![import](https://github.com/fxdeniz/NeSync/assets/104620840/06664327-5bb1-41ac-ac1c-6fe1a8526d38)


## Features
* Fully multi-threaded architecture
* Small memory footprint
* Designed for modern OSes (Minimum required OS: Windows 10 1809  or Linux kernel 5.4)
* Complete privacy (no internet connection at all)


## Outline

* [Few important notes](#few-important-notes)
* [Building](#building)
* [Contributing](#contributing)
* [Future plans](#future-plans)
* [Thanks to](#thanks-to)

## Few important notes

1. You will see different project names in the git history such as `XdBackup` and `BoldBackup`.<br>
   These are old names for the __NeSync__.
   
2. If you detect network activity related with __NeSync__ process you can safely ignore it.<br>
   This activities probably generated by the OS. Especially on Windows (see VirusTotal link in download section).
   
3. MacOS support is not properly tested and I __DO NOT provide__ official builds currently.

4. When importing a zip file, json schema of `import.json` __IS NOT verified__.<br>
   This is a huge security risk. Only import files which you know the source.


## Building

* Only CMake is supported (both on Linux and Windows).
* Minimum Qt 6.3 required.
* Compiling in all platforms tested with gcc compiler (MinGW on Windows).
* I've never tested MSVC.

```
    git clone https://github.com/fxdeniz/NeSync.git
    git submodule update --init
```

* Building on Linux
    1. Install `Qt 5 Compability Module` from `Qt Maintaince Tool`.
    
    2. Install `zlib-devel` and `bzip2-devel` (on fedora) OR `zlib1g-dev` and `libbz2-dev` (on debian).
    
    3. Then compile using Qt Creator.
    
* Building on Windows
    1. Install `Qt 5 Compability Module` from `Qt Maintaince Tool`.
    
    2. Then download [zlib](https://packages.msys2.org/package/mingw-w64-x86_64-zlib) and 
       [bzip2](https://packages.msys2.org/package/mingw-w64-x86_64-bzip2) from `MSYS2`.
       
    3. Extract the zips.
    
    4. Copy the content of the extracted zips to matching folders of your MinGW directory.<br>
       Example MinGW directory `C:\Qt\Tools\mingw1120_64\`. This directory is created when installing Qt 6.4 and Qt 6.5.
       If you have different Qt version then your `C:\Qt\Tools\mingwXXXX_XX\` will be different.
       <br>
       Example for `zlib`:
       ```
            mingw64\bin\zlib1.dll       --->    C:\Qt\Tools\mingw1120_64\bin\zlib1.dll
            
            mingw64\include\zlib.h      --->    C:\Qt\Tools\mingw1120_64\include\zlib.h        
            mingw64\include\zconf.h     --->    C:\Qt\Tools\mingw1120_64\include\zconf.h
            
            mingw64\lib\libz.a          --->    C:\Qt\Tools\mingw1120_64\lib\libz.a
            mingw64\lib\libz.dll.a      --->    C:\Qt\Tools\mingw1120_64\lib\libz.dll.a
       ```
       Do the same for `bzip2`.
       
    5. Then compile using Qt Creator.

## Contributing

I use [camel case](https://en.wikipedia.org/wiki/Camel_case) notation where only class initals are capitalized.<br>
And [Allman style](https://en.wikipedia.org/wiki/Indentation_style#Allman_style) indentation with tab equal to 4 spaces.
    
Example:
```
    class ExampleClass
    {
        void doSomething()
        {
            if(x == y)
                doOtherThing();
            else
            {
                doLongConditionalItem1();
                doLongConditionalItem2();                    
            }
        }
    };
```
    
1. Create an issue.
2. Make a pull request linked to a created issue.


## Future plans

For the near future I'm planning to add these features:
1. Documentation for the code (compatible with Doxygen)
2. Local logging


## Thanks to

* __SpartanJ__ for efsw library | [GitHub Repo](https://github.com/SpartanJ/efsw)
* __stachenov__ for QuaZip library | [GitHub Repo](https://github.com/stachenov/quazip)
* __Qt Framework developers__ for Qt Framework | [The Qt Company Website](https://www.qt.io)
* __SQLite project team__ for SQLite | [SQLite Home Page](https://www.sqlite.org)
* __Jean-loup Gailly__ & __Mark Adler__ for zlib library | [zlib Home Site](https://zlib.net)
* __Julian Seward__ for bzip2 library | [bzip2 Home](https://sourceware.org/bzip2)
* __juicy_fish__ for application icon | [freepik.com](https://www.freepik.com/icon/version-control_5815495)
* __NSIS Community__ for Windows installer | [NSIS Wiki](https://nsis.sourceforge.io)
