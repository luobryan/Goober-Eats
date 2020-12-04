#include "provided.h"
#include "ExpandableHashMap.h"
#include <queue>
#include <list>
#include <set>


using namespace std;

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* stMap;
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    stMap = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{

}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    totalDistanceTravelled = 0;
    route.clear();
    if(start==end){
        return DELIVERY_SUCCESS;
    }
    vector<StreetSegment> testValidityOfStartAndEnd;
    if(!stMap->getSegmentsThatStartWith(start,testValidityOfStartAndEnd)){
        return BAD_COORD;
    }
    if(!stMap->getSegmentsThatStartWith(end,testValidityOfStartAndEnd)){
        return BAD_COORD;
    }
 
    queue<GeoCoord> coordsToLookAt;
    ExpandableHashMap<GeoCoord,StreetSegment> locationOfSegmentGoingIn;
    set<GeoCoord> visited;
    
    coordsToLookAt.push(start);
    visited.insert(start);
 
    while(0==0){
        if(coordsToLookAt.empty()){
            return NO_ROUTE;
        }
        GeoCoord coordLookingAtNow = coordsToLookAt.front();
        coordsToLookAt.pop();
        if(coordLookingAtNow==end){
    
            
            route.clear();
            while(0==0){
                
                route.insert(route.begin(),StreetSegment(locationOfSegmentGoingIn.find(coordLookingAtNow)->start,coordLookingAtNow,locationOfSegmentGoingIn.find(coordLookingAtNow)->name));
               
                totalDistanceTravelled+=distanceEarthMiles(locationOfSegmentGoingIn.find(coordLookingAtNow)->start,coordLookingAtNow);
                coordLookingAtNow = locationOfSegmentGoingIn.find(coordLookingAtNow)->start;
                if(coordLookingAtNow==start){
                    return DELIVERY_SUCCESS;
                }
            }
        }
        vector<StreetSegment> adjacentStreetSegments;
        stMap->getSegmentsThatStartWith(coordLookingAtNow,adjacentStreetSegments);
        for (int a = 0; a < adjacentStreetSegments.size(); a++){
            if(visited.count(adjacentStreetSegments[a].end)==0){
                visited.insert(adjacentStreetSegments[a].end);
                coordsToLookAt.push(adjacentStreetSegments[a].end);
                locationOfSegmentGoingIn.associate(adjacentStreetSegments[a].end,adjacentStreetSegments[a]);
            }
        }
        
       
    }
    return DELIVERY_SUCCESS;
}
/*
 ///test

 
 
 
 //
 */
//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}
