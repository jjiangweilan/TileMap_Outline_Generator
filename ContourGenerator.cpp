//
//  ContourGenerator.cpp
//  MarioGame
//
//  Created by jiehong jiang on 5/31/17.
//
//

#include "ContourGenerator.hpp"

#include "cocos2d.h"
#include <vector>
#include <algorithm>
#include <set>
USING_NS_CC;

#define CREATE_CONNECTED_LINES(_axis) \
for (Line l : lines){ \
if (l.p1._axis == l.p2._axis && l.p1._axis == para){ \
result.insert(l); \
} \
}

#define GET_CONTOUR_FOR(_axis, _max) \
for (int i = 0; i <= _max; i++) { \
    std::set<Line, LineComp> cALines = getLine(i, _axis); \
    for (std::set<Line, LineComp>::iterator iter = cALines.begin(); iter != cALines.end();iter++){ \
        std::set<Line, LineComp>::iterator next = ++iter;--iter; \
        Line c(iter->p1, iter->p2); \
        for(;next != cALines.end();next++,iter++){ \
            if (next->p1 == c.p2) { \
                c.p2 = next->p2; \
            } \
            else break; \
        } \
        group.push_back(c); \
    } \
} \

bool LineComp::operator()(const Line &l1, const Line &l2) const{
    if(l1.p1.x != l2.p2.x){
        return l1.p1.x < l2.p2.x;
    }
    else{
        return l1.p1.y < l2.p2.y;
    }
}

std::vector<Line> ContourGenerator::getContour(){
    std::vector<Line> group;
    
    Size size = targetLayer->getLayerSize();
    
    GET_CONTOUR_FOR('x', size.width);
    GET_CONTOUR_FOR('y', size.height);
    
    return group;

}

std::set<Line, LineComp> ContourGenerator::getLine(int para, char axis){
    std::set<Line, LineComp> result;
    if (axis == 'x'){
        CREATE_CONNECTED_LINES(x);  //connct edges in this axis.
    }
    else if (axis == 'y'){
        CREATE_CONNECTED_LINES(y);
    }
    return result;
}


ContourGenerator::ContourGenerator(cocos2d::TMXLayer* layer){
    targetLayer = layer;
    Size size = layer->getLayerSize();
    for (int c = 0; c < size.width; c++) {
        for (int r = 0; r < size.height; r++) {
            auto tile = layer->getTileAt(Vec2(c, r));
            //this is the start point of one contour
            if (tile != NULL && tile->getUserData() == NULL){
                std::set<Vec2> coordinates = MooreNeighbor::mooreNeighbor(Vec2(c, r), layer);
                populateLines(coordinates);
            }
        }
    }
}

bool ContourGenerator::add(Line edge){
    //find out whether the line is a overlapped edge
    std::vector<Line>::iterator iter = std::find_if(lines.begin(), lines.end(), [&] (const Line &l) -> bool {
        return ((l.p1 == edge.p1 && l.p2 == edge.p2) || (l.p1 == edge.p2 && l.p2 == edge.p1));
    });
    
    //if the line is overlapped delete the same line in the lines
    //if two edges are overlapped, it means there is no need to create the physicsbody for it.
    if (iter != lines.end()){
        lines.erase(iter);
        return false;
    }
    
    lines.push_back(edge);
    return true;
}

void ContourGenerator::populateLines(const std::set<cocos2d::Vec2>& coordinates){
    for(cocos2d::Vec2 x1 : coordinates){
        cocos2d::Vec2 x2 = x1 + cocos2d::Vec2(1, 0);
        cocos2d::Vec2 x3 = x1 + cocos2d::Vec2(0, 1);
        cocos2d::Vec2 x4 = x1 + cocos2d::Vec2(1, 1);
        Line l(x1, x2);
        add(l);
        l = Line(x1, x3);
        add(l);
        l = Line(x3, x4);
        add(l);
        l = Line(x2, x4);
        add(l);
    }
}

namespace NEIGHBOR {
    const Vec2 LEFT = Vec2(-1, 0); //1
    const Vec2 TOP_LEFT = Vec2(-1, -1); //2
    const Vec2 BOTTOM_LEFT = Vec2(-1, 1); //3
    const Vec2 TOP = Vec2(0, -1); //4
    const Vec2 BOTTOM = Vec2(0, 1);
    const Vec2 TOP_RIGHT = Vec2(1, -1);
    const Vec2 RIGHT = Vec2(1, 0);
    const Vec2 BOTTOM_RIGHT = Vec2(1, 1);
}

