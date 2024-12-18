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

/**
 * @file sample.cpp Implementation of Samples
 *
 * The format of .wav PCM encoded files is fairly simple. The "verbatim"
 * 'RIFF' etc. is written as is in the file. The numbers are always
 * written in the little endian way.
 * <ul>
 *  <li>'RIFF'</li>
 *  <li>dword with number of bytes following</li>
 *  <li>'WAVE'</li>
 *  <li>'fmt '</li>
 *  <li>dword with size of 'fmt ' chunk, always 16</li>
 *  <li>word with audio format, always 1 (PCM)</li>
 *  <li>word with number of channels, always 1</li>
 *  <li>dword with sample rate, always 11025, 22050 or 44100</li>
 *  <li>dword with byte rate, always sample rate * number of channels * bits per sample / 8</li>
 *  <li>word with block alignment, always number of channels * bits per sample / 8</li>
 *  <li>word with bits per sample, always 8 or 16</li>
 *  <li>'data'</li>
 *  <li>dword with number of bytes following</li>
 *  <li>actual raw PCM data</li>
 * </ul>
 *
 * This makes the whole thing 44 bytes + the actual payload long.
 */

#include "stdafx.h"
#include "sample.hpp"

/** The size of the RIFF headers of a WAV file */
static const uint32_t RIFF_HEADER_SIZE = 44;

/**
 * Read a string (byte length including termination, actual data) from a reader.
 * @param reader the reader to read from
 * @return the read string
 */
static string ReadString(FileReader &reader)
{
	uint8_t name_len = reader.ReadByte();
	char buffer[256];

	reader.ReadRaw((uint8_t *)buffer, name_len);
	buffer[name_len - 1] = '\0';

	return buffer;
}

/**
 * Write a string (byte length including termination, actual data) to a writer.
 * @param str    the string to write
 * @param writer the writer to write to
 */
static void WriteString(const string str, FileWriter &writer)
{
	uint8_t str_len = (uint8_t)(str.length() + 1);
	writer.WriteByte(str_len);
	writer.WriteRaw((const uint8_t *)str.c_str(), str_len);
}


Sample::Sample(FileReader &reader)
{
	this->offset = reader.ReadDword() & 0x7FFFFFFF;
	this->size   = reader.ReadDword();
}

Sample::Sample(string filename, string name) :
	offset(0),
	name(name),
	filename(filename)
{
	FileReader sample_reader(filename);
	this->ReadSample(sample_reader, false);
}

void Sample::ReadSample(FileReader &reader, bool check_size)
{
	assert(this->sample_data.empty());

	if (reader.ReadDword() != 'FFIR') throw "Unexpected chunk; expected \"RIFF\" in " + reader.GetFilename();

	if (check_size) {
		if (reader.ReadDword() + 8  != size  ) throw "Unexpected RIFF chunk size in " + reader.GetFilename();
	} else {
		this->size = reader.ReadDword() + 8;
	}
	if (reader.ReadDword() != 'EVAW') throw "Unexpected format; expected \"WAVE\" in " + reader.GetFilename();
	if (reader.ReadDword() != ' tmf') throw "Unexpected format; expected \"fmt \" in " + reader.GetFilename();
	if (reader.ReadDword() != 16    ) throw "Unexpected fmt chunk size in " + reader.GetFilename();
	if (reader.ReadWord()  != 1     ) throw "Unexpected audio format; expected \"PCM\" in " + reader.GetFilename();

	this->num_channels = reader.ReadWord();
	if (this->num_channels != 1) throw "Unexpected number of audio channels; expected 1 in " + reader.GetFilename();

	this->sample_rate = reader.ReadDword();
	if (this->sample_rate != 11025 && this->sample_rate != 22050 && this->sample_rate != 44100) throw "Unexpected same rate; expected 11025, 22050 or 44100 in " + reader.GetFilename();

	/* Read these and validate them later on.
	 * Saving them is unnecesary as they can be easily calucated. */
	uint32_t byte_rate   = reader.ReadDword();
	uint16_t block_align = reader.ReadWord();

	this->bits_per_sample = reader.ReadWord();
	if (this->bits_per_sample != 8 && this->bits_per_sample != 16) throw "Unexpected number of bits per channel; expected 8 or 16 in " + reader.GetFilename();

	if (byte_rate != this->sample_rate * this->num_channels * this->bits_per_sample / 8) throw "Unexpected byte rate in " + reader.GetFilename();
	if (block_align != this->num_channels * this->bits_per_sample / 8) throw "Unexpected block align in " + reader.GetFilename();

	if (reader.ReadDword() != 'atad') throw "Unexpected chunk; expected \"data\" in " + reader.GetFilename();

	/* Sometimes the files are padded, which causes them to start at the
	 * wrong offset further on, so just read whatever amount of data was
	 * specified in the top RIFF as long as sample size is within those
	 * boundaries, i.e. within the RIFF. */
	this->sample_size = reader.ReadDword();
	if (this->sample_size + RIFF_HEADER_SIZE > size) throw "Unexpected data chunk size in " + reader.GetFilename();

	this->sample_data.resize(this->size - RIFF_HEADER_SIZE);
	reader.ReadRaw(this->sample_data.data(), this->size - RIFF_HEADER_SIZE);
}

