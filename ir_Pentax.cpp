#include "IRremote.h"
#include "IRremoteInt.h"

//==============================================================================
//                           Pentax
//==============================================================================

#define PENTAX_BITS          6
#define PENTAX_HDR_MARK   12800
#define PENTAX_HDR_SPACE   2950
#define PENTAX_BIT_MARK    1000
#define PENTAX_ONE_SPACE   1000
#define PENTAX_ZERO_SPACE  1000
#define PENTAX_RPT_SPACE   1000

//+=============================================================================
#if SEND_PENTAX
void  IRsend::sendPentax(unsigned long data,  int nbits)
{
	// Set IR carrier frequency
	enableIROut(38);

	// Header
	mark(PENTAX_HDR_MARK);
	space(PENTAX_HDR_SPACE);

	// Data
	for (unsigned long  mask = 1UL << (nbits - 1);  mask;  mask >>= 1) 
	{
		if (data & mask) 
		{
			mark(PENTAX_BIT_MARK);
			space(PENTAX_ONE_SPACE);
		} 
		else 
		{
			mark(PENTAX_BIT_MARK);
			space(PENTAX_ZERO_SPACE);
		}
	}

	// Footer
	mark(PENTAX_BIT_MARK);
	space(0);  // Always end with the LED off
}
#endif

//+=============================================================================
// PENTAXs have a repeat only 4 items long
//
#if DECODE_PENTAX
bool  IRrecv::decodePentax(decode_results *results)
{
	long  data = 0;  // We decode in to here; Start with nothing
	int   offset = 1;  // Index in to results; Skip first entry!?

	// Check header "mark"
	if (!MATCH_MARK(results->rawbuf[offset], PENTAX_HDR_MARK))
	{
		return false;
	}
	offset++;

	// Check we have enough data
	if (results->rawlen < (2 * PENTAX_BITS) + 4)
	{
		return false;
	}

	// Check header "space"
	if (!MATCH_SPACE(results->rawbuf[offset], PENTAX_HDR_SPACE))
	{
		return false;
	}
	offset++;

	// Build the data
	for (int i = 0; i < PENTAX_BITS; i++) 
	{
		// Check data "mark"
		if (!MATCH_MARK(results->rawbuf[offset], PENTAX_BIT_MARK))
		{
			return false;
		}

		offset++;

		// Suppend this bit
		if (MATCH_SPACE(results->rawbuf[offset], PENTAX_ONE_SPACE))
		{
			data = (data << 1) | 1;
		}
		else if (MATCH_SPACE(results->rawbuf[offset], PENTAX_ZERO_SPACE))
		{
			data = (data << 1) | 0;
		}
		else
		{
			return false;
		}

		offset++;
	}

	// Success
	results->bits = PENTAX_BITS;
	results->value = data;
	results->decode_type = PENTAX;

	return true;
}
#endif
