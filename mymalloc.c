#include <stdio.h>
#define INIT_BLOCK 0
#define MEMORY_SIZE 25000
#define HEADER_SIZE sizeof(MEMORY_SIZE)
#define VERBOSE 1


char RAM[MEMORY_SIZE];




void * MyMalloc(int REQUIRED_SIZE){
	//check if the array is initialized for malloc;
	if (RAM[INIT_BLOCK] == 0 && !(RAM[INIT_BLOCK+1] == 'a' || RAM[INIT_BLOCK+1] == 'f' )){
		//uninited
		// initing
		int i;
		//printf("Initializing Memory\n");
		for(i=0;i<MEMORY_SIZE;i++){
			if(VERBOSE)printf("\rInitializing memory in progress %d%%", (i+1)*100/MEMORY_SIZE);
			fflush(stdout);
			RAM[i] = '\0';
		}
		//writing maiden header
		RAM[INIT_BLOCK] = 1;
		RAM[INIT_BLOCK+1] = 'f';
		*(int *)(&RAM[INIT_BLOCK+2]) = MEMORY_SIZE - 2 - HEADER_SIZE;
		if(VERBOSE)printf("\nInitialization completed. %d bytes available for use.\n",*(int *)(&RAM[INIT_BLOCK+2]));
		return MyMalloc(REQUIRED_SIZE);
	}else{
		//inited
		int CURRENT_LOCATION = INIT_BLOCK + 1;
		int FLAG_DETECTED = 0;
		if(VERBOSE)printf("Searching for %d bytes\n",REQUIRED_SIZE );
		while (CURRENT_LOCATION < MEMORY_SIZE && FLAG_DETECTED == 0){
			if(VERBOSE)printf("\tscanning block 0x%05X",CURRENT_LOCATION );
			if (RAM[CURRENT_LOCATION] == 'f'){
				if (*(int *)(&RAM[CURRENT_LOCATION+1])>=REQUIRED_SIZE){
					//space found
					FLAG_DETECTED = 1;
					if(VERBOSE)printf(" - Available\n");
					break;
				}else{
					if(VERBOSE)printf(" - Out of space\n");
					CURRENT_LOCATION += HEADER_SIZE + *(int *)(&RAM[CURRENT_LOCATION+1]) + 1;
				}
			}else{
				//space not found
				if(VERBOSE)printf(" - In use\n");
				CURRENT_LOCATION += HEADER_SIZE + *(int *)(&RAM[CURRENT_LOCATION+1]) + 1;
			}
		}
		if(VERBOSE)printf("Finished searching for %d bytes\n",REQUIRED_SIZE );
		if (FLAG_DETECTED){
			if(VERBOSE)printf("Free space of %d bytes found at %d\n",*(int *)(&RAM[CURRENT_LOCATION+1]),CURRENT_LOCATION );
			if (*(int *)(&RAM[CURRENT_LOCATION+1]) > REQUIRED_SIZE + HEADER_SIZE + 1){
				//split & allocate
				int TOTAL = *(int *)(&RAM[CURRENT_LOCATION+1]);
				RAM[CURRENT_LOCATION] = 'a';
				*(int *)(&RAM[CURRENT_LOCATION+1]) = REQUIRED_SIZE;
				RAM[CURRENT_LOCATION+HEADER_SIZE+1+REQUIRED_SIZE] = 'f';
				*(int *)(&RAM[CURRENT_LOCATION+HEADER_SIZE+2+REQUIRED_SIZE]) = TOTAL - REQUIRED_SIZE - HEADER_SIZE -1;
				if(VERBOSE)printf("Memory location 0x%05x allocated with %d bytes\n\n",(int)(CURRENT_LOCATION+HEADER_SIZE+1),REQUIRED_SIZE);
				return &RAM[CURRENT_LOCATION+HEADER_SIZE+1];
				
			}else{
				//direct
				RAM[CURRENT_LOCATION] = 'a';
				if(VERBOSE)printf("Memory location 0x%05X allocated with %d bytes\n\n",(int)(CURRENT_LOCATION+HEADER_SIZE+1),*(int *)(&RAM[CURRENT_LOCATION+1]) );
				return &RAM[CURRENT_LOCATION+HEADER_SIZE+1];
			}			
		}else{
			if(VERBOSE)printf("Unable to allocate a memory location.\n\n");
		}
	}

}

