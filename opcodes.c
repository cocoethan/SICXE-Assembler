#include "headers.h"
char *mnemonicArray[] = {"ADD", "ADDF", "ADDR", "AND", "CLEAR", "COMP", "COMPF", "COMPR", "DIV", "DIVF", "DIVR", "FIX", "FLOAT", "HIO", "J", "JEQ","JGT","JLT","JSUB","LDA","LDB","LDCH","LDF","LDL","LDS","LDT","LDX","LPS","MUL","MULF","MULR","NORM","OR","RD","RMO","RSUB","SHIFTL","SHIFTR","SIO","SSK","STA","STB","STCH","STF","STI","STL","STS","STSW","STT","STX","SUB","SUBF","SUBR","SVC","TD","TIO","TIX","TIXR","WD"};

char *opcodeArray[] = {"18","58","90","40","B4","28","88","A0","24","64","9C","C4","C0","F4","3C","30","34","38","48","00","68","50","70","08","6C","74","04","D0","20","60","98","C8","44","D8","AC","4C","A4","A8","F0","EC","0C","78","54","80","D4","14","7C","E8","84","10","1C","5C","94","B0","E0","F8","2C","B8","DC"};

char*  formatArray[] = {"3","3","2","3","2","3","3","2","3","3","2","1","1","1","3","3","3","3","3","3","3","3","3","3","3","3","3","3","3","3","2","1","3","3","2","3","2","2","1","3","3","3","3","3","3","3","3","3","3","3","3","3",
"2","2","3","1","3","2","3"};

int XEorSIC[] = {0,1,1,0,1,0,1,1,0,1,1,1,1,1,0,0,0,0,0,0,1,0,1,0,1,1,0,1,0,1,1,1,0,0,1,0,1,1,1,1,0,1,0,1,1,0,1,0,1,0,0,1,1,1,0,1,0,1,0}; //0 = SIC; 1 = SIC/XE

//Hash Function
unsigned int hashOpcode(char *string){

	int value = 0;
	for(int i=0; (string[i] >= 65 && string[i] <= 90); i++){
			value+= string[i];
	}
	return value%OPCODETABLESIZE;
}

//build opcode table. Arrays positions are correctly placed beforehand
void initOpcodeTable(){
	char* codeName;
	char* format;
	char* code;
	int SICorXE;

	int length = sizeof(OpcodeTable)/sizeof(OpcodeTable[0]);
	for(int i=0; i<59; i++){
		codeName = mnemonicArray[i];
		format = formatArray[i];
		code = opcodeArray[i];
		SICorXE = XEorSIC[i];
		InsertOpcode(codeName, atoi(format), code, SICorXE);
	}
}

int InsertOpcode(char* name, int format, char* opcode, int SICorXE){
	int index = hashOpcode(name);
	int x = 0;
	while(OpcodeTable[index].Format != 0){
		++index%OPCODETABLESIZE;
		if(x++ > OPCODETABLESIZE)
			return -1;
	}
	strcpy(OpcodeTable[index].OpCode, opcode);

	sprintf(OpcodeTable[index].Name, "%s",  name);
	OpcodeTable[index].Format = format;
	OpcodeTable[index].opcodeType = SICorXE; 

	return index;

}

//print opcode table
void printOpcodeTable(){
	int length = sizeof(OpcodeTable)/sizeof(OpcodeTable[0]);
	printf("Start\n");
	for(int i=0; i<length; i++)
		printf("|%s|%s|%d|\n", OpcodeTable[i].Name, OpcodeTable[i].OpCode, OpcodeTable[i].Format);
	}

//return index of opcode table if string is found, return -1 if not
int IsAnOpcode(char *test){
	int index = hashOpcode(test);
	for (int i = index; !(i > OPCODETABLESIZE); i++, ++index%OPCODETABLESIZE){
		if(strcmp(test, OpcodeTable[index].Name) == 0){
			return index;
		}
	}
	return -1;
}

