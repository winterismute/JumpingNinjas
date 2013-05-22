#include "Level.h"
#include "LevelFactory.h"

using namespace std;

Level::Level(string& filepath, Ogre::SceneManager* smn, float dim) : mSceneMgr(smn), obj_side(dim)
{
	// Here, we just parse the level file
	parseLevel(filepath);
}

Level::Level(GameState gs, Ogre::SceneManager* smn) : mSceneMgr(smn)
{
	// We retrieve the information from the gamestate object and create old objects
	int bn = gs.getLevelBlockStateNumber();
	for (int i=0; i < bn; i++) {
		LevelBlockState bs = gs.getLevelBlockState(i);
		obj_side = bs.getDimension();
		data.push_back(new LevelBlock(bs.getPosX(), bs.getPosY(), bs.getPosZ(), mSceneMgr, bs.getDimension(), LevelMaterial<Brick>()));
	}
	// Set dimensions!
	width = gs.getLevelWidth();
	height = gs.getLevelHeight();
}

Level::Level(int layout, Ogre::SceneManager* smn, float dim) : mSceneMgr(smn), obj_side(dim)
{
	// Random level generation
	// Depending on the layout, we manually generate blocks
	assert(layout > 0);
	if (layout == 1) {
		data.push_back(createLevelBlock(0.0f, 0.0f, BRICK));
		data.push_back(createLevelBlock(obj_side, 0.0f, BRICK));
		data.push_back(createLevelBlock(obj_side * 2 , 0.0f, BRICK));
		data.push_back(createLevelBlock(obj_side * 3, 0.0f, BRICK));
		data.push_back(createLevelBlock(obj_side * 4, 0.0f, BRICK));
		data.push_back(createLevelBlock(obj_side * 5, 0.0f, BRICK));

		data.push_back(createLevelBlock(obj_side, obj_side, BRICK));

		data.push_back(createLevelBlock(obj_side, obj_side * 2, BRICK));

		data.push_back(createLevelBlock(obj_side, obj_side * 3, BRICK));

		height = 4;
		width = 6;

		player_spawn_positions.push_back(Ogre::Vector3(obj_side * 2, obj_side * 0.5 + 0.1, 0.0f));
		player_spawn_positions.push_back(Ogre::Vector3(obj_side * 4, obj_side * 0.5 + 0.1, 0.0f));
	}
	else if (layout == 2) {
		data.push_back(createLevelBlock(0.0f, 0.0f, ICE));
		data.push_back(createLevelBlock(obj_side, 0.0f, ICE));
		data.push_back(createLevelBlock(obj_side * 2 , 0.0f, ICE));
		data.push_back(createLevelBlock(obj_side * 3, 0.0f, ICE));
		data.push_back(createLevelBlock(obj_side * 4, 0.0f, ICE));
		data.push_back(createLevelBlock(obj_side * 5, 0.0f, ICE));

		data.push_back(createLevelBlock(0.0f, obj_side, BRICK));

		data.push_back(createLevelBlock(0.0f, obj_side * 2, BRICK));
		
		data.push_back(createLevelBlock(0.0f, obj_side * 3, BRICK));

		height = 4;
		width = 6;

		player_spawn_positions.push_back(Ogre::Vector3(obj_side * 2, obj_side * 0.5 + 0.1, 0.0f));
		player_spawn_positions.push_back(Ogre::Vector3(obj_side * 4, obj_side * 0.5 + 0.1, 0.0f));

	}
	else if (layout == 3) {
		data.push_back(createLevelBlock(0.0f, 0.0f, ICE));
		data.push_back(createLevelBlock(obj_side, 0.0f, ICE));
		data.push_back(createLevelBlock(obj_side * 2 , 0.0f, ICE));
		data.push_back(createLevelBlock(obj_side * 5, 0.0f, ICE));
		data.push_back(createLevelBlock(obj_side * 6, 0.0f, ICE));
		data.push_back(createLevelBlock(obj_side * 7, 0.0f, ICE));

		data.push_back(createLevelBlock(obj_side * 2 , obj_side, BRICK));
		data.push_back(createLevelBlock(obj_side * 3, obj_side, BRICK));
		data.push_back(createLevelBlock(obj_side * 4, obj_side, BRICK));
		data.push_back(createLevelBlock(obj_side * 5, obj_side, BRICK));

		data.push_back(createLevelBlock(0.0f, obj_side * 2, BRICK));
		data.push_back(createLevelBlock(obj_side * 7, obj_side * 2, BRICK));

		data.push_back(createLevelBlock(0.0f, obj_side * 3, BRICK));
		data.push_back(createLevelBlock(obj_side * 7, obj_side * 3, BRICK));

		height = 4;
		width = 8;

		player_spawn_positions.push_back(Ogre::Vector3(obj_side * 2, obj_side * 0.5 + 0.1, 0.0f));
		player_spawn_positions.push_back(Ogre::Vector3(obj_side * 4, obj_side * 0.5 + 0.1, 0.0f));

	}
	else {
		data.push_back(createLevelBlock(0.0f, 0.0f, BRICK));
		data.push_back(createLevelBlock(obj_side, 0.0f, BRICK));
		data.push_back(createLevelBlock(obj_side * 2 , 0.0f, BRICK));
		data.push_back(createLevelBlock(obj_side * 3 , 0.0f, BRICK));
		data.push_back(createLevelBlock(obj_side * 4 , 0.0f, BRICK));
		data.push_back(createLevelBlock(obj_side * 5, 0.0f, BRICK));
		data.push_back(createLevelBlock(obj_side * 6, 0.0f, BRICK));
		data.push_back(createLevelBlock(obj_side * 7, 0.0f, BRICK));

		data.push_back(createLevelBlock(obj_side * 2, obj_side, ICE));
		data.push_back(createLevelBlock(obj_side * 2, obj_side, ICE));

		data.push_back(createLevelBlock(0.0f, obj_side * 3, BRICK));
		data.push_back(createLevelBlock(obj_side * 4, obj_side * 3, BRICK));

		data.push_back(createLevelBlock(0.0f, obj_side * 4, BRICK));
		data.push_back(createLevelBlock(obj_side, obj_side * 4, BRICK));
		data.push_back(createLevelBlock(obj_side * 2 , obj_side * 4, BRICK));
		data.push_back(createLevelBlock(obj_side * 3 , obj_side * 4, BRICK));
		data.push_back(createLevelBlock(obj_side * 4 , obj_side * 4, BRICK));
		data.push_back(createLevelBlock(obj_side * 5, obj_side * 4, BRICK));
		data.push_back(createLevelBlock(obj_side * 6, obj_side * 4, BRICK));
		data.push_back(createLevelBlock(obj_side * 7, obj_side * 4, BRICK));

		height = 5;
		width = 8;

		player_spawn_positions.push_back(Ogre::Vector3(obj_side * 2, obj_side * 0.5 + 0.1, 0.0f));
		player_spawn_positions.push_back(Ogre::Vector3(obj_side * 4, obj_side * 0.5 + 0.1, 0.0f));
	}
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
	// The actual parser
	vector<string> in_lines;
	string line;
	ifstream levelfile(fp);
	int line_len = -1;
	int li = 1;

	// We read line by line
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

	// width = length of every line (must be the same for each line)
	// height = number of lines
	if (line_len != -1 && in_lines.size() > 0) {
		width = line_len;
		height = in_lines.size();
	}

	// Level is read upside down: swap lines
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
					lb = createLevelBlock(obj_side * j, obj_side * i, BRICK); 
					data.push_back(lb);
					break;
				case '&':
					lb = createLevelBlock(obj_side * j, obj_side * i, ICE);
					data.push_back(lb);
					break;
				case '@':
					if (player_spawn_positions.size() <= 4) {
						player_spawn_positions.push_back(Ogre::Vector3(obj_side * j, obj_side * i, 0.0f));
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

LevelBlock* Level::createLevelBlock( float xPos, float yPos, LevelBlockMaterial mat)
{
	// We create the right block depending on the material parsed
	LevelMaterial<> material;

	if(mat == ICE)
		material = LevelMaterial<Ice>();
	else if(mat == BRICK)
		material = LevelMaterial<Brick>();
	
	return new LevelBlock(xPos, yPos, 0.0f, mSceneMgr, obj_side, material);
}
