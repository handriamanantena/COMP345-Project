#include "Campaign.h"
#include "Director.h"

// CONSTRUCTOR
Campaign::Campaign()
{
    campaign = new Map[64];
    name = "DEFAULT";
}

Campaign::Campaign(string in)
{
    campaign = new Map[64];
    name = in;
}

// DESTRUCTOR
Campaign::~Campaign()
{
    delete [] campaign;
}

bool Campaign::isNPC(string target)
{
    string type;
    ifstream active;
    active.open("Save_Data/Characters/" + target + ".txt");
    for (unsigned int i = 0; i < 3 ; i++)
        active >> type;
    if (type != "p")
        return true;
    else return false;
}

// SETS ACTIVE MAP
void Campaign::accessMap(int in)
{
    current = in;
    campaign[current].Notify("Map Initialized");
}

// USER ADD MAP TO CAMPAIGN
void Campaign::createMap()
{
    int x, y;
    string name;
    cout << "Input map width: ";
    cin >> x;
    cout << "Input map length: ";
    cin >> y;
    cout << "Enter map name: ";
    cin >> name;
    // Initialize Map //
    Map * map = new Map(name, x, y);
    DisplayMap *display = new DisplayMap(map);
    map->Attach(display);
    campaign[pos] = *map;
    
    // If map is first map in campaign
    if (pos == 0)
    {
        cout << "Enter the coordinates of the start: " << endl;
        cin >> x;
        cin >> y;
        campaign[pos].setCell(x-1, y-1, "CAMPAIGN_START", 0, 0);
        campaign[pos].setStartX(x-1);
        campaign[pos].setStartY(y-1);
    }
    pos++; 
}

// USER EDIT EXISTING MAP
void Campaign::editMap()
{
    ifstream active;
    string line;
    Director * direct = new Director();
    Builder * build = new CharacterBuilder();
    int file = 1;
    int s = 0;
    int x1, y1, x2, y2, type, map, temp;
    char end, tp;
    string name;
    do
    {
        // Shows current map state
        //campaign[current].print();
        cout << "Enter coordinates of cell to modify (row, column): " << endl;
        cin >> y1;
        cin >> x1;
        x1--;
        y1--;

        cout << "What would you like this cell to be?" << endl;
        cout << "1. Wall" << endl << "2. Container" << endl;
        cout << "3. Empty" << endl << "4. Door" << endl;
        cout << "5. NPC" << endl;
        cin >> type;

        switch (type)
        {
            case 1:
                campaign[current].setCell(x1, y1, 'w');
                break;
            case 2:
                campaign[current].setCell(x1, y1, 'c');
                while (1)
                {
                    cout << "Would you like to add an item to chest? (y/n)" << endl;
                    cin >> end;
                    if (end == 'N' || end == 'n')
                        break;
                    else if (end == 'Y' || end == 'y')
                    {
                        cout << "Select item type:" << endl;
                        cout << "1. Armor" << endl;
                        cout << "2. Helmet" << endl;
                        cout << "3. Boots" << endl;
                        cout << "4. Shield" << endl;
                        cout << "5. Weapon" << endl;
                        cout << "6. Ring" << endl;
                        cin >> type;
                        switch (type)
                        {
                            case 1:
                                tp = 'a';
                                break;
                            case 2:
                                tp = 'h';
                                break;
                            case 3:
                                tp = 'b';
                                break;
                            case 4:
                                tp = 's';
                                break;
                            case 5:
                                tp = 'w';
                                break;
                            case 6:
                                tp = 'r';
                                break;
                            default:
                                break;
                        }
                        
                        campaign[current].getCell(x1, y1).getContainer()->addItem(tp, 1);
                    }
                }
                break;
            case 3:
                campaign[current].setCell(x1, y1, 'n');
                break;
            case 4:
                for (unsigned int i = 0; i < pos; i++)
                    cout << i+1 << ". " << campaign[i].getName() << endl;
                cout << "Select a map to link to: ";
                cin >> map;
                temp = current;
                //campaign[map-1].print();
                cout << "Enter the coordinates the door should link to: ";
                cin >> x2;
                cin >> y2;
                x2--;
                y2--;
                campaign[map-1].setCell(x2, y2, campaign[current].getName(), x1, y1);
                accessMap(map-1);
                saveMap();
                accessMap(temp);
                campaign[current].setCell(x1, y1, campaign[map-1].getName(), x2, y2);
                break;
            case 5:
                active.open("Save_Data/Characters/Characters.txt");
                file = 1;
                while (1)
                {
                    cout << "Which character would you like to add?" << endl;
                    while (active >> line)
                    {
                        if (isNPC(line))
                        {
                            cout << file << ". " << line << endl;
                            file++;
                        }
                    }
                    cin >> s;
                    active.clear();
                    active.seekg(0, ios::beg);
                    if (s < file && s > 0)
                    {
                        for (unsigned int it = 0; it < s ; it++)
                        {
                            active >> line;
                            if (!isNPC(line)) it--;
                        }
                        cout << line << endl;
                        direct->setBuilder(build);
                        direct->constructCharacter(line);
                        campaign[current].setCell(x1, y1, direct->getCharacter());
                        campaign[current].addCharacter(direct->getCharacter());
                        active.close();
                        break;
                    }
                    else cerr << "Please enter a valid input" << endl;
                }
                break;
            default:
                break;
        }
        //campaign[current].Notify("Cell set");
        
        cout << "Continue editing map? (y/n): ";
        cin >> end;
        if ((end == 'n') || (end == 'N'))
        {
            if (campaign[current].verify())
                break;
            else
            {
                cout << "This map does not have a valid exit" << endl;
                cout << "Are you sure you want to exit? (y/n): ";
                cin >> end;
                if ((end == 'y') || (end == 'Y')) break;
            }
        }
    } while (1);
    // Updates map when finished
    saveMap();
}