void Sample::ReadCatEntry(FileReader &reader, bool new_format)
{
	assert(this->sample_data.empty());

	if (reader.GetPos() != this->GetOffset()) throw "Invalid offset in file " + reader.GetFilename();

	this->name = ReadString(reader);

	if (!new_format && this->GetName().compare("Corrupt sound") == 0) {
		/* In the old format there was one sample that was raw PCM. */
		this->sample_size     = this->size;
		this->sample_data.resize(this->sample_size);
		reader.ReadRaw(this->sample_data.data(), this->sample_size);

		this->size += RIFF_HEADER_SIZE;
	} else {
		this->ReadSample(reader);
	}

	if (!new_format) {
		/* The old format had sometimes the wrong values for e.g.
		 * sample rate which made the playback too fast. */
		this->num_channels    = 1;
		this->sample_rate     = 11025;
		this->bits_per_sample = 8;
	}

	/* Some kind of data byte, unused */
	reader.ReadByte();

	this->filename = ReadString(reader);
}

void Sample::WriteSample(FileWriter &writer) const
{
	assert(!this->sample_data.empty());

	writer.WriteDword('FFIR');
	writer.WriteDword(this->size - 8);
	writer.WriteDword('EVAW');

	writer.WriteDword(' tmf');
	writer.WriteDword(16);
	writer.WriteWord(1);
	writer.WriteWord(this->num_channels);
	writer.WriteDword(this->sample_rate);
	writer.WriteDword(this->sample_rate * this->num_channels * this->bits_per_sample / 8);
	writer.WriteWord(this->num_channels * this->bits_per_sample / 8);
	writer.WriteWord(this->bits_per_sample);

	writer.WriteDword('atad');
	writer.WriteDword(this->sample_size);
	writer.WriteRaw(this->sample_data.data(), this->size - RIFF_HEADER_SIZE);
}

void Sample::WriteCatEntry(FileWriter &writer) const
{
	assert(!this->sample_data.empty());

	if (writer.GetPos() != this->GetOffset()) throw "Invalid offset when writing file " + writer.GetFilename();

	WriteString(this->GetName(), writer);
	this->WriteSample(writer);

	/* Some kind of separator byte */
	writer.WriteByte(0);

	WriteString(this->GetFilename(), writer);
}

string Sample::GetName() const
{
	return this->name;
}

string Sample::GetFilename() const
{
	return this->filename;
}

void Sample::SetOffset(uint32_t offset)
{
	assert(this->offset == 0);
	this->offset = offset;
}

uint32_t Sample::GetNextOffset() const
{
	return this->offset +
			1 +                            // length of the name
			(this->name.length() + 1) +    // the name + '\0'
			this->size +                   // size of the data
			1 +                            // the delimiter
			1 +                            // length of the filename
			(this->filename.length() + 1); // the filename + '\0'
}

uint32_t Sample::GetOffset() const
{
	return this->offset;
}

uint32_t Sample::GetSize() const
{
	return this->size;
}
