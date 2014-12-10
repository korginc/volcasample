/************************************************************************
	SYRO for volca sample
      Example 
 ***********************************************************************/
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../syro/korg_syro_volcasample.h"
#include "../syro/korg_syro_comp.h"

#define	MAX_SYRO_DATA	10

static const uint8_t wav_header[] = {
	'R' , 'I' , 'F',  'F',		// 'RIFF'
	0x00, 0x00, 0x00, 0x00,		// Size (data size + 0x24)
	'W',  'A',  'V',  'E',		// 'WAVE'
	'f',  'm',  't',  ' ',		// 'fmt '
	0x10, 0x00, 0x00, 0x00,		// Fmt chunk size
	0x01, 0x00,					// encode(wav)
	0x02, 0x00,					// channel = 2
	0x44, 0xAC, 0x00, 0x00,		// Fs (44.1kHz)
	0x10, 0xB1, 0x02, 0x00,		// Bytes per sec (Fs * 4)
	0x04, 0x00,					// Block Align (2ch,16Bit -> 4)
	0x10, 0x00,					// 16Bit
	'd',  'a',  't',  'a',		// 'data'
	0x00, 0x00, 0x00, 0x00		// data size(bytes)
};

#define WAVFMT_POS_ENCODE	0x00
#define WAVFMT_POS_CHANNEL	0x02
#define WAVFMT_POS_FS		0x04
#define WAVFMT_POS_BIT		0x0E

#define WAV_POS_RIFF_SIZE	0x04
#define WAV_POS_WAVEFMT		0x08
#define WAV_POS_DATA_SIZE	0x28


/*----------------------------------------------------------------------------
	Write 32Bit Value
 ----------------------------------------------------------------------------*/
static void set_32Bit_value(uint8_t *ptr, uint32_t dat)
{
	int i;
	
	for (i=0; i<4; i++) {
		*ptr++ = (uint8_t)dat;
		dat >>= 8;
	}
}

/*----------------------------------------------------------------------------
	Read 32Bit Value
 ----------------------------------------------------------------------------*/
static uint32_t get_32Bit_value(uint8_t *ptr)
{
	int i;
	uint32_t dat;
	
	dat = 0;
	
	for (i=0; i<4; i++) {
		dat <<= 8;
		dat |= (uint32_t)ptr[3-i];
	}
	return dat;
}

/*----------------------------------------------------------------------------
	Read 16Bit Value
 ----------------------------------------------------------------------------*/
static uint16_t get_16Bit_value(uint8_t *ptr)
{
	uint16_t dat;
	
	dat = (uint16_t)ptr[1];
	dat <<= 8;
	dat |= (uint16_t)ptr[0];
	
	return dat;
}

/*----------------------------------------------------------------------------
	Read File
 ----------------------------------------------------------------------------*/
static uint8_t *read_file(char *filename, uint32_t *psize)
{
	FILE *fp;
	uint8_t *buf;
	uint32_t size;
	
	fp = fopen((const char *)filename, "rb");
	if (!fp) {
		printf (" File open error, %s \n", filename);
		return NULL;
	}
	
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	buf = malloc(size);
	if (!buf) {
		printf (" Not enough memory for read file.\n");
		fclose(fp);
		return NULL;
	}
	
	if (fread(buf, 1, size, fp) < size) {
		printf (" File read error, %s \n", filename);
		fclose(fp);
		free(buf);
		return NULL;
	}
	
	fclose(fp);
	
	*psize = size;
	return buf;
}

/*----------------------------------------------------------------------------
	Write File
 ----------------------------------------------------------------------------*/
static bool write_file(char *filename, uint8_t *buf, uint32_t size)
{
	FILE *fp;
	
	fp = fopen(filename, "wb");
	if (!fp) {
		printf (" File open error, %s \n", filename);
		return false;
	}
	
	if (fwrite(buf, 1, size, fp) < size) {
		printf (" File write error(perhaps disk space is not enough), %s \n", filename);
		fclose(fp);
		return false;
	}
		
	fclose(fp);
	
	return true;
}

/*****************************************************************************
	functions for analyze command line, load file
 *****************************************************************************/
/*----------------------------------------------------------------------------
	Get Number
 ----------------------------------------------------------------------------*/
