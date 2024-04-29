#include "ICS_Project.h"
// #include"cJSON.c"

void flushInputBuffer() {
    char c;
    if (!feof(stdin) && !ferror(stdin)) {
        do
        {
            c=getchar();
            // printf("\n%d",c); // Read and discard characters until newline or EOF
        }while(c != '\n');
    }
}

void gameLost(Player *player,int *state)
{

    player->currentLocation = strdup("WORLD/CITY");
    player->gold = 0;
    player->xp = 0;
    player->inventory->items[1] = NULL;
    player->inventory->activeItems[0] = 0;
    player->inventory->activeItems[1] = -1;
    *state=0;

}

int getNpcNumber() // This works checked
{
    FILE *file = fopen("characters.json", "r");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open file characters.json for reading\n");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *fileContent = (char *)malloc(fileSize + 1);
    if (fileContent == NULL)
    {
        fprintf(stderr, "Memory allocation failed for file content\n");
        fclose(file);
        return -1;
    }

    size_t bytesRead = fread(fileContent, 1, fileSize, file);
    // if (bytesRead != fileSize) {
    //     printf("Error reading file.\n");
    //     fclose(file);
    //     free(fileContent);
    //     return NULL;
    // }

    fileContent[fileSize] = 0;
    fclose(file);

    cJSON *root = cJSON_Parse(fileContent);
    if (root == NULL)
    {
        printStory("Error parsing JSON data.\n",BMAG,HIG);
        free(fileContent);
        return -1;
    }

    cJSON *charactersArray = cJSON_GetObjectItem(root, "characters");

    return cJSON_GetArraySize(charactersArray);
}

// FROM world.c

void delay2(int milliseconds)
{
    clock_t start_time = clock();
    while (clock() < start_time + milliseconds * CLOCKS_PER_SEC / 1000)
        ;
}

void printStory(const char *sentence, char *textStyle,int time)
{
    // Print color code
    printf("%s", textStyle);

    // Print the sentence with typing effect
    for (int i = 0; i < strlen(sentence); i++)
    {
        printf("%c", sentence[i]);
        fflush(stdout); // Flush the output buffer to ensure immediate printing
        delay2(time);     // Adjust delay as needed (50 milliseconds in this example)
    }

    // Reset color to default
    printf("\033[0m");
}

void printFormattedStringWithColorAndDelay(const char *format, char *textStyle, int delay, ...) {
    va_list args;
    va_start(args, delay);
    printf("%s", textStyle);
    vprintf(format, args);
    printf("\033[0m");
    va_end(args);
    fflush(stdout); // Flush stdout to ensure immediate output
    delay2(delay); // Delay in milliseconds
}

int playMiniGame(Player *player, char *gameName)
{

    // Define an array of string-function pairs
    // correct the names of minigame function to these
    StringFunctionPair functions[] = {
        // {"Combat", Combat},
        {"falconry", falconry},
        {"Combat", Combat},
        {"PrisonEscape", PrisonEscape},
        {"Lockpicking", Lockpicking},
        {"Memory", Memory},
        {"Minesweeper", Minesweeper},
        {"Hangtheman", Hangtheman},
        {"Snakegame", Snakegame},
        {NULL, NULL}
        // add more games
    };

    // to call a function based on input string
    int i;
    for (i = 0; functions[i].str != NULL; i++)
    {
        if (strcmp(gameName, functions[i].str) == 0)
        {
            printf("\e[1;1H\e[2J");
            return functions[i].func(); // Call the function associated with the input string
        }
    }
    printStory("No function found for input: ",BWHT,HIG);
    printStory( gameName,BWHT,HIG);
    printf("\n");
}

