#include <stdio.h>
#include <ctype.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <vorbis/vorbisenc.h>

#define READ 1024
signed char readbuffer[READ * 4 + 44];	/* out of the data
					 * segment, not the stack */

#define OUTALL(a, sz) do{ int i;\
	printf("%s(%li)\n", #a, sz); \
	for (i =0; i < sz; ++i) \
		printf("%c (%0x)\n", (a[i] >= ' ' && a[i] <= 0x7f ?(char)a[i]:' '), (int)a[i]); \
	} while (0)


void
seqslash (char *a, char *b)
{
	int i = strlen (a) - 1;
	while (i >= 0 && a[i] != '/')
		--i;
	a[i] = '\0';
	strcpy (b, a + i + 1);
}

void
seqhyph (char *a, char *b1, char *b2)
{
	int i = 0;
	for (; isdigit (a[i]); ++i)
		b1[i] = a[i];
	b1[i] = '\0';
	if (i == 0) return;
	for (; a[i] == ' ' || a[i] == '_' || a[i] == '.' || a[i] == '-'; ++i);
	strcpy (b2, a + i);
}

void
forsedir (char *file)
{
	int i = 1;
	for (; file[i]; ++i)
		if (file[i] == '/')
		{
			file[i] = '\0';
			mkdir (file, 0755);
			file[i] = '/';
		}
}

int
set_comments (char *fname, vorbis_comment * vc)
{
	static char date[5], album[128], artist[128], title[128], trNo[5],
		genre[64], buf[2048], buf1[1024];
	strcpy (buf, fname);
	buf[strlen (buf) - 4] = '\0';
	seqslash (buf, buf1);
	seqhyph (buf1, trNo, title);
	seqslash (buf, buf1);
	seqhyph (buf1, date, album);
	seqslash (buf, artist);
	seqslash (buf, genre);

	if (!(*date && *album && *artist && *title && *trNo && *genre))
	{
		fprintf (stderr, "File %s mismathes format!\n", fname);
		return -1;
	}
	if (trNo[0] == '0')
	{
		trNo[0] = trNo[1];
		trNo[1] = '\0';
	}

	//printf("tit: %s\ntr %s\nalb %s\ndate %s\natr %s\ngen %s\n",
	//title, trNo, album, date, artist, genre);

	vorbis_comment_add_tag (vc, "TITLE", title);
	vorbis_comment_add_tag (vc, "TRACKNUMBER", trNo);
	vorbis_comment_add_tag (vc, "ALBUM", album);
	vorbis_comment_add_tag (vc, "DATE", date);
	vorbis_comment_add_tag (vc, "ARTIST", artist);
	if (strcasecmp(genre, "nonstyled") != 0)
		vorbis_comment_add_tag (vc, "GENRE", genre);

	if (!trNo[1])
	{
		trNo[1] = trNo[0];
		trNo[0] = '0';
		trNo[2] = '\0';
	}
	sprintf (buf1, "/home/prof/entmt/eaudio/%s/%s/%s - %s/%s - %s.ogg",
		 genre, artist, date, album, trNo, title);
	forsedir (buf1);

	return open (buf1, O_CREAT | O_TRUNC | O_WRONLY, 0644);;
}

