// CreateVersion.cpp : Defines the entry point for the console application.
// Based on: https://github.com/mnunberg/git-describe-parse by M. Nunberg
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <Shlwapi.h>
//#include <boost/algorithm/string.hpp>
//#include <boost/algorithm/string/classification.hpp>
//#include <string>


#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

using namespace std;


//
//static const char* trim(char &input)
//{
//	
//	static char* trimChars = " \t\n";
//	
//	boost::algorithm::replace_all(input, trimChars, {'\0'});
//
//	size_t first = str.find_first_not_of(trimChars);
//	if (string::npos == first)
//	{
//		return str.c_str();
//		
//	}
//
//
//	
//	size_t last = str.find_last_not_of(trimChars);
//	return str.substr(first, (last - first + 1)).c_str();
//}

static void find_delim(char *s, char **res, int *ix)
{
	for (; *ix && s[*ix] != '-'; *ix -= 1);
	if (*ix) {
		s[*ix] = '\0';
		*res = s + *ix + 1;
	}
	else {
		*res = s;
	}
}


int main(int argc, char **argv)
{
	unsigned int major = 0, mid = 0, minor = 0;
	char vbuf[4096] = { 0 };
	char *version="1.0.0", *ncommits="0", *sha="0";
	FILE *po;
	int buflen;
	int outval;
	char* trimChars = " \t\n";

	// Open output file
	char * filename = argv[1];
	if (!filename) {
		fprintf(stderr, "Bad output file name\n");
		return -5;
	}

	FILE * f;
	errno_t err = fopen_s(&f, filename, "w+");
	if (err || !f)
	{
		fprintf(stderr, "Could not oupen output file name (errno=%d)\n", err);
		return -6;
	}

	fprintf(stderr, "Starting CreateVersion\n");
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <Outputfile>\n", argv[0]);
		outval = -1;
		goto FINAL;
	}

	/** Get the output */
	po = popen("cmd /c echo v2.1.8-31-g5d25ffd", "r");
	if (!po) {
		perror("cmd /c echo 42");
		outval = -2;
		goto FINAL;
	}

	if (!fgets(vbuf, sizeof(vbuf), po)) {
		fprintf(stderr, "git describe closed stream\n");
		outval = -3;
		goto FINAL;
	}
	pclose(po);
	buflen = (int)strlen(vbuf);
	vbuf[buflen - 2] = '\0';
	buflen -= 1;

	// SHA1	 (Remove the leading 'g')
	find_delim(vbuf, &sha, &buflen);
	sha++;

	// Commit #	 - Will be used as build number
	find_delim(vbuf, &ncommits, &buflen);
	
	version = vbuf;
	auto trimmed = StrTrim((PTSTR)version, (PCTSTR)trimChars);
	
	// Break the version number into its components
	int nFields = sscanf_s(version, "v%u.%u.%u",&major, &mid, &minor) ;
	if (nFields != 3) {
		fprintf(stderr, "Version string corrupt\n");
		outval = -4;
		goto FINAL;
	}



FINAL:
	// Put data into file
	fprintf(f, "#pragma once\n");
	fprintf(f, "#define VER_H_ %u\n", major);
	fprintf(f, "#define VER_M_ %u\n", mid);
	fprintf(f, "#define VER_L_ %u\n", minor);
	fprintf(f, "#define BUILD %s\n", ncommits);
 	fprintf(f, "\n");
	fprintf(f, "#define FILEVER\t\t\t\t\"%u, %u, %u, %s\"\n", major, mid, minor, ncommits);
	fprintf(f, "#define PRODVER_TAG\t\t\t\"%s\"\n", version);
	fprintf(f, "#define PRODVER_SHA1\t\t\"%s\"\n", sha);
 	fprintf(f, "\n");


	return 0;
}

//#include "stdafx.h"
//
//
//int main()
//{
//    return 0;
//}
//
