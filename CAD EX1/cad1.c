#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

void reqA(int deg, float coef[]);//Requirment A function
void reqB(int deg, float coef[]);//Requirment B function
float polynomial_derivative(int deg, float coef[], float x, bool is_der);//Find polynomial value or derivative
int repetitions,add_subs,multiplications,divisions;

int main(){

    int deg; // Polynomial's degree
    int max_deg = 5; // maximum degree of input polynomial
    float coef[6];//coefficients array
    while (deg <=0 || deg > max_deg) // if polynomial degree is 0 to 5 is accepted else enter again degree
    {
        printf("Enter polynomial's degree: ");
        scanf("%d",&deg);
    }
    printf("Enter Polynomial's coefficients: \n");
    for (int i = 0; i <= deg; i++)//get coefficients
    {
        printf("Give coefficient x^%d: ",deg-i);
        scanf("%f",&coef[i]);
    }
    printf("\nPolynomial is: ");//print polynomial
    for (int i = 0; i <= deg; i++)
    {
        printf("(%f)*x^%d",coef[i],deg-i);
        if (i<deg)
        {
           printf(" + ");
        }
    }
    printf("\n\n-------------------Requirment A--------------------\n");
    reqA(deg,coef);// Requirement A, with analytical derivative
    printf("\n\n-------------------Requirment B--------------------\n");
    reqB(deg,coef);// Requirement B, with arethmetic derivative
    
}

void reqA(int deg, float coef[]){
    //Initialize statistics
    repetitions=0;
    add_subs=0;
    multiplications=0;
    divisions=0;
    float value,derivative,h,e=0.001;
    //starting X0
    float x=10;
    //Basic funtion characteristics
    for (int i = 0; i < 20; i++)
    {   
        repetitions++;
        value = polynomial_derivative(deg,coef,x,false);//find the P(x)
        derivative = polynomial_derivative(deg-1,coef,x,true);//find the P'(x)
        h = -(value/derivative);//find the value to move x
        multiplications++;
        divisions++;
        x += h;//move x by adding h
        add_subs++;
        printf("\nx%d = %f",i,x);
        printf("\nP(%f)=%f",x,value);
        if (fabs(h)<e)//if h is less than error we found an acceptable solution
            break;
        
    }
        printf("\nAnalytical derivation: x = %f",x);
        printf("\nRepetitions are: %d\nAdditions and Substractions are: %d\nMultiplications are: %d\nDivisions are: %d",repetitions,add_subs,multiplications,divisions);
}
void reqB(int deg, float coef[]){
    //Initialize statistics
    repetitions=0;
    add_subs=0;
    multiplications=0;
    divisions=0;
    float value1,value2,derivative,h,e=0.001,delta=0.01;
    //starting X0
    float x=10;
    //Basic funtion characteristics
    for (int i = 0; i < 20; i++)
    {
        repetitions++;
        value1 = polynomial_derivative(deg,coef,x,false);//find the P(x)
        value2 = polynomial_derivative(deg,coef,x+delta,false);//find the P(x+delta)
        derivative = ((value2-value1)/delta);//find the P'(x)
        divisions++;
        add_subs++;
        h = -(value1/derivative);//find the value to move x
        multiplications++;
        divisions++;
        x += h;//move x by adding h
        add_subs++;
        printf("\nx%d = %f",i,x);
        printf("\nP(%f)=%f",x,value1);
        if (fabs(h)<e)//if h is less than error we found an acceptable solution
            break;
        
    }
        printf("\nArithmetic derivation x = %f\n",x);
        printf("\nRepetitions are: %d\nAdditions and Substractions are: %d\nMultiplications are: %d\nDivisions are: %d",repetitions,add_subs,multiplications,divisions);

    
}
//calculate the value of the polynomial or the value of the derivative.
float polynomial_derivative(int deg, float coef[], float x ,bool is_der){
    //initialization
    float value=0,power=1;
    //do the above for every coefficient
    for (int i = deg; i >= 0; i--)
    {
        power=1;
        //calculate the power of x
        for (int j = 0; j < i; j++)
        {
            power  *= x;
            multiplications++;
        }
        //if is_der == True calculate derivative else calcualte polynomial.
        if (is_der)
        {
            //calculate P(x)
            value += power * coef[deg-i] * (float)(i+1);
            multiplications +=2;
            add_subs+=2;
        }
        else
        {
            //calculate P'(x)
            value += power * coef[deg-i];
            add_subs++;
            multiplications++;
        }
        
    }
    return value;
}