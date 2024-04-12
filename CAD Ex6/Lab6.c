//Name: Ntounetas Dimitris
//AM: 2016030141
//OS: Windows 10 Pro
//Built with VS Code IDE 
//Date 06/06/2021
//##File works perfectly with no errors or warnings! If an error occurs please contact me!#################
//###########################################################################################################

//Library includes
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

//Lab6 Functions
void getGoldenInfo(FILE* fpGolden,FILE *fpOut);//gets the info from the Golden Standard file
void startingChecks();
void mainChecking();
void DFS(int current, int node, int *path ,bool golden);
bool checkIfEnd(int current,bool golden);
int find_children(int current,int *children,bool golden);
void addPath(int *path, bool golden);
void EncodePaths();
void checkFilePaths();
void createReport(FILE* fpOut);

//Functions used
void checkStim(int argc, char *argv[]);//checks if a stim file is entered
void getCircuitinfo(FILE* fpIn,FILE* fpOut);//gets the info from the stim file
void printNetlist(FILE* fpOut);


//Memory allocations for Circuit Data.
int VCC,GND,numOfInputs,numOfOutputs,netlistRows,netlistCols;
int GoldenVCC,GoldenGND,GoldenNumOfInputs,GoldenNumOfOutputs,GoldenNetlistRows,GoldenNetlistCols;
int* inputs;
int* GoldenInputs;
int* outputs;
int* GoldenOutputs;
int **netlist;
int **GoldenNetlist;
int count,count1;
bool foundGate = false;
int NOTnum, NANDnum, NORnum, GoldenNOTnum, GoldenNANDnum, GoldenNORnum;
int paths[100][100];
int numOfpaths=0,GoldenNumOfPaths=0;
int GoldenPaths[100][100];
int Encpaths[100][100];
int GoldenEncPaths[100][100];
int EqualGates[100][2];
int numOfEqGates = 0;

