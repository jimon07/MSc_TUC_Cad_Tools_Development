//Name: Ntounetas Dimitris
//AM: 2016030141
//OS: Windows 10 Pro
//Built with VS Code IDE 
//Date 20/04/2021
//##File works perfectly with no errors or warnings! If an error occurs please contact me!#################
//###########################################################################################################

//Library includes
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

//Lab4 Functions
void findPossibleSplitPoints();
void verifySplitPoints();
void splitFile();
void createInputsOutputs();
void crete2Files(FILE* fp1,FILE* fp2);

//Functions used
void checkStim(int argc, char *argv[]);//checks if a stim file is entered
void getCircuitinfo(FILE* fpIn);//gets the info from the stim file

//Memory allocations for Circuit Data.
int VCC,GND,numOfInputs,numOfOutputs,netlistRows,netlistCols,numOfTestIn,numOfTestOut,numOfSimulations,correctSims,numOfInputsFirst,numOfInputsSecond,numOfOutputsFirst,numOfOutputsSecond;
int numOfNodes = 0;
int splitingPoint;
int* inputs;
int* outputs;
int* inputsFirst;
int* inputsSecond;
int* outputsFirst;
int* outputsSecond;
int **netlist;
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
    char file1[64];
    char file2[64];
    char dot = '.';
    // strcat(fileOut,argv[1]);
    strcpy(file1,strtok(argv[1],&dot));
    strcpy(file2,strtok(argv[1],&dot));
    strcat(file1,"_out1.txt");
    strcat(file2,"_out2.txt");

    FILE *fp1 = fopen(file1,"w");
    FILE *fp2 = fopen(file2,"w");
    //get circuit file info
    getCircuitinfo(fpIn);

    crete2Files(fp1,fp2);
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
void getCircuitinfo(FILE* fpIn)
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
            numOfSimulations=0;
            correctSims=0;
            // doTestBench(fpIn,fpOut);
            findPossibleSplitPoints();
            verifySplitPoints();
            splitFile();
            createInputsOutputs();
        }
        if(strcmp(line, "## END_SIMULATION")==0)//For this to work right we have to add a blank row in out text file
                                                //Because we cannot compare last line with a string because of EOF.
        {
            printf("\nSIMULATION ENDED\n");
            break;
        }
    }
}
void findPossibleSplitPoints()
{
    // Find nodes that their gates are not source or drain above 
    for (int i = 1; i < netlistRows; i++)
    {
        for (int j = 0; j < i; j++)
        {
            if ((netlist[i][1]==netlist[j][2])||(netlist[i][1]==netlist[j][3]))
            {
                netlist[i][4]=1;
            }
        }  
    } 
    printf("\nNETLIST IS : (where 0 in first column = PMOS and 1 = NMOS)\n");
    for (int i = 0; i <  netlistRows; i++)
    {   
        for (int j = 0; j < netlistCols+1; j++)
        {
            printf("%d ",netlist[i][j]);
        }
        printf("\n");
    }
}
void verifySplitPoints()
{   
    for (int i = 1; i < netlistRows-1; i++)
    {
        bool foundBefore = false;
        bool foundAfter = false;
        if (netlist[i][4]==1)
        {
            //Check if gate node is present in a Source or Drain Before current node row
            for (int j = 0; j < i; j++)
            {   
                if (((netlist[j][2]==netlist[i][2])||(netlist[j][3]==netlist[i][2]))&&((netlist[i][2]!=VCC)&&(netlist[i][2]!=GND)))
                {
                    foundBefore = true;
                    break;
                }
                if (((netlist[j][2]==netlist[i][3])||(netlist[j][3]==netlist[i][3]))&&((netlist[i][3]!=VCC)&&(netlist[i][3]!=GND)))
                {
                    foundBefore = true;
                    break;
                }
            }
            //Check if gate node is present in a Source or Drain After current node row
            for (int j = i+1; j < netlistRows; j++)
            {
                if (((netlist[j][2]==netlist[i][2])||(netlist[j][3]==netlist[i][2]))&&((netlist[i][2]!=VCC)&&(netlist[i][2]!=GND)))
                {
                    foundAfter = true;
                    break;
                }
                if (((netlist[j][2]==netlist[i][3])||(netlist[j][3]==netlist[i][3]))&&((netlist[i][3]!=VCC)&&(netlist[i][3]!=GND)))
                {
                    foundAfter = true;
                    break;
                }
            }            
            if (!foundBefore && foundAfter)
            {
                netlist[i][5] = 1;
            }
            else if (!foundAfter && foundBefore)
            {
                netlist[i][5] = -1;
            }
            else
            {
                netlist[i][4] = 0;
            } 
        }     
    }
    printf("\nNETLIST IS : (where 0 in first column = PMOS and 1 = NMOS)\n");
    for (int i = 0; i <  netlistRows; i++)
    {   
        for (int j = 0; j < netlistCols+2; j++)
        {
            printf("%d ",netlist[i][j]);
        }
        printf("\n");
    }
}
void splitFile()
{   
    int halfPoint = 0, splitingPointOffset1=0, splitingPointOffset2=0;
    printf("Number of Transistors %d\n",netlistRows);
    // We want to cut the file about in the middle so we start from there.
    halfPoint = netlistRows/2;
    // Check if the halfpoint is a spliting point.
    if (netlist[halfPoint][4]==1&&netlist[halfPoint][5]==-1)
    {
        splitingPoint = halfPoint;
    }
    else if (netlist[halfPoint][4]==1&&netlist[halfPoint][5]==1)
    {
        splitingPoint = halfPoint-1;
    }
    // Move the spliting point up and down and find the poitn of the minimum distance
    else
    {
        for (int i = halfPoint; i >= 0 ; i--)
        {
            if (netlist[i][4]==1&&netlist[i][5]==-1)
            {
                break;
            }
            if (netlist[i][4]==1&&netlist[i][5]==1)
            {
                splitingPointOffset1--;
                break;
            }
            
            splitingPointOffset1--;
        }

        for (int j = halfPoint; j < netlistRows; j++)
        {
            if (netlist[j][4]==1&&netlist[j][5]==1)
            {   
                splitingPointOffset2--;
                break;
            }
            if (netlist[j][4]==1&&netlist[j][5]==-1)
            {
                break;
            }
            splitingPointOffset2++;
        }
        // Find the nearest spliting point
        if (abs(splitingPointOffset1)<=abs(splitingPointOffset2))
        {
            splitingPoint = halfPoint + splitingPointOffset1;
        }
        else
        {
            splitingPoint = halfPoint + splitingPointOffset2;
        }

        printf("\nSplitting point = %d",splitingPoint);  
    }
    // Print Splited netlist
    printf("\nNETLIST IS : (where 0 in first column = PMOS and 1 = NMOS)\n");
    for (int i = 0; i <  netlistRows; i++)
    {   
        
        for (int j = 0; j < netlistCols+2; j++)
        {
            printf("%d ",netlist[i][j]);
        }
        if (i==splitingPoint)
        {
            printf("\n");
        }
        printf("\n");
    }
}
void createInputsOutputs()
{
    // Find first's files inputs
    // Find for every gate that its node doesnt't exist as a source or drain in the first part
    for (int i = 0; i < splitingPoint; i++)
    {
        bool nodeExists = false;
        for (int j = 0; j < splitingPoint; j++)
        {
            if ((netlist[i][1]==netlist[j][2])||(netlist[i][1]==netlist[j][3]))
            {
                nodeExists = true;
                break;
            }
        }
        // Dont add duplicates
        if (!nodeExists)
        {   
            //check for duplicates
            bool duplicate = false;
            for (int k = 0; k < numOfInputsFirst; k++)
            {   
                duplicate = false;
                if (netlist[i][1]==inputsFirst[k])
                {
                    duplicate = true;
                    break;
                }
            }
            // If not a duplicate then add it
            if (!duplicate)
            {
                numOfInputsFirst++;
                if (numOfInputsFirst==1)
                {
                    inputsFirst = (int*)malloc((sizeof(int)*numOfInputsFirst)); //Allocate memory
                    inputsFirst[numOfInputsFirst-1]=netlist[i][1];
                }
                else
                {
                    inputsFirst = (int*)realloc(inputsFirst,(sizeof(int)*numOfInputsFirst)); //Reallocate memory
                    inputsFirst[numOfInputsFirst-1]=netlist[i][1];
                }
            } 
        }
    }
    
    // Find seconds's files inputs
    // Find for every gate that node exists as a source or drain in the first part
    for (int i = splitingPoint+1; i < netlistRows; i++)
    {
        bool nodeExists = false;
        for (int j = splitingPoint+1; j < netlistRows; j++)
        {
            if ((netlist[i][1]==netlist[j][2])||(netlist[i][1]==netlist[j][3]))
            {
                nodeExists = true;
                break;
            }
        }
        // Dont add duplicates
        if (!nodeExists)
        {   
            //check for duplicates
            bool duplicate = false;
            for (int k = 0; k < numOfInputsSecond; k++)
            {   
                duplicate = false;
                if (netlist[i][1]==inputsSecond[k])
                {
                    duplicate = true;
                    break;
                }
            }
            // If not a duplicate then add it
            if (!duplicate)
            {
                numOfInputsSecond++;
                if (numOfInputsSecond==1)
                {
                    inputsSecond = (int*)malloc((sizeof(int)*numOfInputsSecond)); //Allocate memory
                    inputsSecond[numOfInputsSecond-1]=netlist[i][1];
                }
                else
                {
                    inputsSecond = (int*)realloc(inputsSecond,(sizeof(int)*numOfInputsSecond)); //Reallocate memory
                    inputsSecond[numOfInputsSecond-1]=netlist[i][1];
                }
            } 
        }
    }
    //Find First's Outputs
    for (int i = 0; i < splitingPoint; i++)
    {
        for (int j = 0; j < numOfOutputs; j++)
        {
            if ((netlist[i][2]==outputs[j]) || (netlist[i][3]==outputs[j]))
            {
                //check for duplicates
                bool duplicate = false;
                for (int k = 0; k < numOfOutputsFirst; k++)
                {   
                    duplicate = false;
                    if (outputs[j]==outputsFirst[k])
                    {
                        duplicate = true;
                    break;
                    }
                }
                // If not a duplicate then add it
                if (!duplicate)
                {
                    numOfOutputsFirst++;
                    if (numOfOutputsFirst==1)
                    {
                        outputsFirst = (int*)malloc((sizeof(int)*numOfOutputsFirst)); //Allocate memory
                        outputsFirst[numOfOutputsFirst-1]=outputs[j];
                    }
                    else
                    {
                        outputsFirst = (int*)realloc(outputsFirst,(sizeof(int)*numOfOutputsFirst)); //Reallocate memory
                        outputsFirst[numOfOutputsFirst-1]=outputs[j];
                    }
                } 
            }  
        } 
    }
    // Add all the inputs of the second part if they are not original inputs, withoud duplicates
    for (int i = 0; i < numOfInputsSecond; i++)
    {
        //check ig original input
        bool originalInput = false;
        for (int m = 0; m < numOfInputs; m++)
        {
            if (inputsSecond[i]==inputs[m])
            {
                originalInput = true;
                break;
            } 
        }
        // if original input move to the next
        if (originalInput)
        {
            continue;
        }
        //check for duplicates
        bool duplicate = false;
                for (int k = 0; k < numOfOutputsFirst; k++)
                {   
                    duplicate = false;
                    if (inputsSecond[i]==outputsFirst[k])
                    {
                        duplicate = true;
                    break;
                    }
                }
                // If not a duplicate then add it
                if (!duplicate)
                {
                    numOfOutputsFirst++;
                    if (numOfOutputsFirst==1)
                    {
                        outputsFirst = (int*)malloc((sizeof(int)*numOfOutputsFirst)); //Allocate memory
                        outputsFirst[numOfOutputsFirst-1]=inputsSecond[i];
                    }
                    else
                    {
                        outputsFirst = (int*)realloc(outputsFirst,(sizeof(int)*numOfOutputsFirst)); //Reallocate memory
                        outputsFirst[numOfOutputsFirst-1]=inputsSecond[i];
                    }
                } 
    }
    

    //Find Second's Outputs
    for (int i = splitingPoint+1; i < netlistRows; i++)
    {
        for (int j = 0; j < numOfOutputs; j++)
        {
            if ((netlist[i][2]==outputs[j]) || (netlist[i][3]==outputs[j]))
            {
                //check for duplicates
                bool duplicate = false;
                for (int k = 0; k < numOfOutputsSecond; k++)
                {   
                    duplicate = false;
                    if (outputs[j]==outputsSecond[k])
                    {
                        duplicate = true;
                    break;
                    }
                }
                // If not a duplicate then add it
                if (!duplicate)
                {
                    numOfOutputsSecond++;
                    if (numOfOutputsSecond==1)
                    {
                        outputsSecond = (int*)malloc((sizeof(int)*numOfOutputsSecond)); //Allocate memory
                        outputsSecond[numOfOutputsSecond-1]=outputs[j];
                    }
                    else
                    {
                        outputsSecond = (int*)realloc(outputsSecond,(sizeof(int)*numOfOutputsSecond)); //Reallocate memory
                        outputsSecond[numOfOutputsSecond-1]=outputs[j];
                    }
                } 
            }  
        } 
    }
    printf("First Inputs: \n");
    for (int i = 0; i < numOfInputsFirst; i++)
    {
        printf("%d\n",inputsFirst[i]);
    }
    printf("First's Outputs: \n");
    for (int i = 0; i < numOfOutputsFirst; i++)
    {
        printf("%d\n",outputsFirst[i]);
    }
    printf("Second's Inputs: \n");
    for (int i = 0; i < numOfInputsSecond; i++)
    {
        printf("%d\n",inputsSecond[i]);
    }
    printf("Second's Outputs: \n");
    for (int i = 0; i < numOfOutputsSecond; i++)
    {
        printf("%d\n",outputsSecond[i]);
    }

}