void MyFree(void *POINTER){
	//check if the array is initialized for malloc;
	if (RAM[INIT_BLOCK] == 0 && !(RAM[INIT_BLOCK+1] == 'a' || RAM[INIT_BLOCK+1] == 'f' )){
		//uninited
		// initing
		int i;
		//printf("Initializing Memory\n");
		for(i=0;i<MEMORY_SIZE;i++){
			if(VERBOSE)printf("\rInitializing memory in progress %d%%", (i+1)*100/MEMORY_SIZE);
			fflush(stdout);
			RAM[i] = '\0';
		}
		//writing maiden header
		RAM[INIT_BLOCK] = 1;
		RAM[INIT_BLOCK+1] = 'f';
		*(int *)(&RAM[INIT_BLOCK+2]) = MEMORY_SIZE - 2 - HEADER_SIZE;
		if(VERBOSE)printf("\nInitialization completed. %d bytes available for use.\n",*(int *)(&RAM[INIT_BLOCK+2]));
	}else{
		//inited
		//config
		int CFG[2];
		CFG[0] = 0;
		CFG[1] = -1;
		//CFG[2] = -1;

		/*
		if (*(POINTER - HEADER_SIZE -1)=='a'){
			if (CFG[1]==0){
				*(POINTER - HEADER_SIZE -1)= '\0';


			}
			
			//*(int *)(&RAM[CFG[1] + 1]) += *(int *)(&RAM[CURRENT_LOCATION+1]);
			RAM[CFG[1]] = 'f';
			if(VERBOSE)printf("\t\tThe slot 0x%05X merged with 0x%05X\n",CURRENT_LOCATION,CFG[1] );
		}else{
			return;
		}*/
		
		int CURRENT_LOCATION = INIT_BLOCK + 1;
		if(VERBOSE)printf("Searching for mergable free blocks\n");
		while (CURRENT_LOCATION < MEMORY_SIZE){
			if(VERBOSE)printf("\tscanning block 0x%05X",CURRENT_LOCATION );
			if (RAM[CURRENT_LOCATION] == 'f'){
				//free slot
				if(VERBOSE)printf(" - Free block\n");
				if (CFG[0]==0){
					CFG[0] = 1;
					CFG[1] = CURRENT_LOCATION;
				}else if(CFG[0]==1){
					RAM[CURRENT_LOCATION] = '\0';
					*(int *)(&RAM[CFG[1] + 1]) += *(int *)(&RAM[CURRENT_LOCATION+1]);
					if(VERBOSE)printf("\t\tThe slot 0x%05X merged with 0x%05X\n",CURRENT_LOCATION,CFG[1] );
				}
			}else if(RAM[CURRENT_LOCATION] == 'a'){
				if ((void *)&RAM[CURRENT_LOCATION+HEADER_SIZE+1] == POINTER){
					if(VERBOSE)printf(" - Block scheduled for deletion\n");
					RAM[CURRENT_LOCATION] = 'f';
					continue;

				}else{
					if(VERBOSE)printf(" - Used block\n");
					CFG[0] = 0;
				}
			}else{
				//space wtf found
				if(VERBOSE)printf(" - WTF\n");
			}
			CURRENT_LOCATION += HEADER_SIZE + *(int *)(&RAM[CURRENT_LOCATION+1]) + 1;
		}
		if(VERBOSE)printf("Finished searching for mergable free blocks\n\n");

	}
}

void main(){
	char *lst[10];
	lst[0] = MyMalloc(10);
	lst[1] = MyMalloc(20);
	lst[2] = MyMalloc(30);
	lst[3] = MyMalloc(40);
	MyFree(lst[0]);
	MyFree(lst[1]);
	lst[4] = MyMalloc(300);
	
}