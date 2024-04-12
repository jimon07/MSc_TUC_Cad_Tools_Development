//Name: Ntounetas Dimitris
//AM: 2016030141
//OS: Windows 10 Pro
//Built with VS Code IDE 
//Date 20/05/2021
//##File works perfectly with no errors or warnings! If an error occurs please contact me!#################
//###########################################################################################################

//Library includes
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

//Lab5 Functions
void findGates(FILE* fpOut);
void checkNot(int currentTrans,int gateNum,FILE* fpOut);
void checkNand(int currentTrans,int gateNum,FILE* fpOut);
void checkNor(int currentTrans,int gateNum,FILE* fpOut);
void checkForUnusedTrans(FILE* fpOut);

//Functions used
void checkStim(int argc, char *argv[]);//checks if a stim file is entered
void getCircuitinfo(FILE* fpIn,FILE* fpOut);//gets the info from the stim file
void printNetlist(FILE* fpOut);

//Memory allocations for Circuit Data.
int VCC,GND,numOfInputs,numOfOutputs,netlistRows,netlistCols;
int* inputs;
int* outputs;
int **netlist;
int count,count1;
bool foundGate = false;

int main(int argc, char *argv[])
{   
    //Check if circuit file is present
    checkStim(argc, argv);
    //open circuit file
    FILE* fpIn = fopen(argv[1],"r");
    if (fpIn == NULL)
    {
        printf("Could not find file");
    }
    char file1[64];
    char dot = '.';
    
    strcpy(file1,strtok(argv[1],&dot));
    strcat(file1,"_out1.txt");

    FILE *fp1 = fopen(file1,"w");
    //get circuit file info
    getCircuitinfo(fpIn,fp1);

    return 0;
}
void showHelp()
{
    printf("\nAdd text file as argument to simulate Circuit\n");
    printf("\nCheck to give a correctly formated file!!\n");
}
void checkStim(int argc, char *argv[])
{   
    //Only one stim file at a time
    if (argc<2)
    {
        printf("Not enough arguments!\n");
        showHelp();
        exit(1);
    }
    else if (argc >2)
    {
        printf("You entered too many arguments must be the file name\n");
        showHelp();
        exit(1);
    }
}
void getCircuitinfo(FILE* fpIn,FILE* fpOut)
{
    char* line=NULL;
    size_t len=0;
    ssize_t read;

    char* tokenOut;
    char* tokenIn;
    char space = ' ';
    char comaSemiColon[] = "',',';'";
    //Read file line by line
    while((read = getline(&line, &len, fpIn)) != -1)
    {
        line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
        //Find VCC and GND values and save them
        if(strcmp(line, "## RAILS")==0)
        {
            if((read = getline(&line, &len, fpIn)) == -1)
            {
                printf("Problem");
                exit(1);
            }
                line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
                tokenOut = strtok(line, comaSemiColon);    
            while(tokenOut != NULL)
            {
                tokenIn = strdup(tokenOut);
                tokenOut = strtok(NULL, comaSemiColon);
                tokenIn = strtok(tokenIn, &space);
                if (strcmp(tokenIn, "VCC")==0)
                {
                    tokenIn = strtok(NULL, &space);
                    VCC = atoi(tokenIn);    
                }
                else if (strcmp(tokenIn, "GND")==0)
                {
                    tokenIn = strtok(NULL, &space);
                    GND = atoi(tokenIn);    
                } 
            }
        }
        //Find input values and save them
        if(strcmp(line, "## INPUTS")==0)
        {
            if ((read = getline(&line, &len, fpIn)) == -1)
            {
                printf("Problem");
                exit(1);
            }
                line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
                tokenIn = strtok(line, comaSemiColon);
                inputs = (int*)malloc(sizeof(int));
                inputs[0] = atoi(tokenIn); 
                count = 1;
                while (1)
                {
                    tokenIn = strtok(NULL,comaSemiColon);
                    if (tokenIn == NULL)
                    {
                        numOfInputs = count;
                        break;
                    }
                    count++; 
                    inputs = realloc(inputs,sizeof(int)*count);
                    inputs[count-1] = atoi(tokenIn);  
                }
        }
        //Find output values and save them
        if(strcmp(line, "## OUTPUTS")==0)
        {
            if ((read = getline(&line, &len, fpIn)) == -1)
            {
                printf("Problem");
                exit(1);
            }
                line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
                tokenIn = strtok(line, comaSemiColon);
                outputs = (int*)malloc(sizeof(int));
                outputs[0] = atoi(tokenIn); 
                count = 1;
                while (1)
                {
                    tokenIn = strtok(NULL,comaSemiColon);
                    if (tokenIn == NULL)
                    {
                        numOfOutputs = count;
                        break;
                    }
                    count++; 
                    outputs = realloc(outputs,sizeof(int)*count);
                    outputs[count-1] = atoi(tokenIn);     
                }
        }
        //Find netlist values and save them
        if(strcmp(line, "## NETLIST")==0)
        {   count1=0;
            while (1)
            {                
            if ((read = getline(&line, &len, fpIn)) == -1)
            {
                printf("Problem");
                exit(1);
            }
                line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
                if (strcmp(line, "## TESTBENCH")==0)
                {
                    break;
                }
                count=0;
                if (count1==0)
                {
                    netlist = (int**)malloc(sizeof(int));//allocate memory for double pointer row
                    netlist[count1] = (int*)calloc(6,sizeof(int));//allocate memory for array columns
                }
                else
                {
                    netlist = (int**)realloc(netlist,sizeof(int)*(count1+1));//allocate memory for double pointer row
                    netlist[count1] = (int*)calloc(6,sizeof(int));//allocate memory for array columns
                }
                tokenIn = strtok(line, &space);
                tokenIn = strtok(NULL, &space);
                if (strcmp(tokenIn, "PMOS")==0)
                {    
                    netlist[count1][count] = 0;    
                }
                else if (strcmp(tokenIn, "NMOS")==0)
                {
                    netlist[count1][count] = 1;  
                }
                else
                {
                    printf("error");
                    exit(1);
                }
                while (1)
                {
                    tokenIn = strtok(NULL,&space);
                    if (tokenIn == NULL)
                    {
                        netlistRows = count1+1;
                        netlistCols = count+1;
                        break;
                    }
                    count++; 
                    netlist[count1][count] = atoi(tokenIn);
                }
                count1++;
            }
        }
        //Find Testbench values save them and do test
        if(strcmp(line, "## TESTBENCH")==0)
        {   
            printNetlist(fpOut);
            findGates(fpOut);
            printf("END_SIMULATION");
            fprintf(fpOut,"END_SIMULATION");
            checkForUnusedTrans(fpOut);
        }
        if(strcmp(line, "## END_SIMULATION")==0)//For this to work right we have to add a blank row in out text file
        {                                        //Because we cannot compare last line with a string because of EOF.
            printf("\nSIMULATION ENDED\n");
            break;
        }
    }
}

