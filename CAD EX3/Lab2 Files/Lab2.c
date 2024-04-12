//Name: Ntounetas Dimitris
//AM: 2016030141
//OS: Windows 10 Pro
//Built with VS Code IDE 
//Date 28/03/2021
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
void checkStim(int argc, char *argv[]);//checks if a stim file is entered
void getCircuitinfo(FILE* fpIn,FILE* fpOut);//gets the info from the stim file
void doTestBench(FILE* fpIn,FILE* fpOut);//does the TESTBENCH
void showSimulation(FILE * fpOut);//Does simulation and presents it
void showCircuitInfo(FILE * fpOut);//Shows obtained circuit info
void colorGraph(FILE* fpOut);//Does the graph coloring algorithm
int findGraphPos(int nodeVal);//Finds the position of a value in the graph
void showHelp();//Shows help message
void printGraph(FILE* fpOut);//Prints the graph

//Memory allocations for Circuit Data.
int VCC,GND,numOfInputs,numOfOutputs,netlistRows,netlistCols,numOfTestIn,numOfTestOut,numOfGraphs,numOfSimulations,correctSims;
int numOfNodes = 0;
int* inputs;
int* outputs;
int **netlist;
int* testIn;
int* testOut;
int* testVector;
int count,count1;
int* nodes;
int **graph;

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
    // strcat(fileOut,argv[1]);
    fileOut = strtok(argv[1],&dot);
    strcat(fileOut,"_out.txt");
    FILE *fpOut = fopen(fileOut,"w");
    fprintf(fpOut,"Output file for file %s\n\n",argv[1]);
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
                fprintf(fpOut,"Problem");
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
                fprintf(fpOut,"Problem");
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
                fprintf(fpOut,"Problem");
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
                    netlist[count1] = (int*)malloc(sizeof(int)*4);//allocate memory for array columns
                }
                else
                {
                    netlist = (int**)realloc(netlist,sizeof(int)*(count1+1));//allocate memory for double pointer row
                    netlist[count1] = (int*)malloc(sizeof(int)*4);//allocate memory for array columns
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
                    fprintf(fpOut,"error");
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
            doTestBench(fpIn,fpOut);
        }
        if(strcmp(line, "## END_SIMULATION")==0)//For this to work right we have to add a blank row in out text file
                                                //Because we cannot compare last line with a string because of EOF.
        {
            printf("\nSIMULATION ENDED\n");
            fprintf(fpOut,"SIMULATION ENDED");
            break;
        }
    }
}
//Do test 
void doTestBench(FILE* fpIn,FILE* fpOut)
{
    char* line=NULL;
    size_t len=0;
    ssize_t read;
    char* token;
    char comaSemiColon[] = "',',';'";

    while ((read = getline(&line, &len, fpIn)) != -1)
    {   
        line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
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
                token = strtok(line, comaSemiColon);
                testIn = (int*)malloc(sizeof(int));
                testIn[0] = atoi(token);
                count = 1;
                while (1)
                {
                    token = strtok(NULL,comaSemiColon);
                    if (token == NULL)
                    {
                        numOfTestIn = count;
                        break;
                    }
                    count++; 
                    testIn = realloc(testIn,sizeof(int)*count);
                    testIn[count-1] = atoi(token);
                }
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
                token = strtok(line, comaSemiColon); 
                testOut = (int*)malloc(sizeof(int));
                testOut[0] = atoi(token);
                count = 1;
                while (1)
                {
                    token = strtok(NULL,comaSemiColon);
                    if (token == NULL)
                    {
                        numOfTestOut = count;
                        break;
                    }
                    count++; 
                    testOut = realloc(testOut,sizeof(int)*count);
                    testOut[count-1] = atoi(token);
                }
        }
        //Find test vectors values and save them
        if (strcmp(line, "## TEST_VECTORS")==0)
        {
            if ((read = getline(&line, &len, fpIn)) == -1)
            {
                printf("Problem");
                fprintf(fpOut,"Problem");
                exit(1);
            }
            line[strlen(line)-1]='\0'; // Overwrite char \n with \0.
            token = strtok(line, comaSemiColon);
            testVector = (int*)malloc(sizeof(int));
            testVector[0] = atoi(token);
            count = 1;
            while (1)
            {
                token = strtok(NULL,comaSemiColon);
                if (token == NULL)
                    break;
                count++; 
                testVector = realloc(testVector,sizeof(int)*count);
                testVector[count-1] = atoi(token);
            }
        }
        //start simulation
        if (strcmp(line, "## SIMULATE")==0)
        {   
            //save number of simualtions
            numOfSimulations++;
            showSimulation(fpOut);
        }
        //Check if test ended succesfully
        if (strcmp(line, "## END_TEST")==0)
        {   
            if (numOfSimulations == correctSims)
            {
                printf("\nTEST ENDED SUCCESFULY\n\n");
                fprintf(fpOut,"\nTEST ENDED SUCCESFULY\n\n");
                printf("Press ENTER key to continue");
                getchar();
            }
            else
            {
                printf("\nTEST ENDED WITH ERRORS\n");
                printf("\nPlease fix the error indicated above and run the test again\n\n");
                fprintf(fpOut,"\nTEST ENDED WITH ERRORS\n");
                fprintf(fpOut,"\nPlease fix the error indicated above and run the test again\n\n");
                printf("Press ENTER key to continue");
                getchar();
            }
            break;
        }
    }
    return;
} 
// All the information about the circuit
void showCircuitInfo(FILE * fpOut)
{   
    printf("RAILS ARE :\n");
    printf("VCC = %d\n",VCC);
    printf("GND = %d\n",GND);
    printf("INPUTS ARE :\n");
    fprintf(fpOut,"RAILS ARE :\n");
    fprintf(fpOut,"VCC = %d\n",VCC);
    fprintf(fpOut,"GND = %d\n",GND);
    fprintf(fpOut,"INPUTS ARE :\n");

    for (int i = 0; i < numOfInputs; i++)
    {
        printf("%d ",inputs[i]);
        fprintf(fpOut,"%d ",inputs[i]);
    }
    printf("\nOUTPUTS ARE :\n");
    fprintf(fpOut,"\nOUTPUTS ARE :\n");

    for (int i = 0; i < numOfOutputs; i++)
    {
        printf("%d ",outputs[i]);
        fprintf(fpOut,"%d ",outputs[i]);
    }
    printf("\nNETLIST IS : (where 0 in first column = PMOS and 1 = NMOS)\n");
    fprintf(fpOut,"\nNETLIST IS : (where 0 in first column = PMOS and 1 = NMOS)\n");
    for (int i = 0; i <  netlistRows; i++)
    {
        for (int j = 0; j < netlistCols; j++)
        {
            printf("%d ",netlist[i][j]);
            fprintf(fpOut,"%d ",netlist[i][j]);
        }
        printf("\n");
        fprintf(fpOut,"\n");
    }
    printf("TEST IN IS :\n");
    fprintf(fpOut,"TEST IN IS :\n");
    for (int i = 0; i < numOfTestIn; i++)
    {
        printf("%d ",testIn[i]);
        fprintf(fpOut,"%d ",testIn[i]);
    }
    printf("\nTEST OUT IS :\n");
    fprintf(fpOut,"\nTEST OUT IS :\n");
    for (int i = 0; i < numOfTestOut; i++)
    {
        printf("%d ",testOut[i]);
        fprintf(fpOut,"%d ",testOut[i]);
    }
    printf("\nTEST VECTORS ARE :\n");
    fprintf(fpOut,"\nTEST VECTORS ARE :\n");
    for (int i = 0; i < numOfTestIn; i++)
    {
        printf("Input:%d = %d ",testIn[i], testVector[i]);
        fprintf(fpOut,"Input:%d = %d ",testIn[i], testVector[i]);
    }
    return;
}
//show simulation and graph coloring
void showSimulation(FILE * fpOut)
{   
    showCircuitInfo(fpOut);
    printf("\nSIMULATING CIRCUIT...\n\n");
    fprintf(fpOut,"\nSIMULATING CIRCUIT...\n\n");
    colorGraph(fpOut);
    return;
}
//do the graph coloring
void colorGraph(FILE* fpOut)
{   
    int sameAsPrev = 0;
    bool nodeExists,converged;
    nodes = (int*)malloc(sizeof(int)*netlistRows*netlistCols);
    nodes[0]=netlist[0][1];
    numOfNodes=1;
    //calculating Nodes
    //get each node one time without duplicates
    for (int i = 0; i <  netlistRows; i++)
    {
        for (int j = 1; j < netlistCols; j++)
        {
            nodeExists = false;
            for (int k = 0; k < numOfNodes; k++)
            {   
                if (nodes[k]==netlist[i][j])
                {
                    nodeExists = true;
                }
            }
            if (!nodeExists)
            {
                nodes[numOfNodes]=netlist[i][j];
                numOfNodes++;
            }
        }
    }
    printf("Number of Nodes: %d\n",numOfNodes);
    fprintf(fpOut,"Number of Nodes: %d\n",numOfNodes);
    
    //Array sorting in ascending order
    //We have low amount of nodes so we dont care about
    int a;
    for (int i = 0; i < numOfNodes; ++i) 
        {
            for (int j = i + 1; j < numOfNodes; ++j)
            {
                if (nodes[i] > nodes[j]) 
                {
                    a =  nodes[i];
                    nodes[i] = nodes[j];
                    nodes[j] = a;
                }
            }
        }

    printf("Nodes :\n");
    fprintf(fpOut,"Nodes :\n");
    for (int i = 0; i < numOfNodes; i++)
    {
        printf("%d ",nodes[i]);
        fprintf(fpOut,"%d ",nodes[i]);
    }
    numOfGraphs = 3;
    //Allocate memory for graph
    graph = (int**)malloc(sizeof(int)*(numOfGraphs));//allocate memory for graph
    for (int i = 0; i < numOfGraphs; i++)
    {
        graph[i] = (int*)malloc(sizeof(int)*(numOfNodes));//allocate memory for graph
    }
    //Populate graph with nodes
    for (int i = 0; i < numOfNodes; i++)
    {
        graph[0][i]=nodes[i];
    }
    // Populate graph with node's values for first time
    for (int i = 0; i < numOfNodes; i++)
    {   
        if (graph[0][i]==VCC)
        {
            graph[1][i]=1;
        }
        else if (graph[0][i]==GND)
        {
            graph[1][i]=0;
        }
        else
        {
            graph[1][i]=2;
        }
        for (int j = 0; j < numOfTestIn; j++)
        {
            if (graph[0][i]==testIn[j])
            {
                graph[1][i]=testVector[j];
            }   
        }
    }
    // printGraph(fpOut);

    //Color Graph Main algorithm
    int currentGraph=2;
    int node0,node1,gate,mosType,numOfTries=0;
    bool shortCircuit = false;
    //Works until graph haw converged
    while (!converged)
    {      
        //check if short circuit was found
        if (shortCircuit)
        {
            printf("\n\nERROR: The circuit has a shortcircuit. Please try another!\n\n");
            printf("ShortCircuit in NODES %d - %d\n\n",node0,node1);
            printf("Press ENTER key to continue");
            fprintf(fpOut,"\n\nERROR: The circuit has a shortcircuit. Please try another!\n\n");
            fprintf(fpOut,"ShortCircuit in NODES %d - %d\n\n",node0,node1);
            getchar();
            break;
        }
        //calculate number of repetitions
        numOfTries++;
        // printf("\n\nRepetition %d Graph Values are: VCC=1 , GND=0 , X=2 , Z=3 , SC=4\n",numOfTries);
        // fprintf(fpOut,"\n\nRepetition %d Graph Values are: VCC=1 , GND=0 , X=2 , Z=3 , SC=4\n",numOfTries);
        sameAsPrev=0;
        
        //Add new Graph if needed and allocate more memory
        if (currentGraph == (numOfGraphs-1))
        {   
            numOfGraphs++;
            graph = (int**)realloc(graph,sizeof(int)*numOfGraphs);//reallocate memory for graph
            graph[currentGraph+1]=(int*)malloc(sizeof(int)*(numOfNodes));//allocate memory for graph
        }
        for (int i = 0; i < numOfNodes; i++)
        {
            if (graph[currentGraph-1][i]<2)
            {
                graph[currentGraph][i] = graph[currentGraph-1][i];
            }
            else
            {
                graph[currentGraph][i]=2;
            }
        }
        //Color the next node of the graph
        for (int i = 0; i < netlistRows; i++)
        {   
            mosType = netlist[i][0];
            gate = netlist[i][1];
            node0 = netlist[i][2];
            node1 = netlist[i][3];
            
            //Check for short circuits.
            if ((mosType==0) && (graph[currentGraph][findGraphPos(node0)]==1) && (graph[currentGraph][findGraphPos(node1)]==0) && (graph[currentGraph][findGraphPos(gate)]==0))
            {
                graph[currentGraph][findGraphPos(node0)]=4;
                //This is Short Circuit
                shortCircuit = true;
                break;
            }
            if ((mosType==0) && (graph[currentGraph][findGraphPos(node1)]==1) && (graph[currentGraph][findGraphPos(node0)]==0) && (graph[currentGraph][findGraphPos(gate)]==0))
            {
                graph[currentGraph][findGraphPos(node1)]=4;
                //This is Short Circuit
                shortCircuit = true;
                break;
            }
            if ((mosType==1) && (graph[currentGraph][findGraphPos(node0)]==1) && (graph[currentGraph][findGraphPos(node1)]==0) && (graph[currentGraph][findGraphPos(gate)]==1))
            {
                graph[currentGraph][findGraphPos(node0)]=4;
                //This is Short Circuit
                shortCircuit = true;
                break;
            }
            if ((mosType==1) && (graph[currentGraph][findGraphPos(node1)]==1) && (graph[currentGraph][findGraphPos(node0)]==0) && (graph[currentGraph][findGraphPos(gate)]==1))
            {
                graph[currentGraph][findGraphPos(node1)]=4;
                //This is Short Circuit
                shortCircuit = true;
                break;
            }
            //Color next node
            if ((mosType==0) && (graph[currentGraph][findGraphPos(node0)]==2) && (graph[currentGraph][findGraphPos(node1)]!=2) && (graph[currentGraph][findGraphPos(gate)]==0))
            {
                graph[currentGraph][findGraphPos(node0)]=graph[currentGraph][findGraphPos(node1)];
                break;
            }
            //Color next node
            if ((mosType==0) && (graph[currentGraph][findGraphPos(node1)]==2) && (graph[currentGraph][findGraphPos(node0)]!=2) && (graph[currentGraph][findGraphPos(gate)]==0))
            {
                graph[currentGraph][findGraphPos(node1)]=graph[currentGraph][findGraphPos(node0)];
                break;
            }
            //Color next node
            if ((mosType==1) && (graph[currentGraph][findGraphPos(node0)]==2) && (graph[currentGraph][findGraphPos(node1)]!=2) && (graph[currentGraph][findGraphPos(gate)]==1))
            {
                graph[currentGraph][findGraphPos(node0)]=graph[currentGraph][findGraphPos(node1)];
                break;
            }
            //Color next node
            if ((mosType==1) && (graph[currentGraph][findGraphPos(node1)]==2) && (graph[currentGraph][findGraphPos(node0)]!=2) && (graph[currentGraph][findGraphPos(gate)]==1))
            {
                graph[currentGraph][findGraphPos(node1)]=graph[currentGraph][findGraphPos(node0)];
                break;
            } 
        }
        currentGraph++;
        
        // printGraph(fpOut);

        //check if graph converged
        //###########################
        for (int i = 0; i < numOfNodes; i++)
        {
            if (graph[currentGraph-1][i]==graph[currentGraph-2][i])
            {
                sameAsPrev++;
            }
        }  
        if (sameAsPrev==numOfNodes)
        {
            printf("\nGraph converged\n\n");
            fprintf(fpOut,"\nGraph converged\n\n");
            
            converged =true;
            for (int i = 0; i < numOfTestOut; i++)
                {    
                    printf("Output Node %d = %d\n\n",testOut[i],(graph[currentGraph-1][findGraphPos(testOut[i])]));
                    fprintf(fpOut,"Output Node %d = %d\n\n",testOut[i],(graph[currentGraph-1][findGraphPos(testOut[i])]));
                    if ((graph[currentGraph-1][findGraphPos(testOut[i])])==2)
                    {
                        printf("ERROR: Node %d is Z! This is an open Circuit! Please fix this and try again!\n\n",testOut[i]);
                        fprintf(fpOut,"ERROR: Node %d is Z! This is an open Circuit! Please fix this and try again!\n\n",testOut[i]);
                        correctSims--;
                    }
                }
                correctSims++;
            printf("Press ENTER key to continue");
            getchar();
        }
        //###########################
        //End check
    }
    return;
}
//Find the position of a node
int findGraphPos(int nodeVal)
{
    for (int i = 0; i < numOfNodes; i++)
    {
        if (graph[0][i]==nodeVal)
            {   
                    return i;    
            }   
    }
    return -1;
}
void printGraph(FILE* fpOut)
{   //Print graph
        //#########################
        for (int i = 0; i < numOfGraphs-1; i++)
        {   

            printf("\nGraph%d: ",i);
            fprintf(fpOut,"\nGraph%d: ",i);
            for (int j = 0; j < numOfNodes; j++)
            {
                printf(" %d",graph[i][j]);
                fprintf(fpOut," %d",graph[i][j]);
            }
        }
        //###########################
}