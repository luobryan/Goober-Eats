#include "provided.h"
#include "ExpandableHashMap.h"
#include <string>
#include <vector>
#include <functional>
#include <fstream> //is this allowed?
#include <iostream> //is this allowed?
#include <set>

using namespace std;

unsigned int hasher(const GeoCoord& g)
{
   return std::hash<std::string>()(g.latitudeText + g.longitudeText);

}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
private:
    ExpandableHashMap<GeoCoord,vector<StreetSegment> > data;
 
};

StreetMapImpl::StreetMapImpl()
{
}

StreetMapImpl::~StreetMapImpl()
{
    
}

bool StreetMapImpl::load(string mapFile)
{
    //do this
    ifstream infile(mapFile);
    if (!infile){

       return false;
    }

    string streetName;
    while(getline(infile,streetName)){

        int numLinesFollowing;
        infile >> numLinesFollowing;
        infile.ignore(10000,'\n');
        for (int a = 0; a < numLinesFollowing; a++){
            string segment;
            getline(infile,segment);
            string one = "";
            string two = "";
            string three = "";
            string four = "";
            int b = 0;
            while (b!=segment.size()&&segment[b]!=' '){
                one+=segment[b];
                b++;
            }
            b++;
            while (b!=segment.size()&&segment[b]!=' '){
                two+=segment[b];
                b++;
            }
            b++;
            while (b!=segment.size()&&segment[b]!=' '){
                three+=segment[b];
                b++;
            }
            b++;
            while (b!=segment.size()&&segment[b]!=' '){
                four+=segment[b];
                b++;
            }
            //for forward
            GeoCoord forward_start = GeoCoord(one,two);
            GeoCoord forward_end = GeoCoord(three,four);
            vector<StreetSegment>* associatedVector = data.find(forward_start);
            if(associatedVector == nullptr){
                vector<StreetSegment> toPutIn;
                toPutIn.push_back(StreetSegment(forward_start,forward_end,streetName));
                data.associate(forward_start,toPutIn);
            }
            else{
                associatedVector->push_back(StreetSegment(forward_start,forward_end,streetName));
            }
            //for reverse
            GeoCoord reverse_start = GeoCoord(three,four);
            GeoCoord reverse_end = GeoCoord(one,two);
            vector<StreetSegment>* associatedVector_reverse = data.find(reverse_start);
            if(associatedVector_reverse == nullptr){
                vector<StreetSegment> toPutIn;
                toPutIn.push_back(StreetSegment(reverse_start,reverse_end,streetName));
                data.associate(reverse_start,toPutIn);
            }
            else{
                associatedVector_reverse->push_back(StreetSegment(reverse_start,reverse_end,streetName));
            }
        }
    }
    return true;
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    
    const vector<StreetSegment>* vec = data.find(gc);
    if (vec==nullptr){
        return false;
    }
    else{
        segs.clear();
        segs = *vec;
        return true;
    }

}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}