int Campaign::playMap(Character * p)
{
    int target;
    int x, y;
    char move;
    
    campaign[current].addCharacter(p);
    
    int numChars = campaign[current].getNumChars();
    
    x = campaign[current].getStartX();
    y = campaign[current].getStartY();
    
    campaign[current].getCharacter(numChars-1)->setCharX(x);
    campaign[current].getCharacter(numChars-1)->setCharY(y);
    
    campaign[current].setCell(x, y, campaign[current].getCharacter(numChars-1));
    
    for (unsigned int i = 0; i < numChars+1; i++)
    {
        i = i%numChars;
        x = campaign[current].getCharacter(i)->getCharX();
        y = campaign[current].getCharacter(i)->getCharY();
        
        // Player's Turn
        if (campaign[current].getCharacter(i)->getType() == 'p')
        {
            campaign[current].print();
            cout << "What would you like to do?" << endl;
            // Forward Actions
            if (y+1 < campaign[current].getLength())
            {
                cout << "  w. ";
                if (campaign[current].getCell(x, y+1).getType() == 'e')
                    cout << "Attack forward" << endl;
                else if (campaign[current].getCell(x, y+1).getType() == 'd')
                    if (!campaign[current].getCell(x, y+1).getDoor()->getVisited())
                        cout << "Advance to new map" << endl;
                    else cout << "Return to previous map" << endl;
                else if (!(campaign[current].getCell(x, y+1).isBlocked()))
                    cout << "Move forward" << endl;
                else cout << "Blocked" << endl;
            }
            // Right Actions
            if (x+1 < campaign[current].getWidth())
            {
                cout << "  d. ";
                if (campaign[current].getCell(x+1, y).getType() == 'e')
                    cout << "Attack right" << endl;
                else if (campaign[current].getCell(x+1, y).getType() == 'd')
                    if (!campaign[current].getCell(x+1, y).getDoor()->getVisited())
                        cout << "Advance to new map" << endl;
                    else cout << "Return to previous map" << endl;
                else if (!(campaign[current].getCell(x+1, y).isBlocked()))
                    cout << "Move right" << endl;
                else cout << "Blocked" << endl;
            }
            // Backwards Actions
            if (y-1 >= 0)
            {
                cout << "  s. ";
                if (campaign[current].getCell(x, y-1).getType() == 'e')
                    cout << "Attack behind" << endl;
                else if (campaign[current].getCell(x, y-1).getType() == 'd')
                    if (!campaign[current].getCell(x, y-1).getDoor()->getVisited())
                        cout << "Advance to new map" << endl;
                    else cout << "Return to previous map" << endl;
                else if (!(campaign[current].getCell(x, y-1).isBlocked()))
                    cout << "Move backward" << endl;
                else cout << "Blocked" << endl;
            }
            // Left Actions
            if (x-1 >= 0)
            {
                cout << "  a. ";
                if (campaign[current].getCell(x-1, y).getType() == 'e')
                    cout << "Attack left" << endl;
                else if (campaign[current].getCell(x-1, y).getType() == 'd')
                    if (!campaign[current].getCell(x-1, y).getDoor()->getVisited())
                        cout << "Advance to new map" << endl;
                    else cout << "Return to previous map" << endl;
                else if (!(campaign[current].getCell(x-1, y).isBlocked()))
                    cout << "Move left" << endl;
                else cout << "Blocked" << endl;
            }
            cin >> move;
            
            if ((move == 'M') || (move == 'm'))
            {
                saveMap();
                break;
            }//*/
            else if ((move == 'C') || (move == 'c'))
            {
                //character menu
            }
            else if ((move == 'B') || (move == 'b'))
            {
                // backpack menu
            }
            else if ((move == 'L') || (move == 'l'))
            {
                // game log
            }
            
            // Determines move made
            switch (move)
            {
                case 'w':
                    if (y+1 < campaign[current].getLength())
                    {
                        if (campaign[current].getCell(x, y+1).getType() == 'e')
                        {
                            for (unsigned int k = 1; i < numChars; k++)
                                if (campaign[current].getCharacter(k)->getName() == campaign[current].getCell(x, y+1).getCharacter()->getName())
                                {
                                    target = k;
                                    break;
                                }
                            campaign[current].getCharacter(i)->attack(campaign[current].getCharacter(target));
                        }
                        else if (campaign[current].getCell(x, y+1).getType() == 'd')
                        {
                            string link = campaign[current].getCell(x, y+1).getDoor()->getLink();
                            int x2 = campaign[current].getCell(x, y+1).getDoor()->getX();
                            int y2 = campaign[current].getCell(x, y+1).getDoor()->getY();
                            unsigned int nm = 0;
                            for (nm = 0; nm < pos; nm++)
                            {
                                if (campaign[nm].getName() == link)
                                {
                                    campaign[nm].setStartX(x2);
                                    campaign[nm].setStartY(y2);
                                }
                            }
                            if (!(campaign[current].getCell(x, y+1).getDoor()->getVisited()))
                            {
                                campaign[current].getCell(x, y+1).getDoor()->setVisited();
                                
                                cout << "Congratulations of completing the map!" << endl;
                                campaign[current].getCharacter(i)->levelUp();
                                campaign[current].setCell(x,y, 'n');
                                cout << "You have advanced to level " << campaign[current].getCharacter(i)->getLevel() << "!" << endl;
                                return nm-1;
                            }
                            else
                            {
                                campaign[current].setCell(x,y, 'n');
                                cout << "Returning to previous map..." << endl;
                                return nm-1;
                            }
                        }
                        else if (!(campaign[current].getCell(x, y+1).isBlocked()))
                        {
                           campaign[current].setCell(x, y+1, campaign[current].getCharacter(i));
                           if (campaign[current].getCell(x,y).getDoor())
                           {
                               int x2 = campaign[current].getCell(x,y).getDoor()->getX();
                               int y2 = campaign[current].getCell(x,y).getDoor()->getY();
                               string link = campaign[current].getCell(x,y).getDoor()->getLink();
                               campaign[current].setCell(x,y,link, x2, y2);
                               campaign[current].getCell(x,y).getDoor()->setVisited();
                           }
                           else campaign[current].setCell(x, y, 'n');
                           campaign[current].getCharacter(i)->move(x, y+1);
                        }
                        else cout << "Passed Turn" << endl;
                    }
                    break;
                case 'd':
                    if (x+1 < campaign[current].getWidth())
                    {
                        if (campaign[current].getCell(x+1, y).getType() == 'e')
                        {
                            cout << campaign[current].getCell(x+1, y).getCharacter()->getName() << endl;
                            for (unsigned int k = 1; i < numChars; k++)
                            {
                                if (campaign[current].getCharacter(k)->getName() == campaign[current].getCell(x+1, y).getCharacter()->getName())
                                {
                                    target = k;
                                    break;
                                }
                            }
                            campaign[current].getCharacter(i)->attack(campaign[current].getCharacter(target));
                        }
                        else if (campaign[current].getCell(x+1, y).getType() == 'd')
                        {
                            string link = campaign[current].getCell(x+1, y).getDoor()->getLink();
                            int x2 = campaign[current].getCell(x+1, y).getDoor()->getX();
                            int y2 = campaign[current].getCell(x+1, y).getDoor()->getY();
                            unsigned int nm = 0;
                            for (nm = 0; nm < pos; nm++)
                            {
                                if (campaign[nm].getName() == link)
                                {
                                    campaign[nm].setStartX(x2);
                                    campaign[nm].setStartY(y2);
                                }
                            }
                            if (!(campaign[current].getCell(x+1, y).getDoor()->getVisited()))
                            {
                                campaign[current].getCell(x+1, y).getDoor()->setVisited();
                                cout << "Congratulations of completing the map!" << endl;
                                campaign[current].getCharacter(i)->levelUp();
                                campaign[current].setCell(x,y, 'n');
                                cout << "You have advanced to level " << campaign[current].getCharacter(i)->getLevel() << "!" << endl;
                                return nm-1;
                            }
                            else
                            {
                                campaign[current].setCell(x,y, 'n');
                                cout << "Returning to previous map..." << endl;
                                return nm-1;
                            }
                        }
                        else if (!(campaign[current].getCell(x+1, y).isBlocked()))
                        {
                           campaign[current].setCell(x+1, y, campaign[current].getCharacter(i));
                           if (campaign[current].getCell(x,y).getDoor())
                           {
                               int x2 = campaign[current].getCell(x,y).getDoor()->getX();
                               int y2 = campaign[current].getCell(x,y).getDoor()->getY();
                               string link = campaign[current].getCell(x,y).getDoor()->getLink();
                               campaign[current].setCell(x,y,link, x2, y2);
                               campaign[current].getCell(x,y).getDoor()->setVisited();
                           }
                           else campaign[current].setCell(x, y, 'n');
                           campaign[current].getCharacter(i)->move(x+1, y);
                        }
                        else cout << "Passed Turn" << endl;
                    }
                    break;
                case 's':
                    if (y-1 >= 0)
                    {
                        if (campaign[current].getCell(x, y-1).getType() == 'e')
                        {
                            for (unsigned int k = 1; i < numChars; k++)
                                if (campaign[current].getCharacter(k)->getName() == campaign[current].getCell(x, y-1).getCharacter()->getName())
                                {
                                    target = k;
                                    break;
                                }
                            campaign[current].getCharacter(i)->attack(campaign[current].getCharacter(target));
                        }
                        else if (campaign[current].getCell(x, y-1).getType() == 'd')
                        {
                            string link = campaign[current].getCell(x, y-1).getDoor()->getLink();
                            int x2 = campaign[current].getCell(x, y-1).getDoor()->getX();
                            int y2 = campaign[current].getCell(x, y-1).getDoor()->getY();
                            unsigned int nm = 0;
                            for (nm = 0; nm < pos; nm++)
                            {
                                if (campaign[nm].getName() == link)
                                {
                                    campaign[nm].setStartX(x2);
                                    campaign[nm].setStartY(y2);
                                }
                            }
                            if (!(campaign[current].getCell(x, y-1).getDoor()->getVisited()))
                            {
                                campaign[current].getCell(x, y-1).getDoor()->setVisited();
                                cout << "Congratulations of completing the map!" << endl;
                                campaign[current].getCharacter(i)->levelUp();
                                campaign[current].setCell(x,y, 'n');
                                cout << "You have advanced to level " << campaign[current].getCharacter(i)->getLevel() << "!" << endl;
                                return nm-1;
                            }
                            else
                            {
                                campaign[current].setCell(x,y, 'n');
                                cout << "Returning to previous map..." << endl;
                                return nm-1;
                            }
                        }
                        else if (!(campaign[current].getCell(x, y-1).isBlocked()))
                        {
                           campaign[current].setCell(x, y-1, campaign[current].getCharacter(i));
                           if (campaign[current].getCell(x,y).getDoor())
                           {
                               int x2 = campaign[current].getCell(x,y).getDoor()->getX();
                               int y2 = campaign[current].getCell(x,y).getDoor()->getY();
                               string link = campaign[current].getCell(x,y).getDoor()->getLink();
                               campaign[current].setCell(x,y,link, x2, y2);
                               campaign[current].getCell(x,y).getDoor()->setVisited();
                           }
                           else campaign[current].setCell(x, y, 'n');
                           campaign[current].getCharacter(i)->move(x, y-1);
                        }
                        else cout << "Passed Turn" << endl;
                    }
                    break;
                case 'a':
                    if (x-1 >= 0)
                    {
                        if (campaign[current].getCell(x-1, y).getType() == 'e')
                        {
                            for (unsigned int k = 1; i < numChars; k++)
                                if (campaign[current].getCharacter(k)->getName() == campaign[current].getCell(x-1, y).getCharacter()->getName())
                                {
                                    target = k;
                                    break;
                                }
                            campaign[current].getCharacter(i)->attack(campaign[current].getCharacter(target));
                        }
                        else if (campaign[current].getCell(x-1, y).getType() == 'd')
                        {
                            string link = campaign[current].getCell(x-1, y).getDoor()->getLink();
                            int x2 = campaign[current].getCell(x-1, y).getDoor()->getX();
                            int y2 = campaign[current].getCell(x-1, y).getDoor()->getY();
                            unsigned int nm = 0;
                            for (nm = 0; nm < pos; nm++)
                            {
                                if (campaign[nm].getName() == link)
                                {
                                    campaign[nm].setStartX(x2);
                                    campaign[nm].setStartY(y2);
                                }
                            }
                            if (!(campaign[current].getCell(x-1, y).getDoor()->getVisited()))
                            {
                                campaign[current].getCell(x-1, y).getDoor()->setVisited();
                                cout << "Congratulations of completing the map!" << endl;
                                campaign[current].getCharacter(i)->levelUp();
                                campaign[current].setCell(x,y, 'n');
                                cout << "You have advanced to level " << campaign[current].getCharacter(i)->getLevel() << "!" << endl;
                                return nm-1;
                            }
                            else
                            {
                                campaign[current].setCell(x,y, 'n');
                                cout << "Returning to previous map..." << endl;
                                return nm-1;
                            }
                        }
                        else if (!(campaign[current].getCell(x-1, y).isBlocked()))
                        {
                           campaign[current].setCell(x-1, y, campaign[current].getCharacter(i));
                           if (campaign[current].getCell(x,y).getDoor())
                           {
                               int x2 = campaign[current].getCell(x,y).getDoor()->getX();
                               int y2 = campaign[current].getCell(x,y).getDoor()->getY();
                               string link = campaign[current].getCell(x,y).getDoor()->getLink();
                               campaign[current].setCell(x,y,link, x2, y2);
                               campaign[current].getCell(x,y).getDoor()->setVisited();
                           }
                           else campaign[current].setCell(x, y, 'n');
                           campaign[current].getCharacter(i)->move(x-1, y);
                        }
                        else cout << "Passed Turn" << endl;
                    }
                    break;
                default:
                    break;
            }
        }
        
        // Enemy's Turns
        else if (campaign[current].getCharacter(i)->getType() == 'e')
        {
            if (y < campaign[current].getCharacter(numChars-1)->getCharY())
            {
                for (unsigned int j = y; j < campaign[current].getLength(); j++)
                {
                    if (campaign[current].getCell(x, j).isBlocked()) break;
                    else if (j == campaign[current].getCharacter(numChars-1)->getCharY())
                    { 
                        if (campaign[current].getCell(x, y+1).getType() == 'e')
                        {
                            for (unsigned int k = 1; i < numChars; k++)
                            {
                                if (campaign[current].getCharacter(k)->getName() == campaign[current].getCell(x, y+1).getCharacter()->getName())
                                {
                                    target = k;
                                    break;
                                }
                            campaign[current].getCharacter(i)->attack(campaign[current].getCharacter(target));
                            break;
                            }
                        }
                        else if (campaign[current].getCell(x, j).getType() == 'e')
                            if (campaign[current].getCell(x, j).getCharacter()->getType() == 'p')
                        {
                        campaign[current].setCell(x, y+1, campaign[current].getCharacter(i));
                        campaign[current].setCell(x, y, 'n');
                        campaign[current].getCharacter(i)->move(x, y+1);
                        break;
                        }
                    }
                }
            }
            else if (x < campaign[current].getCharacter(numChars-1)->getCharX())
            {
                for (unsigned int j = x; j < campaign[current].getWidth(); j++)
                {
                    if (campaign[current].getCell(j, y).isBlocked()) break;
                    else if (j == campaign[current].getCharacter(numChars-1)->getCharX())
                    {
                        if (campaign[current].getCell(x+1, y).getType() == 'e')
                        {
                            for (unsigned int k = 1; i < numChars; k++)
                            {
                                if (campaign[current].getCharacter(k)->getName() == campaign[current].getCell(x+1, y).getCharacter()->getName())
                                {
                                    target = k;
                                    break;
                                }
                            campaign[current].getCharacter(i)->attack(campaign[current].getCharacter(target));
                            break;
                            }
                        }
                        else if (campaign[current].getCell(j, y).getType() == 'e')
                            if (campaign[current].getCell(j, y).getCharacter()->getType() == 'p')
                        {
                        campaign[current].setCell(x+1, y, campaign[current].getCharacter(i));
                        campaign[current].setCell(x, y, 'n');
                        campaign[current].getCharacter(i)->move(x+1, y);
                        break;
                        }
                    }
                } 
            }
            else if (y > campaign[current].getCharacter(numChars-1)->getCharY())
            {
                for (int j = y; j >= 0; j--)
                {
                    if (campaign[current].getCell(x, j).isBlocked()) break;
                    else if (j == campaign[current].getCharacter(numChars-1)->getCharY())
                    {
                        if (campaign[current].getCell(x, y-1).getType() == 'e')
                        {
                            for (unsigned int k = 1; i < numChars; k++)
                            {
                                if (campaign[current].getCharacter(k)->getName() == campaign[current].getCell(x, y-1).getCharacter()->getName())
                                {
                                    target = k;
                                    break;
                                }
                            campaign[current].getCharacter(i)->attack(campaign[current].getCharacter(target));
                            break;
                            }
                        }
                        else if (campaign[current].getCell(x, j).getType() == 'e')
                            if (campaign[current].getCell(x, j).getCharacter()->getType() == 'p')
                        {
                        campaign[current].setCell(x, y-1, campaign[current].getCharacter(i));
                        campaign[current].setCell(x, y, 'n');
                        campaign[current].getCharacter(i)->move(x, y-1);
                        break;
                        }
                    }
                }
            }
            else if (x > campaign[current].getCharacter(numChars-1)->getCharX())
            {
                for (int j = x; j >= 0; j--)
                {
                    if (campaign[current].getCell(j, y).isBlocked()) break;
                    else if (j == campaign[current].getCharacter(numChars-1)->getCharX())
                    {
                        if (campaign[current].getCell(x-1, y).getType() == 'e')
                        {
                            for (unsigned int k = 1; i < numChars; k++)
                            {
                                if (campaign[current].getCharacter(k)->getName() == campaign[current].getCell(x-1, y).getCharacter()->getName())
                                {
                                    target = k;
                                    break;
                                }
                            campaign[current].getCharacter(i)->attack(campaign[current].getCharacter(target));
                            break;
                            }
                        }
                        else if (campaign[current].getCell(j, y).getType() == 'e')
                            if (campaign[current].getCell(j, y).getCharacter()->getType() == 'p')
                            {
                                campaign[current].setCell(x-1, y, campaign[current].getCharacter(i));
                                campaign[current].setCell(x, y, 'n');
                                campaign[current].getCharacter(i)->move(x-1, y);
                                break;
                            }
                    }
                }
            }
        }
        
        //Ally's Turns
        else
        {
           if (y < campaign[current].getCharacter(numChars-1)->getCharY())
           {
               for (int j = y; j < campaign[current].getLength(); j++)
               {
                   if (campaign[current].getCell(x, j).isBlocked()) break;
                   else if (j == campaign[current].getCharacter(numChars-1)->getCharY())
                        if (campaign[current].getCell(x, j).getType() == 'e')
                            if (campaign[current].getCell(x, j).getCharacter()->getType() == 'p')
                            {
                                campaign[current].setCell(x, y+1, campaign[current].getCharacter(i));
                                campaign[current].setCell(x, y, 'n');
                                campaign[current].getCharacter(i)->move(x, y+1);
                                break;    
                            }
               }     
           }
           else if (x < campaign[current].getCharacter(numChars-1)->getCharX())
           {
                for (int j = x; j < campaign[current].getWidth(); j++)
                {
                   if (campaign[current].getCell(j, y).isBlocked()) break;
                   else if (j == campaign[current].getCharacter(numChars-1)->getCharX())
                        if (campaign[current].getCell(j, x).getType() == 'e')
                            if (campaign[current].getCell(j, x).getCharacter()->getType() == 'p')
                            {
                                campaign[current].setCell(x+1, y, campaign[current].getCharacter(i));
                                campaign[current].setCell(x, y, 'n');
                                campaign[current].getCharacter(i)->move(x+1, y);
                                break;    
                            }
                } 
           }
           else if (y > campaign[current].getCharacter(numChars-1)->getCharY())
           {
               for (int j = y; j >= 0; j--)
               {
                   if (campaign[current].getCell(x, j).isBlocked()) break;
                   else if (j == campaign[current].getCharacter(numChars-1)->getCharY())
                        if (campaign[current].getCell(x, j).getType() == 'e')
                            if (campaign[current].getCell(x, j).getCharacter()->getType() == 'p')
                            {
                                campaign[current].setCell(x, y-1, campaign[current].getCharacter(i));
                                campaign[current].setCell(x, y, 'n');
                                campaign[current].getCharacter(i)->move(x, y-1);
                                break;    
                            }
               }     
           }
           else if (x > campaign[current].getCharacter(numChars-1)->getCharX())
           {
                for (int j = x; j >= 0; j--)
                {
                   if (campaign[current].getCell(j, y).isBlocked()) break;
                   else if (j == campaign[current].getCharacter(numChars-1)->getCharX())
                        if (campaign[current].getCell(j, y).getType() == 'e')
                            if (campaign[current].getCell(j, y).getCharacter()->getType() == 'p')
                            {
                                campaign[current].setCell(x-1, y, campaign[current].getCharacter(i));
                                campaign[current].setCell(x, y, 'n');
                                campaign[current].getCharacter(i)->move(x-1, y);
                                break;    
                            }
                } 
           }
        }
    }
    
    return -1;
}