static int get_commandline_number(char *str, int *ppos)
{
	int num, pos;
	int8_t dat;
	
	num = -1;
	pos = *ppos;

	for (;;) {
		dat = str[pos];
		if ((dat >= '0') && (dat <= '9')) {
			dat -= '0';
			if (num == -1) {
				num = dat;
			} else {
				num = num*10 + dat;
			}
		} else {
			break;
		}
		pos++;
		if (num >= 1000) {
			break;
		}
	}
	
	*ppos = pos;

	return num;
}

/*----------------------------------------------------------------------------
	setup & load file (sample)
 ----------------------------------------------------------------------------*/
static bool setup_file_sample(char *filename, SyroData *syro_data)
{
	uint8_t *src;
	uint32_t wav_pos, size, chunk_size;
	uint32_t wav_fs;
	uint16_t num_of_ch, sample_byte;
	uint32_t num_of_frame;
	
	src = read_file(filename, &size);
	if (!src) {
		return false;
	}
	
	printf ("file = %s, ", filename);
	
	if (size <= sizeof(wav_header)) {
		printf ("wav file error, too small.\n");
		free(src);
		return false;
	}
	
	//------- check header/fmt -------*/
	if (memcmp(src, wav_header, 4)) {
		printf ("wav file error, 'RIFF' is not found.\n");
		free(src);
		return false;
	}

	if (memcmp((src + WAV_POS_WAVEFMT), (wav_header + WAV_POS_WAVEFMT), 8)) {
		printf ("wav file error, 'WAVE' or 'fmt ' is not found.\n");
		free(src);
		return false;
	}
	
	wav_pos = WAV_POS_WAVEFMT + 4;		// 'fmt ' pos
	
	if (get_16Bit_value(src+wav_pos+8+WAVFMT_POS_ENCODE) != 1) {
		printf ("wav file error, encode must be '1'.\n");
		free(src);
		return false;
	}

	num_of_ch = get_16Bit_value(src+wav_pos+8+WAVFMT_POS_CHANNEL);
	if ((num_of_ch != 1) && (num_of_ch != 2)) {
		printf ("wav file error, channel must be 1 or 2.\n");
		free(src);
		return false;
	}
	
	{
		uint16_t num_of_bit;
		
		num_of_bit = get_16Bit_value(src+wav_pos+8+WAVFMT_POS_BIT);
		if ((num_of_bit != 16) && (num_of_bit != 24)) {
			printf ("wav file error, bit must be 16 or 24.\n");
			free(src);
			return false;
		}
		
		sample_byte = (num_of_bit / 8);
	}
	wav_fs = get_32Bit_value(src+wav_pos+8+WAVFMT_POS_FS);
	
	//------- search 'data' -------*/
	for (;;) {
		chunk_size = get_32Bit_value(src+wav_pos+4);
		if (!memcmp((src+wav_pos), "data", 4)) {
			break;
		}
		wav_pos += chunk_size + 8;
		if ((wav_pos+8) > size) {
			printf ("wav file error, 'data' chunk not found.\n");
			free(src);
			return false;
		}
	}
	
	if ((wav_pos+chunk_size+8) > size) {
		printf ("wav file error, illegal 'data' chunk size.\n");
		free(src);
		return false;
	}
	
	//------- setup  -------*/
	num_of_frame = chunk_size  / (num_of_ch * sample_byte);
	chunk_size = (num_of_frame * 2);
	syro_data->pData = malloc(chunk_size);
	if (!syro_data->pData) {
		printf ("not enough memory to setup file. \n");
		free(src);
		return false;
	}
	
	//------- convert to 1ch, 16Bit  -------*/
	{
		uint8_t *poss;
		int16_t *posd;
		int32_t dat, datf;
		uint16_t ch, sbyte;
		
		poss = (src + wav_pos + 8);
		posd = (int16_t *)syro_data->pData;
		
		for (;;) {
			datf = 0;
			for (ch=0; ch<num_of_ch; ch++) {
				dat = ((int8_t *)poss)[sample_byte - 1];
				for (sbyte=1; sbyte<sample_byte; sbyte++) {
					dat <<= 8;
					dat |= poss[sample_byte-1-sbyte];
				}
				poss += sample_byte;
				datf += dat;
			}
			datf /= num_of_ch;
			*posd++ = (int16_t)datf;
			if (!(--num_of_frame)) {
				break;
			}
		}
	}
	
	syro_data->Size = chunk_size;
	syro_data->Fs = wav_fs;
	syro_data->SampleEndian = LittleEndian;
	
	free(src);
	
	printf ("ok.\n");
	
	return true;
}

