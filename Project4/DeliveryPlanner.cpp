#include "provided.h"
#include <vector>
#include <string>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* stMap;
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
{
    stMap = sm;
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    //set up data members
    commands.clear();
    totalDistanceTravelled = 0;
    
    //check empty map
    if (deliveries.size()==0){
        return DELIVERY_SUCCESS;
    }
    
    //optimize
    DeliveryOptimizer delOpt(stMap);
    vector <DeliveryRequest> optimizedDeliveries = deliveries;
    double oldCrowsDist = 0;
    double newCrowsDist = 0;
    delOpt.optimizeDeliveryOrder(depot,optimizedDeliveries, oldCrowsDist, newCrowsDist);
    
    
    //handle all deliveries, starting from depot
    GeoCoord startingPoint = depot;
    for (int a = 0; a < optimizedDeliveries.size(); a++){
        list<StreetSegment> segmentsToDeliveryLoc;
        
        PointToPointRouter ptpr(stMap);
        double tempDist;
        DeliveryResult result = ptpr.generatePointToPointRoute(startingPoint,optimizedDeliveries[a].location,segmentsToDeliveryLoc,tempDist);
        if(result==BAD_COORD){
            return BAD_COORD;
        }
        if(result==NO_ROUTE){
            return NO_ROUTE;
        }
        totalDistanceTravelled+=tempDist;
        list<StreetSegment>::iterator ir = segmentsToDeliveryLoc.begin();
        if(segmentsToDeliveryLoc.size()==0){ //in case you are already at the delivery location
            DeliveryCommand dc2;
            dc2.initAsDeliverCommand(optimizedDeliveries[a].item);
            commands.push_back(dc2);
        }
        while (ir!=segmentsToDeliveryLoc.end()){
            //handle proceed
            double amtToProceed = 0;
            string nameWeAreOn = ir->name;
            double dirToGo = angleOfLine(*ir);
            StreetSegment leaveOffOn;
            string dir;
            if (0<=dirToGo&&dirToGo<22.5){
                dir = "east";
            }
            if (22.5<=dirToGo&&dirToGo<67.5){
                dir = "northeast";
            }
            if (67.5<=dirToGo&&dirToGo<112.5){
                dir = "north";
            }
            if (112.5<=dirToGo && dirToGo< 157.5){
                dir = "northwest";
            }
            if (157.5<=dirToGo&&dirToGo<202.5){
                dir = "west";
            }
            if (202.5<=dirToGo&&dirToGo<247.5){
                dir = "southwest";
            }
            if (247.5<=dirToGo&& dirToGo<292.5){
                dir = "south";
            }
            if (292.5<=dirToGo&&dirToGo<337.5){
                dir = "southeast";
            }
            if (dirToGo>=337.5){
                dir = "east";
            }
            while(ir!=segmentsToDeliveryLoc.end()&&ir->name==nameWeAreOn){
                amtToProceed += distanceEarthMiles(ir->start,ir->end);
                leaveOffOn = *ir;
                ir++;
            }
            DeliveryCommand dc;
            dc.initAsProceedCommand(dir,nameWeAreOn,amtToProceed);
            commands.push_back(dc);
            //handle delivery
            if (ir==segmentsToDeliveryLoc.end()){
                DeliveryCommand dc2;
                dc2.initAsDeliverCommand(optimizedDeliveries[a].item);
                commands.push_back(dc2);

            }
            //handle turn
            else{
                string nameWeAreOnNow = ir->name;
                double dirToTurn = angleBetween2Lines(leaveOffOn,*ir);
                if(dirToTurn>=1&&dirToTurn<180){
                    DeliveryCommand dc3;
                    dc3.initAsTurnCommand("left",ir->name);
                    commands.push_back(dc3);
                }
                else if(dirToTurn>=180 && dirToTurn<=359){
                    DeliveryCommand dc3;
                    dc3.initAsTurnCommand("right",ir->name);
                    commands.push_back(dc3);
                }
                else{
                    //don't do any kind of turn command
                }
            }
        }
        startingPoint = optimizedDeliveries[a].location;
    }
    
    //handle going back from last delivery to depot
    list<StreetSegment> segmentsFromLastDeliveryLocationBackToDepot;
    
    PointToPointRouter ptpr(stMap);
    double tempDist;
    DeliveryResult result = ptpr.generatePointToPointRoute(optimizedDeliveries[optimizedDeliveries.size()-1].location,depot,segmentsFromLastDeliveryLocationBackToDepot,tempDist);
    if(result==BAD_COORD){
        return BAD_COORD;
    }
    if(result==NO_ROUTE){
        return NO_ROUTE;
    }
    
    totalDistanceTravelled+=tempDist;
    
    list<StreetSegment>::iterator ir = segmentsFromLastDeliveryLocationBackToDepot.begin();
    while (ir!=segmentsFromLastDeliveryLocationBackToDepot.end()){
        //handle proceed
        double amtToProceed = 0;
        string nameWeAreOn = ir->name;
        double dirToGo = angleOfLine(*ir);
        StreetSegment leaveOffOn;
        string dir;
        if (0<=dirToGo&&dirToGo<22.5){
            dir = "east";
        }
        if (22.5<=dirToGo&&dirToGo<67.5){
            dir = "northeast";
        }
        if (67.5<=dirToGo&&dirToGo<112.5){
            dir = "north";
        }
        if (112.5<=dirToGo && dirToGo< 157.5){
            dir = "northwest";
        }
        if (157.5<=dirToGo&&dirToGo<202.5){
            dir = "west";
        }
        if (202.5<=dirToGo&&dirToGo<247.5){
            dir = "southwest";
        }
        if (247.5<=dirToGo&& dirToGo<292.5){
            dir = "south";
        }
        if (292.5<=dirToGo&&dirToGo<337.5){
            dir = "southeast";
        }
        if (dirToGo>=337.5){
            dir = "east";
        }
        while(ir!=segmentsFromLastDeliveryLocationBackToDepot.end()&&ir->name==nameWeAreOn){
            amtToProceed += distanceEarthMiles(ir->start,ir->end);
            leaveOffOn = *ir;
            ir++;
        }
        DeliveryCommand dc;
        dc.initAsProceedCommand(dir,nameWeAreOn,amtToProceed);
        commands.push_back(dc);
        //handle delivery
        if (ir==segmentsFromLastDeliveryLocationBackToDepot.end()){
            return DELIVERY_SUCCESS;
        }
        //handle turn
        else{
            string nameWeAreOnNow = ir->name;
            double dirToTurn = angleBetween2Lines(leaveOffOn,*ir);
            if(dirToTurn>=1&&dirToTurn<180){
                DeliveryCommand dc3;
                dc3.initAsTurnCommand("left",ir->name);
                commands.push_back(dc3);
            }
            else if(dirToTurn>=180 && dirToTurn<=359){
                DeliveryCommand dc3;
                dc3.initAsTurnCommand("right",ir->name);
                commands.push_back(dc3);

            }
            else{
                //don't do any kind of turn command
            }
        }
    }
    
    return DELIVERY_SUCCESS;   
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}