int main(int argc, char *argv[])
{   
    //Check if circuit file is present
    checkStim(argc, argv);
    //open circuit file
    FILE* fpIn = fopen(argv[1],"r");
    FILE* fpGolden = fopen(argv[2],"r");
    printf("Golden File for comparison is %s\n",argv[2]);
    if (fpIn == NULL)
    {
        printf("Could not find input file");
    }
    if (fpGolden == NULL)
    {
        printf("Could not find Golden Standard file");
    }
    char file1[64];
    char dot = '.';
    
    strcpy(file1,strtok(argv[1],&dot));
    strcat(file1,"_out1.txt");

    FILE *fp1 = fopen(file1,"w");
    //get Golden Standard File Info
    getGoldenInfo(fpGolden,fp1);
    //get circuit file info
    getCircuitinfo(fpIn,fp1);
    printNetlist(fp1);
    startingChecks(fp1);
    mainChecking(fp1);
    createReport(fp1);

    return 0;
}
void showHelp()
{
    printf("\nAdd input file first as text file and then Golden Standard file\n");
    printf("\nCheck to give a correctly formated file!!\n");
}
void checkStim(int argc, char *argv[])
{   
    //Only two files at a time
    if (argc<3)
    {
        printf("Not enough arguments!\n");
        showHelp();
        exit(1);
    }
    else if (argc >3)
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
    char temp_line[64];
    char space = ' ';
    char comaSemiColon[] = "',',';'";
    char allDelims[] = "',',';',' '";
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
                if (strcmp(line, "## END_SIMULATION")==0)
                {
                    break;
                }
                //Reset columns of netlist
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
                sprintf(temp_line,"%s",line);
                tokenIn = strtok(line, allDelims);
                if (tokenIn[0] == 'G')
                {   
                    //Store gate Number in first column of Netlist
                    netlist[count1][count] = atoi(strtok(tokenIn,"G"));
                    count++;
                }
                tokenIn = strtok(temp_line, allDelims);
                tokenIn = strtok(NULL, allDelims);
                //Create coding for gates: NOR_2 = 0 , NAND_2 = 1, NOT = 2
                if (strcmp(tokenIn, "NOR_2")==0)
                {    
                    netlist[count1][count] = 0;    
                }
                else if (strcmp(tokenIn, "NAND_2")==0)
                {
                    netlist[count1][count] = 1;  
                }
                else if (strcmp(tokenIn, "NOT")==0)
                {
                    netlist[count1][count] = 2;  
                }
                else
                {
                    printf("error\n");
                    exit(1);
                }
                //Read inputs and outputs
                while (1)
                {
                    tokenIn = strtok(NULL,allDelims);
                    if (tokenIn == NULL)
                    {
                        netlistRows = count1+1;
                        netlistCols = count+1;
                        break;
                    }
                    if (strcmp(tokenIn, "IN")==0)
                    {
                        tokenIn = strtok(NULL,allDelims);
                        //First Gate Input
                        count++; 
                        netlist[count1][2] = atoi(tokenIn);
                        // If current Gate is not "NOT" then search next Input
                        count++; 
                        if (netlist[count1][1]!=2)
                        {
                            tokenIn = strtok(NULL,allDelims);
                            //Second gate input
                            netlist[count1][3] = atoi(tokenIn);
                        }
                    }
                    else if (strcmp(tokenIn, "OUT")==0)
                    {   
                        tokenIn = strtok(NULL,allDelims);
                        //First Gate Output
                        count++; 
                        netlist[count1][4] = atoi(tokenIn);
                    }
                    else
                    {
                        printf("Unexpected Gate Inputs or Outputs in Gate G%d in Input file. Check format\n",netlist[count1][0]);
                        fprintf(fpOut,"Unexpected Gate Inputs or Outputs in Gate G%d in Input file. Check format\n",netlist[count1][0]);
                        exit(1);
                    }
                      
                }
                count1++;
            }
        }
        //Find Testbench values save them and do test
        if(strcmp(line, "## TESTBENCH")==0)
        {   
            //Reserved for future use
        }
        if(strcmp(line, "## END_SIMULATION")==0)//For this to work right we have to add a blank row in out text file
        {                                        //Because we cannot compare last line with a string because of EOF.
            // printf("\nSIMULATION ENDED\n");
            break;
        }
    }
}
//gets the info from the Golden Standard file
void getGoldenInfo(FILE* fpGolden, FILE* fpOut)
{
    char* line=NULL;
    size_t len=0;
    ssize_t read;

    char* tokenOut;
    char* tokenIn;
    char temp_line[64];
    char space = ' ';
    char comaSemiColon[] = "',',';'";
    char allDelims[] = "',',';',' '";
    //Read file line by line
    while((read = getline(&line, &len, fpGolden)) != -1)
    {
        line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
        //Find VCC and GND values and save them
        if(strcmp(line, "## RAILS")==0)
        {
            if((read = getline(&line, &len, fpGolden)) == -1)
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
                    GoldenVCC = atoi(tokenIn);    
                }
                else if (strcmp(tokenIn, "GND")==0)
                {
                    tokenIn = strtok(NULL, &space);
                    GoldenGND = atoi(tokenIn);    
                } 
            }
        }
        //Find input values and save them
        if(strcmp(line, "## INPUTS")==0)
        {
            if ((read = getline(&line, &len, fpGolden)) == -1)
            {
                printf("Problem");
                exit(1);
            }
                line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
                tokenIn = strtok(line, comaSemiColon);
                GoldenInputs = (int*)malloc(sizeof(int));
                GoldenInputs[0] = atoi(tokenIn); 
                count = 1;
                while (1)
                {
                    tokenIn = strtok(NULL,comaSemiColon);
                    if (tokenIn == NULL)
                    {
                        GoldenNumOfInputs = count;
                        break;
                    }
                    count++; 
                    GoldenInputs = realloc(GoldenInputs,sizeof(int)*count);
                    GoldenInputs[count-1] = atoi(tokenIn);  
                }
        }
        //Find output values and save them
        if(strcmp(line, "## OUTPUTS")==0)
        {
            if ((read = getline(&line, &len, fpGolden)) == -1)
            {
                printf("Problem");
                exit(1);
            }
                line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
                tokenIn = strtok(line, comaSemiColon);
                GoldenOutputs = (int*)malloc(sizeof(int));
                GoldenOutputs[0] = atoi(tokenIn); 
                count = 1;
                while (1)
                {
                    tokenIn = strtok(NULL,comaSemiColon);
                    if (tokenIn == NULL)
                    {
                        GoldenNumOfOutputs = count;
                        break;
                    }
                    count++; 
                    GoldenOutputs = realloc(GoldenOutputs,sizeof(int)*count);
                    GoldenOutputs[count-1] = atoi(tokenIn);     
                }
        }
        //Find netlist values and save them
        if(strcmp(line, "## NETLIST")==0)
        {   count1=0;
            while (1)
            {                
            if ((read = getline(&line, &len, fpGolden)) == -1)
            {
                printf("Problem");
                exit(1);
            }
                line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
                if (strcmp(line, "## END_SIMULATION")==0)
                {
                    break;
                }
                //Reset columns of netlist
                count=0;
                if (count1==0)
                {
                    GoldenNetlist = (int**)malloc(sizeof(int));//allocate memory for double pointer row
                    GoldenNetlist[count1] = (int*)calloc(6,sizeof(int));//allocate memory for array columns
                }
                else
                {
                    GoldenNetlist = (int**)realloc(GoldenNetlist,sizeof(int)*(count1+1));//allocate memory for double pointer row
                    GoldenNetlist[count1] = (int*)calloc(6,sizeof(int));//allocate memory for array columns
                }
                sprintf(temp_line,"%s",line);
                tokenIn = strtok(line, allDelims);
                if (tokenIn[0] == 'G')
                {   
                    //Store the gate number in first column of netlist
                    GoldenNetlist[count1][count] = atoi(strtok(tokenIn,"G"));
                    count++;
                }
                tokenIn = strtok(temp_line, allDelims);
                tokenIn = strtok(NULL, allDelims);
                //Create coding for gates: NOR_2 = 0 , NAND_2 = 1, NOT = 2
                if (strcmp(tokenIn, "NOR_2")==0)
                {    
                    GoldenNetlist[count1][count] = 0;    
                }
                else if (strcmp(tokenIn, "NAND_2")==0)
                {
                    GoldenNetlist[count1][count] = 1;  
                }
                else if (strcmp(tokenIn, "NOT")==0)
                {
                    GoldenNetlist[count1][count] = 2;  
                }
                else
                {
                    printf("error\n");
                    exit(1);
                }
                //Read inputs and outputs
                while (1)
                {
                    tokenIn = strtok(NULL,allDelims);
                    if (tokenIn == NULL)
                    {
                        GoldenNetlistRows = count1+1;
                        GoldenNetlistCols = count+1;
                        break;
                    }
                    if (strcmp(tokenIn, "IN")==0)
                    {
                        tokenIn = strtok(NULL,allDelims);
                        //First Gate Input
                        count++; 
                        GoldenNetlist[count1][2] = atoi(tokenIn);
                        // If current Gate is not "NOT" then search next Input
                        count++; 
                        if (GoldenNetlist[count1][1]!=2)
                        {
                            tokenIn = strtok(NULL,allDelims);
                            //Second gate input
                            GoldenNetlist[count1][3] = atoi(tokenIn);
                        }
                    }
                    else if (strcmp(tokenIn, "OUT")==0)
                    {   
                        tokenIn = strtok(NULL,allDelims);
                        //First Gate Output
                        count++; 
                        GoldenNetlist[count1][4] = atoi(tokenIn);
                    }
                    else
                    {
                        printf("Unexpected Gate Inputs or Outputs in Gate G%d in Golden Standard file. Check format\n",GoldenNetlist[count1][0]);
                        fprintf(fpOut,"Unexpected Gate Inputs or Outputs in Gate G%d in Golden Standard file. Check format\n",netlist[count1][0]);
                        exit(1);
                    }
                      
                }
                count1++;
            }
        }
        //Find Testbench values save them and do test
        if(strcmp(line, "## TESTBENCH")==0)
        {   
            //Reserved for future use
        }
        if(strcmp(line, "## END_SIMULATION")==0)//For this to work right we have to add a blank row in out text file
        {                                        //Because we cannot compare last line with a string because of EOF.
            
            break;
        }
    }
}
//Simple checks for number of gates and gate types
void startingChecks(FILE * fpOut)
{
    //Check for correct number of Gates
    if (netlistRows != GoldenNetlistRows)
    {
        printf("\nThe 2 Models are not Equivalent!\nThe 2 Circuits have different number of Gates\n");
        fprintf(fpOut,"\nThe 2 Models are not Equivalent!\nThe 2 Circuits have different number of Gates\n");
        exit(1);
    }
    //Check that the types of the gates are similar
    NOTnum = 0;
    NANDnum = 0;
    NORnum = 0;
    GoldenNOTnum = 0;
    GoldenNANDnum = 0;
    GoldenNORnum = 0;
    int n=0;
    //Count Input file's Gate types
    // 0 = NOR_2
    // 1 = NAND_2
    // 2 = NOT
    for (int i = 0; i < netlistRows; i++)
    {
        n=netlist[i][1];
        switch (n)
        {
            case 0:
                NORnum++;
                break;
            case 1:
                NANDnum++;
                break;
            case 2:
                NOTnum++;
                break;
            default:
                break;
        }
    }
    printf("\nNOR Gates: %d NAND Gates: %d NOT Gates: %d",NORnum,NANDnum,NOTnum);
    
    //Count Golden Standard file's Gate types
    // 0 = NOR_2
    // 1 = NAND_2
    // 2 = NOT
    for (int i = 0; i < GoldenNetlistRows; i++)
    {
        n=GoldenNetlist[i][1];   
        switch (n)
        {
            case 0:
                GoldenNORnum++;
                break;
            case 1:
                GoldenNANDnum++;
                break;
            case 2:
                GoldenNOTnum++;
                break;
            default:
                break;
        }
    }
    printf("\nNOR Gates: %d NAND Gates: %d NOT Gates: %d",GoldenNORnum,GoldenNANDnum,GoldenNOTnum);
    
    //Check if Gate types are similar
    if (GoldenNORnum!= NORnum||GoldenNANDnum!= NANDnum||GoldenNOTnum!= NOTnum)
    {
        printf("\nThe 2 Models are not Equivalent!\nThe Gate types are not the same!");
        fprintf(fpOut,"\nThe 2 Models are not Equivalent!\nThe 2 Circuits have different number of Gates\n");
        exit(1);
    }
    
}
//Check to find if all paths of Golden standard are same with Input Circuit
void mainChecking(FILE *fpOut)
{   
    int startingPoints[100];
    int GoldenStartingPoints[100];
    int startingPointNum = 0;
    int GoldenStartingPointNum = 0;
    int currPath[100];
    
    //Find Starting Points of the Path for Input Circuit
    for (int i = 0; i < netlistRows; i++)
    {
        for (int j = 0; j < numOfInputs; j++)
        {
            //Check if First Gate Input is Circuit Input
            if (netlist[i][2]==inputs[j])
            {
                startingPoints[startingPointNum]=i;
                startingPointNum++;
            }
            //Check if Second Gate Input is Circuit Input if the 2 inputs are not equal and the gate is not "NOT"
            if ((netlist[i][3]==inputs[j])&&(netlist[i][1]!=2)&&(netlist[i][3]!=netlist[i][2]))
            {
                startingPoints[startingPointNum]=i;
                startingPointNum++;
            }
        } 
    }
    //Find Starting Points of the Path for Golden Standard Circuit
    for (int i = 0; i < GoldenNetlistRows; i++)
    {
        for (int j = 0; j < GoldenNumOfInputs; j++)
        {
            //Check if First Gate Input is Circuit Input
            if (GoldenNetlist[i][2]==GoldenInputs[j])
            {
                GoldenStartingPoints[GoldenStartingPointNum]=i;
                GoldenStartingPointNum++;
            }
            //Check if Second Gate Input is Circuit Input if the 2 inputs are not equal and the gate is not "NOT"
            if ((GoldenNetlist[i][3]==GoldenInputs[j])&&(GoldenNetlist[i][1]!=2)&&(GoldenNetlist[i][3]!=GoldenNetlist[i][2]))
            {
                GoldenStartingPoints[GoldenStartingPointNum]=i;
                GoldenStartingPointNum++;
            }
        } 
    }
    for (int i = 0; i < startingPointNum; i++)
    {
        // Start Depth First Search for Input file to find paths
        DFS(startingPoints[i],0,currPath,false);
    }
    for (int i = 0; i < GoldenStartingPointNum; i++)
    {
        // Start Depth First Search for Golden Standard file to find paths
        DFS(GoldenStartingPoints[i],0,currPath,true);
    }
    if (numOfpaths!=GoldenNumOfPaths)
    {
        printf("\nThe 2 Models are not Equivalent!\nThe number of paths for the 2 Circuits is Different");
        fprintf(fpOut,"\nThe 2 Models are not Equivalent!\nThe 2 Circuits have different number of Gates\n");
        exit(1);
    }
    //Array sorting in descending order using bubbleshort for Input File's paths
    int a=0;
    for (int i = 0; i < numOfpaths-1; ++i){
        int len_j_plus1,len_j,j;
        int temp[100];   
        for (j = 0; j < numOfpaths-i-1; j++){
            // Calculate current path length
            for (a = 0; paths[j][a]!= -1; a++)
            {
            }
            len_j = a;
            // Calculate next path length
            for (a = 0; paths[j+1][a]!= -1; a++)
            {
            }
            len_j_plus1 = a;
            // If next path is longer move up
            if (len_j_plus1 > len_j)
            {   
                // Store the current path temporarily
                for (a = 0; paths[j][a]!= -1; a++)
                {
                    temp[a]=paths[j][a];
                }
                temp[a] = -1;
                // Change the paths
                for (a = 0; paths[j+1][a]!= -1; a++)
                {
                    paths[j][a] = paths[j+1][a];
                }
                paths[j][a] = -1;
                // Move the current path down
                for (a=0;temp[a]!=-1;a++){
                    paths[j+1][a] = temp[a];
                }
                paths[j+1][a] = -1;
            }
        }      
    } 
    //Array sorting in descending order using bubbleshort for Golden Stanadrd File's paths
    for (int i = 0; i < GoldenNumOfPaths-1; ++i){
        int len_j_plus1,len_j,j;
        int temp[100]; 
        for (j = 0; j < GoldenNumOfPaths-i-1; j++){
            // Calculate current path length
            for (a = 0; GoldenPaths[j][a]!= -1; a++)
            {
            }
            len_j = a;
            // Calculate next path length
            for (a = 0; GoldenPaths[j+1][a]!= -1; a++)
            {
            }
            len_j_plus1 = a;
            // If next path is longer move up
            if (len_j_plus1 > len_j)
            {   
                // Store the current path temporarily
                for (a = 0; GoldenPaths[j][a]!= -1; a++)
                {
                    temp[a]=GoldenPaths[j][a];
                }
                temp[a] = -1;
                // Change the paths
                for (a = 0; GoldenPaths[j+1][a]!= -1; a++)
                {
                    GoldenPaths[j][a] = GoldenPaths[j+1][a];
                }
                GoldenPaths[j][a] = -1;
                // Move the current path down
                for (a=0;temp[a]!=-1;a++){
                    GoldenPaths[j+1][a] = temp[a];
                }
                GoldenPaths[j+1][a] = -1;
            }
        }    
    } 
    // Encode the paths
    EncodePaths();
    checkFilePaths();
}
// Depth First Search algorithm to find paths
void DFS(int current, int node, int *path ,bool golden)
    {
        //Check if current node is an Output node
        if (checkIfEnd(current,golden))
        {
           path[node] = current;
           path[node+1] = -1; 
           // Store current path to paths arrays
           addPath(path,golden);
           return;
        }
        //Find all possible children of current node
        int children[10];
        int childrenNum = find_children(current,children,golden);
        //Add current node to the path
        path[node] = current;
        // Call DFS for all the children nodes
        for (int i = 0; i < childrenNum; i++)
        {
            node++;
            DFS(children[i],node,path,golden);
            path[node]=-1;
            node--;
        }
        return;
    }
