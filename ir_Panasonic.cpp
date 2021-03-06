#include "IRremote.h"
#include "IRremoteInt.h"

//==============================================================================
//       PPPP    AAA   N   N   AAA    SSSS   OOO   N   N  IIIII   CCCC
//       P   P  A   A  NN  N  A   A  S      O   O  NN  N    I    C
//       PPPP   AAAAA  N N N  AAAAA   SSS   O   O  N N N    I    C
//       P      A   A  N  NN  A   A      S  O   O  N  NN    I    C
//       P      A   A  N   N  A   A  SSSS    OOO   N   N  IIIII   CCCC
//==============================================================================

#define PANASONIC_BITS          48
#define PANASONIC_BITS_AV       PANASONIC_BITS
#define PANASONIC_BITS_LIGHT    40	
#define PANASONIC_BITS_MIN		PANASONIC_BITS_LIGHT	
#define PANASONIC_BITS_MAX		PANASONIC_BITS_AV	
#define PANASONIC_HDR_MARK    3502
#define PANASONIC_HDR_SPACE   1750
//#define PANASONIC_BIT_MARK     502
#define PANASONIC_BIT_MARK     400
#define PANASONIC_ONE_SPACE   1244
//#define PANASONIC_ZERO_SPACE   400
#define PANASONIC_ZERO_SPACE   500

//+=============================================================================
#if SEND_PANASONIC
void IRsend::sendPanasonic (unsigned int address,  unsigned long data)
{
	sendPanasonicImpl(35, address, 16, data);
}

void IRsend::sendPanasonicLight(unsigned int address, unsigned long data)
{
	sendPanasonicImpl(38, address, 8, data);
}

void IRsend::sendPanasonicImpl(unsigned int khz, unsigned int address, unsigned int addrLen, unsigned long data)
{
	// Set IR carrier frequency
	enableIROut(khz);

	// Header
	mark(PANASONIC_HDR_MARK);
	space(PANASONIC_HDR_SPACE);

	// Address
	for (unsigned long mask = 1UL << (addrLen - 1); mask; mask >>= 1)
	{
		mark(PANASONIC_BIT_MARK);
		if (address & mask)
		{
			space(PANASONIC_ONE_SPACE);
		}
		else
		{
			space(PANASONIC_ZERO_SPACE);
		}
	}

	// Data
	for (unsigned long mask = 1UL << (32 - 1); mask; mask >>= 1)
	{
		mark(PANASONIC_BIT_MARK);
		if (data & mask)
		{
			space(PANASONIC_ONE_SPACE);
		}
		else
		{
			space(PANASONIC_ZERO_SPACE);
		}
	}

	// Footer
	mark(PANASONIC_BIT_MARK);
	space(0);  // Always end with the LED off
}

#endif

//+=============================================================================
#if DECODE_PANASONIC
bool  IRrecv::decodePanasonic(decode_results *results)
{
	unsigned long long  data = 0;
	int                 offset = 1;

	if (!MATCH_MARK(results->rawbuf[offset++], PANASONIC_HDR_MARK))
	{
		return false;
	}
	if (!MATCH_MARK(results->rawbuf[offset++], PANASONIC_HDR_SPACE))
	{
		return false;
	}

	// Check we have enough data
	if (results->rawlen < (2 * PANASONIC_BITS_MIN) + 4)
	{
		return false;
	}

	// decode address
	int i;
	for (i = 0; (i < PANASONIC_BITS_MAX) && ((offset + 1) < results->rawlen); i++)
	{
		if (!MATCH_MARK(results->rawbuf[offset++], PANASONIC_BIT_MARK))
		{
			return false;
		}

		if (MATCH_SPACE(results->rawbuf[offset], PANASONIC_ONE_SPACE))
		{
			data = (data << 1) | 1;
		}
		else if (MATCH_SPACE(results->rawbuf[offset], PANASONIC_ZERO_SPACE))
		{
			data = (data << 1) | 0;
		}
		else
		{
			return false;
		}

		offset++;
	}

	MATCH_MARK(results->rawbuf[offset++], PANASONIC_BIT_MARK);

	results->value = (unsigned long)data;
	results->address = (unsigned int)(data >> 32);
	results->decode_type = PANASONIC;
	results->bits = i;

	return true;
}
#endif