void findGates(FILE* fpOut)
{
    int gateNum=1;
    for (int i = 0; i < netlistRows; i++)
    {   
        foundGate = false;
        if (netlist[i][4]!=0)
        {
            continue;
        }
        checkNot(i,gateNum,fpOut);
        checkNand(i,gateNum,fpOut);
        checkNor(i,gateNum,fpOut);
        if(foundGate)
        {
            gateNum++;
        }
    }   
}
void checkNot(int currentTrans,int gateNum,FILE* fpOut)
{
    for (int i = 0; i < netlistRows; i++)
    {   
        if (netlist[i][4]!=0)
        {
            continue;
        }
        if ((netlist[currentTrans][0]==0)&&(netlist[i][0]==1))//Check if current transistor is PMOS and i transistor is NMOS
        {
            if (netlist[currentTrans][1]==netlist[i][1])//Check if transistors gates are equal
            {   
                //Check if PMOS Transistor drain is equal to NMOS Transistor drain
                //And check if PMOS Source is VCC and NMOS Source is GND
                if ((netlist[currentTrans][3]==netlist[i][2])&&netlist[currentTrans][2]==VCC&&netlist[i][3]==GND)
                {                                                                                  
                    foundGate = true;
                    netlist[currentTrans][4]=gateNum;
                    netlist[i][4]=gateNum;
                    printf("G%d NOT ; IN %d ; OUT %d\n",gateNum,netlist[currentTrans][1],netlist[currentTrans][3]);
                    fprintf(fpOut,"G%d NOT ; IN %d ; OUT %d\n",gateNum,netlist[currentTrans][1],netlist[currentTrans][3]);
                    return;
                } 
            }  
        }
        else if ((netlist[currentTrans][0]==1)&&(netlist[i][0]==0))
        {
            if (netlist[currentTrans][1]==netlist[i][1])
            {
                if ((netlist[currentTrans][2]==netlist[i][3])&&netlist[currentTrans][3]==GND&&netlist[i][2]==VCC)
                {
                    //Found all transistors and the gates is complete
                    foundGate = true;
                    netlist[currentTrans][4]=gateNum;
                    netlist[i][4]=gateNum;
                    printf("G%d NOT ; IN %d ; OUT %d\n",gateNum,netlist[currentTrans][1],netlist[currentTrans][3]);
                    fprintf(fpOut,"G%d NOT ; IN %d ; OUT %d\n",gateNum,netlist[currentTrans][1],netlist[currentTrans][3]);
                    return;
                } 
            }  
        }  
    }
     
}
void checkNand(int currentTrans,int gateNum,FILE* fpOut)
{
    for (int i = 0; i < netlistRows; i++)
    {
        if (netlist[i][4]!=0||i==currentTrans)
        {
            continue;
        }
        //check if PMOS Transistors are in parallel and Sources are VCC
        if ((netlist[currentTrans][0]==0)&&(netlist[i][0]==0)&&(netlist[currentTrans][2]==VCC)&&(netlist[i][2]==VCC)&&(netlist[currentTrans][2]==netlist[i][2])&&(netlist[currentTrans][3]==netlist[i][3]))
        {
            //Find the NMOS transistor that is in series with the second PMOS Transistor
            for (int j = 0; j < netlistRows; j++)
            {
                if ((netlist[j][0]==1)&&(netlist[j][2]==netlist[i][3]))
                {
                    //Find the last NMOS Transistor in series with the previous NMOS Transistor with its source connected to GND
                    for (int k = 0; k < netlistRows; k++)
                    {
                        if ((netlist[k][0]==1)&&(netlist[k][2]==netlist[j][3])&&(netlist[k][3]==GND))
                        {
                            //Found all transistors and the gates is complete
                            foundGate = true;
                            netlist[currentTrans][4]=gateNum;
                            netlist[i][4]=gateNum;
                            netlist[j][4]=gateNum;
                            netlist[k][4]=gateNum;
                            printf("G%d NAND_2 ; IN %d,%d ; OUT %d\n",gateNum,netlist[currentTrans][1],netlist[i][1],netlist[j][2]);
                            fprintf(fpOut,"G%d NAND_2 ; IN %d,%d ; OUT %d\n",gateNum,netlist[currentTrans][1],netlist[i][1],netlist[j][2]);
                            return;
                        }  
                    } 
                } 
            } 
        }     
    }   
}
void checkNor(int currentTrans,int gateNum,FILE* fpOut)
{
    for (int i = 0; i < netlistRows; i++)
    {
        if (netlist[i][4]!=0||i==currentTrans)
        {
            continue;
        }
        //check if NMOS Transistors are in parallel and Sources are GND
        if ((netlist[currentTrans][0]==1)&&(netlist[i][0]==1)&&(netlist[currentTrans][3]==GND)&&(netlist[i][3]==GND)&&(netlist[currentTrans][2]==netlist[i][2])&&(netlist[currentTrans][3]==netlist[i][3]))
        {
            //Find the PMOS transistor that is in series with the second NMOS Transistor
            for (int j = 0; j < netlistRows; j++)
            {
                if (netlist[j][4]!=0)
                {
                    continue;
                }
                if ((netlist[j][0]==0)&&(netlist[j][3]==netlist[i][2]))
                {
                    //Find the last PMOS Transistor in series with the previous PMOS Transistor with its source connected to VCC
                    for (int k = 0; k < netlistRows; k++)
                    {
                        if (netlist[k][4]!=0)
                        {
                            continue;
                        }
                        if ((netlist[k][0]==0)&&(netlist[k][3]==netlist[j][2])&&(netlist[k][2]==VCC))
                        {
                            //Found all transistors and the gates is complete
                            foundGate = true;
                            netlist[currentTrans][4]=gateNum;
                            netlist[i][4]=gateNum;
                            netlist[j][4]=gateNum;
                            netlist[k][4]=gateNum;
                            printf("G%d NOR_2 ; IN %d,%d ; OUT %d\n",gateNum,netlist[currentTrans][1],netlist[i][1],netlist[j][3]);
                            fprintf(fpOut,"G%d NOR_2 ; IN %d,%d ; OUT %d\n",gateNum,netlist[currentTrans][1],netlist[i][1],netlist[j][3]);
                            return;
                        }  
                    } 
                } 
            } 
        }     
    } 
}

