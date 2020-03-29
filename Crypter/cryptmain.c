#include <Windows.h>
#include <stdio.h>
#include <ImageHlp.h> /* need to link imagehlp.lib (-limagehlp for gcc)*/
#include "cryptmain.h"

static int is_valid_key(BYTE* key);
static int encrypt_section(PSECTION_INFO section, BYTE* key, char* file_name);

int main(int argc, char* argv[])
{
	SECTION_INFO			stub_section;	/* type defined in header*/
	SECTION_INFO			code_section; 

	PIMAGE_NT_HEADERS		pe_header;		/* winapi types (read docs) */
	PIMAGE_SECTION_HEADER	section;
	PIMAGE_OPTIONAL_HEADER	opt_header;
	PLOADED_IMAGE			image;
	DWORD					image_base_addr;
	DWORD					nsections;		
	 
	if (argc > 3)
	{
		printf("[ERROR] too many arguments\n");
		printf(" - usage: crypt <filename> <key>");
		return 1;
	}

	if (argv[2] == NULL || !is_valid_key((BYTE *)argv[2]))
	{
		printf("[ERROR] the key size isn't compatible with the crypter which expects a key size of %d bits\n", KEY_SIZE);
		printf(" - make sure that the defined key size constant matches (in main.h)");
		return 1;
	}

	BYTE *key = (BYTE *) argv[2];

	image= ImageLoad(argv[1], NULL);
	pe_header= image->FileHeader;
	opt_header= &(pe_header->OptionalHeader);
	image_base_addr= opt_header->ImageBase;

	if (pe_header->Signature != IMAGE_NT_SIGNATURE) /* verify that the input file is a PE file*/
	{
		printf("ERROR: PE signature not matched");
		return 1;
	}

	stub_section.praw_data = (DWORD) NULL;
	stub_section.raw_size = (DWORD) NULL;
	stub_section.virtual_address = (DWORD) NULL;
	code_section.praw_data = (DWORD) NULL;
	code_section.raw_size = (DWORD) NULL;
	code_section.virtual_address = (DWORD) NULL;

	nsections = (*pe_header).FileHeader.NumberOfSections;
	section = IMAGE_FIRST_SECTION(pe_header); /* macro finds first section */

	/* walk image sections to find .stub and .code
	 * populate the section info when found */
	for (int i = 0; i < nsections; i++)
	{
		if (strcmp((char *)section->Name, CODE_SECTION_NAME) == 0)
		{
			code_section.praw_data = section->PointerToRawData;
			code_section.raw_size = section->SizeOfRawData;
			code_section.virtual_address = section->VirtualAddress;
		}

		else if (strcmp((char *)section->Name, STUB_SECTION_NAME) == 0) 
		{
			stub_section.praw_data = section->PointerToRawData;
			stub_section.raw_size = section->SizeOfRawData;
			stub_section.virtual_address = section->VirtualAddress;
		}

		section = section + 1;
	}
	
	ImageUnload(image);

	encrypt_section(&code_section, key, argv[1]);
	return 0;
}

static int encrypt_section(PSECTION_INFO section, BYTE *key, char *file_name)
{
	BYTE	*buffer;
	DWORD	offset;
	DWORD	num_bytes;
	FILE	*fptr;

	num_bytes = section->raw_size;
	offset = section->praw_data;
	buffer = (BYTE *)malloc(num_bytes);
	fptr = fopen(file_name, "r+b");

	printf("%x\n", offset);
	printf("%x\n", num_bytes);

	fseek(fptr, offset, SEEK_SET); /* start reading at the section's given offset on disk */
	fread(buffer, sizeof(BYTE), num_bytes, fptr); 

	/* XOR encrypt the buffer */
	int k = KEY_SIZE;
	for (int i = 0; i < num_bytes; i++)
	{
		buffer[i] = buffer[i] ^ key[k--] ^ (i*k - i);
		if (k == 0)
			k = KEY_SIZE;
	}

	fseek(fptr, offset, SEEK_SET);
	fwrite(buffer, sizeof(BYTE), num_bytes, fptr);

	fclose(fptr);

	return 0;
}

static int is_valid_key(BYTE* key)
{
	int n = 0;
	while (key[n++] != '\0');
	return n == KEY_SIZE;
}
