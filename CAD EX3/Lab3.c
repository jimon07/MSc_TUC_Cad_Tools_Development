//Name: Ntounetas Dimitris
//AM: 2016030141
//OS: Windows 10 Pro
//Built with VS Code IDE 
//Date 04/04/2021
//##File works perfectly with no errors or warnings! If an error occurs please contact me!#################
//###########################################################################################################

//Library includes
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

//Functions used
//Lab3 Functions
void createCMOSNetlist(FILE* fpIn,FILE* fpOut,FILE* fplib);
void createConnections(FILE* fpIn,FILE* fpOut,FILE *fplib);
void getGateNet(FILE* fplib,int* gateInputs,int numOfGateInputs,int* gateOutputs,int numOfGateOutputs);
void updateConnections(int* gateInputs,int numOfGateInputs,int* gateOutputs,int numOfGateOutputs);
void nodeCompaction();
void printNetlist(FILE* fpOut);

//Lab2 Functions
void checkStim(int argc, char *argv[]);//checks if a stim file is entered
void getCircuitinfo(FILE* fpIn,FILE* fpOut);//gets the info from the stim file
void showHelp();//Shows help message

//Memory allocations for Circuit Data.
int numOfInputs,numOfOutputs,netlistRows,netlistCols,numOfTestIn,numOfTestOut,Unum,numOfCMOS=0,numOfConnections;
int numOfVCC = 0, numOfGND = 0;
int* VCC;
int* GND;
int* inputs;
int* finalInputs;
int* finalOutput;
int* outputs;
int **netlist;
int **connections;
int * testIn;
int * testOut;
int count,count1;

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
    char* fileOut=NULL;
    char dot = '.';
    fileOut = strtok(argv[1],&dot);
    strcat(fileOut,"_out.txt");
    FILE *fpOut = fopen(fileOut,"w");
    //get circuit file info
    getCircuitinfo(fpIn,fpOut);
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
// Get the file info in order to save them
void getCircuitinfo(FILE* fpIn,FILE* fpOut)
{
    char* line=NULL;
    size_t len=0;
    ssize_t read;
    FILE* fplib = NULL;
    char* tokenOut;
    char* tokenIn;
    char space = ' ';
    char comaSemiColon[] = "',',';'";
    //Read file line by line
    while((read = getline(&line, &len, fpIn)) != -1)
    {
        line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
        if(strcmp(line, "## LIBRARY")==0)
        {
            if ((read = getline(&line, &len, fpIn)) == -1)
            {
                printf("Problem");
                fprintf(fpOut,"Problem");
                exit(1);
            }
                line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
                fplib = fopen(line,"r");
                if (fplib == NULL)
                {
                    printf("Could not find file");
                }
        }
        //Find VCC and GND values and save them
        if(strcmp(line, "## RAILS")==0)
        {
            if((read = getline(&line, &len, fpIn)) == -1)
            {
                printf("Problem");
                fprintf(fpOut,"Problem");
                exit(1);
            }
            line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
            tokenOut = strtok(line, comaSemiColon);    
            while(tokenOut != NULL)
            {
                tokenIn = strdup(tokenOut);
                tokenOut = strtok(NULL, comaSemiColon);
                tokenIn = strtok(tokenIn, &space);
                //if value = VCC save it
                if (strcmp(tokenIn, "VCC")==0)
                {   
                    numOfVCC++;
                    tokenIn = strtok(NULL, &space);
                    if (numOfVCC == 1)
                    {
                        VCC = (int*)malloc(sizeof(int)*numOfVCC);
                        VCC[numOfVCC-1] = atoi(tokenIn); 
                    }
                    else
                    {
                        VCC = (int*)realloc(VCC,sizeof(int)*numOfVCC);
                        VCC[numOfVCC-1] = atoi(tokenIn); 
                    }

                }
                //if value = GND save it
                else if (strcmp(tokenIn, "GND")==0)
                {
                    numOfGND++;
                    tokenIn = strtok(NULL, &space);
                    if (numOfGND == 1)
                    {
                        GND = (int*)malloc(sizeof(int)*numOfGND);
                        GND[numOfGND-1] = atoi(tokenIn); 
                    }
                    else
                    {
                        GND = (int*)realloc(VCC,sizeof(int)*numOfGND);
                        GND[numOfGND-1] = atoi(tokenIn); 
                    }    
                } 
            }
        }
        //Find input values and save them
        if(strcmp(line, "## INPUTS")==0)
        {
            if ((read = getline(&line, &len, fpIn)) == -1)
            {
                printf("Problem");
                fprintf(fpOut,"Problem");
                exit(1);
            }
                line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
                tokenIn = strtok(line, comaSemiColon);
                while (tokenIn != NULL)
                {
                    //Save the values in 2 identical arrays
                    numOfInputs++;
                    if (numOfInputs == 1)
                    {
                        inputs = (int*)malloc(sizeof(int)*numOfInputs);
                        inputs[numOfInputs-1] = atoi(tokenIn); 
                        finalInputs = (int*)malloc(sizeof(int)*numOfInputs);
                        finalInputs[numOfInputs-1] = atoi(tokenIn);  
                    }
                    else
                    {
                        inputs = (int*)realloc(inputs,sizeof(int)*numOfInputs);
                        inputs[numOfInputs-1] = atoi(tokenIn); 
                        finalInputs = (int*)realloc(finalInputs,sizeof(int)*numOfInputs);
                        finalInputs[numOfInputs-1] = atoi(tokenIn); 
                    }
                    tokenIn = strtok(NULL, comaSemiColon);

                }
        }
        //Find output values and save them
        if(strcmp(line, "## OUTPUTS")==0)
        {
            if ((read = getline(&line, &len, fpIn)) == -1)
            {
                printf("Problem");
                fprintf(fpOut,"Problem");
                exit(1);
            }
                line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
                tokenIn = strtok(line, comaSemiColon);
                while (tokenIn != NULL)
                {
                    //Save the values in 2 identical arrays
                    numOfOutputs++;
                    if (numOfOutputs == 1)
                    {
                        outputs = (int*)malloc(sizeof(int)*numOfOutputs);
                        outputs[numOfOutputs-1] = atoi(tokenIn);
                        finalOutput = (int*)malloc(sizeof(int)*numOfOutputs);
                        finalOutput[numOfOutputs-1] = atoi(tokenIn);  
                    }
                    else
                    {
                        outputs = (int*)realloc(outputs,sizeof(int)*numOfOutputs);
                        outputs[numOfOutputs-1] = atoi(tokenIn); 
                        finalOutput = (int*)realloc(finalOutput,sizeof(int)*numOfOutputs);
                        finalOutput[numOfOutputs-1] = atoi(tokenIn); 
                    }
                    tokenIn = strtok(NULL, comaSemiColon);

                }
        }
        //Find netlist values and save them
        if(strcmp(line, "## NETLIST")==0)
        {   
            createCMOSNetlist(fpIn,fpOut,fplib); 
        }
        //Find test in values and save them
        if (strcmp(line, "## TEST_IN")==0)
        {
            if ((read = getline(&line, &len, fpIn)) == -1)
            {
                printf("Problem");
                fprintf(fpOut,"Problem");
                exit(1);
            }
                line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
                tokenIn = strtok(line, comaSemiColon);
                testIn = (int*)malloc(sizeof(int));
                testIn[0] = atoi(tokenIn);
                count = 1;
                while (1)
                {
                    //works for all the test in values
                    tokenIn = strtok(NULL,comaSemiColon);
                    if (tokenIn == NULL)
                    {
                        numOfTestIn = count;
                        break;
                    }
                    count++; 
                    testIn = realloc(testIn,sizeof(int)*count);
                    testIn[count-1] = atoi(tokenIn);
                }
            printf("## TEST_IN\n");
            fprintf(fpOut,"## TEST_IN\n");
            for (int i = 0; i < numOfTestIn; i++)
            {   
                //find if test in a same as input in order to give the right test in of the output file
                bool printed =false;
                for (int j = 0; j < numOfInputs; j++)
                {
                    if (testIn[i]==inputs[j])
                    {   
                        printed = true;
                        printf("%d;",finalInputs[j]);
                        fprintf(fpOut,"%d;",finalInputs[j]);
                    } 
                }
                if (!printed)
                {
                    printf("%d;",testIn[i]);
                    fprintf(fpOut,"%d;",testIn[i]);
                }   
            }
            printf("\n");
            fprintf(fpOut,"\n"); 
        }
        //Find test out values and save them
        if (strcmp(line, "## TEST_OUT")==0)
        {
            if ((read = getline(&line, &len, fpIn)) == -1)
            {
                printf("Problem");
                fprintf(fpOut,"Problem");
                exit(1);
            }
                line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
                tokenIn = strtok(line, comaSemiColon); 
                testOut = (int*)malloc(sizeof(int));
                testOut[0] = atoi(tokenIn);
                count = 1;
                //works for all the test out values
                while (1)
                {
                    tokenIn = strtok(NULL,comaSemiColon);
                    if (tokenIn == NULL)
                    {
                        numOfTestOut = count;
                        break;
                    }
                    count++; 
                    testOut = realloc(testOut,sizeof(int)*count);
                    testOut[count-1] = atoi(tokenIn);
                }
            printf("## TEST_OUT\n");
            fprintf(fpOut,"## TEST_OUT\n");
            for (int i = 0; i < numOfTestOut; i++)
            {   
                //find if test out a same as output in order to give the right test out of the output file
                bool printed =false;
                for (int j = 0; j < numOfOutputs; j++)
                {
                    if (testOut[i]==outputs[j])
                    {   
                        printed = true;
                        printf("%d;",finalOutput[j]);
                        fprintf(fpOut,"%d;",finalOutput[j]);
                    } 
                }
                if (!printed)
                {
                    printf("%d;",testOut[i]);
                    fprintf(fpOut,"%d;",testOut[i]);
                }   
            }
            printf("\n"); 
            fprintf(fpOut,"\n");
        }
        //Find test vectors values and save them
        if (strcmp(line, "## TEST_VECTORS")==0)
        {
            printf("%s\n",line);
            fprintf(fpOut,"%s\n",line); 
            read = getline(&line, &len, fpIn);
            printf("%s",line);
            fprintf(fpOut,"%s",line);
            read = getline(&line, &len, fpIn);
            printf("%s",line);
            fprintf(fpOut,"%s",line);   
        }
        //Check if test ended succesfully
        if (strcmp(line, "## END_TEST")==0)
        {   
            printf("%s\n",line);
            fprintf(fpOut,"%s\n",line);
        }
        //Check if test ended succesfully
        if (strcmp(line, "## END_SIMULATION")==0)
        {   
            printf("%s\n",line);
            fprintf(fpOut,"%s\n",line);
        }
        if (strcmp(line, "## TESTBENCH")==0)
        {
            printf("%s\n",line);
            fprintf(fpOut,"%s\n",line);
        }
    }
}
void createCMOSNetlist(FILE* fpIn,FILE* fpOut,FILE* fplib)
{   
    char* tokenIn;
    char* line = NULL;
    char* in;
    char* out;
    size_t len=0;
    ssize_t read;
    char delim[] = "';',',',' '";
    char val[5];
    count1=0;
    int numOfGateInputs = 0,numOfGateOutputs=0;
    int* gateInputs;
    int* gateOutputs;
            while (1)
            {                
            if ((read = getline(&line, &len, fpIn)) == -1)
            {
                printf("Problem");
                fprintf(fpOut,"Problem");
                exit(1);
            }
                line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
                if (strcmp(line, "")==0)
                {
                    continue;
                }
                //If line is ## TESTBENCH STOP the process
                if (strcmp(line, "## TESTBENCH")==0)
                {   
                    nodeCompaction();
                    printNetlist(fpOut);
                    printf("%s\n",line);
                    fprintf(fpOut,"%s\n",line);
                    break;  
                }
                tokenIn = strtok(line,delim);
                // Find gate
                if (tokenIn[0] == 'G')
                {   
                    Unum++;
                    tokenIn = strtok(NULL, delim);
                    char gatestr[100];
                    fseek(fplib, 0, SEEK_SET);
                    sprintf(gatestr , "## GATE %s\0",tokenIn);
                    numOfGateInputs=0;
                    numOfGateOutputs=0;
                    strtok(NULL,delim);
                    while ((in = strtok(NULL,delim))!=NULL)
                    {
                        if (strcmp(in,"OUT")==0)
                        {
                            break;
                        }
                        numOfGateInputs++;
                        if (numOfGateInputs == 1)
                        {
                            gateInputs = (int*)malloc(sizeof(int)*numOfGateInputs);
                            gateInputs[ numOfGateInputs-1]=atoi(in);
                        }
                        else
                        {
                            gateInputs = (int*)realloc(gateInputs,sizeof(int)*numOfGateInputs);
                            gateInputs[numOfGateInputs-1]=atoi(in);
                        } 
                    }
                    while ((out=strtok(NULL,delim))!=NULL)
                    {
                        numOfGateOutputs++;
                        if(numOfGateOutputs==1)
                        {
                            gateOutputs = (int*)malloc(sizeof(int)*numOfGateOutputs);
                            gateOutputs[numOfGateOutputs-1]=atoi(out);
                        }
                        else
                        {
                            gateOutputs = (int*)realloc(gateOutputs,sizeof(int)*numOfGateOutputs);
                            gateOutputs[numOfGateOutputs-1]=atoi(out);
                        }
                    }
                        updateConnections(gateInputs,numOfGateInputs,gateOutputs,numOfGateOutputs);
                    while((read = getline(&line, &len, fplib)) != -1)
                    {   
                        line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
                        if(strcmp(line, gatestr)==0)
                        {
                            getGateNet(fplib,gateInputs,numOfGateInputs,gateOutputs,numOfGateOutputs);
                            break;
                        }
                        else if (strcmp(line, "## END LIBRARY")==0)
                        {
                            printf("Could not find gate");
                            fprintf(fpOut,"Could not find gate");
                            exit(1);
                        }
                    }
                }
                // Find CMOS
                if (tokenIn[0] == 'U')
                {   
                    Unum++;
                    numOfCMOS++;
                    if (numOfCMOS==1)
                    {
                        netlist = (int**)malloc(sizeof(int));//allocate memory for double pointer row
                        netlist[0] = (int*)malloc(sizeof(int)*4);//allocate memory for array columns
                    }
                    else
                    {
                        netlist = (int**)realloc(netlist,sizeof(int)*numOfCMOS);//allocate memory for double pointer row
                        netlist[numOfCMOS-1] = (int*)malloc(sizeof(int)*4);//allocate memory for array columns
                    }
                    tokenIn = strtok(NULL, delim);
                    if (strcmp(tokenIn, "PMOS")==0)
                    {    
                        netlist[numOfCMOS-1][count] = 0;    
                    }
                    else if (strcmp(tokenIn, "NMOS")==0)
                    {
                        netlist[numOfCMOS-1][count] = 1;  
                    }
                    else
                    {
                        printf("error");
                        exit(1);
                    }
                    while (1)
                    {
                        tokenIn = strtok(NULL,delim);
                        if (tokenIn == NULL)
                        {
                            netlistRows = numOfCMOS;
                            netlistCols = count+1;
                            break;
                        }
                        count++;
                        sprintf(val,"%d%s",Unum,tokenIn);
                        bool inputExists=false;
                        int row =0;
                        for (row = 0; row < numOfConnections; row++)
                        {
                             if (connections[row][0]==atoi(tokenIn))
                             {
                                inputExists = true;
                                break;
                             }    
                        } 
                        if (!inputExists)
                        {   numOfConnections++;
                            if (numOfConnections == 1)
                            {
                                connections = (int**)malloc(sizeof(int));
                                connections[0]= calloc(20,sizeof(int));
                            }
                            else
                            {
                                connections = (int**)realloc(connections,sizeof(int)*numOfConnections);
                                connections[numOfConnections-1]= calloc(20,sizeof(int));
                            }
                            connections[row][0]=atoi(tokenIn);
                            connections[row][1]=atoi(val);
                        }
                        else
                        {
                            int col =0;
                            while (connections[row][col]!=0)
                            {
                                col++;
                            }
                            connections[row][col]=atoi(val);
                        }
                        for (int i = 0; i < numOfInputs; i++)
                        {
                            if (inputs[i]==atoi(tokenIn))
                            {
                                finalInputs[i]=atoi(val);
                            }
                        }
                        for (int i = 0; i < numOfOutputs; i++)
                        {
                            if (outputs[i]==atoi(tokenIn))
                            {
                                finalOutput[i]=atoi(val);
                            }
                        }
                        netlist[numOfCMOS-1][count] = atoi(val);
                    }
                }  
            }
}
//create connections arrray with nodes and their connections
void updateConnections(int* gateInputs,int numOfGateInputs,int* gateOutputs,int numOfGateOutputs)
{   
    bool inputExists = false;
    
    int row=0;
    for (int i = 0; i < numOfGateInputs; i++)
    {   
        inputExists=false;
        for (row = 0; row < numOfConnections; row++)
        {
             if (connections[row][0]==gateInputs[i])
             {
                inputExists = true;
                break;
             }    
        }
        if (!inputExists)
        {   numOfConnections++;
            if (numOfConnections == 1)
            {
                connections = (int**)malloc(sizeof(int));
                connections[0]= calloc(20,sizeof(int));
            }
            else
            {
                connections = (int**)realloc(connections,sizeof(int)*numOfConnections);
                connections[numOfConnections-1]= calloc(20,sizeof(int));
            }
            connections[row][0]=gateInputs[i];
        }
    }
    row=0;
    for (int i = 0; i < numOfGateOutputs; i++)
    {   
        inputExists=false;
        for (row = 0; row < numOfConnections; row++)
        {
             if (connections[row][0]==gateOutputs[i])
             {
                inputExists = true;
                break;
             }    
        }
        if (!inputExists)
        {   numOfConnections++;
            if (numOfConnections == 1)
            {
                connections = (int**)malloc(sizeof(int));
                connections[0]= calloc(20,sizeof(int));
            }
            else
            {
                connections = (int**)realloc(connections,sizeof(int)*numOfConnections);
                connections[numOfConnections-1]= calloc(20,sizeof(int));
            }
            connections[row][0]=gateOutputs[i];
        }
    }
}

