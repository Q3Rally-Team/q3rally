/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2005 Stuart Dalton (badcdev@gmail.com)
Copyright (C) 2005-2006 Joerg Dietrich <dietrich_joerg@gmx.de>
Copyright (C) 2006 Thilo Schulz <arny@ats.s.bawue.de>

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

// MP3 support is enabled by this define
#ifdef USE_CODEC_MP3

// includes for the Q3 sound system
#include "client.h"
#include "snd_codec.h"

// includes for the MP3 codec
#ifdef USE_INTERNAL_MP3
#include "mad.h"
#else
#include <mad.h>
#endif

#define MP3_SAMPLE_WIDTH		2
#define MP3_PCMSAMPLES_PERSLICE		32

// buffer size used when reading through the mp3
#define MP3_DATA_BUFSIZ			128*1024

// undefine this if you don't want any dithering.
#define MP3_DITHERING

// Q3 MP3 codec
snd_codec_t mp3_codec =
{
	"mp3",
	S_MP3_CodecLoad,
	S_MP3_CodecOpenStream,
	S_MP3_CodecReadStream,
	S_MP3_CodecCloseStream,
	NULL
};

// structure used for info purposes
struct snd_codec_mp3_info
{
	byte encbuf[MP3_DATA_BUFSIZ];	// left over bytes not consumed
					// by the decoder.
	struct mad_stream madstream;	// uses encbuf as buffer.
	struct mad_frame madframe;	// control structures for libmad.
	struct mad_synth madsynth;

	byte *pcmbuf;			// buffer for not-used samples.
	int buflen;			// length of buffer data.
	int pcmbufsize;			// amount of allocated memory for
					// pcmbuf. This should have at least
					// the size of a decoded mp3 frame.

	byte *dest;			// copy decoded data here.
	int destlen;			// amount of already copied data.
	int destsize;			// amount of bytes we must decode.
};

/*************** MP3 utility functions ***************/

/*
=================
S_MP3_ReadData
=================
*/

// feed libmad with data
int S_MP3_ReadData(snd_stream_t *stream, struct mad_stream *madstream, byte *encbuf, int encbufsize)
{
	int retval;
	int leftover;

	if(!stream)
		return -1;

	leftover = 0;
	if(madstream->bufend && madstream->next_frame &&
	   madstream->next_frame <= madstream->bufend)
	{
		leftover = madstream->bufend - madstream->next_frame;
		if(leftover > encbufsize)
			return -1;

		if(leftover > 0)
			memmove(encbuf, madstream->next_frame, leftover);
	}


	// Fill the buffer right to the end

	retval = FS_Read(&encbuf[leftover], encbufsize - leftover, stream->file);

	if(retval <= 0)
	{
		// EOF reached, that's ok.
		return 0;
	}

	mad_stream_buffer(madstream, encbuf, retval + leftover);

	return retval;
}

static qboolean S_MP3_ResyncAfterFalseFrame(struct mad_stream *madstream)
{
	if(!madstream || !madstream->this_frame || !madstream->bufend ||
	   madstream->this_frame >= madstream->bufend)
		return qfalse;

	// False MPEG syncs can occur inside ID3 tags and compressed frame data.
	// Resume one byte after the candidate instead of trusting its frame size.
	madstream->next_frame = madstream->this_frame + 1;
	madstream->skiplen = 0;
	madstream->sync = 1;
	return qtrue;
}


/*
=================
S_MP3_Scanfile

Scan the complete stream to determine its sample count and stable format.
I basically used the xmms-mad plugin source to see how this stuff works.

returns a value < 0 on error.
=================
*/