// Check if node is output
bool checkIfEnd(int current,bool golden)
{
    if (golden)
    {
        for (int i = 0; i < GoldenNumOfOutputs; i++)
        {
            if (GoldenNetlist[current][4]==GoldenOutputs[i])
            {
            return true;
            }
        }
    }
    else
    {
        for (int i = 0; i < numOfOutputs; i++)
        {
            if (netlist[current][4]==outputs[i])
            {
                return true;
            }
        }
    }
        return false;
}
//Find nodes that have inputs equal to current node output
int find_children(int current,int *children,bool golden)
{   
    int childrenNum = 0;
    if (golden)
    {
        //Do for Golden Standard File
        for (int i = 0; i < GoldenNetlistRows; i++)
        {   
            //Find gates that first input is the same with current output
            if (GoldenNetlist[i][2]==GoldenNetlist[current][4])
            {
                children[childrenNum] = i;
                childrenNum++;
            }
            //Find gates that second input is the same with current output and not NOT gates
            if ((GoldenNetlist[i][3]==GoldenNetlist[current][4])&&(GoldenNetlist[i][1]!=2))
            {
                children[childrenNum] = i;
                childrenNum++;
            }
        }
    }
    else{
        //Do for Input file
        for (int i = 0; i < netlistRows; i++)
        {
            //Find gates that first input is the same with current output
            if (netlist[i][2]==netlist[current][4])
            {
                children[childrenNum] = i;
                childrenNum++;
            }
            //Find gates that second input is the same with current output and not NOT gates
            if ((netlist[i][3]==netlist[current][4])&&(netlist[i][1]!=2))
            {
                children[childrenNum] = i;
                childrenNum++;
            }
        }
    }
    return childrenNum;
}
void addPath(int *path, bool golden)
{
    if (golden)
    {
        int i;
        // Add -2 in the beggining of the path for checking purposes
        GoldenPaths[GoldenNumOfPaths][0] = -2;
        // Add path to the array of paths
        for (i = 0; path[i] != -1 ; i++)
        {
            GoldenPaths[GoldenNumOfPaths][i+1] = path[i];
        }
        GoldenPaths[GoldenNumOfPaths][i+1] = -1;
        GoldenNumOfPaths++;
    }
    else
    {
        int i;
        // Add -2 in the beggining of the path for checking purposes
        paths[numOfpaths][0] = -2;
        // Add path to the array of paths
        for (i = 0; path[i] != -1 ; i++)
        {
            paths[numOfpaths][i+1] = path[i];
        }
        paths[numOfpaths][i+1] = -1;
        numOfpaths++;
    }
}
// Encode paths in order to check them
void EncodePaths()
{
    int temp;
    //Encode Input File paths
    for (int i = 0; i < numOfpaths; i++)
    {
        int j;
        // Add -2 in the beggining of the path for checking purposes
        Encpaths[i][0]=-2; 
        // For every gate add 0 if NOR_2 , 1 if NAND_2 or 2 if NOT
        for (j = 1; paths[i][j]!= -1; j++)
        {
            temp = paths[i][j];
            if (netlist[temp][1]==0)
            {
                Encpaths[i][j] = 0;
            }
            else if (netlist[temp][1]==1)
            {
                Encpaths[i][j] = 1;
            }
            else if (netlist[temp][1]==2)
            {
                Encpaths[i][j] = 2;
            }
        }
        Encpaths[i][j]=-1; 
    }
    //Encode Golden Standard File paths
    for (int i = 0; i < GoldenNumOfPaths; i++)
    {
        int j;
        // Add -2 in the beggining of the path for checking purposes
        GoldenEncPaths[i][0] = -2;
        // For every gate add 0 if NOR_2 , 1 if NAND_2 or 2 if NOT
        for (j = 1; GoldenPaths[i][j]!= -1; j++)
        {
            temp = GoldenPaths[i][j];
            if (GoldenNetlist[temp][1]==0)
            {
                GoldenEncPaths[i][j] = 0;
            }
            else if (GoldenNetlist[temp][1]==1)
            {
                GoldenEncPaths[i][j] = 1;
            }
            else if (GoldenNetlist[temp][1]==2)
            {
                GoldenEncPaths[i][j] = 2;
            }
        }
        GoldenEncPaths[i][j] = -1; 
    }
}
// Check paths of files to determine gate equalities
void checkFilePaths()
{
    // Memory preping
    for (int i = 0; i < 100; i++)
    {
        EqualGates[i][0]=-1;
        EqualGates[i][1]=-1;
    }
    int GoldenPathLenght = 0;
    int pathLength = 0;
    for (int i = 0; i < GoldenNumOfPaths; i++)
    {
        bool same = true;
        int j;
        int k;
        for (j = 0; j < numOfpaths; j++)
        {
            same = true;
            if (Encpaths[j][0]==-3)
            {
                continue;
            }
            // Measure the path length of Golden Standard file's paths
            for (GoldenPathLenght = 0; GoldenPaths[i][GoldenPathLenght]!=-1; GoldenPathLenght++)
            {

            }
            // Measure the path length of Input file's paths
            for (pathLength = 0; paths[j][pathLength]!=-1; pathLength++)
            {

            }
            printf("\n%d  %d",GoldenPathLenght,pathLength);
            // Check if the 2 paths are equal
            if (GoldenPathLenght!=pathLength)
            {
                same = false;
                continue;
            }
            // Check if every gate type in the path is same
            for (k = 1; k < GoldenPathLenght; k++)
            {
                if (GoldenEncPaths[i][k]!=Encpaths[j][k])
                {
                    same = false;
                    break;
                }
            }
            // Reduce cpu time if find a match
            if (same)
            {
                break;
            }
        }
        // If found similar path mark them and create an equality Array
        if (same)
        {
            // Mark paths
            GoldenEncPaths[i][0]=-3;
            Encpaths[j][0]=-3;
            // Create equality array
            for (int gold = 1; GoldenPaths[i][gold]!=-1; gold++)
            {
                // For current path check all nodes of golden Standard if already in equality array dont add them
                for (k = 0; k < numOfEqGates; k++)
                {
                    if(GoldenPaths[i][gold]==EqualGates[k][0]||paths[j][gold]==EqualGates[k][1])
                    {
                        break;
                    }
                }
                // If not in array add them
                if (k==numOfEqGates)
                {
                    EqualGates[numOfEqGates][0]=GoldenPaths[i][gold];
                    EqualGates[numOfEqGates][1]=paths[j][gold];
                    // temp++;
                    numOfEqGates++;
                }
            }
        }   
    }
}
//Create the ASCII Report for the end User
void createReport(FILE * fpOut)
{
    printf("\n");
    printf("The 2 Models are Equivalent \n\n");
    fprintf(fpOut,"The 2 Models are Equivalent \n\n");
    printf("Their gate Equalities are as described bellow\n\n");
    fprintf(fpOut,"Their gate Equalities are as described bellow\n\n");
    printf("Input File's Specs. Gate number(First Input,Second Input)-->Output == Golden Standards File's Specs. Gate number(First Input,Second Input)-->Output\n\n");
    fprintf(fpOut,"Input File's Specs. Gate number(First Input,Second Input)-->Output == Golden Standards File's Specs. Gate number(First Input,Second Input)-->Output\n\n");
    
    int difference = GoldenNetlistRows - numOfEqGates;
    if (difference==1)
    {
        // bool exist = true;
        for (int i = 0; i < GoldenNetlistRows; i++)
        {
            int j;
            for (j = 0; j < numOfEqGates; j++)
            {
                if (EqualGates[j][0] == i)
                {
                    break;
                }  
            }
            if (j==numOfEqGates)
            {
                EqualGates[numOfEqGates][0]=i;
            }
        }
        for (int i = 0; i < netlistRows; i++)
        {
            int j;
            for (j = 0; j < numOfEqGates; j++)
            {
                if (EqualGates[j][1] == i)
                {
                    break;
                }  
            }
            if (j==numOfEqGates)
            {
                EqualGates[numOfEqGates][1]=i;
            }
        }
        numOfEqGates++;
    }
    
    for (int i = 0; i < numOfEqGates; i++)
    {
        // Correct Print for NOT Gates
        if (netlist[EqualGates[i][1]][1]==2||GoldenNetlist[EqualGates[i][0]][1]==2)
        {
            printf("G%d(%d)-->%d == G%d(%d)-->%d\n",netlist[EqualGates[i][1]][0],netlist[EqualGates[i][1]][2],netlist[EqualGates[i][1]][4],GoldenNetlist[EqualGates[i][0]][0],GoldenNetlist[EqualGates[i][0]][2],GoldenNetlist[EqualGates[i][0 ]][4]);
            fprintf(fpOut,"G%d(%d)-->%d == G%d(%d)-->%d\n",netlist[EqualGates[i][1]][0],netlist[EqualGates[i][1]][2],netlist[EqualGates[i][1]][4],GoldenNetlist[EqualGates[i][0]][0],GoldenNetlist[EqualGates[i][0]][2],GoldenNetlist[EqualGates[i][0 ]][4]);
            
        }
        // Correct Print for other gates
        else
        {
            printf("G%d(%d,%d)-->%d == G%d(%d,%d)-->%d\n",netlist[EqualGates[i][1]][0],netlist[EqualGates[i][1]][2],netlist[EqualGates[i][1]][3],netlist[EqualGates[i][1]][4],GoldenNetlist[EqualGates[i][0]][0],GoldenNetlist[EqualGates[i][0]][2],GoldenNetlist[EqualGates[i][0]][3],GoldenNetlist[EqualGates[i][0 ]][4]);
            fprintf(fpOut,"G%d(%d,%d)-->%d == G%d(%d,%d)-->%d\n",netlist[EqualGates[i][1]][0],netlist[EqualGates[i][1]][2],netlist[EqualGates[i][1]][3],netlist[EqualGates[i][1]][4],GoldenNetlist[EqualGates[i][0]][0],GoldenNetlist[EqualGates[i][0]][2],GoldenNetlist[EqualGates[i][0]][3],GoldenNetlist[EqualGates[i][0 ]][4]);
        }
        
    }
    
}
void printNetlist(FILE* fpOut)
{   
    printf("\nINPUT FILE INFO\n\n");
    printf("## RAILS\n");
    // fprintf(fpOut,"## RAILS\n");
    
    printf("VCC %d ; GND %d\n",VCC,GND);
    // fprintf(fpOut,"VCC %d ; GND %d\n",VCC,GND);
    
    printf("## INPUTS\n");
    // fprintf(fpOut,"## INPUTS\n");
    
    for (int i = 0; i < numOfInputs; i++)
    {   
        if (i==(numOfInputs-1))
        {
            printf("%d\n",inputs[i]);
            // fprintf(fpOut,"%d\n",inputs[i]);
            
            break;
        }
        printf("%d,",inputs[i]);
        // fprintf(fpOut,"%d,",inputs[i]);
    }
    printf("## OUTPUTS\n");
    // fprintf(fpOut,"## OUTPUTS\n");
    for (int i = 0; i < numOfOutputs; i++)
    {   
        if (i==(numOfOutputs-1))
        {
            printf("%d\n",outputs[i]);
            // fprintf(fpOut,"%d\n",outputs[i]);
            break;
        }
        printf("%d,",outputs[i]);
        // fprintf(fpOut,"%d,",outputs[i]);
    }
    printf("Netlist");
    for (int i = 0; i < netlistRows; i++)
    {   
        printf("\n");
        for (int j = 0; j < netlistCols; j++)
        {
            printf("%d ",netlist[i][j]);
        }
    }

    printf("\n\nGOLDEN STANDARD FILE INFO\n\n");
    printf("## RAILS\n");
    // fprintf(fpOut,"## RAILS\n");
    
    printf("VCC %d ; GND %d\n",GoldenVCC,GoldenGND);
    // fprintf(fpOut,"VCC %d ; GND %d\n",VCC,GND);
    
    printf("## INPUTS\n");
    // fprintf(fpOut,"## INPUTS\n");
    
    for (int i = 0; i < GoldenNumOfInputs; i++)
    {   
        if (i==(GoldenNumOfInputs-1))
        {
            printf("%d\n",GoldenInputs[i]);
            // fprintf(fpOut,"%d\n",inputs[i]);
            break;
        }
        printf("%d,",GoldenInputs[i]);
        // fprintf(fpOut,"%d,",inputs[i]);
    }
    printf("## OUTPUTS\n");
    // fprintf(fpOut,"## OUTPUTS\n");
    for (int i = 0; i < GoldenNumOfOutputs; i++)
    {   
        if (i==(GoldenNumOfOutputs-1))
        {
            printf("%d\n",GoldenOutputs[i]);
            // fprintf(fpOut,"%d\n",outputs[i]);
            break;
        }
        printf("%d,",GoldenOutputs[i]);
        // fprintf(fpOut,"%d,",outputs[i]);
    }
    printf("Netlist");
    for (int i = 0; i < GoldenNetlistRows; i++)
    {   
        printf("\n");
        for (int j = 0; j < GoldenNetlistCols; j++)
        {
            printf("%d ",GoldenNetlist[i][j]);
        }
    }
}