/*----------------------------------------------------------------------------
	setup & load file (all)
 ----------------------------------------------------------------------------*/
static bool setup_file_all(char *filename, SyroData *syro_data)
{
	uint32_t size;
	
	syro_data->pData = read_file(filename, &size);
	if (!syro_data->pData) {
		return false;
	}
	
	syro_data->Size = size;
	
	printf ("ok.\n");
	
	return true;
}


/*----------------------------------------------------------------------------
	setup & load file (pattern)
 ----------------------------------------------------------------------------*/
static bool setup_file_pattern(char *filename, SyroData *syro_data)
{
	uint32_t size;
	
	syro_data->pData = read_file(filename, &size);
	if (!syro_data->pData) {
		return false;
	}
	if (size != VOLCASAMPLE_PATTERN_SIZE) {
		printf (" size error\n");
		free(syro_data->pData);
		syro_data->pData = NULL;
		return false;
	}
	
	syro_data->Size = size;
	
	printf ("ok.\n");
	
	return true;	
}

/*----------------------------------------------------------------------------
	analyze command line
	notes : s17c15:sample.wav
            TT~TT~T~~~~T~~~~~
            || || |    +------- sourec file name (unnecessary when 'erase sample' is specified)
            || || +------------ partition mark (:)
            || |+-------------- compression bit (8-16), default=16
            || +--------------- 
            |+----------------- number of sample(0-99) or pattern(1-10)
            +------------------ type of source file
                                s:sample, e:erase sample, p:pattern
 ----------------------------------------------------------------------------*/
static int analayze_commandline(int num_of_argv, char **argv, SyroData *syro_data)
{
	int num_of_data;
	int num, i, pos;
	int num_lower, num_upper;
	int checkcomp, compbit;
	SyroDataType data_type;
	char dat;
	
	num_of_data = 0;
	
	for (i=0; i<num_of_argv; i++) {
		pos = 0;
		num = -1;
		checkcomp = 0;
		compbit = 0;
		num_lower = 0;
		num_upper = 0;
		
		printf (" Input data %d : ", (i+1));
		
		dat = argv[i][pos++];
		switch (dat) {
			case 'S':
			case 's':
				/*----- S : Sample -----*/
				data_type = DataType_Sample_Liner;
				num_lower = 0;
				num_upper = (VOLCASAMPLE_NUM_OF_SAMPLE - 1);
				checkcomp = 1;
				break;
				
			case 'E':
			case 'e':
				/*----- E : Erase sample -----*/
				data_type = DataType_Sample_Erase;
				num_lower = 0;
				num_upper = (VOLCASAMPLE_NUM_OF_SAMPLE - 1);
				checkcomp = 0;
				break;
			
			case 'A':
			case 'a':
				/*----- A : All sample -----*/
				data_type = DataType_Sample_All;
				num = 0;
				checkcomp = 1;
				break;

			case 'P':
			case 'p':
				/*----- P : Pattern -----*/
				data_type = DataType_Pattern;
				num_lower = 1;
				num_upper = VOLCASAMPLE_NUM_OF_PATTERN;
				break;
			
			default:
				printf ("Unknown data type '%c' \n", dat);
				continue;
		}
		
		/*----- Get Number ----*/
		if (num==-1) {
			num = get_commandline_number(argv[i], &pos);
			if (num == -1) {
				printf (" number is not found.\n");
				continue;
			}
			if ((num < num_lower) || (num > num_upper)) {
				printf (" number is out of range(legal : %d~%d). \n", 
					num_lower, num_upper);
				continue;
			}
		}
		
		/*----- Compress ? ----*/
		if (checkcomp) {
			dat = argv[i][pos];
			if ((dat == 'c') || (dat == 'C')) {
				pos++;
				compbit = 16;
				if (argv[i][pos] != ':') {
					compbit = get_commandline_number(argv[i], &pos);
					if (compbit == -1) {
						printf (" number of compression bit is not found.\n");
						continue;
					}
					if ((compbit < 8) || (compbit > 16)) {
						printf (" number of compression bit is out of range(legal : 8~16).\n");
						continue;
					}
				}
			}
		}
		
		/*----- Cehck partition mark ----*/
		dat = argv[i][pos++];
		if (dat != ':') {
			printf (" syntax error (':' is not found).\n");
			continue;
		}
		
		/*----- Load & setup file -------*/
		switch (data_type) {
			case DataType_Sample_Liner:
				if (compbit) {
					data_type = DataType_Sample_Compress;
					printf (" Sample(Compress bit=%d), number = %d, ", compbit, num);
				} else {
					printf (" Sample, number = %d, ", num);
				}
				if (!setup_file_sample(&argv[i][pos], syro_data)) {
					continue;
				}
				break;
			
			case DataType_Sample_All:
				if (compbit) {
					data_type = DataType_Sample_AllCompress;
					printf (" Sample All(Compress bit=%d), ", compbit);
				} else {
					printf (" Sample All, ");
				}
				if (!setup_file_all(&argv[i][pos], syro_data)) {
					continue;
				}
				break;
			
			case DataType_Sample_Erase:
				printf (" Sample erase, number = %d \n", num);
				syro_data->pData = NULL;
				syro_data->Size = 0;
				break;
			
			case DataType_Pattern:
				printf (" Pattern, number = %d, ", num);
				num--;
				if (!setup_file_pattern(&argv[i][pos], syro_data)) {
					continue;
				}
				break;
			
			default:
				continue;
		}
		syro_data->DataType = data_type;
		syro_data->Number = num;
		if (compbit) {
			syro_data->Quality = compbit;
		}
		
		syro_data++;
		num_of_data++;
		if (num_of_data == MAX_SYRO_DATA) {
			break;
		}
	}
	
	return num_of_data;
}