void
converse (char *fname)
{
	ogg_sync_state oy;	/* sync and verify incoming physical
				 * bitstream */
	ogg_stream_state outstr, instr;
	ogg_page og;
	vorbis_info vi;
	vorbis_comment vc;

	ogg_packet header, un;
	ogg_packet header_comm;
	ogg_packet header_code;

	int i, serialno, infile, bytes, outfile;
	char *buffer;
#define BUFSZ 4096

	//fprintf (stderr, "Processing: %s\n", fname);
	i = strlen (fname);
	if (i < 5 || memcmp (fname + i - 4, ".ogg", 5))
	{
		fprintf (stderr, "Not an ogg!\n");
		return;
	}

	infile = open (fname, O_RDONLY);

	vorbis_info_init (&vi);
	vorbis_comment_init (&vc);
	ogg_sync_init (&oy);

	buffer = ogg_sync_buffer (&oy, BUFSZ);
	bytes = read (infile, buffer, BUFSZ);

	ogg_sync_wrote (&oy, bytes);
	ogg_sync_pageout (&oy, &og);
	serialno = ogg_page_serialno (&og);
	ogg_stream_init (&instr, serialno);
	ogg_stream_init (&outstr, serialno);

	if (ogg_stream_pagein (&instr, &og) < 0)
	{
		fprintf (stderr,
			 "Error reading first page of Ogg bitstream data.\n");
		exit (1);
	}

	if (ogg_stream_packetout (&instr, &un) != 1)
	{
		fprintf (stderr, "Error reading initial header packet.\n");
		exit (1);
	}
	// Вот тут мы спасаем пакет
	header = un;
	header.packet = malloc (header.bytes);
	memcpy (header.packet, un.packet, header.bytes);
	vorbis_synthesis_headerin (&vi, &vc, &header);
	i = 0;
	while (i < 2)
	{
		while (i < 2)
		{
			int result = ogg_sync_pageout (&oy, &og);
			if (result == 0)
				break;
			if (result == 1)
			{
				ogg_stream_pagein (&instr, &og);
				while (i < 2)
				{
					ogg_packet *p =
						i ==
						0 ? &header_comm :
						&header_code;
					result = ogg_stream_packetout (&instr,
								       p);
					if (result == 0)
						break;
					if (result < 0)
					{
						/*
						 * Uh oh; data at some point was corrupted or
						 * missing! We can't tolerate that in a header.
						 * Die. 
						 */
						fprintf (stderr,
							 "Corrupt secondary header.  Exiting.\n");
						exit (1);
					}
					vorbis_synthesis_headerin (&vi, &vc,
								   p);
					i++;
				}
			}
		}
		/*
		 * no harm in not checking before adding more 
		 */
		buffer = ogg_sync_buffer (&oy, BUFSZ);
		bytes = read (infile, buffer, BUFSZ);
		if (bytes == 0 && i < 2)
		{
			fprintf (stderr,
				 "End of file %s before finding all Vorbis headers!\n",
				 fname);
			exit (1);
		}
		ogg_sync_wrote (&oy, bytes);
	}


	vorbis_comment_clear (&vc);
	outfile = set_comments (fname, &vc);
	if (outfile == -1)
		goto ready;
	vorbis_commentheader_out (&vc, &header_comm);
	ogg_stream_packetin (&outstr, &header);
	ogg_stream_packetin (&outstr, &header_comm);
	ogg_stream_packetin (&outstr, &header_code);
	while (ogg_stream_flush (&outstr, &og))
	{
		write (outfile, og.header, og.header_len);
		write (outfile, og.body, og.body_len);
	}
	i = 0;
	do
	{
		while (ogg_sync_pageout (&oy, &og) == 1)
		{
			write (outfile, og.header, og.header_len);
			write (outfile, og.body, og.body_len);
			if (ogg_page_eos (&og))
				goto ready;
		}
		buffer = ogg_sync_buffer (&oy, BUFSZ);
		bytes = read (infile, buffer, BUFSZ);
		ogg_sync_wrote (&oy, bytes);
		if (!bytes)
			break;
	}
	while (1);

      ready:
	ogg_stream_clear (&outstr);
	ogg_stream_clear (&instr);
	vorbis_comment_clear (&vc);
	vorbis_info_clear (&vi);

	close (outfile);
	close (infile);
}


static int
one (const struct dirent *unused)
{
	return 1;
}

void
processdir (char *dir)
{
	struct dirent **eps;
	struct stat fs;
	int n, i;
	char *buf = malloc (2048);

	n = scandir (dir, &eps, one, alphasort);
	if (n >= 0)
	{
		for (i = 0; i < n; ++i)
		{
			if (eps[i]->d_name[0] == '.')
				continue;
			sprintf (buf, "%s/%s", dir, eps[i]->d_name);
			stat (buf, &fs);
			printf ("%s\n", eps[i]->d_name);
			if (S_ISREG (fs.st_mode))
				converse (buf);
			else if (S_ISDIR (fs.st_mode))
				processdir (buf);			
		}
	}
	else
		fprintf (stderr, "Couldn't open the directory %s", dir);
	free (buf);
}

int
main ()
{
    static char buf[1024];
    processdir(getcwd(buf, sizeof buf));
    return 0;
}
