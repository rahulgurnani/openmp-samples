#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "omp.h"

typedef long long int big;

extern int isPrime(big);

void eseive(const big num,
	char * arr);

void printPrimes(const big num,
	const char * arr);

void checkAccuracy(const big num, 
		const int numThreads);

void checkPerformance(const big num,
		const int numThreads);

double getWallTime()
{
    struct timeval time;
    if(gettimeofday(&time, NULL))
	return 0;
    double wallTime = (double)time.tv_sec + (double)time.tv_usec * 0.000001;
    return wallTime;
}

big createBigNumber(const int bits)
{
    big num = 0;
    srand(time(NULL));
    for(int i = 0; i < bits; ++i)
    {
	num = num << 1;
	big thisBit = (rand() % 2);
	num |= thisBit;
    }
    return num;
}

char * createArray(const big num)
{
    char * arr = (char *)malloc((num+1) * sizeof(char));
    memset(arr, 0, num * sizeof(char));
    return arr;
}

void destroyArray(char * arr)
{
    if(arr)
	free(arr);
    arr = NULL;
}


int main(int argc, char ** argv)
{
    if(argc < 5)
    {
	printf("Usage : ");
    }
    const int bits = atoi(argv[1]);
    if(bits < 0 || bits > 32)
    {
	printf("Number of bits should be within 0 to 32\n");
	return 0;
    }
    const int numThreads = atoi(argv[2]);
    const int checkAcc = atoi(argv[3]);
    const int checkPerf = atoi(argv[4]);

    const big num = createBigNumber(bits);
    
    printf("Evaluating for number %lld\n", num);

    if(num == 0)
	return 0;
    if(checkAcc && checkPerf)
	printf("Any one of accuracy or performance needs to be tested\n");
 
    if(checkAcc)
    {
	checkAccuracy(num, numThreads);
    }
    else if(checkPerf)
    {
	checkPerformance(num, numThreads);
    }
    return 1;
}

void eseive(const big num,
	char * arr)
{

//Fill in the Code
    arr[0] = 1;
    big curPrime = 1;
    int finished = 0;
    while(!finished)
    {
        #pragma omp parallel shared(arr)
        {
            #pragma omp single 
            {
                int i;
                int flagF = 1;
                for(i =  curPrime+1; i <=num ; i++)
                {
                    if(!arr[i])
                    {
                        flagF = 0;
                        curPrime = i;
                        break;
                    }
                }
                if(flagF)
                    finished = 1;
            }
            if(!finished)
            {
                big myPrime = curPrime;
                big till = (num/myPrime);
                int i;
                #pragma omp for schedule(static)
                for(i = 2 ; i<=till; i++)
                {
                    arr[i*myPrime] = 1;
                }
            }
        }
    }
    return;
}

void checkAccuracy(const big num, 
		const int numThreads)
{
    char * arr = createArray(num);
    omp_set_num_threads(numThreads);
    eseive(num, arr);
    //printPrimes(num, arr);
    for(big i = 3; i < num; ++i)
    {
	if(!arr[i])
	{
	    if(!isPrime(i))
		printf("VIOLATION : %lld is not prime\n", i);
	}
	else
	{
	    if(isPrime(i))
		printf("VIOLATION : %lld is prime\n", i);
	}
    }
    destroyArray(arr);
    return;
}

void checkPerformance(const big num,
		const int numThreads)
{
    char * arr = createArray(num);
    //Warm up cache
    omp_set_num_threads(1);
    eseive(num, arr);
    for(int i = 1; i <= numThreads; ++i)
    {
	omp_set_num_threads(i);
	const double startTime = getWallTime();
	eseive(num, arr);
	const double stopTime = getWallTime();
	printf("Threads %d\tTime %g\n", i, stopTime-startTime);
    }
    destroyArray(arr);
    return;
}

void printPrimes(const big num,
	const char * arr)
{
    printf("Primes are :\n");
    for(big i = 0; i < num; ++i)
    {
	if(!arr[i])
	    printf("%lld\n", i);
    }
    return;
}