cocos2d::Vec2 ContourGenerator::MooreNeighbor::ClockWiseTravel::current = Vec2(-1,-1);
int ContourGenerator::MooreNeighbor::ClockWiseTravel::travelTime = 0;
cocos2d::Vec2 ContourGenerator::MooreNeighbor::ClockWiseTravel::origin = Vec2(-1,-1);
cocos2d::Vec2 ContourGenerator::MooreNeighbor::enter = Vec2(-1, -1);
bool ContourGenerator::MooreNeighbor::ClockWiseTravel::firstTravel = true;

std::set<Vec2> ContourGenerator::MooreNeighbor::mooreNeighbor(cocos2d::Vec2 cCoordinate, cocos2d::TMXLayer* layer){
    std::set<Vec2> neighborSet;
    if(layer->getTileAt(cCoordinate) == NULL) return neighborSet;
    
    enter = cCoordinate;
    ClockWiseTravel::firstTravel = false; //to handle special case
    ClockWiseTravel::current = cCoordinate + NEIGHBOR::LEFT;
    travel(cCoordinate, neighborSet, layer);
    
    return neighborSet;
}

void ContourGenerator::MooreNeighbor::travel(Vec2 cCoordinate,std::set<Vec2>& counterCoordinates, TMXLayer* layer) {
    
    //base case actual test: cCoordinate == enter
    //this termination method needs improvement
    ClockWiseTravel::origin = cCoordinate;
    ClockWiseTravel::travelTime = 1;
    if(cCoordinate == enter && ClockWiseTravel::current == ClockWiseTravel::origin + NEIGHBOR::LEFT && !counterCoordinates.empty()){
        return;
    }
    counterCoordinates.insert(cCoordinate);
    layer->getTileAt(cCoordinate)->setUserData(&TileChecked::check);
    
    while (ClockWiseTravel::travel()) {
        if (ClockWiseTravel::origin == enter && ClockWiseTravel::current == enter + NEIGHBOR::TOP_LEFT && ClockWiseTravel::firstTravel == false)
            return;
        ClockWiseTravel::firstTravel = false;
        
        if(ClockWiseTravel::current.x < 0 || ClockWiseTravel::current.y < 0 || ClockWiseTravel::current.x >= layer->getLayerSize().width || ClockWiseTravel::current.y >= layer->getLayerSize().height)continue;
        
        if (layer->getTileAt(ClockWiseTravel::current) != NULL){
            cCoordinate = ClockWiseTravel::current;
            ClockWiseTravel::traceBack();
            travel(cCoordinate, counterCoordinates, layer);
            break;
        }
    }
    
}


void ContourGenerator::MooreNeighbor::ClockWiseTravel::traceBack(){
    if (current == (origin + NEIGHBOR::LEFT)) current += NEIGHBOR::BOTTOM;
    else if (current == (origin + NEIGHBOR::TOP_LEFT)) current += NEIGHBOR::BOTTOM;
    else if (current == (origin + NEIGHBOR::TOP)) current += NEIGHBOR::LEFT;
    else if (current == (origin + NEIGHBOR::TOP_RIGHT)) current += NEIGHBOR::LEFT;
    else if (current == (origin + NEIGHBOR::RIGHT)) current += NEIGHBOR::TOP;
    else if (current == (origin + NEIGHBOR::BOTTOM_RIGHT)) current += NEIGHBOR::TOP;
    else if (current == (origin + NEIGHBOR::BOTTOM)) current += NEIGHBOR::RIGHT;
    else if (current == (origin + NEIGHBOR::BOTTOM_LEFT)) current += NEIGHBOR::RIGHT;
}
bool ContourGenerator::MooreNeighbor::ClockWiseTravel::travel(){
    if (travelTime == 9)return false;
    if (current == (origin + NEIGHBOR::LEFT)) current += NEIGHBOR::TOP;
    else if (current == (origin + NEIGHBOR::TOP_LEFT)) current += NEIGHBOR::RIGHT;
    else if (current == (origin + NEIGHBOR::TOP)) current += NEIGHBOR::RIGHT;
    else if (current == (origin + NEIGHBOR::TOP_RIGHT)) current += NEIGHBOR::BOTTOM;
    else if (current == (origin + NEIGHBOR::RIGHT)) current += NEIGHBOR::BOTTOM;
    else if (current == (origin + NEIGHBOR::BOTTOM_RIGHT)) current += NEIGHBOR::LEFT;
    else if (current == (origin + NEIGHBOR::BOTTOM)) current += NEIGHBOR::LEFT;
    else if (current == (origin + NEIGHBOR::BOTTOM_LEFT)) current += NEIGHBOR::TOP;
    ++travelTime;
    return true;
}


bool TileChecked::check = false;
