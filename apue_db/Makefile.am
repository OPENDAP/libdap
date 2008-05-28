
AUTOMAKE_OPTIONS = foreign 

AM_CPPFLAGS = -Wall

check_PROGRAMS = t4
noinst_LTLIBRARIES = libapue_db.la

libapue_db_la_SOURCES = error.c lockreg.c db.c apue_db.h

t4_SOURCES = t4.c
t4_LDADD = -L. -lapue_db

TESTS = t4
