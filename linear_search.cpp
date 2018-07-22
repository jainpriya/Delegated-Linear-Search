/*Author:Priya Jain(22)
		 Radha Verma(24)

Distributed linear system using pipe in unix.

Input:File name which is to be read
	  Element to be searched
Approach:The process will be divided into child process looking in one half of the file and the other child looking in other half.
		 More child processes will be recursively called depending upon the size of array.If less than 5 the file will be looked ny simple linear search means.
output: index of the elemnt if found else element not found
*/		 

#include<iostream>
#include<fstream>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>		//for wait function
#include<stdlib.h>
#include<signal.h>
using namespace std;

int arr[1000];			//global array to store file contents

//read file path and store the contents in arrray

int readfile(char argv[])
{
	FILE*fptr;
	char str[5000];
	

	fptr=fopen(argv,"r");
	if(fptr==NULL)
	{
		cout<<"cannot open file\n";
		exit(0);
	}
	char ch=fgetc(fptr);
	int count=0;
	
	while(ch!=EOF)
	{
		str[count++]=ch;
		ch=fgetc(fptr);
	}
	char* pt;
	count=0;
	pt=strtok(str,"\n");
	while(pt!=NULL)
	{
		arr[count++]=atoi(pt);
		pt=strtok(NULL,"\n");
	}
	fclose(fptr);
	return count-1;

}

//search element if less than 5 elemnts are left in the child process

int search(int left,int right,int element)
{
	for(int i=left;i<=right;i++)
	{
		if(arr[i]==element)
			return(i+1);
	}
	return(0);
}

//searching in parent process using fork()

int linear_search(int left,int right,int element)
{
	
	int ret_val2;
	if((right-left)>5)
	{	
		int fd[2];  //for pipe reader and writer process
		if(pipe(fd)==-1)
		{
		//error in opening pipe
			cout<<"pipe failed";
			exit(0);
		}
			pid_t child1,child2;
			child1=fork();
			if(child1==0)
			{
		        	int ret_val;
				close(1);
				close(fd[0]);
				ret_val= linear_search(0,(left+right)/2,element);
				if(ret_val!=0)
					write(fd[1],&ret_val,sizeof(ret_val));
				close(fd[1]);
				exit(0);
			}

			else
			{
				child2=fork();
				if(child2==0)
				{	
					int ret_val;	
					close(1);
					close(fd[0]);
					ret_val= linear_search((left+right)/2+1,right,element);
					if(ret_val>0)
						write(fd[1],&ret_val,sizeof(ret_val));
					close(fd[1]);
					exit(0);
				}
				wait(NULL);
				wait(NULL);
				close(fd[1]);
				int count=read(fd[0],&ret_val2,sizeof(ret_val2));
				close(fd[0]);
				return ret_val2;
				
				
			}
		}
	else
		return search(left,right,element);
	return(0);
}


int main(int argc, char *argv[])
{	
	int ret_val2;
	int element,size;
	char *fname;
	if(argc>3){
		cout<<"Invalid arguments\n";
	}	
    
	if(argv[1] != NULL)			
	{
        	cout<<"Looking for file: "<< argv[1]<<endl;
			size=readfile(argv[1]);
			element=atoi(argv[2]);
       	}

    

	int fd[2];  //for pipe reader and writer process
	if(pipe(fd)==-1)
	{
		//error in opening pipe
		cout<<"pipe failed";
		exit(0);
	}
	pid_t child1,child2;
	child1=fork();

	if(child1==0)
	{
		
    	int ret_val;
		close(1);// closes the file descriptor associated with standard output for the current process,
		close(fd[0]);//closes the read end of the pipe
		ret_val= linear_search(0,size/2,element);
		if(ret_val!=0)
			write(fd[1],&ret_val,sizeof(ret_val));
		close(fd[1]);
		exit(0);
	}

	else
	{
		child2=fork();
		if(child2==0)
		{	
			int ret_val;	
			close(1);
			close(fd[0]);
			ret_val= linear_search(size/2+1,size-1,element);
			if(ret_val>0)
				write(fd[1],&ret_val,sizeof(ret_val));
			close(fd[1]);
			exit(0);
		}
		wait(NULL);
		wait(NULL);
		close(fd[1]);
		read(fd[0],&ret_val2,sizeof(ret_val2));
		
		close(fd[0]);
		if(!ret_val2)
			cout<<"element not found"<<endl;
		else
			{
			cout<<"element found at position "<<ret_val2<<endl;
		       
		}
		kill(0,SIGKILL);	
}
	return 0;
}
