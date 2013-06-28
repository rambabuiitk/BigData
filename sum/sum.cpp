#include <iostream>
#include <fstream>
#include <pthread.h>
#include <string>
#include <string.h>


using namespace std;

string filepath;
unsigned int READBLOCKSIZE = 1024*1024;
unsigned long long nFileLength = 0;

unsigned long long accumulator = 0; 
unsigned int seekIndex[8] = {};
unsigned int threadBlockSize = 0; 
unsigned long long acc[8] = {};

pthread_t thread[8];
void* threadFunc(void* pThreadNum);

time_t seconds1;
time_t seconds2;

int main(int argc, char *argv[])
{
    if (argc < 2) 
    {
        cout << "Please enter a file path\n";
        return -1;
    }

    seconds1 = time (NULL);
   
    string path(argv[1]);
    filepath = path;
    ifstream ifsReadFile(filepath.c_str(), ifstream::binary);  // Create FileStream for the file to be read
    if(0 == ifsReadFile.is_open()) 
    {
        cout << "Could not find/open input file\n";
        return -1;
    }

    ifsReadFile.seekg (0, ios::end);
    nFileLength = ifsReadFile.tellg();           // get file size
    ifsReadFile.seekg (0, ios::beg);

    if(nFileLength < 16*READBLOCKSIZE)
    {
        cout << "Using One Thread "<<endl;
        char* readBuf = new char[READBLOCKSIZE];
        if(0 == readBuf) return -1;

        unsigned int startOffset = 0;   
        if(nFileLength >  READBLOCKSIZE)
        {
            while(startOffset + READBLOCKSIZE < nFileLength)
            {
                ifsReadFile.read(readBuf, READBLOCKSIZE);  
                int* num = reinterpret_cast<int*>(readBuf);
                for(unsigned int i = 0 ; i < (READBLOCKSIZE/4) ; i++) 
                {
                    accumulator += *(num + i);  
                }
                startOffset += READBLOCKSIZE;
            }

        }

        if(nFileLength - (startOffset) > 0)
        {
            ifsReadFile.read(readBuf, nFileLength - (startOffset));  
            int* num = reinterpret_cast<int*>(readBuf);
            for(unsigned int i = 0 ; i < ((nFileLength - startOffset)/4) ; ++i) 
            {
                accumulator += *(num + i);  
            }
        }
        delete[] readBuf; readBuf = 0;
    }
    else
    {
        cout << "Using 8 Threads"<<endl;
        unsigned int currthreadnum[8] = {0,1,2,3,4,5,6,7};
        if(nFileLength > 200000000) READBLOCKSIZE *= 16; // read larger blocks
        
        if(nFileLength % 28)
        {
            threadBlockSize = (nFileLength / 28);
            threadBlockSize *= 4;
        }
        else
        {   
            threadBlockSize = (nFileLength / 7);
        }

        for(int i = 0; i < 8 ; ++i)
        {
            seekIndex[i] = i*threadBlockSize;
        }
        pthread_create(&thread[0], NULL, threadFunc, (void*)(currthreadnum + 0));
        pthread_create(&thread[1], NULL, threadFunc, (void*)(currthreadnum + 1));
        pthread_create(&thread[2], NULL, threadFunc, (void*)(currthreadnum + 2));
        pthread_create(&thread[3], NULL, threadFunc, (void*)(currthreadnum + 3));
        pthread_create(&thread[4], NULL, threadFunc, (void*)(currthreadnum + 4));
        pthread_create(&thread[5], NULL, threadFunc, (void*)(currthreadnum + 5));
        pthread_create(&thread[6], NULL, threadFunc, (void*)(currthreadnum + 6));
        pthread_create(&thread[7], NULL, threadFunc, (void*)(currthreadnum + 7));

        pthread_join(thread[0], NULL);
        pthread_join(thread[1], NULL);
        pthread_join(thread[2], NULL);
        pthread_join(thread[3], NULL);
        pthread_join(thread[4], NULL);
        pthread_join(thread[5], NULL);
        pthread_join(thread[6], NULL);
        pthread_join(thread[7], NULL);

        for(int i = 0; i < 8; ++i)
        {
            accumulator += acc[i];
        }
    }

    seconds2 = time (NULL);
    
    cout << "Total time to add " << nFileLength/4 << " integers -> " << seconds2 - seconds1 << " seconds\n";

    cout << "Sum is"<< accumulator << "\n";      
    return 0;
}

void* threadFunc(void* pThreadNum)
{
    unsigned int threadNum = *reinterpret_cast<int*>(pThreadNum);
    char* localReadBuf = new char[READBLOCKSIZE];
    unsigned int startOffset = seekIndex[threadNum];
    ifstream ifs(filepath.c_str(), ifstream::binary);
    if(0 == ifs.is_open()) 
    {
        cout << "Could not find/open input file\n";
        return 0;
    }   
    ifs.seekg (startOffset, ios::beg); // Seek to the correct offset for this thread
    acc[threadNum] = 0;
    unsigned int endOffset = startOffset + threadBlockSize;
    if(endOffset > nFileLength) endOffset = nFileLength; // for last thread
    
    if((endOffset - startOffset) >  READBLOCKSIZE)
    {
        while(startOffset + READBLOCKSIZE < endOffset)
        {
            ifs.read(localReadBuf, READBLOCKSIZE);  
            int* num = reinterpret_cast<int*>(localReadBuf);
            for(unsigned int i = 0 ; i < (READBLOCKSIZE/4) ; i++) 
            {
                acc[threadNum] += *(num + i);   
            }
            startOffset += READBLOCKSIZE;
        }   
    }

    if(endOffset - startOffset > 0)
    {
        ifs.read(localReadBuf, endOffset - startOffset);
        int* num = reinterpret_cast<int*>(localReadBuf);
        for(unsigned int i = 0 ; i < ((endOffset - startOffset)/4) ; ++i) 
        {
            acc[threadNum] += *(num + i);   
        }
    }

    
    delete[] localReadBuf; localReadBuf = 0;
    return 0;
}
