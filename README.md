# pa-chatbot-qt-v4.2

PraatAnoniem.nl / ChatLokaal.nl chatbot

MIT License

Requirements:

-- Development --
- Visual Studio 2015 or 2017
- HTML Agility Pack
- QT 5

-- Runtime --
- QT 5
- .NET 4.5
- Visual C++ 2015 Runtime
- HTML Agility Pack

-- Development Setup for C++ --
Add to your vs props:

1)  

- x86

            <IncludePath>$(QT_ROOT)5.6\msvc2015_86\include\;$(IncludePath)</IncludePath>
            <LibraryPath>$(QT_ROOT)5.6\msvc2015_86\lib\;$(LibraryPath)</LibraryPath>

 - x64

            <IncludePath>$(QT_ROOT)5.6\msvc2015_64\include\;$(IncludePath)</IncludePath>
            <LibraryPath>$(QT_ROOT)5.6\msvc2015_64\lib\;$(LibraryPath)</LibraryPath>
 
2) Install official QT addon