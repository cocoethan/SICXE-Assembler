#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 

#define SIZE 1024
#define OPCODETABLESIZE 60
#define WORD_LENGTH 3
#define F1_OBJ_LENGTH 1
#define F2_OBJ_LENGTH 2
#define F3_OBJ_LENGTH 3
#define F4_OBJ_LENGTH 4
#define MAX_OBJ_CODE_LENGTH 59

//main.c headers

int validHex(char *test);
int onebXE(char *objCode);

//records.c headers
typedef struct TextRecord{
	int startAddress;
	int length;
	char objectCode[60];
} TRecord, TextRecord;

typedef struct ModificationRecord{
	int startAddress;
	int length;
	char symbolName[7];
} MRecord, ModRecord, ModificationRecord;

typedef struct HeaderRecord{
	char name[7];
	int startAddress;
	int length;
} HRecord, HeaderRecord;

void insertTRecord(TextRecord *TextRecords[], int address, int length, char *objCode);
void insertMRecord(ModificationRecord *ModRecords[], int address, int modLength, char *name);
int Byte2(char *c, char *hexString);

//Symbols.c Headers
typedef struct Symbol{
        int SymTabIndex;
        int address;
        int sourceline;
        char name[7];
} Symbol;


typedef struct Symbol	SYMBOL;

int IsAValidSymbol(Symbol *SymTab[], char *TestSymbol );
unsigned int hash(char *string);
int InsertSymbol(Symbol *SymTab[], char *name, int *addr);
int SearchSymTab(Symbol *SymTab[], char *testName);
void PrintSymTab( Symbol *SymTab[]);
void freeSymTab(Symbol *SymTab[]);
void initSymTab(Symbol *SymTab[]);

//Directives.c Headers
int IsADirective( char *Test);
int Start(Symbol *SymTab[], int index, char *c, int* address);
int Byte( char *c, int *address);
void Word( int *address);
void ResB( char *c, int *address);
void ResW( char *c, int *address);
void ResR( int *address);
void Exports( int *address);
void End();

//opcodes.c Headers
struct opcodes {
	char	OpCode[5];
	char	Name[8];
	int 	Format;
	int 	opcodeType; // 0 = SIC; 1 = SIC/XE
};

typedef struct opcodes OPCODES;


OPCODES OpcodeTable[ OPCODETABLESIZE ];

void initOpcodeTable();
void printOpcodeTable();
int IsAnOpcode(char *test);
int InsertOpcode(char* name, int format, char* opcode, int SICorXE);
unsigned int hashOpcode(char *string);
char* format2OC(char* objCode,char* optok,char* optok2,char* nexttoken,char* operand,int opTableIndex);

