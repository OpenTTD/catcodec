catcodec README
Last updated:    2010-08-25
Release version: 1.0.1
------------------------------------------------------------------------


Table of Contents:
------------------
1) About
2) Contacting
3) Installing
4) Running
5) Compiling


1) About:
-- ------
catcodec decodes and encodes sample catalogues for OpenTTD. These sample
catalogues are not much more than some meta-data (description and file name)
and raw PCM data.

catcodec is licensed under the GNU General Public License version 2.0. For
more information, see the file 'COPYING'.


2) Contact:
-- --------
Contacting the author can be done in two ways:
 - sending an email to rubidium@openttd.org
 - chat with him on IRC; Rubidium can be found in #openttd on irc.oftc.net


3) Installation:
-- -------------
Installing catcodec is fairly straightforward. Just copy the executable into
any directory. It is advised to put the executable in one of the directories
in your path so it can be easily found. For example when compiling OpenSFX.
To uninstall simply remove the executable.


4) Usage:
-- ------
Decoding a sample catalogue, e.g. sample.cat, results in a sample.sfo that
contains the file names and descriptions of the samples and all samples with
the file name as specified in the catalogue.

Encoding a sample catalogue, e.g. sample.cat, reads sample.sfo for the file
names and descriptions. It will then load the samples described in sample.sfo
and encodes these into sample.cat.

Generally speaking encoding a file and then decoding it results in the same
file. Decoding of the original, Transport Tycoon Deluxe, sample format will
force the output to be 11025 Hz, 8 bits mono because the meta-data of some of
the samples is incorrect or even missing.

Only PCM WAVE files with only the format and data chunks are supported. Any
other formats need to be converted into this. Furthermore only 11025 Hz,
22050 Hz and 44100 Hz with 8 or 16 bits per sample single channel PCM WAVE
files are supported.

Options for catcodec are (mutually exclusive):
  -d sample_file  Decode the given sample catalogue into its components. The
                  sample_file must have the extension '.cat'. For the output
                  meta-data file the '.cat' is replaced with '.sfo'. The actual
                  samples, in PCM WAVE format, are extracted into files using
                  the file names, including extension, as described in the
                  catalogue or meta-data file.

                  If any of the files already exists a backup is made, by
                  adding '.bak', overwriting the existing backup.

  -e sample_file  Encode the components for the given sample file into a sample
                  catalogue. The sample_file must have the extension '.cat'.
                  For the input meta-data file the '.cat' is replaced with
                  '.sfo'. The actual samples, in PCM WAVE format, are read from
                  files using the file names, including extension, as described
                  in the meta-data file.

                  If the sample_file already exists a backup is made, by adding
                  '.bak', overwriting the existing backup.


5) Compiling:
-- ----------
GCC/ICC:
  Just use "make", or on non-GNU systems "gmake".

Microsoft Visual C++:
  There is no project file, but you can compile catcodec using this compiler
  by either running "make.bat" or "make -f Makefile.msvc". In both cases the
  compiler's executable "cl.exe" must be in the path.
