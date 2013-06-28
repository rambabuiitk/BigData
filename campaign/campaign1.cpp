#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <time.h>

using namespace std;

typedef std::vector<std::string> StringVector;

void Split( std::string sInput, const std::string& sChars, StringVector& vResult )
{
		std::string sTemp;
		size_t found;
		found = sInput.find_first_of( sChars );

		while ( found != std::string::npos )
		{
				sTemp = sInput.substr ( 0, found );
				vResult.push_back( sTemp );
				sInput.erase( 0, found + 1 );
				found = sInput.find_first_of( sChars );
		}
}

int StringToNum( const std::string& sInput )
{
        std::stringstream ss;
        ss << sInput;
        int nValue;
        ss >> nValue;
        return nValue;
}

int main(int argc, char *argv[])
{
	int 			count = 0;							/* match count for each line */
	int 			maxmatch = 0;						/* max match for outout campaign */
	string name;
	
	if (argc < 2) 
    {
        cout << "Please enter a file path\n";
        return -1;
    }

	
	std::ifstream ifsReadFile( argv[1] );
    if( ifsReadFile )
    {
		cout<<"Wait for user to type in the input data ..."<<endl;
		string input_line;
        getline(cin, input_line);
		StringVector vInput;
		StringVector vOutput;
		Split( input_line, " ", vInput );
		std::string sFileLine;
        while( getline( ifsReadFile, sFileLine ) )
        {
			StringVector vResult;
			Split( sFileLine, " ", vResult );
			int map[65536];
			memset(map, 0, sizeof(int)*65536);
			for(int i=1;i<vResult.size();i++)
			{
				int num = StringToNum(vResult[i]);
				map[num] = 1;
			}
			for(int j=0;j<vInput.size();j++)
			{
				int num = StringToNum(vInput[j]);
				if(map[num] == 1)
					count++;	
			}
			
			if(count > maxmatch || (count == maxmatch && count!= 0))
			{
				maxmatch = count;
				count = 0;
				vOutput.push_back( vResult[0] );
				name = vResult[0];
			}
			else
			{
				count = 0;
			}
		}
		if(maxmatch == 0)
		{
			cout<<"No campaign is matched "<<endl;
		}
		else
		{
			srand ( time(NULL) );
			int index = rand() % vOutput.size();
			cout<<"Output Campaign is : "<<vOutput[index]<<name<<endl;
		}

	}
	else
	{
		std::cerr << "Error: Cannot open the file." << std::endl;
        return -1;
	}


	return 0;
}


