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

/** @file io.hpp Interface for reading/writing to files */

#ifndef IO_H
#define IO_H

/**
 * Simple class to perform binary and string reading from a file.
 */
class FileReader {
	FILE *file;      ///< The file to be read by this instance
	std::string filename; ///< The filename of the file

public:
	/**
	 * Create a new reader for the given file.
	 * @param filename the file to read from
	 * @param binary   read the file as binary or text?
	 */
	FileReader(const std::string &filename, bool binary = true);

	/**
	 * Cleans up our mess
	 */
	~FileReader();


	/**
	 * Read a single byte from the stream.
	 * @return the read byte
	 */
	uint8_t ReadByte();

	/**
	 * Read a word (2 bytes) from the stream in little endian.
	 * @return the read word
	 */
	uint16_t ReadWord();

	/**
	 * Read a dword (4 bytes) from the stream in little endian.
	 * @return the read dword
	 */
	uint32_t ReadDword();

	/**
	 * Read a number of raw bytes from the stream.
	 * @param in     the buffer where to put the data
	 * @param amount the amount of bytes to read
	 */
	void ReadRaw(uint8_t *in, size_t amount);

	/**
	 * Read a line of text from the stream.
	 * @param in     the buffer where to put the data
	 * @param length the maximum amount of bytes to read
	 */
	char *ReadLine(char *in, int length);


	/**
	 * Go to a specific location in the stream.
	 * @param pos the position to go to.
	 */
	void Seek(uint32_t pos);

	/**
	 * Get the current position in the stream.
	 * @return the position in the stream
	 */
	uint32_t GetPos();

	/**
	 * Get the filename of this file.
	 * @return the filename
	 */
	const std::string &GetFilename() const;
};

/**
 * Simple class to perform binary and string writing to a file.
 */
class FileWriter {
	FILE *file;          ///< The file to be read by this instance
	std::string filename;     ///< The filename of the file
	std::string filename_new; ///< The filename for the temporary file

public:
	/**
	 * Create a new writer for the given file.
	 * @param filename the file to write to
	 * @param binary   write the file as binary or text?
	 */
	FileWriter(const std::string &filename, bool binary = true);

	/**
	 * Cleans up our mess
	 */
	~FileWriter();

	/**
	 * Write a single byte to the stream.
	 * @param data the byte to write
	 */
	void WriteByte(uint8_t data);

	/**
	 * Write a word (2 bytes) to the stream in little endian.
	 * @param data the word to write
	 */
	void WriteWord(uint16_t data);

	/**
	 * Write a dword (4 bytes) to the stream in little endian.
	 * @param data the dword to write
	 */
	void WriteDword(uint32_t data);

	/**
	 * Write a number of raw bytes to the stream.
	 * @param out    the buffer of data to write
	 * @param amount the amount of bytes to write
	 */
	void WriteRaw(const uint8_t *out, size_t amount);

	/**
	 * Write a line of text to the stream.
	 * @param format the format of the written string
	 * @param ... the data to actually write
	 */
	void WriteString(const char *format, ...);

	/**
	 * Get the current position in the stream.
	 * @return the position in the stream
	 */
	uint32_t GetPos();

	/**
	 * Get the filename of this file.
	 * @return the filename
	 */
	const std::string &GetFilename() const;

	/**
	 * Close the output, i.e. commit the file to disk.
	 * If this is not done, the file with not be written to disk.
	 */
	void Close();
};

#endif /* IO_H */
