#pragma once


template <class DECODER>
void DecodeDXTBlock(BYTE *dstData, const BYTE *srcBlock, long dstPitch, int bw, int bh)
{
	DECODER decoder;
	decoder.Setup(srcBlock);
	for (int y = 0; y < bh; y++)
	{
		BYTE *dst = dstData - y * dstPitch;
		decoder.SetY(y);
		for (int x = 0; x < bw; x++)
		{
			decoder.GetColor(x, y, (Color8888 &)*dst);
			dst += 4;
		}
	}
}