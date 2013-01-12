#include "jpg.h"

// See: http://www.w3.org/Graphics/JPEG/itu-t81.pdf
//      http://stackoverflow.com/questions/1557071/the-size-of-a-jpegjfif-image
//      http://en.wikipedia.org/wiki/JPEG#JPEG_files

struct jpg_segment {
	uint16_t marker;
	uint16_t length;
	uint8_t  data[];
};

int jpg_isfile(const uint8_t *data, size_t input_len, size_t *lengthptr)
{
	if (input_len < JPG_MIN_SIZE || JPG_2CC(data) != JPG_SOI)
		return 0;

	size_t length = 2;
	int have_scan = 0;

	for (;;)
	{
		const uint8_t *ptr = data + length;
		const struct jpg_segment *segment = (const struct jpg_segment *)ptr;

		if (length > input_len - 2)
			return 0;

		uint16_t marker = segment->marker;

		if (marker == JPG_EOI)
		{
			length += 2;
			break;
		}
		else if (IS_JPG_STANDALONE(marker))
		{
			length += 2;
		}
		else if (length > input_len - 4)
		{
			if (have_scan)
			{
				// file truncated, but because we have the scan we accept it
				length += 2;
				break;
			}
			return 0;
		}
		else if (IS_JPG_MARKER(ptr))
		{
			// scan variable size segment
			size_t seglen = be16toh(segment->length) + 2;

			if (seglen < 4 || SIZE_MAX - seglen < input_len)
				return 0;

			length += seglen;

			if (marker == JPG_SOS)
			{
				have_scan = 1;
				// the scan has no other way to determine the size upfront
				// but it does not contain any markers (0xFF01 ... 0xFFFE)
				// so this scans for the next marker (or EOF):
				while (length < input_len - 2)
				{
					ptr = data + length;

					if (IS_JPG_RSTN(ptr))
					{
						length += 2;
					}
					else if (IS_JPG_MARKER(ptr))
					{
						break;
					}
					else {
						++ length;
					}
				}
			}
		}
		else
		{
			return 0;
		}
	}
	
	if (lengthptr)
		*lengthptr = length;
	
	return 1;
}
