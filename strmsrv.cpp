#include "strmsrv.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
using namespace std;

// To do - Complete this function
StreamService::StreamService()

{
  cUser_ = NULL;
}

// To do - Complete this function
StreamService::~StreamService()
{

    for (size_t i = 0; i < users_.size(); i++)
    {
        delete users_[i];
    }

    for (size_t i = 0; i < content_.size(); i++)
    {
        delete content_[i];
    }
    
    users_.clear();
    
    content_.clear();

}

// Complete - Do not alter
void StreamService::readAndParseData(std::istream& is, Parser& p)
{
    p.parse(is, content_, users_);
    cout << "Read " << content_.size() << " content items." << endl;
    cout << "Read " << users_.size() << " users." << endl;
}

// To do - Complete this function
void StreamService::userLogin(const std::string& uname)
{
    bool validUser = false;
    int userIndex = -1;

    for (unsigned int i = 0; i < users_.size(); i++)
    {
        if(uname == users_[i]->uname) // if user id matches then its valid user
        {
            validUser = true;
            userIndex = i;
        }
    }

    if(validUser == false) // if valid user is false after checking users vector, cant proceed
    {
        throw invalid_argument("Username is not valid");
    }

    if(cUser_ != NULL) // a user is already logged in
    {
        throw runtime_error("Please log out of current user first");
    }


    // if program reaches this point, uname is valid and no current user exists

    cUser_ = users_[userIndex];

}

// To do - Complete this function
void StreamService::userLogout()
{
    if (cUser_ == NULL)
    {
        return; // if no user is logged in, nothing to do
    }
    else if (cUser_ != NULL)
    {
        cUser_ = NULL; // if user was logged in, now they are logged out
    }

}

// To do - Complete this function
std::vector<CID_T> StreamService::searchContent(const std::string& partial) const
{
    std::vector<CID_T> results;
    for(size_t i = 0; i < content_.size(); i++){
        // TO DO - modify this to also push back when the string 'partial'
        //  is contained in the name of the current content. Lookup the string
        //  class documentation to find an appropriate function that can be used
        //  to do this simply.
        if(partial == "*" || content_[i]->name().find(partial, 0) != string::npos){
            results.push_back(i);
        }        
    }
    return results;
}

// Complete - Do not alter
std::vector<CID_T> StreamService::getUserHistory() const
{
    throwIfNoCurrentUser();
    return cUser_->history;
}

// To do - Complete this function
void StreamService::watch(CID_T contentID)
{
    throwIfNoCurrentUser(); // user must be logged in

    bool validCID = false;
    int CIDindex;

    for(size_t i = 0; i < content_.size(); i++)
    {
        if (contentID == content_[i]->id())
        {
            validCID = true;
            CIDindex = i;
        }
    }

    if(validCID == false) // if validCID is false, that means CID did not match any in our vector database
    {
        throw ReviewRangeError("Invalid Content ID.");
    }

    if(content_[CIDindex]->rating() > cUser_->ratingLimit)
    {
        throw RatingLimitError("Exceeds Rating Limit");
    }

    cUser_->history.push_back(contentID); // update appropriate data structures
    content_[CIDindex]->addViewer(cUser_->uname);

}

// To do - Complete this function
void StreamService::reviewShow(CID_T contentID, int numStars)
{
    throwIfNoCurrentUser();
    
    bool CIDvalid = false;
    int CIDindex;

    for (size_t i = 0; i < content_.size(); i++)
    {
        if (contentID == content_[i]->id())
        {
            CIDvalid = true;
            CIDindex = i;
        }
    }

    if(CIDvalid == false || numStars < 0 || numStars > 5)
    {
        throw ReviewRangeError("Check CID or Star Rating");
    }

    content_[CIDindex]->review(numStars);

}

// To do - Complete this function
CID_T StreamService::suggestBestSimilarContent(CID_T contentID) const
{
    // first we will store the set of users who 
    // watched the same content in the below vector

    vector<User*> usersWhoAlsoWatched;

    for (size_t i = 0; i < users_.size(); i++)
    {
        for (size_t j = 0; j < users_[i]->history.size(); j++)
        {
            if (contentID == users_[i]->history[j] && users_[i] != cUser_) //if content ID is present in users history
            {
                usersWhoAlsoWatched.push_back(users_[i]); //add user to vector
            }
        }
    }

    // next we will find the most viewed content to return in 2 steps 

    // step 1: place all CIDs that dont match original AND arent viewed by cUser in a vector

    vector<CID_T> viewedCID; // vector to store CID that meet criteria

    for (size_t i = 0; i < usersWhoAlsoWatched.size(); i++)
    {
        for (size_t j = 0; j < usersWhoAlsoWatched[i]->history.size(); j++)
        {
            if (contentID != usersWhoAlsoWatched[i]->history[j]) // if content is not the same as one passed in
            {
                bool hasNotSeen = true;

                for(size_t x = 0; x < cUser_->history.size(); x++)
                {
                
                    if(usersWhoAlsoWatched[i]->history[j] == cUser_->history[x]) // and content is not viewed by cUser
                    {
                        hasNotSeen = false;
                    }
                }

                if(hasNotSeen == true)
                {   
                    // only content that is not the same as the one passed in and not viewed by cUser will be added
                    viewedCID.push_back(usersWhoAlsoWatched[i]->history[j]); 
                }
                
            }

        }
    }

    // step 2: go through the vector and find most frequent ID

    if(viewedCID.size() != 0) //if there is at least one CID that met criteria
    {
        int maxCount = 0;
        CID_T retval;

        for (size_t i = 0; i < viewedCID.size(); i++)
        {
            int count = 0;
            for (size_t j = 0; j < viewedCID.size(); j++)
            {
                if (viewedCID[i] == viewedCID[j])
                {
                    count++;
                }
            }

            if (count > maxCount)
            {
                maxCount = count;
                retval = viewedCID[i];
            }
        }   

        return retval;

    }
    else
    {
        return -1; // there is nothing that met criteria
    }

    
   
}

// To do - Complete this function
void StreamService::displayContentInfo(CID_T contentID) const
{
    // Do not alter this
    if(! isValidContentID(contentID)){
        throw std::invalid_argument("Watch: invalid contentID");
    }

    // Call the display abitlity of the appropriate content object



    for (size_t i = 0; i < content_.size(); i++)
    {
        if(contentID == content_[i]->id())
        {
            content_[i]->display(cout);
        }

    }

   // complete to display
}

// Complete - Do not alter
bool StreamService::isValidContentID(CID_T contentID) const
{
    return (contentID >= 0) && (contentID < (int) content_.size());
}

// Complete - Do not alter
void StreamService::throwIfNoCurrentUser() const
{
    if(cUser_ == NULL){
        throw UserNotLoggedInError("No user is logged in");
    }
}

// Complete - Do not alter
int StreamService::getUserIndexByName(const std::string& uname) const
{
    for(size_t i = 0; i < users_.size(); i++){
        if(uname == users_[i]->uname) {
            return (int)i;
        }
    }
    return -1;
}
