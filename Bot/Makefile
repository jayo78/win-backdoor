# Compilation requires linking the ws2_32 lib.

FLAGS= -s -ffunction-sections -fdata-sections -Wno-write-strings -fno-exceptions -fmerge-all-constants -static-libstdc++ -static-libgcc
CC= x86_64-w64-mingw32-gcc

bot.exe: botmain.c modules.c modules.h config.h
	$(CC) modules.c botmain.c -o bot.exe -lws2_32 $(FLAGS)
clean:
	rm -rf *.exe 
