/*
 * console.h
 * Author: Christian Würthner
 * Description: console code for project 2
 */
 
#ifndef __CONSOLE__
#define __CONSOLE__

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

/* Declaration of flag for verbose output mode */
extern bool console_verbose_mode;

/* Prints a log message. Logs will only be printed if verbose mode is enabled */
void console_log(const char *composer, const char *format, ...);

/* Prints a log message, will also be printed if verbose mode is disabled */
void console_log_force(const char *composer, const char *format, ...);

/* Prints an error message, will also be printed if verbose mode is disabled */
void console_error(const char *composer, const char *format, ...);

/* Prints the line start with [*composer*] */
void console_print_composer(const char *composer);

#endif