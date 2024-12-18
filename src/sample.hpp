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

/** @file sample.hpp Interface for Sample reading/writing */

#ifndef SAMPLE_HPP
#define SAMPLE_HPP

#include <vector>
#include "io.hpp"

/**
 * Simple in-memory representation of a sample.
 */
class Sample {
private:
	uint32_t offset;          ///< Offset from the begin of the cat
	uint32_t size;            ///< The size of the WAV RIFF, i.e. excluding name and filename

	std::string name;              ///< The name of the sample
	std::string filename;          ///< The filename of the sample

	uint16_t num_channels;    ///< Number of channels; either 1 or 2
	uint32_t sample_rate;     ///< Sample rate; either 11025, 22050 or 44100
	uint16_t bits_per_sample; ///< Number of bits per sample; either 8 or 16

	uint32_t sample_size;     ///< The size of the raw data below
	std::vector<uint8_t> sample_data; ///< The actual raw sample data

public:
	/**
	 * Create a new sample by reading data from a file.
	 * In this case the data comes from a cat file, so for now we only
	 * read the offset and size from the file.
	 * @param reader the file to read from
	 */
	Sample(FileReader &reader);

	/**
	 * Creates a new sample by reading the sample from a given (wav) file.
	 * @param filename the file to read the sample from
	 * @param name     the name of the sample
	 */
	Sample(const std::string &filename, const std::string &name);

	/**
	 * Reads a sample from a reader.
	 * It reads WAV files (if that is the only thing in the file).
	 * This function has some very strict tests on validity of the input file.
	 * @param reader     place to read the sample from
	 * @param check_size whether to check that our size makes sense with the size from the sample
	 */
	void ReadSample(FileReader &reader, bool check_size = true);

	/**
	 * Reads a cat entry from a reader.
	 * This function has some very strict tests on validity of the input file.
	 * @param reader     place to read the cat entry from
	 * @param new_format whether this is the old or new format; there are different strictness tests for both cases
	 */
	void ReadCatEntry(FileReader &reader, bool new_format);

	/**
	 * Write a sample to a writer. If only a sample is written to the
	 * file it would be a valid WAV file.
	 * @param writer place to write the sample to
	 */
	void WriteSample(FileWriter &writer) const;

	/**
	 * Write a cat entry to a writer.
	 * @param writer place to write the cat entry to
	 */
	void WriteCatEntry(FileWriter &writer) const;


	/**
	 * Get the name of the sample.
	 * @return the name of the sample
	 */
	const std::string &GetName() const;

	/**
	 * Get the filename of the sample
	 * @return the filename of the sample
	 */
	const std::string &GetFilename() const;


	/**
	 * Set the offset from the begin of the cat to this cat entry.
	 * @param offset the offset.
	 */
	void SetOffset(uint32_t offset);

	/**
	 * Get the offset for the cat entry that follows us.
	 * @return the offset for the next cat entry
	 */
	uint32_t GetNextOffset() const;

	/**
	 * Get the offset from the begin of the cat to this cat entry.
	 * @return the offset
	 */
	uint32_t GetOffset() const;


	/**
	 * Get the size of the WAV part of the file.
	 * @return the size
	 */
	uint32_t GetSize() const;
};

/** Lets have us a vector of samples */
using Samples = std::vector<Sample>;

#endif /* SAMPLE_HPP */