int S_MP3_Scanfile(snd_stream_t *stream)
{
	struct mad_stream madstream;
	struct mad_frame madframe;
	int retval;
	int samplecount;
	int scanError = 0;
	byte encbuf[MP3_DATA_BUFSIZ];

	// error out on invalid input.
	if(!stream)
		return -1;

	mad_stream_init(&madstream);
	mad_frame_init(&madframe);

	while(1)
	{
		retval = S_MP3_ReadData(stream, &madstream, encbuf, sizeof(encbuf));
		if(retval < 0)
		{
			scanError = -1;
			break;
		}
		else if(retval == 0)
			break;

		// Decode complete frames so sync-like bytes inside tags/audio cannot
		// masquerade as a valid MP3 header and poison the stream format.
		while(1)
		{
			if(mad_frame_decode(&madframe, &madstream) < 0)
			{
				if(madstream.error == MAD_ERROR_BUFLEN)
					break;

				if(MAD_RECOVERABLE(madstream.error))
				{
					mad_stream_skip(&madstream, madstream.skiplen);
					continue;
				}

				if(S_MP3_ResyncAfterFalseFrame(&madstream))
					continue;

				scanError = -1;
				break;
			}

			if(madframe.header.layer != MAD_LAYER_III)
			{
				if(S_MP3_ResyncAfterFalseFrame(&madstream))
					continue;

				scanError = -1;
				break;
			}

			if(!stream->info.samples)
			{
				// Capture the stable format from the first fully decoded frame.
				stream->info.rate = madframe.header.samplerate;
				stream->info.width = MP3_SAMPLE_WIDTH;
				stream->info.channels = MAD_NCHANNELS(&madframe.header);
				stream->info.samples = 0;
				stream->info.size = 0;
				stream->info.dataofs = 0;
			}
			else if(stream->info.rate != madframe.header.samplerate ||
			        stream->info.channels != MAD_NCHANNELS(&madframe.header))
			{
				// This is usually another false sync in arbitrary file bytes.
				if(S_MP3_ResyncAfterFalseFrame(&madstream))
					continue;

				scanError = -1;
				break;
			}

			// Count only frames libmad decoded successfully.
			samplecount = MAD_NSBSAMPLES(&madframe.header) * MP3_PCMSAMPLES_PERSLICE;
			stream->info.samples += samplecount;
			stream->info.size += samplecount * stream->info.channels * stream->info.width;
		}

		if(scanError < 0)
			break;
	}

	mad_frame_finish(&madframe);
	mad_stream_finish(&madstream);
	if(scanError < 0)
		return scanError;

	// Reset the file pointer so we can do the real decoding.
	FS_Seek(stream->file, 0, FS_SEEK_SET);

	return 0;
}

/*
=================
S_MP3_Scanfile

Scan the complete stream to determine its sample count and stable format.
I basically used the xmms-mad plugin source to see how this stuff works.

returns a value < 0 on error.
=================
*/

/************************ dithering functions ***************************/

#ifdef MP3_DITHERING

// All dithering done here is taken from the GPL'ed xmms-mad plugin.

/* Copyright (C) 1997 Makoto Matsumoto and Takuji Nishimura.       */
/* Any feedback is very welcome. For any question, comments,       */
/* see http://www.math.keio.ac.jp/matumoto/emt.html or email       */
/* matumoto@math.keio.ac.jp                                        */

/* Period parameters */
#define MP3_DITH_N 624
#define MP3_DITH_M 397
#define MATRIX_A 0x9908b0df   /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)

static unsigned long mt[MP3_DITH_N]; /* the array for the state vector  */
static int mti=MP3_DITH_N+1; /* mti==MP3_DITH_N+1 means mt[MP3_DITH_N] is not initialized */

/* initializing the array with a NONZERO seed */
void sgenrand(unsigned long seed)
{
    /* setting initial seeds to mt[MP3_DITH_N] using         */
    /* the generator Line 25 of Table 1 in          */
    /* [KNUTH 1981, The Art of Computer Programming */
    /*    Vol. 2 (2nd Ed.), pp102]                  */
    mt[0]= seed & 0xffffffff;
    for (mti=1; mti<MP3_DITH_N; mti++)
        mt[mti] = (69069 * mt[mti-1]) & 0xffffffff;
}