char* format2OC(char* objCode,char* optok,char* optok2,char* nexttoken,char* operand,int opTableIndex){
	strcpy(objCode,OpcodeTable[opTableIndex].OpCode);
					if(strstr(nexttoken,"CLEAR")!=NULL || strstr(nexttoken,"TIXR")!=NULL){
						if(strstr(operand,"A")!=NULL){
							strcat(objCode,"0");
						}else if(strstr(operand,"X")!=NULL){
							strcat(objCode,"1");
						}else if(strstr(operand,"L")!=NULL){
							strcat(objCode,"2");
						}else if(strstr(operand,"B")!=NULL){
							strcat(objCode,"3");
						}else if(strstr(operand,"S")!=NULL){
							strcat(objCode,"4");
						}else if(strstr(operand,"T")!=NULL){
							strcat(objCode,"5");
						}else if(strstr(operand,"F")!=NULL){
							strcat(objCode,"6");
						}else if(strstr(operand,"PC")!=NULL){
							strcat(objCode,"8");
						}else if(strstr(operand,"SW")!=NULL){
							strcat(objCode,"9");
						}
						strcat(objCode,"0");
					}else if(strstr(nexttoken,"SHIFTL")!=NULL || strstr(nexttoken,"SHIFTR")!=NULL){
						optok = strtok(operand,",");
						optok2 = strtok(NULL,"\t\n");

						if(strstr(optok,"A")!=NULL){
							strcat(objCode,"0");
						}else if(strstr(optok,"X")!=NULL){
							strcat(objCode,"1");
						}else if(strstr(optok,"L")!=NULL){
							strcat(objCode,"2");
						}else if(strstr(optok,"B")!=NULL){
							strcat(objCode,"3");
						}else if(strstr(optok,"S")!=NULL){
							strcat(objCode,"4");
						}else if(strstr(optok,"T")!=NULL){
							strcat(objCode,"5");
						}else if(strstr(optok,"F")!=NULL){
							strcat(objCode,"6");
						}else if(strstr(optok,"PC")!=NULL){
							strcat(objCode,"8");
						}else if(strstr(optok,"SW")!=NULL){
							strcat(objCode,"9");
						}
						strcat(objCode,optok2);
					}else if(strstr(nexttoken,"SVC")!=NULL){
						if(strstr(operand,"A")!=NULL){
							strcat(objCode,"0");
						}else if(strstr(operand,"X")!=NULL){
							strcat(objCode,"1");
						}else if(strstr(operand,"L")!=NULL){
							strcat(objCode,"2");
						}else if(strstr(operand,"B")!=NULL){
							strcat(objCode,"3");
						}else if(strstr(operand,"S")!=NULL){
							strcat(objCode,"4");
						}else if(strstr(operand,"T")!=NULL){
							strcat(objCode,"5");
						}else if(strstr(operand,"F")!=NULL){
							strcat(objCode,"6");
						}else if(strstr(operand,"PC")!=NULL){
							strcat(objCode,"8");
						}else if(strstr(operand,"SW")!=NULL){
							strcat(objCode,"9");
						}
						strcat(objCode,"0");
					}else{
						optok = strtok(operand,",");
						optok2 = strtok(NULL,"\t\n");

						if(strstr(optok,"A")!=NULL){
							strcat(objCode,"0");
						}else if(strstr(optok,"X")!=NULL){
							strcat(objCode,"1");
						}else if(strstr(optok,"L")!=NULL){
							strcat(objCode,"2");
						}else if(strstr(optok,"B")!=NULL){
							strcat(objCode,"3");
						}else if(strstr(optok,"S")!=NULL){
							strcat(objCode,"4");
						}else if(strstr(optok,"T")!=NULL){
							strcat(objCode,"5");
						}else if(strstr(optok,"F")!=NULL){
							strcat(objCode,"6");
						}else if(strstr(optok,"PC")!=NULL){
							strcat(objCode,"8");
						}else if(strstr(optok,"SW")!=NULL){
							strcat(objCode,"9");
						}

						if(strstr(optok2,"A")!=NULL){
							strcat(objCode,"0");
						}else if(strstr(optok2,"X")!=NULL){
							strcat(objCode,"1");
						}else if(strstr(optok2,"L")!=NULL){
							strcat(objCode,"2");
						}else if(strstr(optok2,"B")!=NULL){
							strcat(objCode,"3");
						}else if(strstr(optok2,"S")!=NULL){
							strcat(objCode,"4");
						}else if(strstr(optok2,"T")!=NULL){
							strcat(objCode,"5");
						}else if(strstr(optok2,"F")!=NULL){
							strcat(objCode,"6");
						}else if(strstr(optok2,"PC")!=NULL){
							strcat(objCode,"8");
						}else if(strstr(optok2,"SW")!=NULL){
							strcat(objCode,"9");
						}
					}
	return objCode;
}
