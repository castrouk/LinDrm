
/******************************************************************************/

/*                                                                            */

/*  University of Kaiserslautern, Institute of Communications Engineering     */

/*  Copyright (C) 2004 Torsten Schorr                                         */

/*                                                                            */

/*  Author(s)    : Torsten Schorr (schorr@eit.uni-kl.de)                      */

/*  Project start: 15.07.2004                                                 */

/*  Last change  : 03.08.2004                                                 */

/*                                                                            */

/******************************************************************************/

/*                                                                            */

/*  This program is free software; you can redistribute it and/or modify      */

/*  it under the terms of the GNU General Public License as published by      */

/*  the Free Software Foundation; either version 2 of the License, or         */

/*  (at your option) any later version.                                       */

/*                                                                            */

/*  This program is distributed in the hope that it will be useful,           */

/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */

/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */

/*  GNU General Public License for more details.                              */

/*                                                                            */

/*  You should have received a copy of the GNU General Public License         */

/*  along with this program; if not, write to the Free Software               */

/*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

/*                                                                            */

/******************************************************************************/


/******************************************************************************/

/*                                                                            */

/*  deflate_uncompress.c                                                      */

/*                                                                            */

/******************************************************************************/

/*  Description:                                                              */

/*  Decompressor for data in gzip file format (RFC 1952)                      */

/*  compressed with the DEFLATE compressed data format                        */

/*  (RFC 1951). Used for Broadcast Web-Site application                       */

/*  ETSI TS 101 498                                                           */

/*                                                                            */

/*  Usage:                                                                    */

/*                                                                            */

/*  [data, origname, zerror] = deflate_uncompress(input);                     */

/*                                                                            */

/*  uncompresses uint8 input bytes into uint8 data with original filename     */

/*  origname. zerror: error during decompression                              */

/*                                                                            */

/******************************************************************************/


/******************************************************************************/

/*                                                                            */

/*  This program is based on the 'zlib' general purpose compression library   */

/*  version 1.1.3, July 9th, 1998 Jean-loup Gailly and Mark Adler             */

/*                                                                            */

/*                                                                            */

/*  The data format used by the zlib library is described by RFCs (Request for*/

/*  Comments) 1950 to 1952 in the files ftp://ds.internic.net/rfc/rfc1950.txt */

/*  (zlib format), rfc1951.txt (deflate format) and rfc1952.txt (gzip format).*/

/*                                                                            */

/******************************************************************************/


/* code adjusted for use outside MATLAB
*
*  by M.Bos - PA0MBO
*  
*  date Jan 18th 2008
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#define Nargs_rhs_str "1"
#define Nargs_rhs 1
#define PROGNAME "deflate_uncompress"
static int gz_magic[2] = { 0x1f, 0x8b };	/* gzip magic header */