unsigned long genrand(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= MP3_DITH_N) { /* generate MP3_DITH_N words at one time */
        int kk;

        if (mti == MP3_DITH_N+1)   /* if sgenrand() has not been called, */
            sgenrand(4357); /* a default initial seed is used   */

        for (kk=0;kk<MP3_DITH_N-MP3_DITH_M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+MP3_DITH_M] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        for (;kk<MP3_DITH_N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(MP3_DITH_M-MP3_DITH_N)] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        y = (mt[MP3_DITH_N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[MP3_DITH_N-1] = mt[MP3_DITH_M-1] ^ (y >> 1) ^ mag01[y & 0x1];

        mti = 0;
    }

    y = mt[mti++];
    y ^= TEMPERING_SHIFT_U(y);
    y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
    y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
    y ^= TEMPERING_SHIFT_L(y);

    return y;
}

long triangular_dither_noise(int nbits) {
    // parameter nbits : the peak-to-peak amplitude desired (in bits)
    //  use with nbits set to    2 + nber of bits to be trimmed.
    // (because triangular is made from two uniformly distributed processes,
    // it starts at 2 bits peak-to-peak amplitude)
    // see The Theory of Dithered Quantization by Robert Alexander Wannamaker
    // for complete proof of why that's optimal

    long v = (genrand()/2 - genrand()/2); // in ]-2^31, 2^31[
    //int signe = (v>0) ? 1 : -1;
    long P = 1 << (32 - nbits); // the power of 2
    v /= P;
    // now v in ]-2^(nbits-1), 2^(nbits-1) [

    return v;
}

#endif // MP3_DITHERING

/************************ decoder functions ***************************/

/*
=================
S_MP3_Scale

Converts the signal to 16 bit LE-PCM data and does dithering.

- borrowed from xmms-mad plugin source.
=================
*/

/*
 * xmms-mad - mp3 plugin for xmms
 * Copyright (C) 2001-2002 Sam Clegg
 */

signed int S_MP3_Scale(mad_fixed_t sample)
{
	int n_bits_to_loose = MAD_F_FRACBITS + 1 - 16;
#ifdef MP3_DITHERING
	int dither;
#endif

	// round
	sample += (1L << (n_bits_to_loose - 1));

#ifdef MP3_DITHERING
	dither = triangular_dither_noise(n_bits_to_loose + 1);
	sample += dither;
#endif

	/* clip */
	if (sample >= MAD_F_ONE)
		sample = MAD_F_ONE - 1;
	else if (sample < -MAD_F_ONE)
		sample = -MAD_F_ONE;

	/* quantize */
	return sample >> n_bits_to_loose;
}



/*
=================
S_MP3_PCMCopy

Copy and convert pcm data until bytecount bytes have been written.
return the position in pcm->samples.
indicate the amount of actually written bytes in wrotecnt.
=================
*/

int S_MP3_PCMCopy(byte *buf, struct mad_pcm *pcm, int bufofs,
			 int sampleofs, int bytecount, int *wrotecnt)
{
	int written = 0;
	signed int sample;
	int framesize = pcm->channels * MP3_SAMPLE_WIDTH;

	// add new pcm data.
	while(written < bytecount && sampleofs < pcm->length)
	{
		sample = S_MP3_Scale(pcm->samples[0][sampleofs]);

#ifdef Q3_BIG_ENDIAN
		// output to 16 bit big endian PCM
		buf[bufofs++] = (sample >> 8) & 0xff;
		buf[bufofs++] = sample & 0xff;
#else
		// output to 16 bit little endian PCM
		buf[bufofs++] = sample & 0xff;
		buf[bufofs++] = (sample >> 8) & 0xff;
#endif

		if(pcm->channels == 2)
		{
			sample = S_MP3_Scale(pcm->samples[1][sampleofs]);

#ifdef Q3_BIG_ENDIAN
			buf[bufofs++] = (sample >> 8) & 0xff;
			buf[bufofs++] = sample & 0xff;
#else
			buf[bufofs++] = sample & 0xff;
			buf[bufofs++] = (sample >> 8) & 0xff;
#endif
		}

		sampleofs++;
		written += framesize;
	}

	if(wrotecnt)
		*wrotecnt = written;

	return sampleofs;
}


/*
=================
S_MP3_Decode
=================
*/

// gets executed for every decoded frame.
int S_MP3_Decode(snd_stream_t *stream)
{
	struct snd_codec_mp3_info *mp3info;
	struct mad_stream *madstream;
	struct mad_frame *madframe;
	struct mad_synth *madsynth;
	struct mad_pcm *pcm;
	int cursize;
	int samplecount;
	int needcount;
	int wrote;
	int retval;

	if(!stream)
		return -1;

	mp3info = stream->ptr;
	madstream = &mp3info->madstream;
	madframe = &mp3info->madframe;

	while(1)
	{
		if(mad_frame_decode(madframe, madstream) < 0)
		{
			if(madstream->error == MAD_ERROR_BUFLEN)
			{
				// We need more data. Read another chunk and retry.
				retval = S_MP3_ReadData(stream, madstream, mp3info->encbuf, sizeof(mp3info->encbuf));
				if(retval <= 0)
					return retval;
				continue;
			}

			if(MAD_RECOVERABLE(madstream->error))
			{
				mad_stream_skip(madstream, madstream->skiplen);
				continue;
			}

			if(S_MP3_ResyncAfterFalseFrame(madstream))
				continue;

			return -1;
		}

		// Ignore false MPEG headers found in ID3 tags or compressed data.
		if(madframe->header.layer != MAD_LAYER_III ||
		   madframe->header.samplerate != stream->info.rate ||
		   MAD_NCHANNELS(&madframe->header) != stream->info.channels)
		{
			if(S_MP3_ResyncAfterFalseFrame(madstream))
				continue;

			return -1;
		}

		break;
	}

	// generate pcm data
	madsynth = &mp3info->madsynth;
	mad_synth_frame(madsynth, madframe);

	pcm = &madsynth->pcm;

	// see whether we have got enough data now.
	cursize = pcm->length * pcm->channels * stream->info.width;
	needcount = mp3info->destsize - mp3info->destlen;

	// Copy exactly as many samples as required.
	samplecount = S_MP3_PCMCopy(mp3info->dest, pcm,
				    mp3info->destlen, 0, needcount, &wrote);
	mp3info->destlen += wrote;

	if(samplecount < pcm->length)
	{
		// Not all samples got copied. Copy the rest into the pcm buffer.
		samplecount = S_MP3_PCMCopy(mp3info->pcmbuf, pcm,
					    mp3info->buflen,
					    samplecount,
					    mp3info->pcmbufsize - mp3info->buflen,
					    &wrote);
		mp3info->buflen += wrote;


		if(samplecount < pcm->length)
		{
			// The pcm buffer was not large enough. Make it bigger.
			byte *newbuf = Z_Malloc(cursize);

			if(mp3info->pcmbuf)
			{
				memcpy(newbuf, mp3info->pcmbuf, mp3info->buflen);
				Z_Free(mp3info->pcmbuf);
			}

			mp3info->pcmbuf = newbuf;
			mp3info->pcmbufsize = cursize;

			samplecount = S_MP3_PCMCopy(mp3info->pcmbuf, pcm,
						    mp3info->buflen,
						    samplecount,
						    mp3info->pcmbufsize - mp3info->buflen,
						    &wrote);
			mp3info->buflen += wrote;
		}

		// we're definitely done.
		retval = 0;
	}
	else if(mp3info->destlen >= mp3info->destsize)
		retval = 0;
	else
		retval = 1;

	return retval;
}

/*************** Callback functions for quake3 ***************/

/*
=================
S_MP3_CodecOpenStream
=================
*/

snd_stream_t *S_MP3_CodecOpenStream(const char *filename)
{
	snd_stream_t *stream;
	struct snd_codec_mp3_info *mp3info;

	// Open the stream
	stream = S_CodecUtilOpen(filename, &mp3_codec);
	if(!stream || stream->length <= 0)
		return NULL;

	// We have to scan through the MP3 to determine the important mp3 info.
	if(S_MP3_Scanfile(stream) < 0)
	{
		// scanning didn't work out...
		S_CodecUtilClose(&stream);
		return NULL;
	}

	// Initialize the mp3 info structure we need for streaming
	mp3info = Z_Malloc(sizeof(*mp3info));
	if(!mp3info)
	{
		S_CodecUtilClose(&stream);
		return NULL;
	}

	stream->ptr = mp3info;

	// initialize the libmad control structures.
	mad_stream_init(&mp3info->madstream);
	mad_frame_init(&mp3info->madframe);
	mad_synth_init(&mp3info->madsynth);

	if(S_MP3_ReadData(stream, &mp3info->madstream, mp3info->encbuf, sizeof(mp3info->encbuf)) <= 0)
	{
		// we didnt read anything, that's bad.
		S_MP3_CodecCloseStream(stream);
		return NULL;
	}

	return stream;
}

/*
=================
S_MP3_CodecCloseStream
=================
*/

// free all memory we allocated.
void S_MP3_CodecCloseStream(snd_stream_t *stream)
{
	struct snd_codec_mp3_info *mp3info;

	if(!stream)
		return;

	// free all data in our mp3info tree

	if(stream->ptr)
	{
		mp3info = stream->ptr;

		if(mp3info->pcmbuf)
			Z_Free(mp3info->pcmbuf);

		mad_synth_finish(&mp3info->madsynth);
		mad_frame_finish(&mp3info->madframe);
		mad_stream_finish(&mp3info->madstream);

		Z_Free(stream->ptr);
	}

	S_CodecUtilClose(&stream);
}

/*
=================
S_MP3_CodecReadStream
=================
*/
int S_MP3_CodecReadStream(snd_stream_t *stream, int bytes, void *buffer)
{
	struct snd_codec_mp3_info *mp3info;
	int retval;

	if(!stream)
		return -1;

	mp3info = stream->ptr;

	// Make sure we get complete frames all the way through.
	bytes -= bytes % (stream->info.channels * stream->info.width);

	if(mp3info->buflen)
	{
		if(bytes < mp3info->buflen)
		{
			// we still have enough bytes in our decoded pcm buffer
			memcpy(buffer, mp3info->pcmbuf, bytes);

			// remove the portion from our buffer.
			mp3info->buflen -= bytes;
			memmove(mp3info->pcmbuf, &mp3info->pcmbuf[bytes], mp3info->buflen);
			return bytes;
		}
		else
		{
			// copy over the samples we already have.
			memcpy(buffer, mp3info->pcmbuf, mp3info->buflen);
			mp3info->destlen = mp3info->buflen;
			mp3info->buflen = 0;
		}
	}
	else
		mp3info->destlen = 0;

	mp3info->dest = buffer;
	mp3info->destsize = bytes;

	do
	{
		retval = S_MP3_Decode(stream);
	} while(retval > 0);

	// if there was an error return nothing.
	if(retval < 0)
		return 0;

	return mp3info->destlen;
}

/*
=====================================================================
S_MP3_CodecLoad

We handle S_MP3_CodecLoad as a special case of the streaming functions
where we read the whole stream at once.
======================================================================
*/
void *S_MP3_CodecLoad(const char *filename, snd_info_t *info)
{
	snd_stream_t *stream;
	byte *pcmbuffer;

	// check if input is valid
	if(!filename)
		return NULL;

	stream = S_MP3_CodecOpenStream(filename);

	if(!stream)
		return NULL;

        // copy over the info
        info->rate = stream->info.rate;
        info->width = stream->info.width;
        info->channels = stream->info.channels;
        info->samples = stream->info.samples;
        info->dataofs = stream->info.dataofs;

	// allocate enough buffer for all pcm data
	pcmbuffer = Hunk_AllocateTempMemory(stream->info.size);
	if(!pcmbuffer)
	{
		S_MP3_CodecCloseStream(stream);
		return NULL;
	}

	info->size = S_MP3_CodecReadStream(stream, stream->info.size, pcmbuffer);

	if(info->size <= 0)
	{
		// we didn't read anything at all. darn.
		Hunk_FreeTempMemory(pcmbuffer);
		pcmbuffer = NULL;
	}

	S_MP3_CodecCloseStream(stream);

	return pcmbuffer;
}

#define MP3_ID3_MAX_TAG_SIZE (1024 * 1024)

static unsigned int S_MP3_ID3ReadBE(const byte *data, int count)
{
	unsigned int value = 0;
	int i;

	for (i = 0; i < count; i++)
		value = (value << 8) | data[i];

	return value;
}

static unsigned int S_MP3_ID3ReadSyncsafe(const byte *data)
{
	return ((unsigned int)(data[0] & 0x7f) << 21) |
	       ((unsigned int)(data[1] & 0x7f) << 14) |
	       ((unsigned int)(data[2] & 0x7f) << 7) |
	       (unsigned int)(data[3] & 0x7f);
}

static void S_MP3_ID3AppendCodepoint(char *out, int outSize, int *outLength, unsigned int codepoint)
{
	byte encoded;

	if (!out || outSize <= 0 || !outLength || codepoint == 0)
		return;

	/* The game's bitmap font consumes one legacy byte per glyph, not UTF-8. */
	if (codepoint <= 0xff) {
		encoded = (byte)codepoint;
	} else {
		switch (codepoint) {
		case 0x20ac: encoded = 0x80; break; /* Euro */
		case 0x201a: encoded = 0x82; break;
		case 0x0192: encoded = 0x83; break;
		case 0x201e: encoded = 0x84; break;
		case 0x2026: encoded = 0x85; break;
		case 0x2020: encoded = 0x86; break;
		case 0x2021: encoded = 0x87; break;
		case 0x02c6: encoded = 0x88; break;
		case 0x2030: encoded = 0x89; break;
		case 0x0160: encoded = 0x8a; break;
		case 0x2039: encoded = 0x8b; break;
		case 0x0152: encoded = 0x8c; break;
		case 0x017d: encoded = 0x8e; break;
		case 0x2018: encoded = 0x91; break;
		case 0x2019: encoded = 0x92; break;
		case 0x201c: encoded = 0x93; break;
		case 0x201d: encoded = 0x94; break;
		case 0x2022: encoded = 0x95; break;
		case 0x2013: encoded = 0x96; break;
		case 0x2014: encoded = 0x97; break;
		case 0x02dc: encoded = 0x98; break;
		case 0x2122: encoded = 0x99; break;
		case 0x0161: encoded = 0x9a; break;
		case 0x203a: encoded = 0x9b; break;
		case 0x0153: encoded = 0x9c; break;
		case 0x017e: encoded = 0x9e; break;
		case 0x0178: encoded = 0x9f; break;
		default: encoded = '?'; break;
		}
	}

	if (*outLength + 1 >= outSize)
		return;

	out[*outLength] = (char)encoded;
	(*outLength)++;
	out[*outLength] = '\0';
}

static unsigned int S_MP3_ID3ReadUtf16(const byte *data, int littleEndian)
{
	if (littleEndian)
		return (unsigned int)data[0] | ((unsigned int)data[1] << 8);

	return ((unsigned int)data[0] << 8) | (unsigned int)data[1];
}

static qboolean S_MP3_ID3DecodeText(const byte *data, int dataLength, char *out, int outSize)
{
	int encoding;
	int outLength = 0;
	int i;

	if (!data || dataLength < 1 || !out || outSize < 2)
		return qfalse;

	out[0] = '\0';
	encoding = data[0];
	data++;
	dataLength--;

	if (encoding == 0) {
		for (i = 0; i < dataLength && data[i]; i++)
			S_MP3_ID3AppendCodepoint(out, outSize, &outLength, data[i]);
	} else if (encoding == 3) {
		for (i = 0; i < dataLength && data[i]; ) {
			unsigned int codepoint;
			int sequenceLength;
			byte first = data[i];

			if (first < 0x80) {
				codepoint = first;
				sequenceLength = 1;
			} else if ((first & 0xe0) == 0xc0 && i + 1 < dataLength && (data[i + 1] & 0xc0) == 0x80) {
				codepoint = ((unsigned int)(first & 0x1f) << 6) | (data[i + 1] & 0x3f);
				sequenceLength = 2;
			} else if ((first & 0xf0) == 0xe0 && i + 2 < dataLength &&
				   (data[i + 1] & 0xc0) == 0x80 && (data[i + 2] & 0xc0) == 0x80) {
				codepoint = ((unsigned int)(first & 0x0f) << 12) |
					    ((unsigned int)(data[i + 1] & 0x3f) << 6) | (data[i + 2] & 0x3f);
				sequenceLength = 3;
			} else if ((first & 0xf8) == 0xf0 && i + 3 < dataLength &&
				   (data[i + 1] & 0xc0) == 0x80 && (data[i + 2] & 0xc0) == 0x80 &&
				   (data[i + 3] & 0xc0) == 0x80) {
				codepoint = ((unsigned int)(first & 0x07) << 18) |
					    ((unsigned int)(data[i + 1] & 0x3f) << 12) |
					    ((unsigned int)(data[i + 2] & 0x3f) << 6) | (data[i + 3] & 0x3f);
				sequenceLength = 4;
			} else {
				codepoint = '?';
				sequenceLength = 1;
			}

			S_MP3_ID3AppendCodepoint(out, outSize, &outLength, codepoint);
			i += sequenceLength;
		}
	} else if (encoding == 1 || encoding == 2) {
		int littleEndian = (encoding == 1);

		if (encoding == 1 && dataLength >= 2) {
			if (data[0] == 0xff && data[1] == 0xfe) {
				littleEndian = qtrue;
				data += 2;
				dataLength -= 2;
			} else if (data[0] == 0xfe && data[1] == 0xff) {
				littleEndian = qfalse;
				data += 2;
				dataLength -= 2;
			}
		}

		for (i = 0; i + 1 < dataLength; i += 2) {
			unsigned int codepoint = S_MP3_ID3ReadUtf16(data + i, littleEndian);

			if (!codepoint)
				break;

			if (codepoint >= 0xd800 && codepoint <= 0xdbff && i + 3 < dataLength) {
				unsigned int low = S_MP3_ID3ReadUtf16(data + i + 2, littleEndian);
				if (low >= 0xdc00 && low <= 0xdfff) {
					codepoint = 0x10000 + ((codepoint - 0xd800) << 10) + (low - 0xdc00);
					i += 2;
				} else {
					codepoint = '?';
				}
			}

			S_MP3_ID3AppendCodepoint(out, outSize, &outLength, codepoint);
		}
	}

	return out[0] ? qtrue : qfalse;
}

static void S_MP3_ID3Trim(char *text)
{
	int length;
	int start = 0;

	if (!text)
		return;

	length = strlen(text);
	while (length > 0 && (text[length - 1] == ' ' || text[length - 1] == '\t' || text[length - 1] == '\r' || text[length - 1] == '\n'))
		text[--length] = '\0';
	while (text[start] == ' ' || text[start] == '\t' || text[start] == '\r' || text[start] == '\n')
		start++;
	if (start)
		memmove(text, text + start, strlen(text + start) + 1);
}

static void S_MP3_ID3ReadFrames(byte *data, int dataLength, int majorVersion, byte flags,
				char *title, char *artist, char *album)
{
	int offset = 0;
	int frameHeaderSize = (majorVersion == 2) ? 6 : 10;

	if (majorVersion >= 3 && (flags & 0x40)) {
		unsigned int extendedSize;
		if (dataLength < 4)
			return;
		extendedSize = (majorVersion == 4) ? S_MP3_ID3ReadSyncsafe(data) : S_MP3_ID3ReadBE(data, 4);
		if (majorVersion == 3)
			extendedSize += 4;
		if (extendedSize > (unsigned int)dataLength)
			return;
		offset = (int)extendedSize;
	}

	while (offset + frameHeaderSize <= dataLength) {
		const byte *frame = data + offset;
		const byte *frameData;
		unsigned int frameSize;
		char *destination = NULL;
		int destinationSize = 0;

		if (!frame[0])
			break;

		if (majorVersion == 2) {
			frameSize = S_MP3_ID3ReadBE(frame + 3, 3);
			if (!memcmp(frame, "TT2", 3) && !title[0]) {
				destination = title;
				destinationSize = 256;
			} else if (!memcmp(frame, "TP1", 3) && !artist[0]) {
				destination = artist;
				destinationSize = 256;
			} else if (!memcmp(frame, "TAL", 3) && !album[0]) {
				destination = album;
				destinationSize = 256;
			}
		} else {
			frameSize = (majorVersion == 4) ? S_MP3_ID3ReadSyncsafe(frame + 4) : S_MP3_ID3ReadBE(frame + 4, 4);
			if (!memcmp(frame, "TIT2", 4) && !title[0]) {
				destination = title;
				destinationSize = 256;
			} else if (!memcmp(frame, "TPE1", 4) && !artist[0]) {
				destination = artist;
				destinationSize = 256;
			} else if (!memcmp(frame, "TALB", 4) && !album[0]) {
				destination = album;
				destinationSize = 256;
			}

			if (frame[9])
				destination = NULL;
		}

		if (frameSize > (unsigned int)(dataLength - offset - frameHeaderSize))
			break;

		frameData = frame + frameHeaderSize;
		if (destination && S_MP3_ID3DecodeText(frameData, (int)frameSize, destination, destinationSize))
			S_MP3_ID3Trim(destination);

		offset += frameHeaderSize + (int)frameSize;
	}
}

static void S_MP3_ID3ReadV1Field(const byte *data, int dataLength, char *out, int outSize)
{
	int i;
	int outLength = 0;

	if (!data || !out || outSize < 2)
		return;

	out[0] = '\0';
	for (i = 0; i < dataLength && data[i]; i++)
		S_MP3_ID3AppendCodepoint(out, outSize, &outLength, data[i]);
	S_MP3_ID3Trim(out);
}

qboolean S_MP3_CodecGetMetadata(const char *filename,
				char *title, int titleSize,
				char *artist, int artistSize,
				char *album, int albumSize)
{
	fileHandle_t file = 0;
	int fileLength;
	byte header[10];
	char parsedTitle[256] = "";
	char parsedArtist[256] = "";
	char parsedAlbum[256] = "";
	qboolean found = qfalse;

	if (title && titleSize > 0)
		title[0] = '\0';
	if (artist && artistSize > 0)
		artist[0] = '\0';
	if (album && albumSize > 0)
		album[0] = '\0';
	if (!filename || !*filename)
		return qfalse;

	fileLength = FS_FOpenFileRead(filename, &file, qtrue);
	if (fileLength < 0 || !file)
		return qfalse;

	if (fileLength >= (int)sizeof(header) && FS_Read(header, sizeof(header), file) == sizeof(header) &&
	    !memcmp(header, "ID3", 3) && header[3] >= 2 && header[3] <= 4) {
		unsigned int tagSize = S_MP3_ID3ReadSyncsafe(header + 6);
		if (tagSize <= MP3_ID3_MAX_TAG_SIZE && tagSize <= (unsigned int)(fileLength - 10)) {
			byte *tagData = Z_Malloc((int)tagSize);
			if (tagData) {
				if (FS_Read(tagData, (int)tagSize, file) == (int)tagSize) {
					int i;
					int writeIndex = 0;
					if (header[5] & 0x80) {
						for (i = 0; i < (int)tagSize; i++) {
							tagData[writeIndex++] = tagData[i];
							if (tagData[i] == 0xff && i + 1 < (int)tagSize && tagData[i + 1] == 0)
								i++;
						}
						tagSize = writeIndex;
					}
					if (header[3] >= 3 || !(header[5] & 0x40))
						S_MP3_ID3ReadFrames(tagData, (int)tagSize, header[3], header[5],
								   parsedTitle, parsedArtist, parsedAlbum);
				}
				Z_Free(tagData);
			}
		}
	}

	if (fileLength >= 128) {
		byte v1[128];
		if (FS_Seek(file, fileLength - 128, FS_SEEK_SET) >= 0 && FS_Read(v1, sizeof(v1), file) == sizeof(v1) &&
		    !memcmp(v1, "TAG", 3)) {
			if (!parsedTitle[0])
				S_MP3_ID3ReadV1Field(v1 + 3, 30, parsedTitle, sizeof(parsedTitle));
			if (!parsedArtist[0])
				S_MP3_ID3ReadV1Field(v1 + 33, 30, parsedArtist, sizeof(parsedArtist));
			if (!parsedAlbum[0])
				S_MP3_ID3ReadV1Field(v1 + 63, 30, parsedAlbum, sizeof(parsedAlbum));
		}
	}

	FS_FCloseFile(file);

	S_MP3_ID3Trim(parsedTitle);
	S_MP3_ID3Trim(parsedArtist);
	S_MP3_ID3Trim(parsedAlbum);
	if (parsedTitle[0]) {
		if (title && titleSize > 0)
			Q_strncpyz(title, parsedTitle, titleSize);
		found = qtrue;
	}
	if (parsedArtist[0]) {
		if (artist && artistSize > 0)
			Q_strncpyz(artist, parsedArtist, artistSize);
		found = qtrue;
	}
	if (parsedAlbum[0]) {
		if (album && albumSize > 0)
			Q_strncpyz(album, parsedAlbum, albumSize);
		found = qtrue;
	}

	return found;
}

#endif // USE_CODEC_MP3
