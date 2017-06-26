// portsf++
//
//  This file is part of portsf++
//  Copyright(c) 2017, James Surgenor
//
//  A C++ wrapper around a modified version of portsf by Richard Dobson.
//
//  portsf++ was created because portsf doesn't have any C++ wrappers.
//  portsf also internally manages the files, using a fixed length array -
//  a limit we can do without if the library relinquishes memory
//  management.
//
//  portsf++ uses RAII to ensure resources are properly allocated and
//  cleared up, using portsf's original mechanisms
//
//  portsf++ also uses pimpl to shield the inner workings of portsf,
//  some of which are now exposed with the modifications made to the
//  underlying library.
//

#ifndef _PORTSF_HPP_
#define _PORTSF_HPP_

namespace psf
{
    enum class HandleType
    {
        READ = 0,
        WRITE,
        INVALID
    };

    enum class HeaderFormat
    {
        WAV = 0,
        AIFF
    };

    enum class SampleFormat
    {
        INT16 = 0,
        INT24,
        FLOAT32
    };

    enum class SeekMode
    {
        SET = 0,
        CUR,
        END
    };

    class SoundFile
    {
    public:
        // Construct a READ handle
        SoundFile(const char* path);

        // Disable copy
        SoundFile(const SoundFile& other) = delete;

        // Construct a WRITE handle
        SoundFile(const char* path, const HeaderFormat headerFormat, const SampleFormat sampleFormat = SampleFormat::INT16, const int numChannels = 2, const int sampleRate = 44100);

        // Construct a WRITE handle from READ handle
        // This uses the properties (HeaderFormat, SampleFormat, etc) from an existing READ handle to construct a WRITE handle
        SoundFile(const char* path, const SoundFile& readHandle);

        // Read numFrames from file into where (float)
        int read(float* where, const int numFrames);

        // Read numFrames from file into where (double)
        int read(double* where, const int numFrames);

        // Write numFrames from what into file
        int write(float* what, const int numFrames);

        // Write numFrames from what into file
        int write(double* what, const int numFrames);

        // Close and invalidate the handle
        void close();

        // Returns the number of channels in the file
        int numChannels() const;
        // Returns the number of frames in the file (samples per channel)
        int numFrames() const;
        // Returns the sample rate of the file
        int sampleRate() const;

        // Checks if this is a READ handle
        bool isRead() const;
        // Checks if this is a WRITE handle
        bool isWrite() const;
        // Checks if this is a valid handle
        bool isValid() const;

        // Seeks in the file to frameOffset, based on the SeekMode
        // SeekMode::SET - from beginning
        // SeekMode::CUR - from current position
        // SeekMode::END - from end
        void seek(int frameOffset, SeekMode seekMode);

        // Print out all info about this handle
        void dump();

        // Closes the soundfile
        ~SoundFile();

    private:
        struct portsf_impl;
        portsf_impl* impl;
        HandleType m_handleType;
        bool isClosed;

        void markAsInvalid();
    };

} // psf namespace

#endif // _PORTSF_HPP_
