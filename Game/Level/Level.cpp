#include "Level.h"
#include "LevelFactory.h"

using namespace std;

Level::Level(string& filepath, Ogre::SceneManager* smn) : mSceneMgr(smn), obj_side(OBJ_SIZE_X)
{
	parseLevel(filepath);
}

Level::Level(GameState gs, Ogre::SceneManager* smn) : mSceneMgr(smn), obj_side(OBJ_SIZE_X)
{
	int bn = gs.getLevelBlockStateNumber();
	for (int i=0; i < bn; i++) {
		LevelBlockState bs = gs.getLevelBlockState(i);
		data.push_back(new LevelBlock(bs.getPosX(), bs.getPosY(), bs.getPosZ(), mSceneMgr, bs.getDimension(), LevelMaterial<Brick>()));
	}
	// Set dimensions!
	width = gs.getLevelWidth();
	height = gs.getLevelHeight();
}

Level::Level(Ogre::SceneManager* smn) : mSceneMgr(smn), obj_side(OBJ_SIZE_X)
{
	// Random level generation
	//std::vector<LevelBlock*> row;

	data.push_back(createLevelBlock(0.0f, 0.0f, BRICK));
	data.push_back(createLevelBlock(OBJ_SIZE_X, 0.0f, BRICK));
	data.push_back(createLevelBlock(OBJ_SIZE_X * 2 , 0.0f, BRICK));
	data.push_back(createLevelBlock(OBJ_SIZE_X * 3, 0.0f, BRICK));
	data.push_back(createLevelBlock(OBJ_SIZE_X * 4, 0.0f, BRICK));
	data.push_back(createLevelBlock(OBJ_SIZE_X * 5, 0.0f, BRICK));

	data.push_back(createLevelBlock(0.0f, OBJ_SIZE_Y, BRICK));

	data.push_back(createLevelBlock(0.0f, OBJ_SIZE_Y * 2, BRICK));

	height = 3;
	width = 6;

	player_spawn_positions.push_back(Ogre::Vector3(OBJ_SIZE_X * 2, OBJ_SIZE_Y * 0.5 + 0.1, 0.0f));
	player_spawn_positions.push_back(Ogre::Vector3(OBJ_SIZE_X * 3, OBJ_SIZE_Y * 0.5 + 0.1, 0.0f));

 }

Level::~Level()
{
	// I WILL ALSO FREE HERE THE OBJECT POINTERS!
	for (std::vector<LevelBlock*>::iterator ite = data.begin(); ite != data.end(); ++ite) {
		delete (*ite);
		(*ite) = NULL;
	}
	data.clear();
}

void Level::parseLevel(string& fp) {
	vector<string> in_lines;
	string line;
	ifstream levelfile(fp);
	int line_len = -1;
	int li = 1;

	if (levelfile.is_open()) {
		while (levelfile.good()) {
			getline(levelfile, line);
			if (line_len == -1 || (line_len == line.length())) {
				line_len = line.length();
				in_lines.push_back(line);
				//cerr << "Line number: " << li << endl;
				//cerr << "Line length: " << line_len << endl;
			}
			else {
				cerr << "Error: not every line has the same length!" << endl;
				cerr << "Line number: " << li << endl;
				cerr << "Line length: " << line_len << endl;
			}
			li++;
		}
	}
	else {
		cout << "Cannot open the level file!" << endl;
	}

	if (line_len != -1 && in_lines.size() > 0) {
		width = line_len;
		height = in_lines.size();
	}

	/*
	// Allocate the memory for the level
	for (int i=0; i < height; ++i) {
		vector<LevelBlock*> row; 
		for (int j=0; j < width; ++j) {
			row.push_back(NULL);
		}
		data.push_back(row);
	}
	*/

	// Swap
	int mid = height / 2;
	for (int i=0; i < mid; i++) {
		in_lines[i].swap(in_lines[height - i -1]);
	}

	// Level is split into strings. Now, let's create objects
	bool error_found = false;
	if (line_len != -1 && in_lines.size() > 0) {
		//width = line_len;
		//height = in_lines.size();
		for (int i=0; i < height && !error_found; i++) {
			for (int j=0; j < width && !error_found; j++) {
				LevelBlock* lb = NULL;
				switch ( in_lines[i].at(j) )
				{
				case ' ':
				// It's a space: just skip it
					break;
				case '#':
					//LevelBlock* lb = new LevelBlock(OBJ_SIZE_X * i, OBJ_SIZE_Y * j, 0.0f, mSceneMgr
					lb = createLevelBlock(OBJ_SIZE_X * j, OBJ_SIZE_Y * i, BRICK); 
					data.push_back(lb);
					//data[i][j] = lb;
					break;
				case '&':
					lb = createLevelBlock(OBJ_SIZE_X * j, OBJ_SIZE_Y * i, ICE);
					data.push_back(lb);
					break;
				case '@':
					if (player_spawn_positions.size() <= 4) {
						player_spawn_positions.push_back(Ogre::Vector3(OBJ_SIZE_X * j, OBJ_SIZE_X * i, 0.0f));
					}
					else {
						cerr << "ERROR: Too many spwan positions!" << endl;
						error_found = true;
					}
					break;
				default:
					cerr << "ERROR: Unrecognized symbol in the level file." << endl;
					error_found = true;
				}
			}
		}
	}

	if (!error_found) {
		//cout << "Level read successfully!" << endl;
	}

	levelfile.close();
}

LevelBlock* Level::createLevelBlock( float xPos, float yPos, LevelBlockMaterial mat )
{
	LevelMaterial<> material;

	if(mat == ICE)
		material = LevelMaterial<Ice>();
	else if(mat == BRICK)
		material = LevelMaterial<Brick>();
	
	return new LevelBlock(xPos, yPos, 0.0f, mSceneMgr, OBJ_SIZE_X, material);
}
