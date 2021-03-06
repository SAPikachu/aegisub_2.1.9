// Copyright (c) 2008, Rodrigo Braz Monteiro
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name of the Aegisub Group nor the names of its contributors
//     may be used to endorse or promote products derived from this software
//     without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// -----------------------------------------------------------------------------
//
// AEGISUB
//
// Website: http://aegisub.cellosoft.com
// Contact: mailto:zeratul@cellosoft.com
//


///////////
// Headers
#include "config.h"

#include "audio_provider_convert.h"
#include "audio_provider_downmix.h"
#include "aegisub_endian.h"


///////////////
// Constructor
ConvertAudioProvider::ConvertAudioProvider(AudioProvider *src) {
	source = src;
	channels = source->GetChannels();
	num_samples = source->GetNumSamples();
	sample_rate = source->GetSampleRate();
	bytes_per_sample = 2;

	sampleMult = 1;
	if (sample_rate < 16000) sampleMult = 4;
	else if (sample_rate < 32000) sampleMult = 2;
	sample_rate *= sampleMult;
	num_samples *= sampleMult;
}


//////////////
// Destructor
ConvertAudioProvider::~ConvertAudioProvider() {
	delete source;
}


/////////////////////
// Convert to 16-bit
void ConvertAudioProvider::Make16Bit(const char *src, short *dst, int64_t count) {
	for (int64_t i=0;i<count;i++) {
		dst[i] = (short(src[i])-128)*255;
	}
}


//////////////////////
// Change sample rate
// This requres 16-bit input
// The SampleConverter is a class overloading operator() with a function from short to short
template<class SampleConverter>
void ConvertAudioProvider::ChangeSampleRate(const short *src, short *dst, int64_t count, const SampleConverter &converter) {
	// Upsample by 2
	if (sampleMult == 2) {
		int64_t size = count/2;
		short cur;
		short next = 0;
		for (int64_t i=0;i<size;i++) {
			cur = next;
			next = converter(*src++);
			*(dst++) = cur;
			*(dst++) = (cur+next)/2;
		}
		if (count%2) *(dst++) = next;
	}

	// Upsample by 4
	else if (sampleMult == 4) {
		int64_t size = count/4;
		short cur;
		short next = 0;
		for (int64_t i=0;i<size;i++) {
			cur = next;
			next = converter(*src++);
			*(dst++) = cur;
			*(dst++) = (cur*3+next)/4;
			*(dst++) = (cur+next)/2;
			*(dst++) = (cur+next*3)/4;
		}
		for (int i=0;i<count%4;i++) *(dst++) = next;
	}

	// Nothing much to do, just ensure correct endedness
	else if (sampleMult == 1) {
		while (count-- > 0) {
			*dst++ = converter(*src++);
		}
	}
}


// Do-nothing sample converter for ChangeSampleRate
struct NullSampleConverter {
	inline short operator()(const short val) const {
		return val;
	}
};

// Endian-swapping sample converter for ChangeSampleRate
struct EndianSwapSampleConverter {
	inline short operator()(const short val) const {
		return (short)Endian::Reverse((uint16_t)val);
	};
};

void DownsampleBitDepthTo16(void* src, short* destination, int64_t count, int src_bytes_per_sample, bool is_native_endian)
{
	// sanity check
	if (src_bytes_per_sample <= 2)
	{
		throw _T("Input bitdepth is 16bit or less, something is wrong in the code!");
	}
	else if (src_bytes_per_sample > 8)
	{
		throw _T("Input bitdepth is 64bit or more, that is unsupported now.");
	}
	for (int64_t i = 0; i < count; i++)
	{
		int64_t sample = 0;
		if (is_native_endian)
		{
#ifdef HAVE_LITTLE_ENDIAN
			memcpy(&sample, src, src_bytes_per_sample);
#else
			memcpy(((char*)&sample) + (sizeof(int64_t) - src_bytes_per_sample), src, src_bytes_per_sample);
#endif
		} else {
			for (int byte_index = 0; i < src_bytes_per_sample; i++)
			{
#ifdef HAVE_LITTLE_ENDIAN
				*(((char*)&sample) + byte_index) = 
					*(((char*)src) + (src_bytes_per_sample - byte_index - 1));
#else
				*(((char*)&sample) + sizeof(int64_t) - 1 - byte_index) = 
					*(((char*)src) + byte_index);
#endif
			}
		}

		sample >>= ((src_bytes_per_sample - sizeof(short)) * 8);
		*destination = (short)sample;
		src = (char*)src + src_bytes_per_sample;
		destination++;
	}
}

/////////////
// Get audio
void ConvertAudioProvider::GetAudio(void *destination, int64_t start, int64_t count) {
	// Bits per sample
	int srcBps = source->GetBytesPerSample();

	// Nothing to do
	if (sampleMult == 1 && srcBps == 2) {
		source->GetAudio(destination,start,count);
	}

	// Convert
	else {
		// Allocate buffers with sufficient size for the entire operation
		size_t fullSize = count;
		int64_t srcCount = count / sampleMult;
		short *buffer1 = NULL;
		short *buffer2 = NULL;
		short *last = NULL;

		bool isNativeEndian = source->AreSamplesNativeEndian();

		if (srcBps > 2)
		{
			fullSize *= srcBps;
			// don't mess up the size, 
			// just allocate more memory than needed
		}

		// Read audio
		buffer1 = new short[fullSize * channels];
		source->GetAudio(buffer1,start/sampleMult,srcCount);

		// Convert from 8-bit to 16-bit
		if (srcBps == 1) {
			if (sampleMult == 1) {
				Make16Bit((const char*)buffer1,(short*)destination,srcCount * channels);
			}
			else {
				buffer2 = new short[fullSize * channels];
				Make16Bit((const char*)buffer1,buffer2,srcCount * channels);
				last = buffer2;
			}
		}

		else if (srcBps == 2) {
			// Already 16-bit
			last = buffer1;
		} else {
			if (sampleMult == 1) {
				DownsampleBitDepthTo16(buffer1,(short*)destination, srcCount * channels, srcBps, isNativeEndian);
			}
			else {
				buffer2 = new short[fullSize * channels];
				DownsampleBitDepthTo16(buffer1, buffer2, srcCount * channels, srcBps, isNativeEndian);
				last = buffer2;
			}

			// endianness is corrected when converting bitdepth
			isNativeEndian = true;
		}

		// Convert sample rate
		if (sampleMult != 1 && isNativeEndian) {
			ChangeSampleRate(last,(short*)destination,count * channels, NullSampleConverter());
		}
		else if (!isNativeEndian) {
			ChangeSampleRate(last,(short*)destination,count * channels, EndianSwapSampleConverter());
		}

		delete [] buffer1;
		delete [] buffer2;
	}
}

// See if we need to downmix the number of channels
AudioProvider *CreateConvertAudioProvider(AudioProvider *source_provider) {
	AudioProvider *provider = source_provider;

	// Aegisub requires 16 bit samples,
	// some audio players break with low samplerates,
	// everything breaks with wrong-ended samples.
	if (provider->GetBytesPerSample() != 2 ||
		provider->GetSampleRate() < 32000 ||
		!provider->AreSamplesNativeEndian())
	{
		provider = new ConvertAudioProvider(provider);
	}

	// We also require mono audio for historical reasons
	if (provider->GetChannels() != 1)
	{
		provider = new DownmixingAudioProvider(provider);
	}

	return provider;
}