void getGateNet(FILE* fplib,int* gateInputs,int numOfGateInputs,int* gateOutputs,int numOfGateOutputs)
{
    char* tokenIn;
    char* tokenOut;
    char val[5];
    char* line = NULL;
    size_t len=0;
    ssize_t read;
    char space = ' ';
    char comaSemiColon[] = "',',';'";
    char delim[] = "';',',',' '";
    count1=0;
    bool gateDone = false;
    int numOfIn = 0;
    int numOfOut = 0;

    while(((read = getline(&line, &len, fplib)) != -1) && (gateDone==false)) 
    {
        line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
        //Find VCC and GND values and save them
        if(strcmp(line, "## RAILS")==0)
        {
            if((read = getline(&line, &len, fplib)) == -1)
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
                    numOfVCC++;
                    tokenIn = strtok(NULL, &space);
                    sprintf(val,"%d%s",Unum,tokenIn);
                    if (numOfVCC == 1)
                    {
                        VCC = (int*)malloc(sizeof(int)*numOfVCC);
                        VCC[numOfVCC-1] = atoi(val); 
                    }
                    else
                    {
                        VCC = (int*)realloc(VCC,sizeof(int)*numOfVCC);
                        VCC[numOfVCC-1] = atoi(val); 
                    }

                }
                else if (strcmp(tokenIn, "GND")==0)
                {
                    numOfGND++;
                    tokenIn = strtok(NULL, &space);
                    sprintf(val,"%d%s",Unum,tokenIn);
                    if (numOfGND == 1)
                    {
                        GND = (int*)malloc(sizeof(int)*numOfGND);
                        GND[numOfGND-1] = atoi(val); 
                    }
                    else
                    {
                        GND = (int*)realloc(GND,sizeof(int)*numOfGND);
                        GND[numOfGND-1] = atoi(val); 
                    }    
                } 
            }
        }
        //Find input values and save them
        if(strcmp(line, "## INPUTS")==0)
        {
            if ((read = getline(&line, &len, fplib)) == -1)
            {
                printf("Problem");
                exit(1);
            }
                line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
                tokenIn = strtok(line, comaSemiColon);
            while (tokenIn != NULL)
            {
                numOfIn++;
                bool inputExists = false;
                int row=0;
                for (row = 0; row < numOfConnections; row++)
                    {
                         if (connections[row][0]==gateInputs[numOfIn-1])
                         {
                            inputExists = true;
                            break;
                         }    
                    }
                    int col =0;
                    while (connections[row][col]!=0)
                    {
                        col++;
                    } 

                if (inputExists)
                {   
                    sprintf(val,"%d%s",Unum,tokenIn);
                    connections[row][col]=atoi(val);
                }
                for (int i = 0; i < numOfInputs; i++)
                    {
                         if (finalInputs[i]==gateInputs[numOfIn-1])
                         {
                            finalInputs[i] = atoi(val);
                            break;
                         }    
                    }
                tokenIn = strtok(NULL,delim);
            }
        }
        //Find output values and save them
        if(strcmp(line, "## OUTPUTS")==0)
        {
            if ((read = getline(&line, &len, fplib)) == -1)
            {
                printf("Problem");
                // fprintf(fpOut,"Problem");
                exit(1);
            }
                line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
                tokenIn = strtok(line, comaSemiColon);
            while (tokenIn != NULL)
            {
                numOfOut++;
                bool inputExists = false;
                int row=0;
                for (row = 0; row < numOfConnections; row++)
                    {
                         if (connections[row][0]==gateOutputs[numOfOut-1])
                         {
                            inputExists = true;
                            break;
                         }    
                    }
                    int col =0;
                    while (connections[row][col]!=0)
                    {
                        col++;
                    } 

                if (inputExists)
                {   
                    sprintf(val,"%d%s",Unum,tokenIn);
                    connections[row][col]=atoi(val);
                }
                for (int i = 0; i < numOfInputs; i++)
                    {
                         if (finalOutput[i]==gateOutputs[numOfOut-1])
                         {
                            finalOutput[i] = atoi(val);
                            break;
                         }    
                    }
                tokenIn = strtok(NULL,delim);
            }
        }
        
        if(strcmp(line, "## NETLIST")==0)
        {
            while (1)
            {   
                count=0;             
            if ((read = getline(&line, &len, fplib)) == -1)
            {
                printf("Problem");
                exit(1);
            }
                line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
                if (strcmp(line, "## END_GATE")==0)
                {   
                    gateDone = true;
                    break;
                }
                numOfCMOS++;
                if (numOfCMOS==1)
                {
                    netlist = (int**)malloc(sizeof(int));//allocate memory for double pointer row
                    netlist[0] = (int*)malloc(sizeof(int)*4);//allocate memory for array columns
                }
                else
                {
                    netlist = (int**)realloc(netlist,sizeof(int)*(numOfCMOS));//allocate memory for double pointer row
                    netlist[numOfCMOS-1] = (int*)malloc(sizeof(int)*4);//allocate memory for array columns
                }
                tokenIn = strtok(line, &space);
                tokenIn = strtok(NULL, &space);
                if (strcmp(tokenIn, "PMOS")==0)
                {    
                    netlist[numOfCMOS-1][count] = 0;    
                }
                else if (strcmp(tokenIn, "NMOS")==0)
                {
                    netlist[numOfCMOS-1][count] = 1;  
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
                        netlistRows = numOfCMOS;
                        netlistCols = count+1;
                        break;
                    }
                    count++;
                    sprintf(val,"%d%s",Unum,tokenIn);
                    netlist[numOfCMOS-1][count] = atoi(val);
                }
            }
        } 
    }
}
void nodeCompaction()
{
    //VCC Compaction Removes all the nodes flaged as VCC and updates them with the first
    for (int i = 1; i < numOfVCC; i++)
    {
        for (int j = 0; j < netlistRows; j++)
        {
            for (int k = 1; k < netlistCols; k++)
            {
                if (netlist[j][k]==VCC[i])
                {
                    netlist[j][k]=VCC[0];
                }  
            } 
        }  
    }
    //GND Compaction Removes all the nodes flaged as GND and updates them with the first
    for (int i = 1; i < numOfGND; i++)
    {
        for (int j = 0; j < netlistRows; j++)
        {
            for (int k = 1; k < netlistCols; k++)
            {
                if (netlist[j][k]==GND[i])
                {
                    netlist[j][k]=GND[0];
                }
            } 
        }
    }
    //Connections Compaction Removes all the nodes flaged as a connection and updates them with the first
    for (int i = 0; i < numOfConnections; i++)
    {   
        int j=1;
        while (connections[i][j]!=0)
        {
            for (int k = 0; k < netlistRows; k++)
            {
                for (int l = 1; l < netlistCols; l++)
                {
                    if (netlist[k][l]==connections[i][j])
                    {
                    netlist[k][l]=connections[i][1];
                    }
                }
            }
            j++;
        }   
    }  
}
void printNetlist(FILE* fpOut)
{
    printf("## RAILS\n");
    fprintf(fpOut,"## RAILS\n");
    printf("VCC %d ; GND %d\n",VCC[0],GND[0]);
    fprintf(fpOut,"VCC %d ; GND %d\n",VCC[0],GND[0]);
    printf("## INPUTS\n");
    fprintf(fpOut,"## INPUTS\n");
    for (int i = 0; i < numOfInputs; i++)
    {   
        if (i==(numOfInputs-1))
        {
            printf("%d\n",finalInputs[i]);
            fprintf(fpOut,"%d\n",finalInputs[i]);
            break;
        }
        printf("%d,",finalInputs[i]);
        fprintf(fpOut,"%d,",finalInputs[i]);
    }
    printf("## OUTPUTS\n");
    fprintf(fpOut,"## OUTPUTS\n");
    for (int i = 0; i < numOfOutputs; i++)
    {   
        if (i==(numOfOutputs-1))
        {
            printf("%d\n",finalOutput[i]);
            fprintf(fpOut,"%d\n",finalOutput[i]);
            break;
        }
        printf("%d,",finalOutput[i]);
        fprintf(fpOut,"%d,",finalOutput[i]);
    }
    printf("## NETLIST\n");
    fprintf(fpOut,"## NETLIST\n");
    int num=0;
    for (int i = 0; i < netlistRows; i++)
    {   
        num++;
        if (netlist[i][0]==0)
        {
            printf("U%d PMOS ",num);
            fprintf(fpOut,"U%d PMOS ",num);
        }
        else
        {
            printf("U%d NMOS ",num);
            fprintf(fpOut,"U%d NMOS ",num);
        }
        for (int j = 1; j < netlistCols; j++)
        {
            printf("%d ",netlist[i][j]);
            fprintf(fpOut,"%d ",netlist[i][j]);
        }
        printf("\n");
        fprintf(fpOut,"\n");
    }
}