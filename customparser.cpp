// Complete me
#include "customparser.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

const char* error_msg_1 = "Cannot read integer n";
const char* error_msg_2 = "Error in content parsing";
const char* error_msg_3 = "Error in user parsing";

// To Do - Complete this function
void CustomFormatParser::parse(std::istream& is, std::vector<Content*>& content, std::vector<User*>& users)
{
    // Erase any old contents
    content.clear();
    users.clear();
    
    unsigned int numContentItems;
    int temp;
    string throwAway;

    // TO DO - Add your code below.

    is >> temp; // try

    if(is.fail()) // check
    {
        throw ParserError(error_msg_1); // cant read int n
    } 

    numContentItems = temp; // use

    for (size_t i = 0; i < numContentItems; i++)
    {
        string stemp;
        int ContentID;
        int ContentType;
        string ContentName;

        is >> temp;

        if(is.fail())
        {
          throw ParserError(error_msg_2);
        }

        ContentID = temp;

        is >> temp;

        if(is.fail())
        {
          throw ParserError(error_msg_2);
        }

        ContentType = temp;


        getline(is, throwAway);

        getline(is, throwAway, '\t'); // get rid of tab

        cout << "throwAway: " << throwAway << endl;

        getline(is, stemp); // read in content name into temp string

        if(is.fail())
        {
            throw ParserError(error_msg_2);
        }

        ContentName = stemp;
 
        if(ContentType == 0) // ie if its a movie
        {
          
          int array[3];
          int tempArray[3];

          for(size_t i = 0; i < 3; i++)
          {
              is >> tempArray[i];

              if(is.fail())
              {
                  throw ParserError(error_msg_2);
              }

              array[i] = tempArray[i];
          }

          Content* movie = new Movie(ContentID, ContentName, array[0], array[1], array[2]);

          content.push_back(movie);
        }

        else if(ContentType == 1) // ie if its a series
        {
          int array[4];
          int tempArray[4];

          for(size_t i = 0; i < 4; i++)
          {
              is >> tempArray[i];

              if(is.fail())
              {
                  throw ParserError(error_msg_2);
              }

              array[i] = tempArray[i];
          }

          Content* series = new Series(ContentID, ContentName, array[0], array[1], array[2], array[3]);

          content.push_back(series);
        }
        else
        {
          cout << "Content Type: " << ContentType << endl;
          throw ParserError(error_msg_2);
        }

        getline(is, throwAway); // throw away newline char

        getline(is, stemp); // copy line w user info into temp

        stringstream ss2(stemp);

        string userViewed;

        while(ss2 >> userViewed)
        {
          content[i]->addViewer(userViewed);
        }
    }

    // now we will move on to reading in users information

    string userName;
    int ratinglmt;
    string temp3;
    
    
    while(is >> userName)
    {

      is >> ratinglmt;
      if (is.fail())
      {
        throw ParserError(error_msg_3);
      }
      User* cuser = new User(userName, ratinglmt);

      users.push_back(cuser);

      getline(is, throwAway); // throw away new line char

      getline(is, temp3); // copy in user's watch history

      stringstream ss3(temp3); // put watch history into ss3

      int watchedID;

      while(ss3 >> watchedID)
      {
        cuser->addToHistory(watchedID);
      }
    }

 

}