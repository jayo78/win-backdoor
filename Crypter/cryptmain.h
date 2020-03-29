
/* This structure holds information about a section */
typedef struct section_info {
	DWORD virtual_address;	/* address of section relative to the image base when loaded into memory */
	DWORD raw_size;			/* size of the section on disk */
	DWORD praw_data;		/* pointer to the section on disk*/
} SECTION_INFO, *PSECTION_INFO;

/* These need to match the section names in the pe file 
 * that the crypter is meant to encrypt and patch */
#define CODE_SECTION_NAME ".code" 
#define STUB_SECTION_NAME ".stub"

	/* defines the key size for encryption*/
#define KEY_SIZE 8