// SAVE MAP TO FILE
void Campaign::saveMap() const
{
    // Formats file name, and opens map file (creates if new)
    string target;
    target = campaign[current].getName() +".txt";
    ofstream active;
    active.open("Save_Data/Campaigns/" + target);

    // Writes map name
    active << campaign[current].getName() << '\n';

    // Writes dimensions
    active << campaign[current].getLength() << ' ';
    active << campaign[current].getWidth() << '\n';

    // Runs through each cell and writes contents
    for (unsigned int i = 0; i < campaign[current].getLength(); i++)
        for (unsigned int j = 0; j < campaign[current].getWidth(); j++)
        {
            active << campaign[current].getCell(j, i).getType();
            switch (campaign[current].getCell(j, i).getType())
            {
                case 'n':
                    active << '\n';
                    break;
                case 'd':
                    active << ' ' << campaign[current].getCell(j, i).getDoor()->getLink();
                    active << ' ' << campaign[current].getCell(j, i).getDoor()->getX();
                    active << ' ' << campaign[current].getCell(j, i).getDoor()->getY();
                    active << '\n';
                    break;
                case 'c':
                    active << ' ' << campaign[current].getCell(j, i).getContainer()->getSize();
                    for (unsigned int k = 0; k < campaign[current].getCell(j, i).getContainer()->getSize(); k++) {
                        active << '\n' << campaign[current].getCell(j, i).getContainer()->getItem(k).getType();
                        active << ' ' << campaign[current].getCell(j, i).getContainer()->getItem(k).getEnchantment();
                    }
                    active << '\n';
                    break;
                case 'e':
                    active << ' ' << campaign[current].getCell(j, i).getCharacter()->getName() << '\n';
                    cout << campaign[current].getCell(j, i).getCharacter()->getName() << endl;
                    break;
                case 'w':
                    active << '\n';
                    break;
                default:
                    break;
            }
        }
    // Confirmation message
    cout << "Map saved to " << target << endl;
    active.close();
}

