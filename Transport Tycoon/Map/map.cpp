#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <math.h>
#include <climits>
#include <cfloat>

#include "map.h"

#include "tile.h"
#include "programengine.h"
#include "world.h"
#include "object.h"
#include <random>




float deepWaterThreshold = 0.4f;
float shallowWaterThreshold = 0.45f;        //water
float desertThreshold = 0.47f;              //sand
float plainsThreshold = 0.60f;              //plaint
float grasslandThreshold = 0.65f;            //forest
float forestThreshold = 0.75f;              //stone
float hillsThreshold = 0.80f;               //ston2
float mountainsThreshold = 0.88f;           // snow


std::string IndTypeToString(rs::IndustryType type)
{
	switch (type)
	{
	case rs::IndustryType::COALMINE: return "Coal Mine";
		break;
	case rs::IndustryType::POWERSTATION: return "Power Station";
		break;
	case rs::IndustryType::IRONOREMINE: return "Iron Ore Mine";
		break;
	case rs::IndustryType::STEELMILL: return "Steel Mill";
		break;
	case rs::IndustryType::FARM: return "Farm";
		break;
	case rs::IndustryType::FACTORY: return "Factory";
		break;
	case rs::IndustryType::FOREST: return "Forest";
		break;
	case rs::IndustryType::SAWMILL: return "Saw Mill";
		break;
	case rs::IndustryType::OILWELLS: return "Oil Mill";
		break;
	case rs::IndustryType::OILRIG: return "Oil Rig";
		break;
	case rs::IndustryType::OILREFINERY: return "Oil Refinery";
		break;
	case rs::IndustryType::BANK: return "Bank";
		break;
	default:
		break;
	}
}


float dRand(double dMax)
{
    double d = (double)rand()/RAND_MAX;
    double result = (d-0.5)*dMax;
    return result;
}


Map::Map()
{

}

Map::Map(World *world, ng::ProgramEngine *engine):
    m_world(world),m_engine(engine)
{

}


Map::Map(World* world, ng::ProgramEngine* engine, int size):
    m_world(world),m_engine(engine)
{
    m_heightMapSize = size;
    generateHeightMap(size);
    generateMap();
}

