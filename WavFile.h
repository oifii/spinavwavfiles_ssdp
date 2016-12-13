/*
 * Copyright (c) 2010-2016 Stephane Poirier
 *
 * stephane.poirier@oifii.org
 *
 * Stephane Poirier
 * 3532 rue Ste-Famille, #3
 * Montreal, QC, H2X 2L1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef WAVFILE_H
#define WAVFILE_H

#include <stdio.h>

#ifndef uint
typedef unsigned int uint;
#endif           


/// WAV audio file 'riff' section header
typedef struct 
{
    char riff_char[4];
    int  package_len;
    char wave[4];
} WavRiff;

/// WAV audio file 'format' section header
typedef struct 
{
    char  fmt[4];
    int   format_len;
    short fixed;
    short channel_number;
    int   sample_rate;
    int   byte_rate;
    short byte_per_sample;
    short bits_per_sample;
} WavFormat;

/// WAV audio file 'data' section header
typedef struct 
{
    char  data_field[4];
    uint  data_len;
} WavData;


/// WAV audio file header
typedef struct 
{
    WavRiff   riff;
    WavFormat format;
    WavData   data;
} WavHeader;


/// Class for reading WAV audio files.
class WavInFile
{
private:
    /// File pointer.
    FILE *fptr;

    /// Counter of how many bytes of sample data have been read from the file.
    uint dataRead;

    /// WAV header information
    WavHeader header;

    /// Read WAV file headers.
    /// \return zero if all ok, nonzero if file format is invalid.
    int readWavHeaders();

    /// Checks WAV file header tags.
    /// \return zero if all ok, nonzero if file format is invalid.
    int checkCharTags();

    /// Reads a single WAV file header block.
    /// \return zero if all ok, nonzero if file format is invalid.
    int readHeaderBlock();

    /// Reads WAV file 'riff' block
    int readRIFFBlock();

public:
    /// Constructor: Opens the given WAV file. If the file can't be opened,
    /// throws 'runtime_error' exception.
    WavInFile(const char *filename);

    /// Destructor: Closes the file.
    ~WavInFile();

    /// Close the file. Notice that file is automatically closed also when the 
    /// class instance is deleted.
    void close();

    /// Rewind to beginning of the file
    void rewind();

    /// Get sample rate.
    uint getSampleRate() const;

    /// Get number of bits per sample, i.e. 8 or 16.
    uint getNumBits() const;

    /// Get sample data size in bytes. Ahem, this should return same information as 
    /// 'getBytesPerSample'...
    uint getDataSizeInBytes() const;

    /// Get total number of samples in file.
    uint getNumSamples() const;

    /// Get number of bytes per audio sample (e.g. 16bit stereo = 4 bytes/sample)
    uint getBytesPerSample() const;
    
    /// Get number of audio channels in the file (1=mono, 2=stereo)
    uint getNumChannels() const;

    /// Get the audio file length in milliseconds
    uint getLengthMS() const;

    /// Reads audio samples from the WAV file. This routine works only for 8 bit samples.
    /// Reads given number of elements from the file or if end-of-file reached, as many 
    /// elements as are left in the file.
    ///
    /// \return Number of 8-bit integers read from the file.
    int read(char *buffer, int maxElems);

    /// Reads audio samples from the WAV file to 16 bit integer format. Reads given number 
    /// of elements from the file or if end-of-file reached, as many elements as are 
    /// left in the file.
    ///
    /// \return Number of 16-bit integers read from the file.
    int read(short *buffer,     ///< Pointer to buffer where to read data.
             int maxElems       ///< Size of 'buffer' array (number of array elements).
             );

    /// Reads audio samples from the WAV file to floating point format, converting 
    /// sample values to range [-1,1[. Reads given number of elements from the file
    /// or if end-of-file reached, as many elements as are left in the file.
    ///
    /// \return Number of elements read from the file.
    int read(float *buffer,     ///< Pointer to buffer where to read data.
             int maxElems       ///< Size of 'buffer' array (number of array elements).
             );

    /// Check end-of-file.
    ///
    /// \return Nonzero if end-of-file reached.
    int eof() const;
};



/// Class for writing WAV audio files.
class WavOutFile
{
private:
    /// Pointer to the WAV file
    FILE *fptr;

    /// WAV file header data.
    WavHeader header;

    /// Counter of how many bytes have been written to the file so far.
    int bytesWritten;

    /// Fills in WAV file header information.
    void fillInHeader(const uint sampleRate, const uint bits, const uint channels);

    /// Finishes the WAV file header by supplementing information of amount of
    /// data written to file etc
    void finishHeader();

    /// Writes the WAV file header.
    void writeHeader();

public:
    /// Constructor: Creates a new WAV file. Throws a 'runtime_error' exception 
    /// if file creation fails.
    WavOutFile(const char *fileName,    ///< Filename
               int sampleRate,          ///< Sample rate (e.g. 44100 etc)
               int bits,                ///< Bits per sample (8 or 16 bits)
               int channels             ///< Number of channels (1=mono, 2=stereo)
               );

    /// Destructor: Finalizes & closes the WAV file.
    ~WavOutFile();

    /// Write data to WAV file. This function works only with 8bit samples. 
    /// Throws a 'runtime_error' exception if writing to file fails.
    void write(const char *buffer,     ///< Pointer to sample data buffer.
               int numElems             ///< How many array items are to be written to file.
               );

    /// Write data to WAV file. Throws a 'runtime_error' exception if writing to
    /// file fails.
    void write(const short *buffer,     ///< Pointer to sample data buffer.
               int numElems             ///< How many array items are to be written to file.
               );

    /// Write data to WAV file in floating point format, saturating sample values to range
    /// [-1..+1[. Throws a 'runtime_error' exception if writing to file fails.
    void write(const float *buffer,     ///< Pointer to sample data buffer.
               int numElems             ///< How many array items are to be written to file.
               );

    /// Finalize & close the WAV file. Automatically supplements the WAV file header
    /// information according to written data etc.
    ///
    /// Notice that file is automatically closed also when the class instance is deleted.
    void close();
};

#endif
