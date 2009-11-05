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

/** @file io.cpp Implementation of reading/writing to files */

#include "stdafx.h"
#include "io.hpp"

FileReader::FileReader(string filename, bool binary)
{
	this->file = fopen(filename.c_str(), binary ? "rb" : "r");
	this->filename = filename;

	if (this->file == NULL) {
		throw "Could not open " + filename + " for reading";
	}
}

FileReader::~FileReader()
{
	fclose(this->file);
}

uint8_t FileReader::ReadByte()
{
	uint8_t b;
	this->ReadRaw(&b, 1);
	return b;
}

uint16_t FileReader::ReadWord()
{
	uint16_t b = this->ReadByte();
	return (this->ReadByte() << 8) | b;
}

uint32_t FileReader::ReadDword()
{
	uint32_t b = this->ReadWord();
	return (this->ReadWord() << 16) | b;
}

void FileReader::ReadRaw(uint8_t *in, size_t amount)
{
	if (fread(in, 1, amount, this->file) != amount) {
		throw "Unexpected end of " + this->filename;
	}
}

char *FileReader::ReadLine(char *in, int length)
{
	char *ret = fgets(in, length, this->file);

	/* fgets doesn't guarantee string termination if no newline/EOF is found */
	in[length - 1] = '\0';

	return ret;
}

void FileReader::Seek(uint32_t pos)
{
	if (fseek(this->file, pos, SEEK_SET) != 0) throw "Seeking in " + this->filename + " failed.";
}

uint32_t FileReader::GetPos()
{
	return ftell(this->file);
}

string FileReader::GetFilename() const
{
	return this->filename;
}


FileWriter::FileWriter(string filename, bool binary)
{
	this->filename_new = filename + ".new";
	this->filename = filename;

	this->file = fopen(filename_new.c_str(), binary ? "w+b" : "w+");

	if (this->file == NULL) {
		throw "Could not open " + this->filename_new + " for writing";
	}
}

FileWriter::~FileWriter()
{
	if (this->file != NULL) {
		fclose(this->file);
		unlink(this->filename_new.c_str());
	}
}

void FileWriter::WriteByte(uint8_t data)
{
	this->WriteRaw(&data, 1);
}

void FileWriter::WriteWord(uint16_t data)
{
	this->WriteByte(data & 0xFF);
	this->WriteByte(data >> 8);
}

void FileWriter::WriteDword(uint32_t data)
{
	this->WriteWord(data & 0xFFFF);
	this->WriteWord(data >> 16);
}

void FileWriter::WriteRaw(const uint8_t *out, size_t amount)
{
	assert(this->file != NULL);

	if (fwrite(out, 1, amount, this->file) != amount) {
		throw "Unexpected failure while writing to " + this->filename;
	}
}

void FileWriter::WriteString(const char *format, ...)
{
	assert(this->file != NULL);

	va_list ap;

	va_start(ap, format);
	if (vfprintf(this->file, format, ap) < 0) {
		throw "Unexpected failure while writing to " + this->filename;
	}
	va_end(ap);
}

uint32_t FileWriter::GetPos()
{
	assert(this->file != NULL);

	return ftell(this->file);
}

string FileWriter::GetFilename() const
{
	return this->filename;
}

void FileWriter::Close()
{
	/* First close the .new file */
	fclose(this->file);
	this->file = NULL;

	/* Then remove the existing .bak file */
	string filename_bak = this->filename + ".bak";
	if (unlink(filename_bak.c_str()) != 0 && errno != ENOENT) {
		fprintf(stderr, "Warning: could not remove %s (%s)\n", filename_bak.c_str(), strerror(errno));
	}

	/* Then move the existing file to .bak */
	if (rename(this->filename.c_str(), filename_bak.c_str()) != 0 && errno != ENOENT) {
		fprintf(stderr, "Warning: could not rename %s to %s (%s)\n", this->filename.c_str(), filename_bak.c_str(), strerror(errno));
	}

	/* And finally move the .new file to the actual wanted filename */
	if (rename(this->filename_new.c_str(), this->filename.c_str()) != 0) {
		fprintf(stderr, "Warning: could not rename %s to %s (%s)\n", this->filename_new.c_str(), this->filename.c_str(), strerror(errno));
		throw "Could not close " + this->filename;
	}
}
