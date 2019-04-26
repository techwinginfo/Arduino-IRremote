#include "IRremote.h"
#include "IRremoteInt.h"

//==============================================================================
//                           YAMAZEN
//==============================================================================

#define YAMAZEN_BITS          32
#define YAMAZEN_HDR_MARK    3100
#define YAMAZEN_HDR_SPACE   1500
#define YAMAZEN_BIT_MARK     350
#define YAMAZEN_ONE_SPACE   1150
#define YAMAZEN_ZERO_SPACE   450
#define YAMAZEN_RPT_SPACE    750

//+=============================================================================
#if SEND_YAMAZEN
void  IRsend::sendYamazen (unsigned long data,  int nbits)
{
	// Set IR carrier frequency
	enableIROut(38);

	// Header
	mark(YAMAZEN_HDR_MARK);
	space(YAMAZEN_HDR_SPACE);

	// Data
	for (unsigned long  mask = 1UL << (nbits - 1);  mask;  mask >>= 1) 
	{
		if (data & mask) 
		{
			mark(YAMAZEN_BIT_MARK);
			space(YAMAZEN_ONE_SPACE);
		} 
		else 
		{
			mark(YAMAZEN_BIT_MARK);
			space(YAMAZEN_ZERO_SPACE);
		}
	}

	// Footer
	mark(YAMAZEN_BIT_MARK);
	space(0);  // Always end with the LED off
}
#endif

//+=============================================================================
// YAMAZENs have a repeat only 4 items long
//
#if DECODE_YAMAZEN
bool  IRrecv::decodeYamazen(decode_results *results)
{
	long  data = 0;  // We decode in to here; Start with nothing
	int   offset = 1;  // Index in to results; Skip first entry!?

	// Check header "mark"
	if (!MATCH_MARK(results->rawbuf[offset], YAMAZEN_HDR_MARK))
	{
		return false;
	}
	offset++;

	// Check for repeat
	if ((irparams.rawlen == 4)
		&& MATCH_SPACE(results->rawbuf[offset], YAMAZEN_RPT_SPACE)
		&& MATCH_MARK(results->rawbuf[offset + 1], YAMAZEN_BIT_MARK)
		) 
	{
		results->bits = 0;
		results->value = REPEAT;
		results->decode_type = YAMAZEN;
		return true;
	}

	// Check we have enough data
	if (irparams.rawlen < (2 * YAMAZEN_BITS) + 4)
	{
		return false;
	}

	// Check header "space"
	if (!MATCH_SPACE(results->rawbuf[offset], YAMAZEN_HDR_SPACE))
	{
		return false;
	}
	offset++;

	// Build the data
	for (int i = 0; i < YAMAZEN_BITS; i++) 
	{
		// Check data "mark"
		if (!MATCH_MARK(results->rawbuf[offset], YAMAZEN_BIT_MARK))
		{
			return false;
		}

		offset++;

		// Suppend this bit
		if (MATCH_SPACE(results->rawbuf[offset], YAMAZEN_ONE_SPACE))
		{
			data = (data << 1) | 1;
		}
		else if (MATCH_SPACE(results->rawbuf[offset], YAMAZEN_ZERO_SPACE))
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
	results->bits = YAMAZEN_BITS;
	results->value = data;
	results->decode_type = YAMAZEN;

	return true;
}
#endif
