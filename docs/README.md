# catcodec

## Table of Contents

- 1) About
- 2) Contacting
- 3) Installing
- 4) Running
- 5) Compiling

## 1) About
catcodec decodes and encodes sample catalogues for OpenTTD. These sample
catalogues are not much more than some meta-data (description and file name)
and raw PCM data.

catcodec is licensed under the GNU General Public License version 2.0. For
more information, see the file 'COPYING'.

## 2) Contact

Contacting the author can be done in two ways:
 - sending an email to rubidium@openttd.org
 - chat with him on IRC; Rubidium can be found in #openttd on irc.oftc.net


## 3) Installation

Installing catcodec is fairly straightforward. Just copy the executable into
any directory. It is advised to put the executable in one of the directories
in your path so it can be easily found. For example when compiling OpenSFX.
To uninstall simply remove the executable.

## 4) Usage

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

While catcodec does not care about the audio file format, the format must be
supported by OpenTTD. Currently only Ogg Opus and PCM WAVE .wav files are
supported, and the audio must be mono.

Options for catcodec are (mutually exclusive):
* -d sample_file
    Decode the given sample catalogue into its components. The
    sample_file must have the extension '.cat'. For the output
    meta-data file the '.cat' is replaced with '.sfo'. The audio
    data is extracted into files using the file names, including
    extension, as described in the catalogue or meta-data file.

    If any of the files already exists a backup is made, by
    adding '.bak', overwriting the existing backup.

* -e sample_file
    Encode the components for the given sample file into a sample
    catalogue. The sample_file must have the extension '.cat'.
    For the input meta-data file the '.cat' is replaced with
    '.sfo'. The audio data is read from files using the file
    names, including extension, as described in the meta-data file.

    If the sample_file already exists a backup is made, by adding
    '.bak', overwriting the existing backup.

## 5) Compiling

Minimum required version of CMake is 3.16.
By default this produces a Debug build with assertations enabled.

```bash
mkdir build
cd build
cmake ..
cmake --build .
```