Map::~Map()
{
    std::cout << "MAP::destructor..." << std::endl;


    for(int i = 0; i < m_heightMapSize; ++i)
    {
        delete m_heightMap[i];
    }
    delete m_heightMap;

    for(int i = 0; i < m_mapSize; ++i)
    {
        delete m_map[i];
    }
    delete m_map;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Map methods
///

int Map::generateMap()
{
    initialiseMap();
    //showTileDistribution();
    generateObjects();

    // Add stuff /...
    return 0;
}

int Map::initialiseEmptyMap()
{
	int size = m_mapSize;
	m_map = new Tile**[size];
	for (int i = 0; i < size; ++i)
	{
		m_map[i] = new Tile*[size];
		for (int j = 0; j < size; ++j)
		{
			int height = 0;

			rs::TileType tileType = rs::TileType::VOID;

			//m_map[i][j] = new Tile(height, getTileTexture(height), tileType, m_world->getDrawnFlag());
		}
	}
	return 0;
}


int Map::initialiseMap()
{
    int size = m_mapSize;
    m_map = new Tile** [size];
    for(int i = 0; i < size; ++i)
    {
        m_map[i] = new Tile* [size];
        for(int j = 0; j < size; ++j)
        {
            int height = this->getMapHeight(i,j);

            rs::TileType tileType = getTileType(height);

            m_map[i][j] = new Tile(height, getTileTexture(height),tileType, m_world->getDrawnFlag());
        }
    }
    return 0;
}




bool Map::isValidIndustry(bool condition, int x, int y, Tile*** map, rs::IndustryType type)
{
    int rows = m_industryMaps[type].m_sizeX;
    int columns = m_industryMaps[type].m_sizeY;

    for(int i = y - rows, i2 = 0; i <= y, i2 < rows; ++i,++i2)
    {
        for(int j = x, j2 = 0; j <= x + columns, j2 < columns; ++j,++j2)
        {
            if(m_industryMaps[type].m_map[i2][j2] == 'x' || m_industryMaps[type].m_map[i2][j2] == '1')
            {
                if(i > 0 && i < m_mapSize && j > 0 && j < m_mapSize)
                {
                    if(condition)
                    {
                        if(map[j][i]->m_tileStatObj != nullptr)
                            return false;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
        }
    }
    return true;
}

bool Map::isValidGreenery(int x, int y, Tile *** map)
{	// Checks is tile velide for placement a greenery object
	if (map[x][y]->m_tileType == rs::TileType::PLAIN || map[x][y]->m_tileType == rs::TileType::FOREST)
	{
		if (map[x][y]->m_tileStatObj != NULL)
			return false;
	}
	else
	{
		return false;
	}
	return true;
}

int Map::generateObjects()
{

	int generatedItems = 5;


	sf::Clock clock;

    rs::IndustryType type;

    loadIndustryMaps();

	std::cout << "Elapset time for loading ind. maps:" << clock.getElapsedTime().asSeconds() << std::endl; clock.restart();
	

    for(int i = 0; i < 9; ++i)
    {
        int qvote = 5;
		int built = 0;
	
		type = rs::IndustryType(i);

        while(qvote > 0)
        {
            bool isCreated = false;
			int tries = 0;

            while(!isCreated && tries < 30)
            {
                int x = (int)rand()%(this->getMapSize()-1)+1;
                int y = (int)rand()%(this->getMapSize()-1)+1;
				++tries;

				isCreated = createIndustry(x, y, type);
				if (isCreated) ++built;
            }
            qvote--;
        }
		std::cout << "Build ind. " << IndTypeToString(type) << " : " << built << std::endl;;

    }
	std::cout << "Ind count:" << m_world->m_objStaticContainer.size() << std::endl;

	std::cout << "Elapset time for placing ind." << clock.getElapsedTime().asSeconds() << std::endl; clock.restart();
	
	this->placeGreenery();

	std::cout << "Elapset time for placing greenery." << clock.getElapsedTime().asSeconds() << std::endl; clock.restart();
	std::cout << "Objects:" << m_world->m_objStaticContainer.size() << std::endl;
    return 0;
}

rs::Rectangle Map::getMapEdges()
{
    rs::Rectangle rect;
    rect.bottomLeft.setValues(-m_mapSize/2*64,m_mapSize/2*32);
    rect.topLeft.setValues(0,0);
    rect.topRight.setValues(m_mapSize/2*64,m_mapSize/2*32);
    rect.bottomRight.setValues(0,m_mapSize*32);
    return rect;
}


void Map::loadIndustryMaps()
{	// Loads Industry maps

    const char temp_matrix1[4][5] = {{'0','0','0','0','0'},
                                    {'0','x','x','x','0'},
                                    {'x','x','x','1','0'},
                                    {'x','x','x','0','0'}};
    this->m_industryMaps[rs::IndustryType::COALMINE].CreateIndustryMap(4,4,temp_matrix1);


    const char temp_matrix2[4][5] = {{'x','x','0','0','0'},
                                    {'x','x','0','0','0' },
                                    {'x','x','0','0','0' },
                                    {'x','1','0','0','0' }};
    this->m_industryMaps[rs::IndustryType::POWERSTATION].CreateIndustryMap(4,2,temp_matrix2);



	const char temp_matrix3[4][5] = {{'0','0','0','0','0' },
									{ '0','0','0','0','0' },
									{ 'x','0','0','0','0' },
									{ '1','0','0','0','0' } };
	this->m_industryMaps[rs::IndustryType::BANK].CreateIndustryMap(4, 1, temp_matrix3);


	const char temp_matrix4[4][5] = {{'0','0','0','0','0' },
									{ 'x','x','x','x','0' },
									{ 'x','x','x','1','0' },
									{ 'x','x','0','0','0' } };
	this->m_industryMaps[rs::IndustryType::FACTORY].CreateIndustryMap(4, 4, temp_matrix4);


	const char temp_matrix5[4][5] = {{'0','0','0','0','0' },
									{ 'x','x','x','x','0' },
									{ 'x','x','x','x','0' },
									{ 'x','x','x','1','0' } };
	this->m_industryMaps[rs::IndustryType::FARM].CreateIndustryMap(4, 4, temp_matrix5);


	const char temp_matrix6[4][5] = {{'x','x','x','x','0' },
									{ 'x','x','x','x','x' },
									{ 'x','x','x','x','1' },
									{ 'x','x','x','x','0' } };
	this->m_industryMaps[rs::IndustryType::FOREST].CreateIndustryMap(4, 5, temp_matrix6);


	const char temp_matrix7[4][5] = {{'x','x','x','x','0' },
									{ 'x','x','x','x','0' },
									{ 'x','x','x','x','0' },
									{ 'x','x','x','1','0' } };
	this->m_industryMaps[rs::IndustryType::IRONOREMINE].CreateIndustryMap(4, 4, temp_matrix7);


	const char temp_matrix8[4][5] = {{'0','0','0','0','0' },
									{ 'x','x','x','x','x' },
									{ 'x','x','x','x','x' },
									{ 'x','x','x','x','1' } };
	this->m_industryMaps[rs::IndustryType::OILREFINERY].CreateIndustryMap(4, 5, temp_matrix8);


	const char temp_matrix9[4][5] = {{'0','0','0','0','0' },
									{ '0','0','0','0','0' },
									{ '0','0','0','0','0' },
									{ '1','0','0','0','0' } };
	this->m_industryMaps[rs::IndustryType::OILRIG].CreateIndustryMap(4, 1, temp_matrix9);


	const char temp_matrix10[4][5] = {{'0','0','0','0','0' },
									 { 'x','x','1','0','0' },
									 { 'x','0','0','0','0' },
									 { 'x','0','0','0','0' } };
	this->m_industryMaps[rs::IndustryType::OILWELLS].CreateIndustryMap(4, 3, temp_matrix10);


	const char temp_matrix11[4][5] = {{'0','0','0','0','0' },
									 { 'x','x','x','0','0' },
									 { 'x','x','1','0','0' },
									 { 'x','x','0','0','0' } };
	this->m_industryMaps[rs::IndustryType::SAWMILL].CreateIndustryMap(4, 3, temp_matrix11);


	const char temp_matrix12[4][5] = {{'x','x','x','0','0' },
									 { 'x','x','x','x','0' },
									 { 'x','x','x','1','0' },
									 { 'x','x','x','0','0' } };
	this->m_industryMaps[rs::IndustryType::STEELMILL].CreateIndustryMap(4, 4, temp_matrix12);
}

void Map::placeGreenery()
{	// Forest generator
	// control radius, objects in a radius
	// probabilities

	int radius = 7;

	int objectsForest = 20;
	int objectsPlain = 3;

	rs::GreeneryType type;
	std::default_random_engine generator;
	std::bernoulli_distribution distribution(0.4);

	std::random_device rd{};
	std::mt19937 gen{ rd() };
	std::normal_distribution<> d{ 5,2 };

	for (int x = 0; x < m_mapSize; ++x) {
		for (int y = 0; y < m_mapSize; ++y) {

			if ((m_map[x][y]->m_tileType == rs::TileType::FOREST || m_map[x][y]->m_tileType == rs::TileType::PLAIN)
				&& isValidGreenery(x, y, m_map))
			{
				int trueFalse = distribution(generator);
				rs::Point center;
				center.setValues(x, y);
				int objectsNumberRad = countObjectRadius(rs::ObjectType::GREENERY, center, radius);

				if(m_map[x][y]->m_tileType == rs::TileType::FOREST &&
					trueFalse && objectsNumberRad <= objectsForest)
				{

					int probability = round(d(gen));

					if (probability <= 4)
						type = rs::GreeneryType::TREE_1;
					else if (probability > 4 && probability <= 6 )
						type = rs::GreeneryType::TREE_2;
					else if (probability > 6)
						type = rs::GreeneryType::TREE_3;

					rs::Greenery newStruct = m_engine->m_texmng->getGreeneryStruct(type);

					GreeneryObject* newObject = new GreeneryObject(newStruct, x, y);

					m_world->addObject(newObject);

					m_map[x][y]->m_tileStatObj = newObject;
					m_map[x][y]->isMainStatic = true;
				}
				else if(m_map[x][y]->m_tileType == rs::TileType::PLAIN &&
					trueFalse && objectsNumberRad <= objectsPlain)
				{
					int probability = round(d(gen));

					if (probability <= 3)
						type = rs::GreeneryType::TREE_1;
					else if (probability > 3 && probability <= 4)
						type = rs::GreeneryType::TREE_2;
					else if (probability > 4)
						type = rs::GreeneryType::TREE_3;

					rs::Greenery newStruct = m_engine->m_texmng->getGreeneryStruct(type);

					GreeneryObject* newObject = new GreeneryObject(newStruct, x, y);

					m_world->addObject(newObject);

					m_map[x][y]->m_tileStatObj = newObject;
					m_map[x][y]->isMainStatic = true;
				}
			}
		}
	}
}

bool Map::createIndustry(int x, int y, rs::IndustryType type)
{
	
	bool condition = false;

	switch (type)
	{
	case rs::IndustryType::POWERSTATION:
	case rs::IndustryType::STEELMILL:
	case rs::IndustryType::FACTORY:
	case rs::IndustryType::OILREFINERY:
	case rs::IndustryType::BANK:
	{
		condition = m_map[x][y]->m_tileType == rs::TileType::PLAIN ||
					m_map[x][y]->m_tileType == rs::TileType::FOREST ||
					m_map[x][y]->m_tileType == rs::TileType::STONE ||
					m_map[x][y]->m_tileType == rs::TileType::SAND;
	}
	break;
	case rs::IndustryType::COALMINE:
	{
		condition = m_map[x][y]->m_tileType == rs::TileType::PLAIN ||
					m_map[x][y]->m_tileType == rs::TileType::FOREST ||
					m_map[x][y]->m_tileType == rs::TileType::STONE ||
					m_map[x][y]->m_tileType == rs::TileType::ROCKS;
	}
	break;
	case rs::IndustryType::IRONOREMINE:
	{
		condition = m_map[x][y]->m_tileType == rs::TileType::STONE ||
					m_map[x][y]->m_tileType == rs::TileType::SAND ||
					m_map[x][y]->m_tileType == rs::TileType::ROCKS;
	}
	break;
	case rs::IndustryType::FARM:
	{
		condition = m_map[x][y]->m_tileType == rs::TileType::PLAIN;
	}
	break;
	case rs::IndustryType::FOREST: {
		condition = m_map[x][y]->m_tileType == rs::TileType::FOREST;
	}
	break;
	case rs::IndustryType::SAWMILL:
	{
		condition = m_map[x][y]->m_tileType == rs::TileType::FOREST ||
					m_map[x][y]->m_tileType == rs::TileType::PLAIN;
	}
	break;
	case rs::IndustryType::OILWELLS:
	{
		condition = m_map[x][y]->m_tileType == rs::TileType::SAND ||
					m_map[x][y]->m_tileType == rs::TileType::PLAIN ||
					m_map[x][y]->m_tileType == rs::TileType::ROCKS;
	}
	break;
	case rs::IndustryType::OILRIG:
	{
		condition = m_map[x][y]->m_tileType == rs::TileType::WATER;
	}
	break;
	default:
	{
		condition = false;
	}
	break;
	}

	bool isCreated = false;

	if (condition)
	{
		isCreated = isValidIndustry(condition, x, y, m_map, type);

		if (isCreated)
		{
			sf::Texture* newTexture = m_engine->m_texmng->getTextureRef(type);

			Industry* newObject = new Industry(rs::ObjectType::INDUSTRY, newTexture, type, x, y);

			m_world->addObject(newObject);

			this->placeIndustry(newObject);

		}
	}
	return isCreated;
}

int Map::countObjectRadius(rs::ObjectType type, rs::Point point, int rad)
{
	int count = 0;
	for (auto i : m_world->m_objStaticContainer)
	{
		if (i->m_objectType == type)
		{
			GreeneryObject* obj = dynamic_cast<GreeneryObject*>(i);
			bool inRadius = pow(obj->m_x - point.x, 2) + pow(obj->m_y - point.y, 2) < pow(rad, 2);
			if (inRadius) ++count;
		}
	}
	return count;
}


/////////////////////////////////////////////////////////////////////////////////////////
/// Height map methods
///

int Map::initialiseHeightMap()
{
    int size = m_heightMapSize;
    m_heightMap = new float* [size];
    for(int i = 0; i < size; ++i)
    {
        m_heightMap[i] = new float[size];
        for(int j = 0; j < size; ++j)
            m_heightMap[i][j] = 0;
    }
    return 0;
}


int Map::generateHeightMap(int size)
{
    m_mapSize = size;
    m_heightMapSize = size + 1;

    initialiseHeightMap();
    processHeightMap();

    //Map landscape normalising
    float max = FLT_MIN;
    float min = FLT_MAX;
    for (int i = 0; i < m_heightMapSize; ++i) {
        for (int j = 0; j < m_heightMapSize; ++j) {
            if (m_heightMap[i][j] > max) max = m_heightMap[i][j];
            if (m_heightMap[i][j] < min) min = m_heightMap[i][j];
        }
    }

    for(int row = 0; row < m_heightMapSize; ++row){
        for(int col = 0; col < m_heightMapSize; ++col)
        {
            // Normalisong
            m_heightMap[row][col] = (float)(m_heightMap[row][col]-min)/(max-min);

            if (m_heightMap[row][col] < deepWaterThreshold) m_heightMap[row][col] = 0;
            else if (m_heightMap[row][col] < shallowWaterThreshold) m_heightMap[row][col] = 1;
            else if (m_heightMap[row][col] < desertThreshold) m_heightMap[row][col] = 2;
            else if (m_heightMap[row][col] < plainsThreshold) m_heightMap[row][col] = 3;
            else if (m_heightMap[row][col] < forestThreshold) m_heightMap[row][col] = 4;
            else if (m_heightMap[row][col] < hillsThreshold) m_heightMap[row][col] = 5;
            else if (m_heightMap[row][col] < mountainsThreshold) m_heightMap[row][col] = 6;
            else m_heightMap[row][col] = 7;
        }
    }
    return 0;
}


int Map::processHeightMap()
{
    std::cout << "MAP::ProgramEngine..." << std::endl;

    // Initializing srand for random values :
    srand(time(NULL));

    // Defining the corners values and range value:
    m_range = 1;
    int F = 1.2 ; // smothness

    m_heightMap[0][0] = dRand(m_range);
    m_heightMap[0][m_heightMapSize-1] = dRand(m_range);
    m_heightMap[m_heightMapSize-1][0] = dRand(m_range);
    m_heightMap[m_heightMapSize-1][m_heightMapSize-1] = dRand(m_range);

    //Processing Diamond Square Alg.
    for(int sideLength = m_heightMapSize-1; sideLength >= 2; sideLength /= 2, m_range *= pow(2,-F))
    {
        int halfSide = sideLength/2;

        squareStep(sideLength, halfSide);
        diamondStep(sideLength, halfSide);
    }

    return 0;
}



int Map::squareStep(int sideLength, int halfSide)
{
    for(int x = 0; x < m_heightMapSize-1; x += sideLength)
        {
            for(int y = 0; y < m_heightMapSize-1; y += sideLength)
            {
                double avg = m_heightMap[x][y] + m_heightMap[x+sideLength][y] + m_heightMap[x][y+sideLength] + m_heightMap[x+sideLength][y+sideLength];
                avg /= 4.0;
                m_heightMap[x+halfSide][y+halfSide] = avg + dRand(m_range);
            }
        }
    return 0;
}


void Map::diamondStep(int sideLength, int halfSide)
{
    for(int x = 0; x < m_heightMapSize-1; x += halfSide)
    {
        for(int y = (x+halfSide)%sideLength; y < m_heightMapSize -1; y +=sideLength)
        {
            double avg = m_heightMap[(x-halfSide+m_heightMapSize-1)%(m_heightMapSize-1)][y] +
                m_heightMap[(x+halfSide)%(m_heightMapSize-1)][y] +
                m_heightMap[x][(y+halfSide)%(m_heightMapSize-1)] +
                m_heightMap[x][(y-halfSide+m_heightMapSize-1)%(m_heightMapSize-1)];

            avg /= 4.0;
            m_heightMap[x][y] = avg + dRand(m_range);

            if(x == 0) m_heightMap[m_heightMapSize-1][y] = (float)avg;
            if(y == 0) m_heightMap[x][m_heightMapSize-1] = (float)avg;
        }
    }
}


sf::Texture& Map::getTileTexture(const int cellHeight) const
{
    if( cellHeight == 0)
    {
        return *m_engine->m_texmng->getTextureRef("bg_deepwater");
    }
    else if( cellHeight == 1)
    {
        return *m_engine->m_texmng->getTextureRef("bg_water");
    }
    else if(cellHeight == 2)
    {
        return *m_engine->m_texmng->getTextureRef("bg_sand");
    }
    else if(cellHeight == 3)
    {
        return *m_engine->m_texmng->getTextureRef("bg_plain");
    }
    else if(cellHeight == 4)
    {
        return *m_engine->m_texmng->getTextureRef("bg_forest");
    }
    else if(cellHeight == 5)
    {
        return *m_engine->m_texmng->getTextureRef("bg_stone1");
    }
    else if(cellHeight == 6)
    {
        return *m_engine->m_texmng->getTextureRef("bg_stone2");
    }
    else if(cellHeight == 7)
    {
        return *m_engine->m_texmng->getTextureRef("bg_snow");
    }

}

rs::TileType Map::getTileType(const int cellHeight) const
{
    if(cellHeight == 0)        return rs::TileType::DEEPWATER;
    else if(cellHeight == 1)   return rs::TileType::WATER;
    else if(cellHeight == 2)    return rs::TileType::SAND;
    else if(cellHeight == 3)    return rs::TileType::PLAIN;
    else if(cellHeight == 4)    return rs::TileType::FOREST;
    else if(cellHeight == 5)    return rs::TileType::STONE;
    else if(cellHeight == 6)    return rs::TileType::ROCKS;
    else if(cellHeight == 7)    return rs::TileType::SNOW;
    else return rs::TileType::VOID;
}

void Map::loadMapSetup(World * world, ng::ProgramEngine * engine, bool* flag)
{ // For loading from file
	m_world = world;
	m_engine = engine;
	for (int i = 0; i < m_mapSize; ++i)
	{
		for (int j = 0; j < m_mapSize; ++j)
		{
			m_map[i][j]->drawFlag = this->m_world->getDrawnFlag();
			m_map[i][j]->isSpriteDrawn = *(m_map[i][j]->drawFlag);

			float x, y;  x = i; y = j;
			rs::twoDToIso(x, y, 64, 32);
			m_map[i][j]->m_sprite.setPosition(x, y);
			m_map[i][j]->m_sprite.setOrigin(sf::Vector2f(0.0f, 0.0f));
			m_map[i][j]->m_sprite.setTexture(getTileTexture(m_map[i][j]->m_tileHeight));
			m_map[i][j]->m_tileStatObj = nullptr;

		}

	}

	loadIndustryMaps();
}

void Map::placeIndustry(Industry * ind)
{
	int x, y;
	rs::IndustryType type = ind->getType();
	int rows = m_industryMaps[type].m_sizeX;
	int columns = m_industryMaps[type].m_sizeY;
	x = ind->m_x;
	y = ind->m_y;

	for (int i = y - rows + 1, i2 = 0; i <= y, i2 < rows; ++i, ++i2)
	{
		for (int j = x, j2 = 0; j <= x + columns, j2 < columns; ++j, ++j2)
		{

			// Look in Map::loadIndustryMaps() for more info
			if (m_industryMaps[type].m_map[i2][j2] == 'x')
			{
				//m_map[j][i]->m_sprite.setTexture(*m_engine->m_texmng->getTextureRef("bg_red"));
				m_map[j][i]->m_tileStatObj = ind;
			}
			if (m_industryMaps[type].m_map[i2][j2] == '1')
			{
				//m_map[j][i]->m_sprite.setTexture(*m_engine->m_texmng->getTextureRef("bg_factory"));
				m_map[j][i]->m_tileStatObj = ind;
				m_map[j][i]->isMainStatic = true;
			}
		}
	}

}