Player *createNewPlayer(char *playerID)
{
    // printf("Creating new player...\n");

    // Allocate memory for the player struct
    Player *newPlayer = (Player *)malloc(sizeof(Player));
    if (newPlayer == NULL)
    {
        fprintf(stderr, "Memory allocation failed for Player\n");
        exit(EXIT_FAILURE);
    }
    // printf("Allocated memory for player struct.\n");

    // Initialize player ID
    newPlayer->id = strdup(playerID);
    if (newPlayer->id == NULL)
    {
        fprintf(stderr, "Memory allocation failed for ID\n");
        exit(EXIT_FAILURE);
    }
    // printf("Initialized player ID: %s\n", newPlayer->id);

    // Initialize player name
    newPlayer->name = strdup("Gaius Marcus Agrippa");
    if (newPlayer->name == NULL)
    {
        fprintf(stderr, "Memory allocation failed for name\n");
        exit(EXIT_FAILURE);
    }
    // printf("Initialized player name: %s\n", newPlayer->name);

    // Allocate memory for player stats
    newPlayer->stats = (Stats *)malloc(sizeof(Stats));
    if (newPlayer->stats == NULL)
    {
        fprintf(stderr, "Memory allocation failed for stats\n");
        exit(EXIT_FAILURE);
    }
    // printf("Allocated memory for player stats.\n");

    // Initialize player inventory
    newPlayer->inventory = (Inventory *)malloc(sizeof(Inventory));
    if (newPlayer->inventory == NULL)
    {
        fprintf(stderr, "Memory allocation failed for inventory\n");
        exit(EXIT_FAILURE);
    }
    // printf("Allocated memory for player inventory.\n");

    // Allocate memory for inventory items and active items
    newPlayer->inventory->items = (char **)malloc(11*sizeof(char *));
    newPlayer->inventory->activeItems = (int *)malloc(11*sizeof(int));
    if (newPlayer->inventory->items == NULL || newPlayer->inventory->activeItems == NULL)
    {
        fprintf(stderr, "Memory allocation failed for inventory items or active items\n");
        exit(EXIT_FAILURE);
    }
    // printf("Allocated memory for inventory items and active items.\n");

    // Initialize current location
    newPlayer->currentLocation = strdup("WORLD/CITY");
    if (newPlayer->currentLocation == NULL)
    {
        fprintf(stderr, "Memory allocation failed for current location\n");
        exit(EXIT_FAILURE);
    }
    // printf("Initialized current location: %s\n", newPlayer->currentLocation);

    // Allocate memory for active quests
    newPlayer->activeQuests = (char **)malloc(sizeof(char *));
    if (newPlayer->activeQuests == NULL)
    {
        fprintf(stderr, "Memory allocation failed for active quests\n");
        exit(EXIT_FAILURE);
    }
    // printf("Allocated memory for active quests.\n");

    // Get the number of NPCs
    int numOfNpcs = getNpcNumber();

    // Allocate memory for NPC info
    newPlayer->NPCInfo = (int **)malloc(sizeof(int *) * numOfNpcs);
    if (newPlayer->NPCInfo == NULL)
    {
        fprintf(stderr, "Memory allocation failed for NPC info\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < numOfNpcs; i++)
    {
        newPlayer->NPCInfo[i] = (int *)calloc(3, sizeof(int));
        if (newPlayer->NPCInfo[i] == NULL)
        {
            fprintf(stderr, "Memory allocation failed for NPC info\n");
            exit(EXIT_FAILURE);
        }
    }
    // printf("Allocated memory for NPC info.\n");

    // Initialize player properties
    newPlayer->level = 1;

    // Initialize player stats
    newPlayer->stats->HP = 100;
    newPlayer->stats->atk = 10;
    newPlayer->stats->def = 5;
    newPlayer->stats->agi = 8;
    newPlayer->stats->str = 12;
    newPlayer->stats->dex = 9;
    newPlayer->stats->intel = 6;
    newPlayer->stats->luck = 7;
    // printf("Initialized player properties and stats.\n");

    // Initialize player inventory size and items
    newPlayer->inventory->size = 10;
    newPlayer->inventory->items[0]=strdup("Gladiator's Sword");
    newPlayer->inventory->items[1] = NULL;
    newPlayer->inventory->activeItems[0] = 0;  // not equiped
    newPlayer->inventory->activeItems[1] = -1; // NULL

    // printf("Initialized player inventory.\n");

    // Initialize other player properties
    newPlayer->wtdLevel = 0;
    newPlayer->xp = 0;
    newPlayer->gold = 50;
    // printf("Initialized other player properties.\n");

    // Initialize active quests
    newPlayer->activeQuests[0] = NULL;
    // printf("Initialized active quests.\n");

    // printf("Player creation successful.\n");

    return newPlayer;
}

void savePlayerData(Player *player) // This works checked
{
    cJSON *root = cJSON_CreateObject();
    cJSON *playerObject = cJSON_CreateObject();

    cJSON_AddItemToObject(root, "player", playerObject);

    cJSON_AddStringToObject(playerObject, "id", player->id);
    cJSON_AddStringToObject(playerObject, "name", player->name);
    cJSON_AddNumberToObject(playerObject, "level", player->level);
    cJSON_AddNumberToObject(playerObject, "wtdLevel", player->wtdLevel);
    cJSON_AddNumberToObject(playerObject, "xp", player->xp);
    cJSON_AddNumberToObject(playerObject, "gold", player->gold);
    cJSON_AddStringToObject(playerObject, "currentLocation", player->currentLocation);

    cJSON *statsObject = cJSON_CreateObject();
    cJSON_AddItemToObject(playerObject, "stats", statsObject);

    cJSON_AddNumberToObject(statsObject, "hp", player->stats->HP);
    cJSON_AddNumberToObject(statsObject, "atk", player->stats->atk);
    cJSON_AddNumberToObject(statsObject, "def", player->stats->def);
    cJSON_AddNumberToObject(statsObject, "agi", player->stats->agi);
    cJSON_AddNumberToObject(statsObject, "str", player->stats->str);
    cJSON_AddNumberToObject(statsObject, "dex", player->stats->dex);
    cJSON_AddNumberToObject(statsObject, "intel", player->stats->intel);
    cJSON_AddNumberToObject(statsObject, "luck", player->stats->luck);

    cJSON *inventoryObject = cJSON_CreateObject();
    cJSON_AddItemToObject(playerObject, "inventory", inventoryObject);

    cJSON_AddNumberToObject(inventoryObject, "size", player->inventory->size);
    cJSON *itemsArray = cJSON_AddArrayToObject(inventoryObject, "items");

    for (int i = 0; player->inventory->items[i]; i++)
    {
        cJSON *item = cJSON_CreateObject();
        cJSON_AddItemToArray(itemsArray, item);
        cJSON_AddStringToObject(item, "item", player->inventory->items[i]);
        cJSON_AddNumberToObject(item, "isEquiped", player->inventory->activeItems[i]);
    }

    cJSON *npcsArray = cJSON_AddArrayToObject(playerObject, "NPCInfo");
    int n = getNpcNumber();
    for (int i = 0; i < n; i++)
    {
        cJSON *npc = cJSON_CreateObject();
        cJSON_AddItemToArray(npcsArray, npc);
        cJSON_AddNumberToObject(npc, "npc_id", i);
        cJSON_AddNumberToObject(npc, "quest_lvl", player->NPCInfo[i][QUEST_LVL]);
        cJSON_AddNumberToObject(npc, "quest_status", player->NPCInfo[i][QUEST_STATUS]);
        cJSON_AddNumberToObject(npc, "relationship", player->NPCInfo[i][RELATIONSHIP]);
        // printf("\nNPC %d: quest_lvl=%d, quest_status=%d, relationship=%d\n", i, player->NPCInfo[i][QUEST_LVL], player->NPCInfo[i][QUEST_STATUS], player->NPCInfo[i][RELATIONSHIP]);

    }

    cJSON *activeQuestArray = cJSON_AddArrayToObject(playerObject, "activeQuests");
    for (int i = 0; player->activeQuests[i]; i++)
    {
        cJSON *quest = cJSON_CreateObject();
        cJSON_AddItemToArray(activeQuestArray, quest);
        cJSON_AddStringToObject(quest, "QuestID", player->activeQuests[i]);
    }

    char *fileName = createFileName(player->id);
    // printf("%s", fileName);
    FILE *file = fopen(fileName, "w");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file for reading\n");
        return;
    }

    char *jsonData = cJSON_Print(root);
    fprintf(file, "%s", jsonData);
    fclose(file);
    free(jsonData);
    cJSON_Delete(root);
}

void freePlayer(Player *player)
{
    if (player == NULL) {
        return; // Nothing to free if player is NULL
    }

    // Free dynamically allocated members
    free(player->id);
    free(player->name);
    free(player->currentLocation);

    // Free Stats structure
    free(player->stats);

    // Free Inventory structure
    if (player->inventory != NULL) 
    {
        // Free items in inventory
        for (int i = 0; i < player->inventory->size; i++)
        {
            free(player->inventory->items[i]);
        }
        free(player->inventory->items);
        free(player->inventory->activeItems);
        free(player->inventory);
    }

    int n=getNpcNumber();
    // Free NPCInfo array
    if (player->NPCInfo != NULL) 
    {
        for (int i = 0; i<n; i++) 
        {
            free(player->NPCInfo[i]);
        }
        free(player->NPCInfo);
    }

    // Free activeQuests array
    if (player->activeQuests != NULL) 
    {
        for (int i = 0; player->activeQuests[i] != NULL; i++) 
        {
            free(player->activeQuests[i]);
        }
        free(player->activeQuests);
    }

    // Free the player structure itself
    free(player);
}

void showPlayerStats(Player *player)
{
    printf("\n");
    printStory("Player Stats:\n",BYEL,MED);
    printFormattedStringWithColorAndDelay("ID: %s\n",BYEL,MED, player->id);
    printFormattedStringWithColorAndDelay("Name: %s\n", BYEL,MED,player->name);
    printFormattedStringWithColorAndDelay("Level: %d\n",  BYEL,MED,player->level);
    printFormattedStringWithColorAndDelay("Experience Points: %d\n", BYEL,MED, player->xp);
    printFormattedStringWithColorAndDelay("Gold: %d\n", BYEL,MED, player->gold);
    printFormattedStringWithColorAndDelay("Current Location: %s\n", BYEL,MED, player->currentLocation);
    printFormattedStringWithColorAndDelay("Health Points (HP): %d (100)\n", BYEL,MED, player->stats->HP);
    printFormattedStringWithColorAndDelay("Attack (ATK): %d (10)\n", BYEL,MED, player->stats->atk);
    printFormattedStringWithColorAndDelay("Defense (DEF): %d (5)\n", BYEL,MED, player->stats->def);
    printFormattedStringWithColorAndDelay("Agility (AGI): %d (8)\n", BYEL,MED, player->stats->agi);
    printFormattedStringWithColorAndDelay("Strength (STR): %d (12)\n", BYEL,MED, player->stats->str);
    printFormattedStringWithColorAndDelay("Dexterity (DEX): %d (9)\n", BYEL,MED, player->stats->dex);
    printFormattedStringWithColorAndDelay("Intelligence (INT): %d (6)\n", BYEL,MED, player->stats->intel);
    printFormattedStringWithColorAndDelay("Luck: %d (7)\n", BYEL,MED,player->stats->luck);
}

void showPlayerInventory(Player *player)
{
    printf("\n");
    printStory("Player Inventory:\n", BYEL, MED);

    for (int i = 0; player->inventory->items[i] != NULL; i++)
    {
        if (player->inventory->activeItems[i] == 1)
        {
            printFormattedStringWithColorAndDelay("%s (Equipped) \n",BYEL,MED, player->inventory->items[i]);
        }
        else
        {
            printFormattedStringWithColorAndDelay("%s\n",BYEL,MED, player->inventory->items[i]);
        }
    }
}

char *createFileName( char *name) {
    // Define a constant extension
    const char *extension = ".json";

    // Calculate the length of the name and extension
    size_t nameLength = strlen(name);
    size_t extensionLength = strlen(extension);

    // Allocate memory for the filename
    char *fileName = (char *)malloc(nameLength + extensionLength + 1); // +1 for the null terminator
    if (fileName == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Copy the name to the filename buffer
    strcpy(fileName, name);

    // Concatenate the extension to the filename
    strcat(fileName, extension);

    return fileName;
}


Player *loadPlayerData(char *playerID) 
{
    char *fileName = createFileName(playerID);
    // printf("%s", fileName);
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file for reading\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    // printf("\nhi");

    char *fileContent = (char *)malloc(fileSize + 1);
    if (fileContent == NULL) {
        fprintf(stderr, "Memory allocation failed for file content\n");
        fclose(file);
        return NULL;
    }
    // printf("\nhi");

    fread(fileContent, 1, fileSize, file);
    fclose(file);
    fileContent[fileSize] = '\0';
    // printf("\nhi");

    cJSON *root = cJSON_Parse(fileContent);
    cJSON *playerObject = cJSON_GetObjectItem(root, "player");
    // printf("\nhi");

    Player *player = createNewPlayer(playerID); // Create a new player structure
    // printf("\nhi");

    // Load player properties from JSON object
    strcpy(player->id, cJSON_GetObjectItem(playerObject, "id")->valuestring);
    strcpy(player->name, cJSON_GetObjectItem(playerObject, "name")->valuestring);
    player->level = cJSON_GetObjectItem(playerObject, "level")->valueint;
    player->wtdLevel = cJSON_GetObjectItem(playerObject, "wtdLevel")->valueint;
    player->xp = cJSON_GetObjectItem(playerObject, "xp")->valueint;
    player->gold = cJSON_GetObjectItem(playerObject, "gold")->valueint;
    player->currentLocation = strdup(cJSON_GetObjectItem(playerObject, "currentLocation")->valuestring);

    // Load player stats from JSON object
    cJSON *statsObject = cJSON_GetObjectItem(playerObject, "stats");
    player->stats->HP = cJSON_GetObjectItem(statsObject, "HP")->valueint;
    player->stats->atk = cJSON_GetObjectItem(statsObject, "atk")->valueint;
    player->stats->def = cJSON_GetObjectItem(statsObject, "def")->valueint;
    player->stats->agi = cJSON_GetObjectItem(statsObject, "agi")->valueint;
    player->stats->str = cJSON_GetObjectItem(statsObject, "str")->valueint;
    player->stats->dex = cJSON_GetObjectItem(statsObject, "dex")->valueint;
    player->stats->intel = cJSON_GetObjectItem(statsObject, "intel")->valueint;
    player->stats->luck = cJSON_GetObjectItem(statsObject, "luck")->valueint;

    cJSON *inventoryObject = cJSON_GetObjectItem(playerObject, "inventory");
    player->inventory->size = cJSON_GetObjectItem(inventoryObject, "size")->valueint;
    cJSON *itemsArray = cJSON_GetObjectItem(inventoryObject, "items");

    cJSON *item = NULL;
    int i = 0;
    cJSON_ArrayForEach(item, itemsArray) {
        player->inventory->items[i] = strdup(cJSON_GetObjectItem(item, "item")->valuestring);
        player->inventory->activeItems[i] = cJSON_GetObjectItem(item, "isEquiped")->valueint;
        i++;
    }
    player->inventory->items[i] = NULL;

    cJSON *npcsArray = cJSON_GetObjectItem(playerObject, "NPCInfo");

    cJSON *npc = NULL;
    i = 0;
    cJSON_ArrayForEach(npc, npcsArray) {
        player->NPCInfo[i][QUEST_LVL] = cJSON_GetObjectItem(npc, "quest_lvl")->valueint;
        player->NPCInfo[i][QUEST_STATUS] = cJSON_GetObjectItem(npc, "quest_status")->valueint;
        player->NPCInfo[i][RELATIONSHIP] = cJSON_GetObjectItem(npc, "relationship")->valueint;
        i++;
    }

    cJSON *activeQuestArray = cJSON_GetObjectItem(playerObject, "activeQuests");
    cJSON *quest = NULL;
    i = 0;
    int n = cJSON_GetArraySize(activeQuestArray);
    player->activeQuests = (char **)malloc(sizeof(char *) * (n + 1));
    cJSON_ArrayForEach(quest, activeQuestArray) {
        player->activeQuests[i] = strdup(cJSON_GetObjectItem(quest, "QuestID")->valuestring);
        i++;
    }
    player->activeQuests[i] = NULL;

    cJSON_Delete(root);
    free(fileContent);
    // printf("\nplayer data loaded successfully.");
    return player;
}

Player *gameInitializer(char *PlayerID) // This works checked
{
    // asks player wheather to start a new game
    // (warns that load game will be deleted)
    // or to load game and if the file does not exist
    // it gives a prompt telling player to start a new game.
    // It also makes a new json file or load json files based on
    // player choise and returns a player variable
    int input;
    Player *player=NULL;

    printStory("\nEnter (1) to Start a New Game",YEL,20);
    printStory("\nEnter (2) Load an Old Game",YEL,20);
    printStory("\nEnter a Choice : ",YEL,20);
    scanf("%d", &input);
    flushInputBuffer();

    if (input == 1)
    {
        // Start a new game
        // Create a new player object and initialize its properties
        char *story =  "\n In the aftermath of war, Marcus Agrippa sought solace in the tranquil embrace of his farm,\n where the earth's gentle rhythm promised respite from the horrors of battle.\n But peace was fleeting, shattered by the cruel hand of fate. Returning home one fateful eve,\n Marcus found his sanctuary engulfed in flames, a grim omen of the tragedy that awaited.\n Amidst the smoldering ruins, he discovered the lifeless bodies of his beloved wife and son,\n their innocence consumed by the merciless flames of war. With nothing left to lose,\n Marcus's heart ignited with a searing thirst for vengeance.\n His once-pure hands now hardened into fists, clenched in defiance against\n the darkness that had stolen his loved ones. Guided by the flickering flames\n of retribution, Marcus embarked on a perilous journey, his path illuminated by\n the glint of steel and the echoes of his anguished cries. And it was within\n the hallowed walls of the Colosseum, where blood flowed like wine and echoes\n of cheers mingled with the moans of the fallen, that Marcus's quest for justice began.\n In the heart of the arena, amidst the clash of swords and the roar of the crowd,\n Marcus Agrippa emerged as a beacon of hope, his resolve unyielding against the tide of despair.\n And as he faced his first opponent,\n the world watched in awe as a new legend was born: the legend of Marcus Agrippa, the Avenger.\n";
        printStory(story, BMAG,LOW);

        player = createNewPlayer(PlayerID);
        // savePlayerData(player);

        // showPlayerStats(player);
        // showPlayerInventory(player);
    }
    else if (input == 2)
    {
        // printf("into load if");
        // printStory("The Load Functionality is currently unavailable",31,0,100);
        // return NULL;
        // Load an old game
        player = loadPlayerData(PlayerID);
        if (player == NULL)
        {
            printf("No saved game found. Starting a new game...\n");
            // printf("Invalid choice. Starting a new game...\n");
            player = createNewPlayer(PlayerID);
            savePlayerData(player);
        }
        else
        {
            printStory("Old game loaded successfully\n",YEL,LOW);
        }
    }
    else
    {
        printf("Invalid choice. Starting a new game...\n");
        player = createNewPlayer(PlayerID);
        savePlayerData(player);
    }

    return player;
}

void selectState(Player *player,int *state) // This works checked
{
    char input;
    // printStory("\nEnter (n/N) to Choose  Navigation Mode",GRN,MED);
    // printStory("\nEnter (i/I) to Choose Interaction Mode",CYN,MED);
    // printStory("\nEnter (q/Q) to Choose       Quest Mode",MAG,MED);
    printStory("\nChoose a Mode to continue your Journey (n/i/q) : ",YEL,LOW);

    // getchar();
    input=getc(stdin);
    flushInputBuffer();

    // if(getchar()!='\n')
    // {
    //     printStory("\n\nLooks like you have stumbled and fell into Shadow. Hope you make it through the next time",WHT,BOLD,HIG);
    //     *state=-1;
    // }
    // else
    if(input=='n'||input=='N') *state=0;
    else if(input=='i'||input=='I') *state=1;
    else if(input=='q'||input=='Q') *state=2;
    else if(input=='e'||input=='E') *state=-1;
    else
    if(input=='p'||input=='P')
    {
        showPlayerStats(player);
        showPlayerInventory(player);
        printStory("Default state set to Navigation",YEL,LOW);
        return;         
    }
    else    
    {
        printStory("\n\nLooks like you have stumbled and fell into Shadow. Hope you make it through the next time",BMAG,HIG);
        *state=-1;
    }

}

void navigationMode(Player *player, int *state) // This works checked BUT ADD SOME INTERACTIVE ELEMENTS ___VERY BLAND
{
    // printf("\nEntered navigation");
    FILE *file = fopen("navigations.json", "r");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open file for reading\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *fileContent = (char *)malloc(fileSize + 1);
    if (fileContent == NULL)
    {
        fprintf(stderr, "Memory allocation failed for file content\n");
        fclose(file);
        return;
    }

    size_t bytesRead = fread(fileContent, 1, fileSize, file);
    // if (bytesRead != fileSize) {
    //     printf("Error reading file.\n");
    //     fclose(file);
    //     free(fileContent);
    //     return ;
    // }

    fileContent[fileSize] = 0;
    fclose(file);

    cJSON *root = cJSON_Parse(fileContent);
    if (root == NULL)
    {
        printStory("Error parsing JSON data.\n",BWHT,HIG);
        free(fileContent);
        return;
    }

    cJSON *currentLocationInJson = cJSON_GetObjectItem(root, "locations");

    cJSON *children = cJSON_GetObjectItem(currentLocationInJson, "children");

    char *token_prev = NULL;
    char *duplicate = strdup(player->currentLocation); // IMP BECAUSE STRTOK PUTS NULL WHERE IT FINDS LIMITERS
    char *token = strtok(duplicate, "/");

    while (token != NULL)
    {
        cJSON *child = children->child;

        while (child != NULL)
        {
            cJSON *childName = cJSON_GetObjectItem(child, "name");
            if (strcmp(token, childName->valuestring) == 0)
            {
                children = cJSON_GetObjectItem(child, "children");
                currentLocationInJson = child;
                break;
            }

            child = child->next;
        }

        token_prev = strdup(token);
        token = strtok(NULL, "/");
    }

    char input;
    // printf("HU");
    // flushInputBuffer();

    if (strcmp(token_prev, "WORLD") == 0)
    {
        // printf("\nin if");
        for (int i = 0; i < cJSON_GetArraySize(children); i++)
        {
            cJSON *child = cJSON_GetArrayItem(children, i);
            printFormattedStringWithColorAndDelay("\nEnter (%d) to go to %s",GRN,LOW, i + 1, cJSON_GetObjectItem(child, "name")->valuestring);
        }

        // printStory("\nOr Enter (i/I) for Interaction Mode",);
        // printf("\nEnter (q/Q) for Quest Mode");
        // printf("\nEnter (e/E) to Exit the Game");

        printStory("\nEnter your choice : ",YEL,LOW);
        // getchar();
        // flushInputBuffer();
        input = getc(stdin);

        flushInputBuffer();
        // printf("c=%c,",input);

        // while (input <= '0' || input > (cJSON_GetArraySize(children) + '0'))
        // {
        //     // printf("\nin loop");

        //     if (input == 'e' || input == 'E')
        //     {
        //         *state = -1;
        //         return;
        //     }

        //     if (input == 'i' || input == 'I')
        //     {
        //         *state = 1;
        //         return;
        //     }

        //     if (input == 'q' || input == 'Q')
        //     {
        //         *state = 2;
        //         return;
        //     }

        //     printf("\nSuch a place doesn't exist. Enter a Valid Location!");
        //     printf("\nChoose a Location to move to : ");
        //     getchar();
        //     input = getc(stdin);
        //     // flushInputBuffer();
        // }

        if (input == 'e' || input == 'E')
        {
            *state = -1;
            return;
        }

        if(input=='p'||input=='P')
        {
            showPlayerStats(player);
            showPlayerInventory(player);
            return;         
        }

        if (input == 'i' || input == 'I')
        {
            *state = 1;
            return;
        }

        if (input == 'q' || input == 'Q')
        {
            *state = 2;
            return;
        }
    }
    else
    {
        // printf("\nin else");
        printFormattedStringWithColorAndDelay("\nEnter (0) to go out of %s",GRN,LOW, token_prev);
        for (int i = 0; i < cJSON_GetArraySize(children); i++)
        {
            cJSON *child = cJSON_GetArrayItem(children, i);
            // printf("\nEnter (%d) to go to %s", (i + 1), cJSON_GetObjectItem(child, "name")->valuestring);
            printFormattedStringWithColorAndDelay("\nEnter (%d) to go to %s",GRN,LOW, i + 1, cJSON_GetObjectItem(child, "name")->valuestring);
        }

        // printf("\n");
        // printf("\nOr Enter (i/I) for Interaction Mode");
        // printf("\nEnter (q/Q) for Quest Mode");
        // printf("\nEnter (e/E) to Exit the Game");

        printf("\n");
        printStory("\nEnter your choice : ",YEL,LOW);
        // getchar();
        // flushInputBuffer();
        input = getc(stdin);
        flushInputBuffer();
        // printf("\nc=%c",input);

        // while (input < '0' || input > (cJSON_GetArraySize(children) + '0'))
        // {
        //     printf("\nin loop");
        //     if (input == 'e' || input == 'E')
        //     {
        //         *state = -1;
        //         return;
        //     }

        //     if (input == 'i' || input == 'I')
        //     {
        //         *state = 1;
        //         return;
        //     }

        //     if (input == 'q' || input == 'Q')
        //     {
        //         *state = 2;
        //         return;
        //     }

        //     printf("\nSuch a place doesn't exist. Enter a Valid Location!");
        //     printf("\nChoose a Location to move to : ");
        //     getchar();
        //     input = getc(stdin);
        //     // flushInputBuffer();
        // printf("\nc=%c",input);

        // }
        // printf("%c",input);
        if (input == 'e' || input == 'E')
        {
            *state = -1;
            return;
        }

        if(input=='p'||input=='P')
        {
            showPlayerStats(player);
            showPlayerInventory(player);
            return;         
        }

        if (input == 'i' || input == 'I')
        {
            *state = 1;
            return;
        }

        if (input == 'q' || input == 'Q')
        {
            *state = 2;
            return;
        }
    }
    // printf("%c",input);
    if(input > '0' && input <= (cJSON_GetArraySize(children) + '0'))
    {
        // printf("\nin if2");
        cJSON *child = cJSON_GetArrayItem(children, input - '1');
        // printf("\n%s",cJSON_GetObjectItem(child,"name")->valuestring);
        strcat(player->currentLocation, "/");
        strcat(player->currentLocation, cJSON_GetObjectItem(child, "name")->valuestring);
    }
    else
    if(input=='0')
    {
        // printf("\nin else2");
        char *str = player->currentLocation;
        int j = 0;

        for (int i = 0; str[i]; i++)
        {
            if (str[i] == '/')
                j = i;
        }

        if (j)
            str[j] = 0;
    }
    else
    {
        printStory("\nYou walked into an dark and lonely Alley! ",BYEL,MED);
        printStory("\nSomething or Someone zapped at you from behind. Hope you be careful next time.\n",BRED,MED);
        gameLost(player,state);
        return;
    }
    // printf("\n%s",player->currentLocation);
}

// Function to add activated quest to activeQuests array of strings
void addActiveQuest(Player *player, char *questID) // This works checked (A)
{
    // Find the number of active quests
    int numActiveQuests = 0;
    while (player->activeQuests[numActiveQuests] != NULL)
    {
        // printf("+\n");
        numActiveQuests++;
    }
    // printf("Number of active quests %d\n", numActiveQuests);

    // Allocate memory for the new quest ID string
    char *newQuestID = strdup(questID);
    if (newQuestID == NULL)
    {
        // Handle memory allocation error
        printStory("Error: Memory allocation failed\n",BWHT,HIG);
        return;
    }

    // Reallocate memory for the activeQuests array to accommodate the new quest ID
    player->activeQuests = (char **)realloc(player->activeQuests, (numActiveQuests + 2) * sizeof(char *));
    if (player->activeQuests == NULL)
    {
        // Handle memory reallocation error
        printStory("Error: Memory reallocation failed\n",BWHT,HIG);
        free(newQuestID); // Free the allocated memory for the new quest ID
        return;
    }

    // Add the new quest ID to the activeQuests array
    player->activeQuests[numActiveQuests] = newQuestID;
    player->activeQuests[numActiveQuests + 1] = NULL; // Null-terminate the array
    // printf("Active Quests: ");
    // for (int i = 0; i < numActiveQuests + 1; i++)
    // {
    //     printf("%s ", player->activeQuests[i]);
    // }
    printf("\n");
}

// Function to find quest level
int getQuestLevel(Player *player, char *npc, char *questID) // This works checked (A)
{
    // Find NPC ID
    int npcID = getNPCID(npc);

    // Find quest level in NPCInfo array
    int questLevel = -1; // Initialize to an invalid value
    if (npcID >= 0)
    {
        // Check if questID exists for the NPC
        char *npcQuestID = getQuestID(player, npc);
        if (npcQuestID != NULL && strcmp(npcQuestID, questID) == 0)
        {
            // Get quest level from NPCInfo array
            questLevel = player->NPCInfo[npcID][0];
        }
    }

    return questLevel;
}

// Function to activate quest
void activateQuest(Player *player, char *npc, char *NPCQuestID) // This works checked (A)
{
    // Assume NPCInfo is properly initialized and contains valid data
    // Find the NPC ID based on the given NPC name
    int npcID = getNPCID(npc);

    // Find the index corresponding to the NPC ID in NPCInfo
    int npcIndex = npcID; // For simplicity, assuming the NPC ID corresponds to its index in NPCInfo

    // Find the Quest level based on the given NPCQuestID
    int questLevel = getQuestLevel(player, npc, NPCQuestID);

    // Update the quest status of the relevant NPC for the given quest level
    player->NPCInfo[npcIndex][1] = 1; // Set quest status to 1 (active)

    // Add the activated quest to the activeQuests array
    addActiveQuest(player, NPCQuestID);
}

// Function to parse quests.json and get the quest location
char *getQuestLocation(char *NPCQuestID) // This works checked (A)
{
    char *location = NULL;

    // Read the contents of the quests.json file
    FILE *file = fopen("quests.json", "r");
    if (file != NULL)
    {
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *fileContent = (char *)malloc(fileSize + 1);
        if (fileContent != NULL)
        {
            fread(fileContent, 1, fileSize, file);
            fileContent[fileSize] = '\0';

            // Parse the JSON content
            cJSON *root = cJSON_Parse(fileContent);
            if (root != NULL)
            {
                cJSON *quests = cJSON_GetObjectItem(root, "quests");
                if (quests != NULL && cJSON_IsArray(quests))
                {
                    cJSON *quest;
                    cJSON_ArrayForEach(quest, quests)
                    {
                        cJSON *questIDJSON = cJSON_GetObjectItem(quest, "QuestID");
                        if (questIDJSON != NULL && cJSON_IsString(questIDJSON) && strcmp(questIDJSON->valuestring, NPCQuestID) == 0)
                        {
                            // Quest found, get its location
                            cJSON *questLocation = cJSON_GetObjectItem(quest, "Location");
                            if (questLocation != NULL && cJSON_IsString(questLocation))
                            {
                                location = strdup(questLocation->valuestring);
                            }
                            break; // No need to continue searching
                        }
                    }
                }
                cJSON_Delete(root);
            }
            free(fileContent);
        }
        fclose(file);
    }

    return location;
}

// Function to parse quests.json and get the quest description
char *getQuestDescription(char *NPCQuestID) // This works checked (A)
{
    char *description = NULL;

    // Read the contents of the quests.json file
    FILE *file = fopen("quests.json", "r");
    if (file != NULL)
    {
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *fileContent = (char *)malloc(fileSize + 1);
        if (fileContent != NULL)
        {
            fread(fileContent, 1, fileSize, file);
            fileContent[fileSize] = '\0';

            // Parse the JSON content
            cJSON *root = cJSON_Parse(fileContent);
            if (root != NULL)
            {
                cJSON *quests = cJSON_GetObjectItem(root, "quests");
                if (quests != NULL && cJSON_IsArray(quests))
                {
                    cJSON *quest;
                    cJSON_ArrayForEach(quest, quests)
                    {
                        cJSON *questIDJSON = cJSON_GetObjectItem(quest, "QuestID");
                        if (questIDJSON != NULL && cJSON_IsString(questIDJSON) && strcmp(questIDJSON->valuestring, NPCQuestID) == 0)
                        {
                            // Quest found, get its description
                            cJSON *questDesc = cJSON_GetObjectItem(quest, "Description");
                            if (questDesc != NULL && cJSON_IsString(questDesc))
                            {
                                description = strdup(questDesc->valuestring);
                            }
                            break; // No need to continue searching
                        }
                    }
                }
                cJSON_Delete(root);
            }
            free(fileContent);
        }
        fclose(file);
    }

    return description;
}

void giveQuestReward(Player *player, char *questID) // This works checked (A)
{
    // Read the contents of the quests.json file
    FILE *file = fopen("quests.json", "r");
    if (file != NULL)
    {
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *fileContent = (char *)malloc(fileSize + 1);
        if (fileContent != NULL)
        {
            fread(fileContent, 1, fileSize, file);
            fileContent[fileSize] = '\0';

            // Parse the JSON content
            cJSON *root = cJSON_Parse(fileContent);
            if (root != NULL)
            {
                cJSON *quests = cJSON_GetObjectItem(root, "quests");
                if (quests != NULL && cJSON_IsArray(quests))
                {
                    cJSON *quest;
                    cJSON_ArrayForEach(quest, quests)
                    {
                        cJSON *questIDJSON = cJSON_GetObjectItem(quest, "QuestID");
                        if (questIDJSON != NULL && cJSON_IsString(questIDJSON) && strcmp(questIDJSON->valuestring, questID) == 0)
                        {
                            // Quest found, get its reward
                            cJSON *reward = cJSON_GetObjectItem(quest, "Reward");
                            if (reward != NULL && cJSON_IsObject(reward))
                            {
                                // Update player's attributes based on the reward
                                cJSON *xp = cJSON_GetObjectItem(reward, "xp");
                                cJSON *gold = cJSON_GetObjectItem(reward, "gold");
                                cJSON *item = cJSON_GetObjectItem(reward, "item");
                                cJSON *relationship = cJSON_GetObjectItem(reward, "relationship");

                                if (xp != NULL && cJSON_IsNumber(xp))
                                {
                                    player->xp += xp->valueint;
                                }
                                if (gold != NULL && cJSON_IsNumber(gold))
                                {
                                    player->gold += gold->valueint;
                                }
                                if (item != NULL && cJSON_IsString(item))
                                {
                                    // Append the item to the inventory
                                    if (player->inventory->items == NULL)
                                    {
                                        player->inventory->items = (char **)malloc(sizeof(char *));
                                        player->inventory->size = 1;
                                    }
                                    else
                                    {
                                        player->inventory->items = (char **)realloc(player->inventory->items, (player->inventory->size + 1) * sizeof(char *));
                                        player->inventory->size++;
                                    }
                                    player->inventory->items[player->inventory->size - 1] = strdup(item->valuestring);
                                }
                                if (relationship != NULL && cJSON_IsNumber(relationship))
                                {
                                    // Update the relationship with the NPC
                                    int npcID = atoi(strtok(questID, "_"));
                                    player->NPCInfo[npcID][2] += relationship->valueint;
                                }

                                // Increase quest level and reset quest status
                                int npcID = atoi(strtok(questID, "_"));
                                player->NPCInfo[npcID][0]++;   // Increase quest level
                                player->NPCInfo[npcID][1] = 0; // Reset quest status to 0 (incomplete/inactive)
                            }
                            break; // No need to continue searching
                        }
                    }
                }
                cJSON_Delete(root);
            }
            free(fileContent);
        }
        fclose(file);
    }
}

// Function to check if there is any active quest for the NPC
bool anyActiveQuest(Player *player, char *npc) // This works checked (A)
{
    char *questID = getQuestID(player, npc);
    // printf("in anyActiveQuest, questID = %s\n", questID);
    // Check if the NPC has any active quests
    for (int i = 0; player->activeQuests[i] != NULL; i++)
    {
        // printf("Active = %s, Here = %s", player->activeQuests[i], questID);
        if (strcmp(player->activeQuests[i], questID) == 0)
        {
            // Active quest found
            // printf("TRUE\n");
            return true;
        }
    }

    // No active quest for the NPC
    // printf("FALSE\n");
    return false;
}

// Function to check if there is any quest to submit to the NPC
bool anyQuesttoSubmit(Player *player, char *npcName) // This works checked(A)
{
    // printf("Debug anyQuesttoSubmit1\n");
    int npcID = getNPCID(npcName);
    if (npcID == -1)
    {
        // NPC not found
        return false;
    }

    // Get the quest ID for the NPC
    char *questID = getQuestID(player, npcName);
    if (questID == NULL)
    {
        // Quest ID not found
        return false;
    }

    // Check if the quest status is "to-submit" (status = 2)
    if (player->NPCInfo[npcID][1] == 2)
    {
        // Quest to submit found
        free(questID); // Free the allocated memory
        return true;
    }

    // No quest to submit
    free(questID); // Free the allocated memory
    return false;
}

// Function to parse characters.json and get the NPC ID
int getNPCID(char *npcName) // This works checked (A)
{
    // printf("Getting NPC ID\n");
    // Read the contents of the characters.json file
    FILE *file = fopen("characters.json", "r");
    if (file != NULL)
    {
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *fileContent = (char *)malloc(fileSize + 1);
        if (fileContent != NULL)
        {
            fread(fileContent, 1, fileSize, file);
            fileContent[fileSize] = '\0';

            // Parse the JSON content
            cJSON *root = cJSON_Parse(fileContent);
            if (root != NULL)
            {
                cJSON *characters = cJSON_GetObjectItem(root, "characters");
                if (characters != NULL && cJSON_IsArray(characters))
                {
                    cJSON *character;
                    cJSON_ArrayForEach(character, characters)
                    {
                        cJSON *name = cJSON_GetObjectItem(character, "name");
                        if (name != NULL && cJSON_IsString(name) && strcmp(name->valuestring, npcName) == 0)
                        {
                            cJSON *id = cJSON_GetObjectItem(character, "ID");
                            if (id != NULL && cJSON_IsNumber(id))
                            {
                                // NPC found, return its ID
                                cJSON_Delete(root);
                                fclose(file);
                                free(fileContent);
                                // printf("Got NPCID\n");
                                return id->valueint;
                            }
                        }
                    }
                }
                cJSON_Delete(root);
            }
            free(fileContent);
        }
        fclose(file);
    }

    // NPC not found or error reading file
    return -1;
}

// Function to get the quest ID for a given NPC and player
char *getQuestID(Player *player, char *npcName) // Test case 1 passed(A)
{
    // printf("Debug getQuestID1\n");
    int npcID = getNPCID(npcName);
    if (npcID == -1)
    {
        // NPC not found
        return NULL;
    }
    // printf("Found NPC\n");

    // Get the quest level from the NPCInfo array
    int questLevel = player->NPCInfo[npcID][0];
    // printf("%d =  Quest Level\n", questLevel);
    // printf("|||Got quest level = %d|||\n", questLevel);

    // Construct and return the quest ID string
    char *questID = (char *)malloc(20 * sizeof(char)); // Adjust size as needed
    if (questID != NULL)
    {
        sprintf(questID, "%d_%d", npcID, questLevel);
    }
        // printf("%s = Quest ID for NPC\n", questID);

    return questID;
}

char *questDialogues(char *questID) // This works checked (A)
{
    // printf("Debug: Entering questDialogues\n");

    char *dialoguesString = NULL;

    // Read the contents of the dialogue.json file
    FILE *file = fopen("dialogue.json", "r");
    if (file != NULL)
    {
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *fileContent = (char *)malloc(fileSize + 1);
        if (fileContent != NULL)
        {
            fread(fileContent, 1, fileSize, file);
            fileContent[fileSize] = '\0';

            // Parse the JSON content
            cJSON *root = cJSON_Parse(fileContent);
            if (root != NULL)
            {
                cJSON *dialogues = cJSON_GetObjectItem(root, "dialogues");
                if (dialogues != NULL && cJSON_IsArray(dialogues))
                {
                    cJSON *dialogue;
                    cJSON_ArrayForEach(dialogue, dialogues)
                    {
                        cJSON *questIDJSON = cJSON_GetObjectItem(dialogue, "Quest_ID");
                        if (questIDJSON != NULL && cJSON_IsString(questIDJSON) && strcmp(questIDJSON->valuestring, questID) == 0)
                        {
                            cJSON *textArray = cJSON_GetObjectItem(dialogue, "Text");
                            if (textArray != NULL && cJSON_IsArray(textArray))
                            {
                                cJSON *text;
                                cJSON *tempString = cJSON_CreateString("");
                                cJSON_ArrayForEach(text, textArray)
                                {
                                    if (cJSON_IsString(text))
                                    {
                                        char *tempText = strdup(text->valuestring); // Use strdup to duplicate the string
                                        if (tempText != NULL)
                                        {
                                            // Append the duplicated string to the dialoguesString
                                            if (dialoguesString != NULL)
                                            {
                                                // Reallocate memory to include the new string
                                                dialoguesString = realloc(dialoguesString, strlen(dialoguesString) + strlen(tempText) + 2);
                                                strcat(dialoguesString, " ");      // Add space between dialogue entries
                                                strcat(dialoguesString, tempText); // Append the new string
                                                free(tempText);                    // Free the duplicated string
                                            }
                                            else
                                            {
                                                // Allocate memory for the first dialogue entry
                                                dialoguesString = strdup(tempText);
                                                free(tempText); // Free the duplicated string
                                            }
                                        }
                                    }
                                }
                            }
                            break; // No need to continue searching
                        }
                    }
                }
                cJSON_Delete(root);
            }
            free(fileContent);
        }
        fclose(file);
    }

    return dialoguesString;
}

// Function to interact with given NPC
void interactWith(Player *player, char *npc) // Requires Quest Submission
{
    // printf("Debug 0 : %s\n", npc);

    char *NPCQuestID = getQuestID(player, npc);

    // // If any quest to submit:
    if (anyQuesttoSubmit(player, npc))
    {
        // add ui statements
        giveQuestReward(player, NPCQuestID);
        NPCQuestID = getQuestID(player, npc);
        // printf("%s = Quest ID for NPC\n", NPCQuestID);

        printStory("\nThe Quest has been submited and Rewards have been given to you!",BYEL,MED);
    }
    // char *NPCQuestID = getQuestID(player, npc);

    if (anyActiveQuest(player, npc) == true)
    {
        printStory("\nYou have an active quest related to this NPC. Please complete the quest first\n",YEL,LOW);
    }
    else
    {
        printf("%s = Quest ID for NPC\n", NPCQuestID);
        char *dialogues = questDialogues(NPCQuestID); // NPCQuestID instead of String
        if (dialogues != NULL)
        {
            printf("\n");
            // printf("%s",dialogues);
            printStory(dialogues,BLU,MED); // UI MODIFICATIONS REQ IF TIME
            printf("\n");
            free(dialogues);             // Free the allocated memory
        }

        // Quest description and reward
        char *questDescription = getQuestDescription(NPCQuestID);
        if (questDescription != NULL)
        {
            printStory("\nQuest description : ",UCYN,MED);
            printFormattedStringWithColorAndDelay("\n%s\n",CYN,MED, questDescription);
            free(questDescription);

            // PRINT THE QUEST REWARDS

            printStory("\nQuest HINT : ",UCYN,MED);
            // Quest Location
            char *questLocation = getQuestLocation(NPCQuestID);
            if (questLocation != NULL)
            {
                printFormattedStringWithColorAndDelay("\nYou must go to %s to complete this Quest\n",CYN,MED, questLocation);
                free(questLocation);
            }

            // Get input to accept or reject
            printStory("\nEnter (1) to accept this quest right now\nEnter (0) to reject this quest for now",YEL,LOW);
            printStory("\n\nEnter your choice :",YEL,LOW);
            int questinput;
            scanf("%d", &questinput);
            flushInputBuffer();
            // If accepted, add to active quests
            if (questinput == 1)
            {
                activateQuest(player, npc, NPCQuestID);
                printStory("\nQuest Accepted!\n",YEL,MED);
            }
        }
        else
        {
            printFormattedStringWithColorAndDelay("\nYou have been ignored by %s\n",BRED, MED,npc);
        }
    }
}

// Function to choose NPCs from available NPCs
void chooseNPC(char **NPCsAvailable, Player *player, int *state)
{
    int i;
    char input;

    // printf("Available NPCs:\n");
    if(!NPCsAvailable[0])
        printStory("\nNo NPCs near you!\n",BCYN,MED);

    for (i = 0; NPCsAvailable[i]; i++)
    {
        printFormattedStringWithColorAndDelay("\nEnter (%d) to interact with %s",CYN,LOW, i + 1, NPCsAvailable[i]);
    }

    // printf("\nOr Enter (n/N) for Navigation Mode");
    // printf("\nEnter (q/Q) for Quest Mode");
    // printf("\nEnter (e/E) to Exit the Game\n");
    printf("\n");
    printStory("\nEnter Your choice : ",YEL,LOW);
    
    // getchar();
    // flushInputBuffer();
    input = getc(stdin);
    flushInputBuffer();
    
    // printf("%c",input);

    for (i = 1; NPCsAvailable[i - 1]; i++);
    // printf("c=%c,i=%d,",input,i);
    // while (input < '1' || input > i + '1')
    // {
    //     // printf("\nin loop");
    //     if (input == 'e' || input == 'E')
    //     {
    //         *state = -1;
    //         return;
    //     }

    //     if (input == 'n' || input == 'N')
    //     {
    //         *state = 0;
    //         return;
    //     }

    //     if (input == 'q' || input == 'Q')
    //     {
    //         *state = 2;
    //         return;
    //     }

        //    printf("\nSuch an NPC doesn't exist. Enter a Valid NPC!");
    //     printf("\nChoose a NPC to interact with : ");
    //     getchar();
    //     input = getc(stdin);
    //     printf("\n%c",input);
    //     // flushInputBuffer();
    // }
 
    if (input == 'e' || input == 'E')
    {
        *state = -1;
        return;
    }
    else
    if (input == 'n' || input == 'N')
    {
        *state = 0;
        return;
    }
    else
    if (input == 'q' || input == 'Q')
    {
        *state = 2;
        return;
    }
    else
    if(input=='p'||input=='P')
    {
        showPlayerStats(player);
        showPlayerInventory(player);
        return;         
    }
    else
    if(input>='1'&& input<=i+'0')
    {
        char *chooseNPC = NPCsAvailable[input - '1'];
        interactWith(player, chooseNPC);
    }
    else
    {
        printStory("\nYou Have Interacted with some wrong individuals.",BYEL,MED);
        printStory("\nYou Were Beaten to Death. Hope you be careful next time.\n",BRED,MED);
        gameLost(player,state);
        return;
    }
}

// Function to return array of NPCs available at the specified locationNode
char **returnNPCsAvailable(char *locationNode) // This works checked
{
    char **NPCsAvailable = NULL;

    // Read the contents of the locations.json file
    FILE *file = fopen("locations.json", "r");
    if (file != NULL)
    {
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *fileContent = (char *)malloc(fileSize + 1);
        if (fileContent != NULL)
        {
            fread(fileContent, 1, fileSize, file);
            fileContent[fileSize] = '\0';

            // Parse the JSON content
            cJSON *root = cJSON_Parse(fileContent);
            if (root != NULL)
            {
                cJSON *locations = cJSON_GetObjectItem(root, "locations");
                if (locations != NULL && cJSON_IsArray(locations))
                {
                    cJSON *location;
                    cJSON_ArrayForEach(location, locations)
                    {
                        cJSON *name = cJSON_GetObjectItem(location, "name");
                        if (name != NULL && cJSON_IsString(name) && strcmp(name->valuestring, locationNode) == 0)
                        {
                            cJSON *npcs = cJSON_GetObjectItem(location, "NPCs");
                            if (npcs != NULL && cJSON_IsArray(npcs))
                            {
                                int numNPCs = cJSON_GetArraySize(npcs);
                                NPCsAvailable = (char **)malloc((numNPCs + 1) * sizeof(char *));
                                if (NPCsAvailable != NULL)
                                {
                                    for (int i = 0; i < numNPCs; i++)
                                    {
                                        cJSON *npcObj = cJSON_GetArrayItem(npcs, i);
                                        cJSON *npc = cJSON_GetObjectItem(npcObj, "npc");
                                        if (npc != NULL && cJSON_IsString(npc))
                                        {
                                            NPCsAvailable[i] = strdup(npc->valuestring);
                                        }
                                    }
                                    NPCsAvailable[numNPCs] = NULL; // Null-terminate the array
                                }
                            }
                            break; // No need to continue searching
                        }
                    }
                }
                cJSON_Delete(root);
            }
            free(fileContent);
        }
        fclose(file);
    }

    return NPCsAvailable;
}

void equipItem(Player *player, int index)
{
    FILE *file = fopen("items.json", "r");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open file for reading\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *fileContent = (char *)malloc(fileSize + 1);
    if (fileContent == NULL)
    {
        fprintf(stderr, "Memory allocation failed for file content\n");
        fclose(file);
        return;
    }

    size_t bytesRead = fread(fileContent, 1, fileSize, file);
    // if (bytesRead != fileSize) {
    //     printf("Error reading file.\n");
    //     fclose(file);
    //     free(fileContent);
    //     return ;
    // }

    fileContent[fileSize] = 0;
    fclose(file);

    cJSON *root = cJSON_Parse(fileContent);
    if (root == NULL)
    {
        printStory("Error parsing JSON data.\n",WHT,HIG);
        free(fileContent);
        return;
    }

    cJSON *itemsArray = cJSON_GetObjectItem(root, "items");

    cJSON *item = NULL;
    int i = 0;
    cJSON_ArrayForEach(item, itemsArray)
    {
        if (strcmp(cJSON_GetObjectItem(item, "name")->valuestring, player->inventory->items[index]) == 0)
        {
            cJSON *buffs = cJSON_GetObjectItem(item, "buff");
            player->stats->HP += cJSON_GetObjectItem(buffs, "hp")->valueint;
            player->stats->atk += cJSON_GetObjectItem(buffs, "atk")->valueint;
            player->stats->def += cJSON_GetObjectItem(buffs, "def")->valueint;
            player->stats->agi += cJSON_GetObjectItem(buffs, "agi")->valueint;
            player->stats->str += cJSON_GetObjectItem(buffs, "str")->valueint;
            player->stats->dex += cJSON_GetObjectItem(buffs, "dex")->valueint;
            player->stats->intel += cJSON_GetObjectItem(buffs, "intel")->valueint;
            player->stats->luck += cJSON_GetObjectItem(buffs, "luck")->valueint;
            break;
        }
    }

    player->inventory->activeItems[index] = 1;

    printStory("\nItem Has been Equiped!",YEL,MED);
}

void unequipItem(Player *player, int index)
{
    FILE *file = fopen("items.json", "r");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open file for reading\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *fileContent = (char *)malloc(fileSize + 1);
    if (fileContent == NULL)
    {
        fprintf(stderr, "Memory allocation failed for file content\n");
        fclose(file);
        return;
    }

    size_t bytesRead = fread(fileContent, 1, fileSize, file);
    // if (bytesRead != fileSize) {
    //     printf("Error reading file.\n");
    //     fclose(file);
    //     free(fileContent);
    //     return ;
    // }

    fileContent[fileSize] = 0;
    fclose(file);

    cJSON *root = cJSON_Parse(fileContent);
    if (root == NULL)
    {
        printStory("Error parsing JSON data.\n",WHT,HIG);
        free(fileContent);
        return;
    }

    cJSON *itemsArray = cJSON_GetObjectItem(root, "items");

    cJSON *item = NULL;
    int i = 0;
    cJSON_ArrayForEach(item, itemsArray)
    {
        if (strcmp(cJSON_GetObjectItem(item, "name")->valuestring, player->inventory->items[index]) == 0)
        {
            cJSON *buffs = cJSON_GetObjectItem(item, "buff");
            player->stats->HP -= cJSON_GetObjectItem(buffs, "hp")->valueint;
            player->stats->atk -= cJSON_GetObjectItem(buffs, "atk")->valueint;
            player->stats->def -= cJSON_GetObjectItem(buffs, "def")->valueint;
            player->stats->agi -= cJSON_GetObjectItem(buffs, "agi")->valueint;
            player->stats->str -= cJSON_GetObjectItem(buffs, "str")->valueint;
            player->stats->dex -= cJSON_GetObjectItem(buffs, "dex")->valueint;
            player->stats->intel -= cJSON_GetObjectItem(buffs, "intel")->valueint;
            player->stats->luck -= cJSON_GetObjectItem(buffs, "luck")->valueint;
            break;
        }
    }

    player->inventory->activeItems[index] = 0;
    printStory("\nItem Has been Unequiped!",YEL,MED);
}

// Function to find the last location node in the currentLocation array of Player
void interactionMode(Player *player, int *state) // This works checked (A)
{
    // printf("In Interaction\n");
    // Find and return the last string in currentLocation array
    char *locationNode = player->currentLocation;
    int i;
    for (i = 0; player->currentLocation[i]; i++)
        if (player->currentLocation[i] == '/')
            locationNode = player->currentLocation + i + 1;

    // printf("Got Location node =%s\n",locationNode);
    // Now go to locations.json and find "locationNode", and return array of NPCs available
    char **NPCsAvailable = returnNPCsAvailable(locationNode);

    chooseNPC(NPCsAvailable, player, state);

    return;
}

void questMode(Player *player, int *state)
{
    char input;
    int i;

    printf("\n"); // Heading
    for (i = 0; player->inventory->items[i]; i++)
        printFormattedStringWithColorAndDelay("\nEnter (%d) to select Item => %s",MAG,LOW, i + 1, player->inventory->items[i]);

    printStory("\nOr Enter (s/S) to Skip",YEL,MED);
    printf("\n");
    printStory("\nEnter your choice : ",YEL,LOW);
    input = getc(stdin);
    flushInputBuffer();

    if (input == 'e' || input == 'E')
    {
        *state = -1;
        return;
    }

    if(input=='p'||input=='P')
    {
        showPlayerStats(player);
        showPlayerInventory(player);
        return;         
    }

    if (input == 'i' || input == 'I')
    {
        *state = 1;
        return;
    }

    if (input == 'n' || input == 'N')
    {
        *state = 0;
        return;
    }

    if (input >= '1' && input <= i + '0')
    {
        if (player->inventory->activeItems[input - '1'])
        {
            char toUnequip;
            printStory("\nItem Already Equiped.",MAG,LOW);
            printStory("\nDo you want to Unequip it (Y/N) :",MAG,MED);
            // getchar();
            toUnequip = getc(stdin);
            flushInputBuffer();

            if (toUnequip == 'y' || toUnequip == 'Y')
                unequipItem(player, input - '1');
        }
        else
        {
            char toEquip;
            printStory("\nItem is NOT Equiped.",MAG,LOW);
            printStory("\nDo you want to Equip it (Y/N) :",MAG,MED);
            // getchar();
            toEquip = getc(stdin);
            flushInputBuffer();

            if (toEquip == 'y' || toEquip == 'Y')
                equipItem(player, input - '1');
        }
    }

    FILE *file = fopen("quests.json", "r");
    if (!file)
    {
        fprintf(stderr, "Failed to open quests.json\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *fileContent = (char *)malloc(fileSize + 1);
    if (!fileContent)
    {
        fprintf(stderr, "Memory allocation failed for file content\n");
        fclose(file);
        return;
    }

    fread(fileContent, 1, fileSize, file);
    fclose(file);
    fileContent[fileSize] = 0;

    cJSON *root = cJSON_Parse(fileContent);
    if (!root)
    {
        const char *error = cJSON_GetErrorPtr();
        if (error != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error);
        }
        cJSON_Delete(root);
        free(fileContent);
        return;
    }

    cJSON *questsArray = cJSON_GetObjectItem(root, "quests");

    char *cur_loc = player->currentLocation;
    int j = 0;
    int choises[10];

    for (i = 0; cur_loc[i]; i++)
        if (cur_loc[i] == '/')
            j = i + 1;

    cur_loc = player->currentLocation + j;

    int n;
    for (n = 0; player->activeQuests[n]; n++)
        ;

    for (i = 0, j = 0; i < n; i++)
    {
        cJSON *ptr = questsArray->child;
        int k = 0;
        char *questId = strdup(player->activeQuests[i]);
        while (ptr)
        {
            // printf("\nQid=%s,jQid=%s,cur_loc=%s,jloc=%s",questId,cJSON_GetObjectItem(ptr,"QuestID")->valuestring,cur_loc,cJSON_GetObjectItem(ptr,"Location")->valuestring);
            if (strcmp(questId, cJSON_GetObjectItem(ptr, "QuestID")->valuestring) == 0 && strcmp(cur_loc, cJSON_GetObjectItem(ptr, "Location")->valuestring) == 0)
                choises[j++] = k;
            ptr = ptr->next;
            k++;
        }
    }
    choises[j] = -1;

    for (j = 0; j < 10 && choises[j] >= 0; j++)
    {
        // printf("\n%d",choises[j]);
        cJSON *quest = cJSON_GetArrayItem(questsArray, choises[j]);
        printFormattedStringWithColorAndDelay("\nEnter (%d) to start quest=> %s",MAG,LOW, j + 1, cJSON_GetObjectItem(quest, "name")->valuestring);
    }

    if (choises[0] == -1)
    {
        printStory("\nNo Active quest at this location",MAG,MED);
        printStory("\nExplore More!",BMAG,MED);
    }

    // printf("\nOr Enter (i/I) for Interaction Mode");
    // printf("\nEnter (n/N) for Navigation Mode");
    // printf("\nEnter (e/E) to Exit the Game");

    printStory("\nEnter your choice : ",YEL,LOW);
    // getchar();
    input = getc(stdin);
    // printf("%c",input);
    flushInputBuffer();

    if (input == 'e' || input == 'E')
    {
        *state = -1;
        return;
    }
    else
    if(input=='p'||input=='P')
    {
        showPlayerStats(player);
        showPlayerInventory(player);
        return;         
    }
    else
    if (input == 'i' || input == 'I')
    {
        *state = 1;
        return;
    }
    else
    if (input == 'n' || input == 'N')
    {
        *state = 0;
        return;
    }
    else
    if(input>='1' && input<=j+'0')
    {
        cJSON *child = cJSON_GetArrayItem(questsArray, choises[input - '1']);
        // printf("game = %s", cJSON_GetObjectItem(child, "Minigame")->valuestring);
        // flushInputBuffer();
        int result = playMiniGame(player, cJSON_GetObjectItem(child, "Minigame")->valuestring); // in player.c
        // printf("%d = Result\n", result);
        flushInputBuffer();
        printf("\e[1;1H\e[2J");
        if (result)
        {
            char *questId = cJSON_GetObjectItem(child, "QuestID")->valuestring;
            // printf("%s - questid\n", questId);
            int npcId = questId[0] - '0';
            // printf("%d = npcid\n", npcId);
            player->NPCInfo[npcId][QUEST_STATUS] = 2;
            for (int i = 0; player->activeQuests[i]; i++)
            {
                if (strcmp(player->activeQuests[i], questId) == 0)
                {
                    // printf("%d = i\n", i);
                    for (int j = i; player->activeQuests[j]; j++)
                    {
                        player->activeQuests[j] = player->activeQuests[j + 1];
                    }
                    break;
                }
            }
        }
        else
        {
            printStory("You Lost the game and DIED\n",BRED,HIG); // Print Game OVER STATEMENT and items lost and gold 0
            gameLost(player,state);
            return;
        }
    }
    else
    {
        printStory("\nYou started an Unknown Dangerous Task! ",BYEL,MED);
        printStory("\nYou met a Hoard of Barbarian and DIED. Hope you be careful next time.\n",BRED,MED);
        gameLost(player,state);
        // printf("%d",*state);
        return;
    }
}

void processState(Player *player, int *state)
{
    if (*state == 0)
        navigationMode(player, state);
    else if (*state == 1)
        interactionMode(player, state);
    else if (*state == 2)
        questMode(player, state);
}
