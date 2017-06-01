//
//  ContourGenerator.hpp
//  MarioGame
//
//  Created by jiehong jiang on 5/31/17.
//
//

#ifndef ContourGenerator_hpp
#define ContourGenerator_hpp
#include "Line.h"
USING_NS_CC;

struct LineComp{
    bool operator()(const Line&, const Line&) const;
};

struct TileChecked{
    static bool check;
};

class ContourGenerator {
    TMXLayer* targetLayer;
    std::set<Vec2> coordinates;
    std::vector<Line> lines;
    
    /**
     add a edge to lines
     this function will filter out repeated tile edges
     @param Line line to add
     @return if line is successfully added.
     */
    bool add(Line);
    
    /**
     after using mooreneighbor finding all the contour tiles, this method populates "lines" with all the tiles' edges

     @param std::set<cocos2d::Vec2>& the set with all the tils' coordinates
     */
    void populateLines(const std::set<cocos2d::Vec2>&);
    
    
    /**
     get all the lines in speficied number of axis.

     @param int number of axis
     @param char axis 'x' or 'y'
     @return the lines in that number axis
     */
    std::set<Line, LineComp> getLine(int, char);
    
    struct MooreNeighbor{
        static std::set<cocos2d::Vec2> mooreNeighbor(cocos2d::Vec2, cocos2d::TMXLayer*);
        struct ClockWiseTravel{
            static cocos2d::Vec2 current;
            static int travelTime;
            static cocos2d::Vec2 origin;
            static bool travel();
            static void traceBack();
            static bool firstTravel;
        };
    private:
        static cocos2d::Vec2 enter;
        static void travel(cocos2d::Vec2,std::set<cocos2d::Vec2>&, cocos2d::TMXLayer*);
    };
    
    /**
     get the contour (angle to angle line) for specified axis
     
     @param axis
     @return
     */
    
public:
    ContourGenerator(cocos2d::TMXLayer*);
    
    /**
     get a vector of lines that include all the lines that make up the contour

     @return the vector of lines
     */
    std::vector<Line> getContour();
    
};
#endif /* ContourGenerator_hpp */
