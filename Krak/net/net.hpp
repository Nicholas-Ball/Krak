#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#endif


//checks if file exits
inline bool Exists (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

//runs command without popup cross platform
void run(const char* command)
{
    #ifdef __linux__
				system(command);
    #elif _WIN32
        WinExec(command,SW_HIDE);
    #else
    #endif
}

class net{
    private:
        //resposne of curl
        std::string Data;

        //status code
        std::string Code;
    public:
        //constructor
        net()
        {
          srand(time(NULL));
        }

        //Get request
        void Get(std::string url,int delay = 0)
        {
          int r1 = (int)(rand() % 9999999999);
          int r2 = (int)(rand() % 9999999999);

          std::string f1 = std::to_string(r1) + ".tmp";
          std::string f2 = std::to_string(r2) + ".tmp";
          srand(r2);

          //get url
          const std::string command = ("curl -m 2 -X GET -s -o "+f1+" -w \"%{http_code}\" \""+url+"\" > "+f2);
          run(command.c_str());

          //ensure data is writen to files before continuing
          std::this_thread::sleep_for(std::chrono::milliseconds(delay));

          std::ifstream file(f1);
          std::ifstream status(f2);

          //get status code
          getline(status,this->Code);
          status.close();

          this->Data = "";
          std::string line;

          //add response to data var
          while(getline(file,line))
          {
            this->Data += line;
          }

          if(this->Data == "")
          {
            //ensure data is writen to files before continuing
            std::this_thread::sleep_for(std::chrono::milliseconds(131));
            Get(url,delay = 500);
          }

          //close files and delete them
          file.close();
          remove(f1.c_str());
          remove(f2.c_str());
        }

        //Post request
        void Post(std::string url, std::string data,int delay = 0)
        {

          int r1 = (int)(rand() % 9999999999);
          int r2 = (int)(rand() % 9999999999);

          std::string f1 = std::to_string(r1) + ".tmp";
          std::string f2 = std::to_string(r2) + ".tmp";
          srand(r2);

          //Post url
          const std::string command = ("curl -X POST -d "+data+" -s -o "+f1+" -w \"%{http_code}\" \""+url+"\" > "+f2);
          run(command.c_str());

          //ensure data is writen to files before continuing
          std::this_thread::sleep_for(std::chrono::milliseconds(delay));

          std::ifstream file(f1);
          std::ifstream status(f2);

          //get status code
          getline(status,this->Code);
          status.close();

          this->Data = "";
          std::string line;

          //add response to data var
          while(getline(file,line))
          {
            this->Data += line;
          }

          if(this->Data == "")
          {
            //ensure data is writen to files before continuing
            std::this_thread::sleep_for(std::chrono::milliseconds(131));
            Post(url,data,delay = 500);
          }

          //close files and delete them
          file.close();
          remove(f1.c_str());
          remove(f2.c_str());
        }

        //Response accsessor
        std::string Response()
        {
            return this->Data;
        }

        //Status Code accsessor
        std::string StatusCode()
        {
            return this->Code;
        }
};