/* gzip flag byte */
#define ASCII_FLAG   0x01	/* bit 0 set: file probably ascii text */
#define HEAD_CRC     0x02	/* bit 1 set: header CRC present */
#define EXTRA_FIELD  0x04	/* bit 2 set: extra field present */
#define ORIG_NAME    0x08	/* bit 3 set: original file name present */
#define COMMENT      0x10	/* bit 4 set: file comment present */
#define RESERVED     0xE0	/* bits 5..7: reserved */
int
deflate_uncompress(unsigned char *input, int l_in, char /*@out@ */ *origname,
           int , unsigned char /*@out@ */ *data, int *,
		   double /*@out@ */ *lhs2)
{
  unsigned char *compressed_data, *uncompressed_data;
  int N, zerror;
//  int dims[2];
  int expected_size, len, method, flags, startpos, i;

  uLong crc;
  z_stream stream, *streamp = &stream;

  /* in */
  N = l_in;
  compressed_data = input;
  if (N < 10)
    {
      printf("not enough input data\n");
      exit(EXIT_FAILURE);
    }
  streamp->zalloc = (alloc_func) NULL;
  streamp->zfree = (free_func) NULL;
  streamp->opaque = (voidpf) 0;
  streamp->next_out = Z_NULL;
  streamp->avail_in = 0;
  streamp->next_in = compressed_data;
  zerror = inflateInit2(streamp, -MAX_WBITS);

  /* windowBits is passed < 0 to tell that there is no zlib header.
   * Note that in this case inflate *requires* an extra "dummy" byte
   * after the compressed stream in order to complete decompression and
   * return Z_STREAM_END. Here the gzip CRC32 ensures that 4 bytes are
   * present after the compressed stream.
   */
  if (zerror != Z_OK)
    {
      inflateEnd(streamp);
      printf("Error using inflateInit2\n");
      exit(1);
    }

  /* Check the gzip magic header */
  for (len = 0; len < 2; len++)
    {
      if (compressed_data[len] != gz_magic[len])
	{
	  printf("No gzip format\n");
	  exit(1);
	}
    }
  method = compressed_data[2];
  flags = compressed_data[3];
  if (method != Z_DEFLATED || (flags & RESERVED) != 0)
    {
      inflateEnd(streamp);
      printf("No deflate format or reserved flags!\n");
      exit(1);
    }

  /* Discard time, xflags and OS code: */
  startpos = 10;
  if ((flags & EXTRA_FIELD) != 0)
    {				/* skip the extra field */
      len = compressed_data[startpos] + (compressed_data[startpos + 1] << 8);
      startpos += len + 2;
    }
  if ((flags & ORIG_NAME) != 0)

    {				/* skip the original file name */
      i = 0;

      {
	origname[i] = compressed_data[startpos];
      }
      while ((compressed_data[startpos++] != 0) && (startpos < N));
    }
  else

    {
      origname[0] = '\0';
    }
  if ((flags & COMMENT) != 0)
    {				/* skip the .gz file comment */
      while ((compressed_data[startpos++] != 0) && (startpos < N));
    }
  if ((flags & HEAD_CRC) != 0)
    {				/* skip the header crc */
      startpos += 2;
    }
  if (startpos > N - 8)
    {
      inflateEnd(streamp);
      printf("not enough input data\n");
      exit(1);
    }
  expected_size =
    ((int) compressed_data[N - 1] << 24) +
    ((int) compressed_data[N - 2] << 16) +
    ((int) compressed_data[N - 3] << 8) + ((int) compressed_data[N - 4]);
  if (expected_size > (0x01 << 30))
    {
      inflateEnd(streamp);
      printf
	("error in expected file size or file too large (support for 1GB files)\n");
      exit(1);
    }
  uncompressed_data =
    (unsigned char *) malloc(expected_size * sizeof(unsigned char));
  if (uncompressed_data == NULL)
    {
      inflateEnd(streamp);
      printf("error allocating memory\n");
      exit(1);
    }

  /* prepare z_stream data structure */
  streamp->avail_in = N - startpos;
  streamp->next_in = compressed_data + startpos;
  streamp->avail_out = expected_size;
  streamp->next_out = uncompressed_data;

  /* uncompress data */
  zerror = inflate(streamp, Z_FINISH);
  *lhs2 = 0.0;
  if (zerror == Z_STREAM_END)
    {

      /* check crc over uncompressed data: */
      crc = crc32(0L, Z_NULL, 0);
      crc =
	crc32(crc, uncompressed_data,
	      (uInt) (streamp->next_out - uncompressed_data));
      if ((((int) compressed_data[N - 5] << 24) +
	   ((int) compressed_data[N - 6] << 16) +
	   ((int) compressed_data[N - 7] << 8) +
     ((uLong) compressed_data[N - 8]) != crc) | (expected_size != streamp->next_out - uncompressed_data))
	{
	  *lhs2 = -1.0;
	}
//      dims[0] = 1;
//      dims[1] = streamp->next_out - uncompressed_data;
      memcpy(data, uncompressed_data,
	     (streamp->next_out - uncompressed_data) * sizeof(unsigned char));
    }
  else
    {
      *data = (double) zerror;
    } inflateEnd(streamp);
  free(uncompressed_data);
  return zerror;
}