void Campaign::saveCharacter(Character * local) const
{
    // Accesses character file
    string target;
    target = local->getName() +".txt";
    ofstream active;
    active.open("Save_Data/Characters/" + target);
    
    // Writes character name
    active << local->getName() << '\n';

    // Writes character role/type
    active << local->getRole() << '\n';
    active << local->getType() << '\n';
    
    // Writes character stats
    active << local->getLevel() << '\n';
    active << local->getHealth() << '\n';
    active << local->getMax() << '\n';
    
    for (unsigned int i = 0; i < 7; i++)
    {
        active << local->getStat(i) << ' ';
    }
    active << local->getStat(7) << '\n';
    
    active << local->getStrAttackBonus() << '\n';
    active << local->getDexAttackBonus() << '\n';
    
    active << local->getAValue() << '\n';
    active << local->getHValue() << '\n';
    active << local->getBValue() << '\n';
    active << local->getSValue() << '\n';
    active << local->getWValue() << '\n';
    active << local->getRValue() << '\n';
    
    // Confirmation message
    cout << "Character saved to " << target << endl;
    active.close();  
}

// ADDS EXISTING MAP
void Campaign::addMap(Map & loaded)
{
    campaign[pos] = loaded;
    pos++;
}

string Campaign::getName() const { return name; }

// ACCESS MAP
Map Campaign::getMap(int x) const
{
    return campaign[x];
}

// ACCESS LAST MAP
int Campaign::getPos() const
{
    return pos;
}

// LIST MAPS
void Campaign::print() const
{
    unsigned int i = 0;
    for (; i < pos; i++)
        cout << i+1 << ". " << campaign[i].getName() << endl;
    cout << i+1 << ". New Map" << endl;
}