void crete2Files(FILE* fp1,FILE* fp2)
{
    printf("## RAILS\n");
    fprintf(fp1,"## RAILS\n");
    fprintf(fp2,"## RAILS\n");

    printf("VCC %d ; GND %d\n",VCC,GND);
    fprintf(fp1,"VCC %d ; GND %d\n",VCC,GND);
    fprintf(fp2,"VCC %d ; GND %d\n",VCC,GND);
    printf("## INPUTS\n");
    fprintf(fp1,"## INPUTS\n");
    fprintf(fp2,"## INPUTS\n");
    for (int i = 0; i < numOfInputsFirst; i++)
    {   
        if (i==(numOfInputsFirst-1))
        {
            printf("%d\n",inputsFirst[i]);
            fprintf(fp1,"%d\n",inputsFirst[i]);
            break;
        }
        printf("%d,",inputsFirst[i]);
        fprintf(fp1,"%d,",inputsFirst[i]);
    }
    for (int i = 0; i < numOfInputsSecond; i++)
    {   
        if (i==(numOfInputsSecond-1))
        {
            printf("%d\n",inputsSecond[i]);
            fprintf(fp2,"%d\n",inputsSecond[i]);
            break;
        }
        printf("%d,",inputsSecond[i]);
        fprintf(fp2,"%d,",inputsSecond[i]);
    }
    printf("## OUTPUTS\n");
    fprintf(fp1,"## OUTPUTS\n");
    fprintf(fp2,"## OUTPUTS\n");
    for (int i = 0; i < numOfOutputsFirst; i++)
    {   
        if (i==(numOfOutputsFirst-1))
        {
            printf("%d\n",outputsFirst[i]);
            fprintf(fp1,"%d\n",outputsFirst[i]);
            break;
        }
        printf("%d,",outputsFirst[i]);
        fprintf(fp1,"%d,",outputsFirst[i]);
    }
    for (int i = 0; i < numOfOutputsSecond; i++)
    {   
        if (i==(numOfOutputsSecond-1))
        {
            printf("%d\n",outputsSecond[i]);
            fprintf(fp2,"%d\n",outputsSecond[i]);
            break;
        }
        printf("%d,",outputsSecond[i]);
        fprintf(fp2,"%d,",outputsSecond[i]);
    }
    printf("## NETLIST\n");
    fprintf(fp1,"## NETLIST\n");
    fprintf(fp2,"## NETLIST\n");
    int num=0;
    for (int i = 0; i <= splitingPoint; i++)
    {   
        num++;
        if (netlist[i][0]==0)
        {
            printf("U%d PMOS ",num);
            fprintf(fp1,"U%d PMOS ",num);
        }
        else
        {
            printf("U%d NMOS ",num);
            fprintf(fp1,"U%d NMOS ",num);
        }
        for (int j = 1; j < netlistCols; j++)
        {
            printf("%d ",netlist[i][j]);
            fprintf(fp1,"%d ",netlist[i][j]);
        }
        printf("\n");
        fprintf(fp1,"\n");
    }
    for (int i = splitingPoint+1; i < netlistRows; i++)
    {   
        num++;
        if (netlist[i][0]==0)
        {
            printf("U%d PMOS ",num);
            fprintf(fp2,"U%d PMOS ",num);
        }
        else
        {
            printf("U%d NMOS ",num);
            fprintf(fp2,"U%d NMOS ",num);
        }
        for (int j = 1; j < netlistCols; j++)
        {
            printf("%d ",netlist[i][j]);
            fprintf(fp2,"%d ",netlist[i][j]);
        }
        printf("\n");
        fprintf(fp2,"\n");
    }
}