void printNetlist(FILE* fpOut)
{
    printf("## RAILS\n");
    fprintf(fpOut,"## RAILS\n");
    
    printf("VCC %d ; GND %d\n",VCC,GND);
    fprintf(fpOut,"VCC %d ; GND %d\n",VCC,GND);
    
    printf("## INPUTS\n");
    fprintf(fpOut,"## INPUTS\n");
    
    for (int i = 0; i < numOfInputs; i++)
    {   
        if (i==(numOfInputs-1))
        {
            printf("%d\n",inputs[i]);
            fprintf(fpOut,"%d\n",inputs[i]);
            
            break;
        }
        printf("%d,",inputs[i]);
        fprintf(fpOut,"%d,",inputs[i]);
    }
    printf("## OUTPUTS\n");
    fprintf(fpOut,"## OUTPUTS\n");
    for (int i = 0; i < numOfOutputs; i++)
    {   
        if (i==(numOfOutputs-1))
        {
            printf("%d\n",outputs[i]);
            fprintf(fpOut,"%d\n",outputs[i]);
            break;
        }
        printf("%d,",outputs[i]);
        fprintf(fpOut,"%d,",outputs[i]);
    }
}
void checkForUnusedTrans(FILE* fpOut)
{
    for (int i = 0; i < netlistRows; i++)
    {
        if (netlist[i][4]==0)
        {
            printf("\nTransistor U%d is not used in a NOT , NOR or NAND gate\n",i+1);
            fprintf(fpOut,"\nTransistor U%d is not used in a NOT , NOR or NAND gate\n",i+1);
        }
    } 
}