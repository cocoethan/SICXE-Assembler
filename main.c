#include "headers.h"


int main( int argc, char* argv[]){

	//Main variable declaration

	FILE *fp;
	FILE *objFP;
	char line[SIZE];
	char *newsym;
	char *nexttoken;
	char *operand;
	char *operandCPY;
	char xbpe_str[2];
	char asciiHexString[SIZE*2] = {0};
	char objCode[SIZE] = {0};
	char hexString[SIZE*2]={0};
	char programStartName[7];
	int counter = 0;
	int startCounter = 0;
	int programLength = 0;
	int objLength = 0;
	int endRecordAddress = 0;
	int opTableIndex = 0;
	int currentIndex = 0;
	int startFlag = 0;
	int endFlag = 0;
	int opclc = 0;

	char* baseOperand;
	int baseAddress = 0;
	int baseFlag = 0;


	//2 Byte Register Numbers //-Ethan Coco
	char *optok;//-Ethan Coco
	char *optok2;//-Ethan Coco

	int xbpeFlag = 0;

	int eflag = 0;//-Ethan Coco //"+" flag ( Format 4 e bit flag)
	int iflag = 0;//-Ethan Coco // "#" flag (immediate i bit flag)
	int nflag = 0;//"@" flag ( n bit flag)


	struct Symbol **SymbolHashTable = malloc(SIZE * sizeof(SYMBOL *));
	struct TextRecord **TextRecords = malloc(SIZE * sizeof(TRecord *));
  	struct ModificationRecord **ModRecords = malloc (SIZE * sizeof(MRecord *));
	struct HeaderRecord header;

	//Check to see if there are 2 arguments
	if ( argc != 2 ) {
	printf("ERROR: Usage: %s filename.\n", argv[0]);
	return 0;
	}

	//open the file in "read" only mode
	fp = fopen( argv[1], "r");

	//If file being opened brings back NULL, throw error
	if ( fp == NULL ) {
		printf("ERROR: %s could not be opened for reading.\n", argv[1] );
		exit(0);
	}

	initOpcodeTable(); // Build opcode table

	// Memory Allocation
	newsym = malloc(  SIZE * sizeof(char) );
	memset( newsym, '\0', SIZE * sizeof(char) );

	nexttoken = malloc(  SIZE * sizeof(char) );
	memset( nexttoken, '\0', SIZE * sizeof(char) );

  	operand = malloc( SIZE * sizeof(char) );
	memset( operand, '\0', SIZE * sizeof(char) );

  	operandCPY = malloc(SIZE * sizeof(char) );
	memset(operandCPY, '\0', SIZE * sizeof(char) );

  	optok = malloc(SIZE * sizeof(char));//-Ethan Coco
	memset(optok,'\0',SIZE * sizeof(char));//-Ethan Coco

  	optok2 = malloc(SIZE * sizeof(char));//-Ethan Coco
	memset(optok2,'\0',SIZE * sizeof(char));//-Ethan Coco

	baseOperand = malloc(SIZE * sizeof(char));
	memset(baseOperand,'\0',SIZE * sizeof(char));

  	//First pass
  	while(  fgets( line , SIZE , fp ) != NULL   ) {
		if( counter >= 32768){
			printf("ERROR. PROGRAM DOES NOT FIT SIC MEMORY.\n");
			exit(0);
		}
		currentIndex = 0;
		eflag = 0;
   		//If the line starts with #, continue
    		if ( line[0] == 35){
        		continue;
    		}

		//If(first character is between A-Z)
        	if (  ((line[0] >= 65 ) && ( line[0] <= 90 )) || line != NULL  )  {
			//tokenize the line using tabs and newline being delimiters.
			newsym = strtok( line," \t\n");
           		nexttoken = strtok( NULL," \t\n" ); // Gets next token by submitting null
           		operand = strtok( NULL, " \t\n" );

			if(newsym[0] == 43){ //Check if the first character is "+"
				newsym[0] = '\n';
				memmove(newsym,newsym+1,strlen(newsym));
				eflag = 1;
			}

			if(strstr(newsym, "RSUB") != NULL){
				counter += 3;
				continue;
			}

            		//Check to see if first token is an instruction
			if( ( (opTableIndex = IsAnOpcode(newsym) ) != -1) ){
				operand = nexttoken;
				nexttoken = newsym;
				newsym = NULL;

				//Steven Dear
				if(operand[0]==35){ //Check if first character is "#"
					operand[0] = '\n';
					memmove(operand,operand+1,strlen(operand));
				}
				else if(operand[0]==64){//"@"
					operand[0] = '\n';
					memmove(operand,operand+1,strlen(operand));
				}

				//End

				if(eflag == 1)
					counter += 4;
				else
					counter += OpcodeTable[opTableIndex].Format;
				continue;
			}
			else if(IsADirective(newsym) != 0){
				operand = nexttoken;
				nexttoken = newsym;
				newsym = NULL;
			}
           		else if ( IsAValidSymbol(SymbolHashTable, newsym) == 0 ) {
				printf("ERROR. %s IS AN  INVALID SYMBOL.\n", newsym);
                		exit(0);
           		}

			if (newsym != NULL){
			currentIndex = InsertSymbol(SymbolHashTable, newsym, &counter); //inserts new symbol into Hash Table
			}

			switch(IsADirective(nexttoken)){
				case 1: ; //START DIRECTIVE
					if((validHex(operand) == 0) || (operand[0] != '0')){
						printf("ERROR. INVALID HEX NUMBER. \n");
						exit(0);
					}
					if(Start(SymbolHashTable, currentIndex, operand, &counter)== 0){
						printf("ERROR. INVALID START NUMBER. \n");
						exit(0);
					}
					startFlag++;
					startCounter = counter;
				break;
                		case 2: //BYTE DIRECTIVE
					if(Byte(operand, &counter) == 0){
						printf("ERROR. ODD NUMBER OF HEXES. \n");
						exit(0);
					}
				break;
                		case 3:	//WORD DIRECTIVE
                			if(atoi(operand) >= 8388607){
        	        		    	printf("ERROR. WORD CONSTANT IS TOO LARGE. \n");
	                		        exit(0);
                			}

					Word(&counter);
                		break;
                		case 4: //RESB DIRECTIVE
					ResB(operand, &counter);
                		break;
                		case 5: //RESW DIRECTIVE
					ResW(operand, &counter);
               		 	break;
              			case 6: //RESR DIRECTIVE
					ResR(&counter);
                		break;
                		case 7: //EXPORTS DIRECTIVE
					Exports(&counter);
                		break;
				case 8: //END DIRECTIVE
					End(&counter);
					endFlag++;
				break;
				default:
				if(nexttoken[0] == 43){
					nexttoken[0] = '\n';
					memmove(nexttoken,nexttoken+1,strlen(nexttoken));
					eflag = 1;
				}
				if(((opTableIndex = IsAnOpcode(nexttoken))!=-1)){
					if(eflag == 1){
						counter += 4;
					}else{
						counter += OpcodeTable[opTableIndex].Format;
					}
				}
				break;
			}
		}
	}

	if(startFlag != 1 || endFlag != 1){
		printf("ERROR. NONE/MORE THAN ONE START OR END DIRECTIVE PRESENT.\n");
		exit(0);
	}

	rewind(fp); //set file pointer to beginning of file
	programLength = counter - startCounter; //calculates length of the program
	counter = 0; //reset counter
	char *objFile = strcat(argv[1], ".obj"); //"filenamr.obj"
	objFP = fopen(objFile , "w");
	//PASS 2
	while(  fgets( line , SIZE , fp ) != NULL   ) {
    		//If the line starts with #, continue
        	if ( line[0] == 35){
        		continue;
        	}
		iflag = 0;
		eflag = 0;
		nflag = 0;
		xbpeFlag = 0;

        	//If(first character is between A-Z)
       		if (  ((line[0] >= 65 ) && ( line[0] <= 90 )) || line != NULL  )  {
			//tokenize line
        		newsym = strtok( line, " \t\n"); //token 1
        		nexttoken = strtok( NULL, " \t\n"  ); //token 2
       			operand = strtok( NULL, "\t\n" ); //token 3

			currentIndex = SearchSymTab(SymbolHashTable, newsym); //Check if first token is a symbol
			if(currentIndex != -1){ //Symbol has been found
			}
			else if( strstr(newsym, "RSUB") != NULL){ //Check for RSUB opcode
				strcpy(objCode, "4F0000");
				insertTRecord(TextRecords, counter,F3_OBJ_LENGTH, objCode);
				counter +=3;
				continue;
                	}
                	else{ 							//if first token is not a symbol or RSUB opcode, rearrange token variables for consistency; 
                		operand = nexttoken;
                		nexttoken = newsym;
				newsym = NULL;
			}


			if(nexttoken[0]==43){ //"+"
				nexttoken[0] = '\n';
				memmove(nexttoken,nexttoken+1,strlen(nexttoken));
				eflag = 1;
			}

			opTableIndex = IsAnOpcode(nexttoken);

			if(OpcodeTable[opTableIndex].Format == 1){//checks if xe instruction is a format 1 instruction//-Ethan Coco
				strcpy(objCode,OpcodeTable[opTableIndex].OpCode);
				insertTRecord(TextRecords,counter,F1_OBJ_LENGTH,objCode);
				counter +=1;
				continue;
			}

			if(operand[0]==35){//"#"
				operand[0] = '\n';
				memmove(operand,operand+1,strlen(operand));
				iflag = 1;
			}
			else if(operand[0]==64){//"@"
				operand[0] = '\n';
				memmove(operand,operand+1,strlen(operand));
				nflag = 1;
			}

			switch(IsADirective(nexttoken)){
				case 1: ; //START DIRECTIVE
					strcpy(header.name, newsym); //HEADER PROGRAM = First token
					header.startAddress = SymbolHashTable[currentIndex]->address; //HEADER STARTING ADDRESS = newsym.address
					header.length = programLength;	//HEADER LENGTH OF PROGRAM = programLength (caclulated after Pass 1)
					strcpy(programStartName, SymbolHashTable[currentIndex]->name); //programStartName = newsym.name (for Modification Records)
					counter = SymbolHashTable[currentIndex]->address; //update counter with symbol's address
				continue;
				case 2: //BYTE DIRECTIVE
					counter = SymbolHashTable[currentIndex]->address; //update counter with current symbol's address
					objLength = Byte2(operand, hexString); // remove char or hex indicator and return the length of the hex string
					insertTRecord(TextRecords, counter, objLength, hexString); //Insert into Text Records
					counter += objLength;
				continue;
				case 3: //WORD DIRECTIVE
					counter = SymbolHashTable[currentIndex]->address; //update counter
					int x = atoi(operand); //convert operand into an int
					sprintf(asciiHexString,"%06X",x); //convert int operand to a hex string
					insertTRecord(TextRecords, counter, WORD_LENGTH, asciiHexString); //Insert into Text Records
					counter += 3;
				continue;
				case 4: //RESB DIRECTIVE
					counter = SymbolHashTable[currentIndex]->address; //update counter
					ResB(operand, &counter); //update counter
				continue;
				case 5: //RESW DIRECTIVE
					counter = SymbolHashTable[currentIndex]->address; //update counter
					ResW(operand, &counter); //update counter
				continue;
				case 6: //RESR DIRECTIVE
					counter = SymbolHashTable[currentIndex]->address; //update counter
					ResR(&counter); //update counter
				continue;
				case 7: //EXPORTS DIRECTIVE
					counter = SymbolHashTable[currentIndex]->address; //update counter
					Exports(&counter);
				continue;
				case 8: //END DIRECTIVE
					if(SearchSymTab(SymbolHashTable, operand) == -1){ //if symbol is not in SymbolHashTable, exit program
						printf("ERROR. SYMBOL IS NOT PRESENT IN SYMBOL TABLE.\n");
						exit(0);
					}
					endRecordAddress = SymbolHashTable[SearchSymTab(SymbolHashTable, operand)]->address; //endRecordAddress = Third token.address
				continue;
				case 9:	//BASE DIRECTIVE
					currentIndex = SearchSymTab(SymbolHashTable,operand);
					strcpy(baseOperand,operand);
					baseAddress = SymbolHashTable[currentIndex]->address;
					baseFlag = 1;
				continue;
				case 10: // NOBASE DIRECTIVE
					baseFlag = 0;
				continue;
				default :

				break;
			}
				//Check and handle format 2 instructions
			if(OpcodeTable[opTableIndex].Format==2){
				strcpy(objCode,format2OC(objCode,optok,optok2,nexttoken,operand,opTableIndex));
				insertTRecord(TextRecords,counter,F2_OBJ_LENGTH,objCode);
				counter += 2;
			 }
				//Check and handle Format 3 instructions
			else if(OpcodeTable[opTableIndex].Format==3 && eflag == 0){//-Ethan Coco
				int difference = 0;
				strcpy(objCode, OpcodeTable[opTableIndex].OpCode); //copy opcode hex
				strcpy(operandCPY, operand); //copy third token to avoid unintentional modification of original
				int number = atoi(operand);

				if(nflag==1){ // "@" detected
					opclc = (int)strtol(objCode,NULL,16);
					opclc = opclc + 2;
					sprintf(objCode,"%02X",opclc);
					opclc = 0;
					if(strstr(operandCPY, ",X") != NULL){
						printf("ERROR. INDEXED ADDRESSING CANNOT BE COMBINED WITH INDIRECT ADDRESSING\n");
						exit(0);
					}
					if(number != 0 || operand[0] == '0'){//@ is followed by a number
						sprintf(hexString,"%03X", number);
						sprintf(xbpe_str, "%X", xbpeFlag);
						strcat(objCode, xbpe_str);
						strcat(objCode, hexString);
						if(newsym == NULL){
							insertTRecord(TextRecords,counter,F3_OBJ_LENGTH,objCode);
							counter += 3;
						}
						else{
							counter = SymbolHashTable[currentIndex]->address;
							insertTRecord(TextRecords,counter,F3_OBJ_LENGTH,objCode);
							counter += 3;
						}
						continue;
					}
				}
				else if(iflag == 1){ // "#" detected
					opclc = (int)strtol(objCode,NULL,16);
					opclc = opclc + 1;
					sprintf(objCode,"%02X",opclc);
					opclc = 0;

					if(strstr(operandCPY, ",X") != NULL){
						printf("ERROR. INDEXED ADDRESSING CANNOT BE COMBINED WITH IMMEDIATE ADDRESSING\n");
						exit(0);
					}
					if(number != 0 || operand[0] == '0'){//# is followed by a number
						sprintf(hexString,"%03X", number);
						sprintf(xbpe_str, "%X", xbpeFlag);
						strcat(objCode, xbpe_str);
						strcat(objCode, hexString);

						if(newsym == NULL){
							//counter += 3;
							insertTRecord(TextRecords,counter,F3_OBJ_LENGTH,objCode);
							counter += 3;
						}
						else{
							counter = SymbolHashTable[currentIndex]->address;
							insertTRecord(TextRecords,counter,F3_OBJ_LENGTH,objCode);
							counter += 3;
						}
						continue;
					}
				}
				else if(OpcodeTable[opTableIndex].opcodeType != 1){	//Simple addressing for non-XE instructions
					opclc = (int)strtol(objCode,NULL,16);
					opclc = opclc + 3;
					sprintf(objCode,"%02X",opclc);
					opclc = 0;
				}
				if(strstr(operandCPY, ",X") != NULL){ //Indexed Addressing
					operand = strtok(operand, ",X");
					int indexedAddress = SearchSymTab(SymbolHashTable, operand);
					if(indexedAddress == -1){
						printf("ERROR. SYMBOL %s IS NOT PRESENT IN SYMBOL TABLE.\n", operand);
						exit(0);
					}
					xbpeFlag += 8;
					if(baseFlag == 1){
						difference = SymbolHashTable[indexedAddress]->address - baseAddress;
						xbpeFlag += 4;
					}
				}

				if(strcmp(baseOperand,operandCPY)==0 && baseFlag == 1 && strcmp(nexttoken,"LDA")!=0 && strcmp(nexttoken,"STA")!=0){
					difference = 0;
					xbpeFlag += 4;
				}
				else if(baseFlag ==1 && xbpeFlag == 12){
				}
				else{
					difference = SymbolHashTable[SearchSymTab(SymbolHashTable,operand)]->address - (counter + 3);
					xbpeFlag += 2;
				}


				sprintf(xbpe_str, "%X", xbpeFlag);
				sprintf(hexString,"%03X", difference); //convert difference to hex, output to "hexString" variable
        
        				if(difference<0){
					sprintf(hexString,"%s",hexString + strlen(hexString) - 3);
				}
        
				strcat(objCode, xbpe_str);
				strcat(objCode, hexString);
					if(newsym == NULL){
					insertTRecord(TextRecords,counter,F3_OBJ_LENGTH,objCode);
					counter += 3;
				}
				else{
					counter = SymbolHashTable[currentIndex]->address;
					insertTRecord(TextRecords,counter,F3_OBJ_LENGTH,objCode);
					counter += 3;
				}
				continue;
			}
			//Handle Format 4 instructions
			else{
				strcpy(objCode, OpcodeTable[opTableIndex].OpCode); //copy opcode hex
				strcpy(operandCPY, operand); //copy third token to avoid unintentional modification of original
				int number = atoi(operand);

				if(strcmp(baseOperand,operandCPY)==0 && baseFlag == 1 && strcmp(nexttoken,"LDA")!=0 && strcmp(nexttoken,"STA")!=0){
					xbpeFlag += 4;
				}else{
					//xbpeFlag += 2;
				}
				xbpeFlag += 1;

				if(nflag==1){ // "@" detected
					opclc = (int)strtol(objCode,NULL,16);
					opclc = opclc + 2;
					sprintf(objCode,"%02X",opclc);
					opclc = 0;
					if(strstr(operandCPY, ",X") != NULL){
						printf("ERROR. INDEXED ADDRESSING CANNOT BE COMBINED WITH INDIRECT ADDRESSING\n");
						exit(0);
					}
					if(number != 0 || operand[0] == '0'){//# is followed by a number
						sprintf(hexString,"%05X", number);
						sprintf(xbpe_str, "%X", xbpeFlag);
						strcat(objCode, xbpe_str);
						strcat(objCode, hexString);
						if(newsym == NULL){
							insertTRecord(TextRecords,counter,F4_OBJ_LENGTH,objCode);
							counter += 4;
						}
						else{
							counter = SymbolHashTable[currentIndex]->address;
							insertTRecord(TextRecords,counter,F4_OBJ_LENGTH,objCode);
							counter += 4;
						}
						continue;
					}
					}
				else if(iflag == 1){ // "#" detected
					opclc = (int)strtol(objCode,NULL,16);
					opclc = opclc + 1;
					sprintf(objCode,"%02X",opclc);
					opclc = 0;
					if(strstr(operandCPY, ",X") != NULL){
						printf("ERROR. INDEXED ADDRESSING CANNOT BE COMBINED WITH IMMEDIATE ADDRESSING\n");
						exit(0);
					}
					if(number != 0 || operand[0] == '0'){//# is followed by a number
						char xbpe_str[2];
						sprintf(hexString,"%05X", number);
						sprintf(xbpe_str, "%X", xbpeFlag);
						strcat(objCode, xbpe_str);
						strcat(objCode, hexString);
						if(newsym == NULL){
							insertTRecord(TextRecords,counter,F4_OBJ_LENGTH,objCode);
							counter += 4;
						}
						else{
							counter = SymbolHashTable[currentIndex]->address;
							insertTRecord(TextRecords,counter,F4_OBJ_LENGTH,objCode);
							counter += 4;
						}
						continue;
					}
				}
				else if(OpcodeTable[opTableIndex].opcodeType != 1){	//Simple addressing for non-XE instructions
					opclc = (int)strtol(objCode,NULL,16);
					opclc = opclc + 3;
					sprintf(objCode,"%02X",opclc);
					opclc = 0;
				}

				if(strstr(operandCPY, ",X") != NULL){ //Indexed Addressing
					operand = strtok(operand, ",X");
					if(SearchSymTab(SymbolHashTable, operand) == -1){
						printf("ERROR. SYMBOL %s IS NOT PRESENT IN SYMBOL TABLE.\n", operand);
						exit(0);
					}
					xbpeFlag += 8;
				}
				sprintf(xbpe_str, "%X", xbpeFlag);
				sprintf(hexString,"%05X", SymbolHashTable[SearchSymTab(SymbolHashTable, operand)]->address); //convert symbol's address to hex, output to "hexString" variable
				strcat(objCode, xbpe_str);
				strcat(objCode, hexString);
				if(newsym == NULL){
					insertTRecord(TextRecords,counter,F4_OBJ_LENGTH,objCode);
					insertMRecord(ModRecords, counter, strlen(hexString), programStartName);
					counter += 4;
				}
				else{
					counter = SymbolHashTable[currentIndex]->address;
					insertTRecord(TextRecords,counter,F4_OBJ_LENGTH,objCode);
					insertMRecord(ModRecords, counter, strlen(hexString), programStartName);
					counter += 4;
				}
				continue;
			}

		}
	}

	fprintf(objFP,"H%s\t%06X%06X\n",header.name,header.startAddress,header.length);
	for(int i=0; i<SIZE; i++){//Prints only object code for the time being, will change //-Ethan Coco
		if(TextRecords[i])
			fprintf(objFP,"T%06X%02X%s\n",TextRecords[i]->startAddress,TextRecords[i]->length,TextRecords[i]->objectCode);
	}
	for(int i = 0; i<SIZE; i++){
		if(ModRecords[i])
			fprintf(objFP,"M%06X%02X+%s\n",ModRecords[i]->startAddress,ModRecords[i]->length,ModRecords[i]->symbolName);
	}
	fprintf(objFP,"E%06X\n",endRecordAddress);
/*
	//Alternative for testing objcode only
	for(int i = 0; i<SIZE; i++){
		if(TextRecords[i])
			printf("%s\n",TextRecords[i]->objectCode);
	}
*/
	//Closing statements
	fclose( fp );
	fclose( objFP );
	free(SymbolHashTable);
	free(TextRecords);
	free(ModRecords);

	return 0;
}

int validHex(char *test){ //check if string is a valid hex string
	int length = strlen(test);
	for(int i=0; i<length && test[i] != 13; i++){
		if(! (isxdigit(test[i]) ) )
			return 0;
	}
	return 1;
}
