//
//  Line.h
//  MarioGame
//
//  Created by jiehong jiang on 5/30/17.
//
//

#ifndef Line_h
#define Line_h
struct Line{
    cocos2d::Vec2 p1;
    cocos2d::Vec2 p2;
    Line(cocos2d::Vec2 start, cocos2d::Vec2 end):p1(start), p2(end){}
};

#endif /* Line_h */
