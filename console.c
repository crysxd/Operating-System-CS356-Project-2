/*
 * console.c
 * Author: Christian Würthner
 * Description: console helper for project 2
 */

#include "console.h"

/* Definition iof fields declared in header */
bool console_verbose_mode = false;

/* Prints a log message. Logs will only be printed if verbose mode is enabled */
void console_log(const char *composer, const char *format, ...) {
	/* Cancel if verbose mode is disabled */
	if(!console_verbose_mode) {
		return;
	}

	/* Print line start */
	console_print_composer(composer);

	/* Standard printf, copied from GNU _printf() source.
	   GNU _printf() source is available here (checked 2015-04-05): 
	   https://sourceware.org/git/?p=glibc.git;a=blob;f=stdio-common/printf.c */
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
	
    /* Start new line */
	printf("\n");	
}

/* Prints a log message, will also be printed if verbose mode is disabled */
void console_log_force(const char *composer, const char *format, ...) {
	/* Print line start */
	console_print_composer(composer);

	/* Standard printf, copied from GNU _printf() source.
	   GNU _printf() source is available here (checked 2015-04-05): 
	   https://sourceware.org/git/?p=glibc.git;a=blob;f=stdio-common/printf.c */
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
	
    /* Start new line */
	printf("\n");
}

/* Prints an error message, will also be printed if verbose mode is disabled */
void console_error(const char *composer, const char *format, ...) {
	/* Print line start */
	console_print_composer(composer);

	/* Print error tag */
	printf("[ERROR] ");

	/* Standard printf, copied from GNU _printf() source.
	   GNU _printf() source is available here (checked 2015-04-05): 
	   https://sourceware.org/git/?p=glibc.git;a=blob;f=stdio-common/printf.c */
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    /* Start new line */
	printf("\n");
}

/* Prints the line start with [*composer*] */
void console_print_composer(const char *composer) {
	/* Create line start */
	char buf[13];
	sprintf(buf, "[%s]", composer);

	/* Print line start */
	printf("%-12s", buf);
}