/*----------------------------------------------------------------------------
	free data memory
 ----------------------------------------------------------------------------*/
static void free_syrodata(SyroData *syro_data, int num_of_data)
{
	int i;
	
	for (i=0; i<num_of_data; i++) {
		if (syro_data->pData) {
			free(syro_data->pData);
			syro_data->pData = NULL;
		}
		syro_data++;
	}
}


/*****************************************************************************
	Main
 *****************************************************************************/
/*----------------------------------------------------------------------------
	Main (Execution)
 ----------------------------------------------------------------------------*/
static int main2(int argc, char **argv)
{
	SyroData syro_data[MAX_SYRO_DATA];
	SyroStatus status;
	SyroHandle handle;
	int num_of_data;
	uint8_t *buf_dest;
	uint32_t size_dest;
	uint32_t frame, write_pos;
	int16_t left, right;
	
	if (argc <= 2) {
		printf (" Syntax : >%s generate_file_name.wav src_file1 src_file2 .... \n", argv[0]);
		return 1;
	}
	
	num_of_data = analayze_commandline((argc-2), &argv[2], syro_data);
	if (!num_of_data) {
		printf (" Effective file is not found. \n");
		return 1;
	}
	
	//----- Start ------
	status = SyroVolcaSample_Start(&handle, syro_data, num_of_data, 0, &frame);
	if (status != Status_Success) {
		printf (" Start error, %d \n", status);
		free_syrodata(syro_data, num_of_data);
		return 1;
	}
	
	size_dest = (frame * 4) + sizeof(wav_header);
	
	buf_dest = malloc(size_dest);
	if (!buf_dest) {
		printf (" Not enough memory for write file.\n");
		SyroVolcaSample_End(handle);
		free_syrodata(syro_data, num_of_data);
		return 1;
	}
	
	memcpy(buf_dest, wav_header, sizeof(wav_header));
	set_32Bit_value((buf_dest + WAV_POS_RIFF_SIZE), ((frame * 4) + 0x24));
	set_32Bit_value((buf_dest + WAV_POS_DATA_SIZE), (frame * 4));
	
	//----- convert loop ------
	write_pos = sizeof(wav_header);
	while (frame) {
		SyroVolcaSample_GetSample(handle, &left, &right);
		buf_dest[write_pos++] = (uint8_t)left;
		buf_dest[write_pos++] = (uint8_t)(left >> 8);
		buf_dest[write_pos++] = (uint8_t)right;
		buf_dest[write_pos++] = (uint8_t)(right >> 8);
		frame--;
	}
	SyroVolcaSample_End(handle);
	free_syrodata(syro_data, num_of_data);
	
	//----- write ------
	
	if (write_file(argv[1], buf_dest, size_dest)) {
		printf (" Complete to convert.\n");
	}
	free(buf_dest);
	
	return 0;
}

/*----------------------------------------------------------------------------
	Main (Wrapper)
 ----------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
	int ret;
	
	ret = main2(argc, argv);

	return ret;
}

