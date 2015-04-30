CC     = @/usr/bin/gcc
CFLAGS = -Wall -g -std=c99
ECHO   = @echo

all:
	@mkdir -p bin
	$(ECHO) "Output directories created"
	$(CC) $(CFLAGS) -o bin/simulator scheduler.c cpu.c memory.c pager.c control.c
	$(ECHO) "Build simulator"
	$(ECHO) "All binaries build to directory 'bin/'"

clean:
	@rm -rf bin
	$(ECHO) "All binaries removed"