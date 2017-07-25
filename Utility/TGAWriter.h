#pragma once

#include "../SRay/Common.h"
#include "Utility.h"

namespace SRay
{
	namespace Utility
	{
		/*
		dépendances: structure "color" et fonction "Negative"
		*/
		class TGAWriter
		{
		public:
			TGAWriter(color *pimage, ushort pwidth, ushort pheight)
				:image(pimage), width(pwidth), height(pheight)
			{	}
			void print(char* text, ushort x, ushort y)
			{
				return print(text, x, y, MAX(uchar(round((float)height / (100.0f * (float)CHAR_HEIGHT))), 1));
			}
			void print(char* text, ushort x, ushort y, uchar pixelwidth)
			{
				color *curpix;
				char *cptr = text;
				char c = cptr[0];
				const char *e;
				int pos;
				uint final_char_width = pixelwidth*(CHAR_WIDTH + SPACE_WIDTH), char_offset;

				while (c)
				{
					char_offset = int(cptr - text)*final_char_width;
					if ((char_offset + CHAR_WIDTH) > width)
						break;
					curpix = image + (y*width + x) + char_offset;
					if (curpix >= (image + height*width))
						break;
					if (c >= 'A' && c <= 'Z')
						TOLOWER(c);
					e = strchr(alphabet, c);
					pos = (int)(e - alphabet);
					for (ushort i = 0; i < CHAR_HEIGHT; i++)
					{
						for (uchar k = 0; k < pixelwidth; k++)
						{
							for (ushort j = 0; j < CHAR_WIDTH; j++)
							{
								for (uchar l = 0; l < pixelwidth; l++)
								{
									// si e est NULL, pos est invalide => on saute la lettre
									if (e && refBuf[pos][i][j] == 'o')
										Negative(curpix);
									curpix++;
								}
							}
							curpix += (width - (CHAR_WIDTH*pixelwidth));
						}
					}
					c = *++cptr;
				}
			}
			void write(char *fname)
			{
				FILE *ofile = NULL;
				color tmpCol;
				int i = 0, j = 0;
				if ((ofile = fopen(fname, "w+b")) == NULL)
					return;
				putc(0, ofile);
				putc(0, ofile);
				putc(2, ofile);						 /* uncompressed COLOR */
				putc(0, ofile); putc(0, ofile);
				putc(0, ofile); putc(0, ofile);
				putc(0, ofile);
				putc(0, ofile); putc(0, ofile);		   /* X origin */
				putc(0, ofile); putc(0, ofile);		   /* y origin */
				putc((width & 0x00FF), ofile);
				putc((width & 0xFF00) >> 8, ofile);
				putc((height & 0x00FF), ofile);
				putc((height & 0xFF00) >> 8, ofile);
				putc(24, ofile);						/* 24 bit bitmap */
				putc(0, ofile);

				// retourner horizontalement
				for (i = height - 1; i >= 0; i--)
				for (j = 0; j < width; j++)
				{
					//fwrite(&image[i*width], width*sizeof(color), 1, ofile); // fwrite : b,g,r
					tmpCol.b = image[i*width + j].r;
					tmpCol.g = image[i*width + j].g;
					tmpCol.r = image[i*width + j].b;
					fwrite(&tmpCol, sizeof(color), 1, ofile); // fwrite : b,g,r
				}

				fclose(ofile);
			}

			const static uchar SPACE_WIDTH = 1;
			const static uchar CHAR_WIDTH = 5;
			const static uchar CHAR_HEIGHT = 5;
		protected:
			ushort width;
			ushort height;
			color *image;
		private:
			const static char alphabet[];// = "abcdefghijklmnopqrstuvwxyz0123456789!?:=,.-() #'*/";
			const static char refBuf[50][CHAR_HEIGHT][CHAR_WIDTH + 1];
		};

	}
}