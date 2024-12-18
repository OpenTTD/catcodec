/* $Id$ */

/*
 * catcodec is a tool to decode/encode the sample catalogue for OpenTTD.
 * Copyright (C) 2009  Remko Bijker
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/** @file catcodec.cpp Encoding and decoding of "cat" files */

#include "stdafx.h"
#include "io.hpp"
#include "sample.hpp"
#include "version.h"

/** Are we run interactively, i.e. from the console, or from a script? */
static bool _interactive;

/** Show progress, but only on interactive consoles */
static void ShowProgress()
{
	if (!_interactive) return;

	printf(".");
	fflush(stdin);
}


/**
 * Read a cat file from a reader and extract it's samples.
 * @param samples collection to put our samples in
 * @param reader  reader for the file
 */
static void ReadCat(Samples &samples, FileReader &reader)
{
	uint32_t count = reader.ReadDword();
	bool new_format = (count >> 31) != 0;
	count &= 0x7FFFFFFFU;
	count /= 8;

	reader.Seek(0);
	for (uint32_t i = 0; i < count; i++) {
		samples.emplace_back(reader);
	}

	for (auto iter = samples.begin(); iter != samples.end(); ++iter) {
		iter->ReadCatEntry(reader, new_format);
		ShowProgress();
	}
}

/**
 * Write a cat file (including the samples) to a cat file
 * @param samples collection samples to write to the cat file
 * @param writer writer for the file
 */
static void WriteCat(Samples &samples, FileWriter &writer)
{
	uint32_t offset = (uint32_t)samples.size() * 8;
	for (auto iter = samples.begin(); iter != samples.end(); ++iter) {
		Sample &sample = *iter;

		sample.SetOffset(offset);
		offset = sample.GetNextOffset();

		writer.WriteDword(sample.GetOffset() | (1U << 31));
		writer.WriteDword(sample.GetSize());
	}

	for (auto iter = samples.begin(); iter != samples.end(); ++iter) {
		iter->WriteCatEntry(writer);
		ShowProgress();
	}
}


/**
 * Read a sfo file from a reader and and the samples mentioned in there
 * @param samples collection to put our samples in
 * @param reader  reader for the sfo file
 */
static void ReadSFO(Samples &samples, FileReader &reader)
{
	/* Temporary read buffer; 512 is long enough for all valid
	 * lines because the filename and name may be at most 255.
	 * Add a single space separator and the terminator and you
	 * got exactly 512. */
	char buffer[512] = "";
	char *filename;

	while (reader.ReadLine(buffer, sizeof(buffer)) != NULL) {
		/* Line with comment */
		if (strncmp(buffer, "//", 2) == 0) continue;

		char *name;
		if (*buffer == '"') {
			filename = buffer + 1;
			name = strchr(filename, '"');
		} else {
			filename = buffer;
			name = strchr(filename, ' ');
		}
		if (name == NULL) {
			throw "Invalid format for " + reader.GetFilename() + " at [" + buffer + "]";
		}

		*name = '\0';
		name++;
		while (isspace(*name)) name++;

		char *newline = name + strlen(name) - 1;
		while (isspace(*newline)) {
			*newline = '\0';
			newline--;
		}

		if (strlen(filename) + 1 > 255) throw "Filename is too long in " + reader.GetFilename() + " at [" + buffer + "]";
		if (strlen(name)     + 1 > 255) throw "Name is too long in " + reader.GetFilename() + " at [" + name + "]";

		samples.emplace_back(filename, name);

		ShowProgress();
	}
}

/**
 * Write a sfo file and the samples to disk
 * @param samples collection samples to write to the sfo file and disk
 * @param writer writer for the sfo file
 */
static void WriteSFO(Samples &samples, FileWriter &writer)
{
	writer.WriteString("// \"file name\" internal name\n");

	for (auto iter = samples.begin(); iter != samples.end(); ++iter) {
		Sample &sample = *iter;

		writer.WriteString("\"%s\" %s\n", sample.GetFilename().c_str(), sample.GetName().c_str());

		FileWriter sample_writer(sample.GetFilename());
		sample.WriteSample(sample_writer);
		sample_writer.Close();

		ShowProgress();
	}
}


/**
 * Show the help to the user.
 * @param cmd the command line the user used
 */
void ShowHelp(const char *cmd)
{
	printf(
		"catcodec version %s - Copyright 2009 by Remko Bijker\n"
		"Usage:\n"
		"  %s -d <sample file>\n"
		"    Decode all samples in the sample file and put them in this directory\n"
		"  %s -e <sample file>\n"
		"    Encode all samples in this directory and put them in the sample file\n"
		"\n"
		"<sample file> denotes the .cat file you want to work on, e.g. sample.cat\n"
		"\n"
		"catcodec is Copyright 2009 by Remko Bijker\n"
		"You may copy and redistribute it under the terms of the GNU General Public\n"
		"License version 2, as stated in the file 'COPYING'\n",
		_catcodec_version, cmd, cmd
	);
}

/**
 * Oh hello, the user has found the way in :)
 * @param argc the number of arguments + 1
 * @param argv list with given arguments
 */
int main(int argc, char *argv[])
{
	int ret = 0;
	Samples samples;
	_interactive = isatty(fileno(stdout)) == 1;

	try {
		if (argc != 3 || (strcmp(argv[1], "-d") != 0 && strcmp(argv[1], "-e") != 0)) {
			ShowHelp(argv[0]);
			return 0;
		}

		char sfo_file[1024];
		strncpy(sfo_file, argv[2], sizeof(sfo_file));
		char *ext = strrchr(sfo_file, '.');
		if (ext == NULL || strlen(ext) != 4 || strcmp(ext, ".cat") != 0) {
			throw std::string("Unexpected extension; expected \".cat\"");
		}
		strcpy(ext, ".sfo");

		if (strcmp(argv[1], "-d") == 0) {
			/* Decode the file, so read the cat and then write the sfo */

			if (_interactive) printf("Reading %s\n", argv[2]);
			FileReader reader(argv[2]);
			ReadCat(samples, reader);

			if (_interactive) printf("\nWriting %s\n", sfo_file);
			FileWriter sfo_writer(sfo_file, false);
			WriteSFO(samples, sfo_writer);
			sfo_writer.Close();
		} else if (strcmp(argv[1], "-e") == 0) {
			/* Encode the file, so read the sfo and then write the cat */

			if (_interactive) printf("Reading %s\n", sfo_file);
			FileReader sfo_reader(sfo_file, false);
			ReadSFO(samples, sfo_reader);

			if (_interactive) printf("\nWriting %s\n", argv[2]);
			FileWriter cat_writer(argv[2]);
			WriteCat(samples, cat_writer);
			cat_writer.Close();
		} else {
			/* Some invalid second param -> show the help */
			ShowHelp(argv[0]);
			return -1;
		}
		if (_interactive) printf("\nDone\n");

	} catch (const std::string &s) {
		fprintf(stderr, "An error occured: %s\n", s.c_str());
		ret = -1;
	}

	return ret;
}
