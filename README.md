# TileMap_Outline_Generator
ContourGenerator generates a vector of Lines that outline the tiles in the layer you pass to it

# How To Use
1. initialize ContourGenerator with your TMXLayer.
2. call generator.getContour().
3. the return object is a vector of Lines
4. the points of lines are based on tile map coordinate system, so you need to create your converter.

# Usage Example

```cpp
void LevelOneScene::_addCollisionToTiles(){
    TMXTiledMap* map = reinterpret_cast<TMXTiledMap*>(this->getChildByTag(TILEMAP_TAG));
    auto* layer = map->getLayer("Tile Layer 1");
 
    auto generator = ContourGenerator(layer);
    std::vector<Line> lines = generator.getContour();
    
    for (Line l : lines){
        createPhysicsForLine(l); // this function will convert l to sprite coordinate system by using the third method.
    }
    
}

void LevelOneScene::createPhysicsForLine(Line line){
    auto* lineSprite = DrawNode::create();
    TMXTiledMap* map = reinterpret_cast<TMXTiledMap*>(this->getChildByTag(TILEMAP_TAG));
    auto* layer = map->getLayer("Tile Layer 1");
    
    Vec2 origin = convertCoordinateToPoint(line.p1, layer);
    Vec2 destination = convertCoordinateToPoint(line.p2, layer);
    lineSprite->drawLine(origin, destination, Color4F::WHITE);
    
    lineSprite->setPosition(Vec2(0, this->getContentSize().height - 20)); //I think cocos2d-x has some inconsistant between tile coordinate system and sprite coordinate system. 20 is for compensation 
    this->addChild(lineSprite);
    
    Vec2 points[] = {origin, destination};
    auto body = PhysicsBody::createEdgeChain(points, 2);
    lineSprite->setPhysicsBody(body);
}

Vec2 LevelOneScene::convertCoordinateToPoint(Vec2 coordinate, TMXLayer* layer){
    float height = layer->getTileSet()->_tileSize.height;
    float width = layer->getTileSet()->_tileSize.width;
    float totoalHeight = layer->getLayerSize().height;
    float x = width*coordinate.x;
    float y = totoalHeight - height * coordinate.y;
    
    return Vec2(x,y);
}
```

# Result with code above
<img src="https://github.com/jjiangweilan/TileMap_Outline_Generator/blob/master/raw/Result1.png" width = "40%" /><img src="https://github.com/jjiangweilan/TileMap_Outline_Generator/blob/master/raw/Result2.png" width = "40%" /><img src="https://github.com/jjiangweilan/TileMap_Outline_Generator/blob/master/raw/Result3.png" width = "40%" />
