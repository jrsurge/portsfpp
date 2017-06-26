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

#include <portsf.hpp>
#include <portsf.h>

#include <iostream>

namespace psf
{
    struct SoundFile::portsf_impl
    {
        PSFFILE* file;
        psf_props props;
    };

    SoundFile::SoundFile(const char* path)
        : impl(new portsf_impl), isClosed(false)
    {
        int error = psf_sndOpen(path, &impl->props, 0, &impl->file);
        if (impl->file == nullptr || error != PSF_E_NOERROR)
        {
            markAsInvalid();
        }
        else
        {
            m_handleType = HandleType::READ;
        }
    }

    SoundFile::SoundFile(const char* path, const HeaderFormat headerFormat, const SampleFormat sampleFormat, const int numChannels, const int sampleRate)
        : impl(new portsf_impl), isClosed(false)
    {
        impl->props.chans = numChannels;
        impl->props.chformat = psf_channelformat::STDWAVE;

        switch (headerFormat)
        {
        case HeaderFormat::WAV: impl->props.format = psf_format::PSF_STDWAVE; break;
        case HeaderFormat::AIFF: impl->props.format = psf_format::PSF_AIFF; break;
        }

        switch (sampleFormat)
        {
        case SampleFormat::INT16: impl->props.samptype = psf_stype::PSF_SAMP_16; break;
        case SampleFormat::INT24: impl->props.samptype = psf_stype::PSF_SAMP_24; break;
        case SampleFormat::FLOAT32: impl->props.samptype = psf_stype::PSF_SAMP_IEEE_FLOAT; break;
        }

        impl->props.srate = sampleRate;

        int error = psf_sndCreate(path, &impl->props, 0, 1, PSF_CREATE_WRONLY, &impl->file);
        if (impl->file == nullptr || error != PSF_E_NOERROR)
        {
            markAsInvalid();
        }
        else
        {
            m_handleType = HandleType::WRITE;
        }
    }

    SoundFile::SoundFile(const char* path, const SoundFile& readHandle)
        : impl(new portsf_impl), isClosed(false)
    {
        if (readHandle.isValid())
        {
            impl->props = readHandle.impl->props;
            int error = psf_sndCreate(path, &impl->props, 0, 1, PSF_CREATE_WRONLY, &impl->file);
            if (impl->file == nullptr || error != PSF_E_NOERROR)
            {
                markAsInvalid();
            }
            else
            {
                m_handleType = HandleType::WRITE;
            }
        }
        else
        {
            markAsInvalid();
        }
    }

    int SoundFile::read(float* where, const int numFrames)
    {
        if (m_handleType == HandleType::READ && m_handleType != HandleType::INVALID)
        {
            return psf_sndReadFloatFrames(impl->file, where, numFrames);
        }
        else
        {
            std::cout << "Not a valid READ handle\n";
            return 0;
        }
    }

    int SoundFile::read(double* where, const int numFrames)
    {
        if (m_handleType == HandleType::READ && m_handleType != HandleType::INVALID)
        {
            return psf_sndReadDoubleFrames(impl->file, where, numFrames);
        }
        else
        {
            std::cout << "Not a valid READ handle\n";
            return 0;
        }
    }

    int SoundFile::write(float* what, const int numFrames)
    {
        if (m_handleType == HandleType::WRITE && m_handleType != HandleType::INVALID)
        {
            return psf_sndWriteFloatFrames(impl->file, what, numFrames);
        }
        else
        {
            std::cout << "Not a valid WRITE handle\n";
            return 0;
        }
    }

    int SoundFile::write(double* what, const int numFrames)
    {
        if (m_handleType == HandleType::WRITE && m_handleType != HandleType::INVALID)
        {
            return psf_sndWriteDoubleFrames(impl->file, what, numFrames);
        }
        else
        {
            std::cout << "Not a valid WRITE handle\n";
            return 0;
        }
    }

    void SoundFile::close()
    {
        // Make sure we only close things once
        if(!isClosed)
        {
            markAsInvalid();

            // Attempt to close and free the PSFFILE
            int error = psf_sndClose(impl->file);

            // portsf only frees the PSFFILE itself if it's valid
            if (error != PSF_E_NOERROR)
                free(impl->file); // PSFFILE GETS ALLOCATED WITH MALLOC IN PORTSF, USE FREE

            delete impl; // IMPL GETS ALLOCATED WITH NEW IN PORTSF++, USE DELETE

            isClosed = true;
        }
    }

    int SoundFile::numChannels() const
    {
        return impl->props.chans;
    }

    int SoundFile::numFrames() const
    {
        return psf_sndSize(impl->file);
    }

    int SoundFile::sampleRate() const
    {
        return impl->props.srate;
    }

    bool SoundFile::isRead() const
    {
        return m_handleType == HandleType::READ;
    }
    bool SoundFile::isWrite() const
    {
        return m_handleType == HandleType::WRITE;
    }
    bool SoundFile::isValid() const
    {
        return m_handleType != HandleType::INVALID;
    }

    void SoundFile::seek(int frameOffset, SeekMode seekMode)
    {
        int mode;
        switch (seekMode)
        {
        case SeekMode::SET: mode = PSF_SEEK_SET; break;
        case SeekMode::CUR: mode = PSF_SEEK_CUR; break;
        case SeekMode::END: mode = PSF_SEEK_END; break;
        }

        psf_sndSeek(impl->file, frameOffset, mode);
    }

    void SoundFile::dump()
    {
        std::cout << "HandleType: " << (int)m_handleType << "\n";
        if (m_handleType != HandleType::INVALID)
        {
        std::cout << "\nSoundFile: " << impl->file->filename << "\n"
            << "Channels: " << numChannels() << "\n"
                << "Frames: " << numFrames() << "\n"
                << "HeaderFormat: " << impl->props.format << "\n"
                << "SampleFormat: " << impl->props.samptype << "\n"
                << "SampleRate: " << impl->props.srate << "\n";
        }
    }

    void SoundFile::markAsInvalid()
    {
        m_handleType = HandleType::INVALID;
    }

    SoundFile::~SoundFile()
    {
        close();
    }
}
