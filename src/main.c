// Bonne pratique, pour les includes systemes toujours utiliser <> et "" pour les includes de votre projet.
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "utils.h"
#include "stb_image.h"
#include "stb_image_write.h"

#define HEAP_SIZE 2048 * 2048 * 4
static uint8_t* heap = NULL;
static size_t heap_top = 0;
void* allocate(size_t size) {
    size_t old_top = heap_top;
    heap_top += size;
    assert(heap_top <= HEAP_SIZE);
    return &heap[old_top];
}

int TheMazeToExplore;

const char* img_names[] = {
    "31.bmp",				//indexe == 0
    "64.bmp",				//indexe == 1
    "128.bmp",				//indexe == 2
    "512.bmp",				//indexe == 3
    "45565.bmp",			//indexe == 4
    "braid2k.png",			//indexe == 5
    "combo400.png",			//indexe == 6
    "perfect2k.png"			//indexe == 7
};

const size_t img_name_len = sizeof(img_names) / sizeof(img_names[TheMazeToExplore]);


typedef struct {
    int PosX;
	int PosY;
    int GetOriginalCost;
	int HeuristiqueCost;
    int FinalCost;
    struct Node* parent;
} Node;

int Heuristique(int PosX1, int PosY1, int PosX2, int PosY2) {
	//Manhattan distance
	return (abs(PosX1 - PosX2) + abs(PosY1 - PosY2));
}

void ColorThePathThroughTheMaze(unsigned char* img, Node* path[], int LenghtOfThePath, int TempValueForTheColoring) {
	for (int i = 0; i < LenghtOfThePath; i++) {
        int index = (path[i]->PosY * TempValueForTheColoring + path[i]->PosX) * 3;
        img[index] = 255;    //red color
        img[index + 1] = 0;  //green color
        img[index + 2] = 0;  //blue color
    }
}

void A_Star(unsigned char* img, int Width, int Height) {
	int channels = 3;
	//find the star and end points

	//find this pixel: P(x, 0)
    int StartX = -1, StartY = -1;
    for (int TempX = 0; TempX < Width; TempX++) {
        int index = (0 * Width + TempX) * channels; //first row
        if (img[index] != 0 || img[index + 1] != 0 || img[index + 2] != 0) { //not black
            StartX = TempX;
            StartY = 0;
			printf("\033[32mStart position found!\033[0m\n");
            break;
        }
    }

    //find this pixel: P(x, Height - 1)
    int EndX = -1, EndY = -1;
    for (int TempX = 0; TempX < Width; TempX++) {
        int index = ((Height - 1) * Width + TempX) * channels; //bottom row
        if (img[index] != 0 || img[index + 1] != 0 || img[index + 2] != 0) { //not black
            EndX = TempX;
            EndY = Height - 1;
			printf("\033[32mEnd position found!\033[0m\n\n");
            break;
        }
    }

    //handle the inexistance of the start or end position
    if (StartX == -1 || EndX == -1) {
        printf("\033[31mNo valid starting or ending position found!\033[0m\n");
		//since no valid start positions were found, exit
        stbi_image_free(img);
        free(heap);
        return 1;
    }

	//show the start and end positions
	printf("\033[32mStart Position: (%d, %d)\033[0m\n", StartX, StartY);
	printf("\033[32mEnd Position: (%d, %d)\033[0m\n\n", EndX, EndY);
	
	// Initialize open and closed lists
	Node** ListOpen = (Node**)malloc(Width * Height * sizeof(Node*));
	Node** ListClosed = (Node**)malloc(Width * Height * sizeof(Node*));
	int NumberOfNodesInOpen = 0, NumberOfNodesInClosed = 0;
	
	// Add start node to open list
	Node* StartNode = (Node*)malloc(sizeof(Node));
	StartNode->PosX = StartX;
	StartNode->PosY = StartY;
	StartNode->GetOriginalCost = 0;
	StartNode->HeuristiqueCost = Heuristique(StartX, StartY, EndX, EndY);
	StartNode->FinalCost = StartNode->GetOriginalCost + StartNode->HeuristiqueCost;
	StartNode->parent = NULL;
	ListOpen[NumberOfNodesInOpen++] = StartNode;
	
	while (NumberOfNodesInOpen > 0) {
		//search for node with the lowest final_cost
		Node* TheCurrentNode = ListOpen[0];
		int TheIndexCurrentlyUsed = 0;
		
		for (int i = 1; i < NumberOfNodesInOpen; i++) {
			if (ListOpen[i]->FinalCost < TheCurrentNode->FinalCost) {
				TheCurrentNode = ListOpen[i];
				TheIndexCurrentlyUsed = i;
			}
		}
		
		//the list open is getting emptier
		for (int i = TheIndexCurrentlyUsed; i < NumberOfNodesInOpen - 1; i++) {
			ListOpen[i] = ListOpen[i + 1];
		}
		NumberOfNodesInOpen--;
		
		//tell current pixel
		printf("\033[33mCurrent Pixel: (%d, %d)\033[0m\r", TheCurrentNode->PosX, TheCurrentNode->PosY); //i searched, and \r means that the cursor returns at the start of the line, and so i put it to avoid getting a lot of useless lines
		fflush(stdout);

		//has the end been reached?
		if (TheCurrentNode->PosX == EndX && TheCurrentNode->PosY == EndY) {
			printf("\n\n\033[32mPath Found!\033[0m\n"); //tell that a path has been found (meaning the end has been reached)
			Node** path = (Node**)malloc(Width * Height * sizeof(Node*));
			int LenghtOfThePath = 0;
			while (TheCurrentNode != NULL) {
				path[LenghtOfThePath++] = TheCurrentNode;
				TheCurrentNode = TheCurrentNode->parent;
			}
			ColorThePathThroughTheMaze(img, path, LenghtOfThePath, Width);
			return;
		}

		// Add current node to closed list
		ListClosed[NumberOfNodesInClosed++] = TheCurrentNode;

		//visit the neighbor
		for (int DiagonalX = -1; DiagonalX <= 1; DiagonalX++) {
			for (int DiagonalY = -1; DiagonalY <= 1; DiagonalY++) {
				if (abs(DiagonalX) == abs(DiagonalY)) {
					//ignore diagonals
					continue;
				} 

				int NeighborX = TheCurrentNode->PosX + DiagonalX;
				int NeighborY = TheCurrentNode->PosY + DiagonalY;

				//is the neighbor inbound
				if (NeighborX < 0 || NeighborX >= Width || NeighborY < 0 || NeighborY >= Height) continue;

				// Check if the pixel is walkable (not black)
				int index = (NeighborY * Width + NeighborX) * 3;
				if (img[index] == 0 && img[index + 1] == 0 && img[index + 2] == 0) {
					continue; //not walkable
				}

				//calcul of the prices
				int GetOriginalCost = TheCurrentNode->GetOriginalCost + 1;
				int HeuristiqueCost = Heuristique(NeighborX, NeighborY, EndX, EndY);
				int FinalCost = GetOriginalCost + HeuristiqueCost;

				//is the neighbor in closed list
				int InClosedList = 0;
				for (int i = 0; i < NumberOfNodesInClosed; i++) {
					if (ListClosed[i]->PosX == NeighborX && ListClosed[i]->PosY == NeighborY) {
						InClosedList = 1;
						break;
					}
				}
				if (InClosedList) continue;

				//is the neighbor in the open
				Node* NeighborNode = NULL;
				for (int i = 0; i < NumberOfNodesInOpen; i++) {
					if (ListOpen[i]->PosX == NeighborX && ListOpen[i]->PosY == NeighborY) {
						NeighborNode = ListOpen[i];
						break;
					}
				}

				if (NeighborNode == NULL) {
					//summon a new neighbor
					NeighborNode = (Node*)malloc(sizeof(Node));
					NeighborNode->PosX = NeighborX;
					NeighborNode->PosY = NeighborY;
					NeighborNode->GetOriginalCost = GetOriginalCost;
					NeighborNode->HeuristiqueCost = HeuristiqueCost;
					NeighborNode->FinalCost = FinalCost;
					NeighborNode->parent = TheCurrentNode;

					//the neighbor joins the list open
					ListOpen[NumberOfNodesInOpen++] = NeighborNode;
				} else if (GetOriginalCost < NeighborNode->GetOriginalCost) {
					//update the neighbor
					NeighborNode->GetOriginalCost = GetOriginalCost;
					NeighborNode->FinalCost = FinalCost;
					NeighborNode->parent = TheCurrentNode;
				}
			}
		}
	}
	//no path were found
	printf("\n\n\033[31m\nPath not found!\033[0m\n");
}

void ExploreTheMaze() {

	TheMazeToExplore;

	heap = (uint8_t*)malloc(HEAP_SIZE);
    assert(heap != NULL);

    int Width;
	int Height;
	int channels;
    unsigned char* img = stbi_load(img_names[TheMazeToExplore], &Width, &Height, &channels, 0);
    if (img == NULL) {
		//announces the image failled to load
        printf("\n\033[31mError loading the image\033[0m\n");
        exit(1);
    }

	//confirm the image loaded successfully
    printf("\n\033[32mLoaded maze %s with a Width of %dpx, a Height of %dpx and %d channels\033[0m\n\n", img_names[TheMazeToExplore], Width, Height, channels);
    A_Star(img, Width, Height);

    //save the image to a solution_ file
    char filepath[_MAX_PATH] = { 0 };
    snprintf(filepath, _MAX_PATH, "solution_%s", img_names[TheMazeToExplore]);
    stbi_write_bmp(filepath, Width, Height, channels, img);

	//check for writing faillure
	if (stbi_write_bmp(filepath, Width, Height, channels, img) == 0) {
    	printf("\n\033[31mError writing image: %s\033[0m\n", stbi_failure_reason());
	} else {
		//tell the name of the image with the path through the maze
    	printf("\n\033[32mImage successfully written to %s\033[0m\n\n", filepath);
	}
	
    stbi_image_free(img);
    free(heap);
}

int main(int argc, char** argv) {
//yes, i made a small, colored ui for this


//and i just got yelled at 'cause i'm awake at 2am finishing this

	int Option;

	printf("\033[34mThis program can run throug mazes. To open a maze, write the index number of the maze in question.\033[0m\n");
	printf("\033[35mIndex \033[37m| \033[36mName\033[0m\n");
	printf("\033[35m0 \033[37m| \033[36m31.bmp\033[0m\n");
	printf("\033[35m1 \033[37m| \033[36m64.bmp\033[0m\n");
	printf("\033[35m2 \033[37m| \033[36m128.bmp\033[0m\n");
	printf("\033[35m3 \033[37m| \033[36m512.bmp\033[0m\n");
	printf("\033[35m4 \033[37m| \033[36m45565.bmp\033[0m\n");
	printf("\033[35m5 \033[37m| \033[36mbraid2k.png\033[0m\n");
	printf("\033[35m6 \033[37m| \033[36mcombo400.png\033[0m\n");
	printf("\033[35m7 \033[37m| \033[36mperfect2k.png\033[0m\n");
	printf("\033[35m8 \033[37m| \033[36mexit the program\033[0m\n");
	printf("\033[34mYour choice\033[37m: \033[35m");
	scanf("%d", &Option);
	printf("\033[0m");
	printf("\033[0m");

	while (Option != 8) {
		if (Option == 0) {
			TheMazeToExplore = Option;

			ExploreTheMaze();
			
			printf("\033[34mDo you wish to explore more mazes? If yes, enter the index.\033[0m\n");
			printf("\033[35mIndex \033[37m| \033[36mName\033[0m\n");
			printf("\033[35m0 \033[37m| \033[36m31.bmp\033[0m\n");
			printf("\033[35m1 \033[37m| \033[36m64.bmp\033[0m\n");
			printf("\033[35m2 \033[37m| \033[36m128.bmp\033[0m\n");
			printf("\033[35m3 \033[37m| \033[36m512.bmp\033[0m\n");
			printf("\033[35m4 \033[37m| \033[36m45565.bmp\033[0m\n");
			printf("\033[35m5 \033[37m| \033[36mbraid2k.png\033[0m\n");
			printf("\033[35m6 \033[37m| \033[36mcombo400.png\033[0m\n");
			printf("\033[35m7 \033[37m| \033[36mperfect2k.png\033[0m\n");
			printf("\033[35m8 \033[37m| \033[36mexit the program\033[0m\n");
			printf("\033[34mYour choice\033[37m: \033[35m");
			scanf("%d", &Option);
			printf("\033[0m");
		}
		if (Option == 1) {
			TheMazeToExplore = Option;
			
			ExploreTheMaze();
			
			printf("\033[34mDo you wish to explore more mazes? If yes, enter the index.\033[0m\n");
			printf("\033[35mIndex \033[37m| \033[36mName\033[0m\n");
			printf("\033[35m0 \033[37m| \033[36m31.bmp\033[0m\n");
			printf("\033[35m1 \033[37m| \033[36m64.bmp\033[0m\n");
			printf("\033[35m2 \033[37m| \033[36m128.bmp\033[0m\n");
			printf("\033[35m3 \033[37m| \033[36m512.bmp\033[0m\n");
			printf("\033[35m4 \033[37m| \033[36m45565.bmp\033[0m\n");
			printf("\033[35m5 \033[37m| \033[36mbraid2k.png\033[0m\n");
			printf("\033[35m6 \033[37m| \033[36mcombo400.png\033[0m\n");
			printf("\033[35m7 \033[37m| \033[36mperfect2k.png\033[0m\n");
			printf("\033[35m8 \033[37m| \033[36mexit the program\033[0m\n");
			printf("\033[34mYour choice\033[37m: \033[35m");
			scanf("%d", &Option);
			printf("\033[0m");
		}
		if (Option == 2) {
			TheMazeToExplore = Option;

			ExploreTheMaze();
			
			printf("\033[34mDo you wish to explore more mazes? If yes, enter the index.\033[0m\n");
			printf("\033[35mIndex \033[37m| \033[36mName\033[0m\n");
			printf("\033[35m0 \033[37m| \033[36m31.bmp\033[0m\n");
			printf("\033[35m1 \033[37m| \033[36m64.bmp\033[0m\n");
			printf("\033[35m2 \033[37m| \033[36m128.bmp\033[0m\n");
			printf("\033[35m3 \033[37m| \033[36m512.bmp\033[0m\n");
			printf("\033[35m4 \033[37m| \033[36m45565.bmp\033[0m\n");
			printf("\033[35m5 \033[37m| \033[36mbraid2k.png\033[0m\n");
			printf("\033[35m6 \033[37m| \033[36mcombo400.png\033[0m\n");
			printf("\033[35m7 \033[37m| \033[36mperfect2k.png\033[0m\n");
			printf("\033[35m8 \033[37m| \033[36mexit the program\033[0m\n");
			printf("\033[34mYour choice\033[37m: \033[35m");
			scanf("%d", &Option);
			printf("\033[0m");
		}
		if (Option == 3) {
			TheMazeToExplore = Option;

			ExploreTheMaze();
			
			printf("\033[34mDo you wish to explore more mazes? If yes, enter the index.\033[0m\n");
			printf("\033[35mIndex \033[37m| \033[36mName\033[0m\n");
			printf("\033[35m0 \033[37m| \033[36m31.bmp\033[0m\n");
			printf("\033[35m1 \033[37m| \033[36m64.bmp\033[0m\n");
			printf("\033[35m2 \033[37m| \033[36m128.bmp\033[0m\n");
			printf("\033[35m3 \033[37m| \033[36m512.bmp\033[0m\n");
			printf("\033[35m4 \033[37m| \033[36m45565.bmp\033[0m\n");
			printf("\033[35m5 \033[37m| \033[36mbraid2k.png\033[0m\n");
			printf("\033[35m6 \033[37m| \033[36mcombo400.png\033[0m\n");
			printf("\033[35m7 \033[37m| \033[36mperfect2k.png\033[0m\n");
			printf("\033[35m8 \033[37m| \033[36mexit the program\033[0m\n");
			printf("\033[34mYour choice\033[37m: \033[35m");
			scanf("%d", &Option);
			printf("\033[0m");
		}
		if (Option == 4) {
			TheMazeToExplore = Option;

			ExploreTheMaze();
			
			printf("\033[34mDo you wish to explore more mazes? If yes, enter the index.\033[0m\n");
			printf("\033[35mIndex \033[37m| \033[36mName\033[0m\n");
			printf("\033[35m0 \033[37m| \033[36m31.bmp\033[0m\n");
			printf("\033[35m1 \033[37m| \033[36m64.bmp\033[0m\n");
			printf("\033[35m2 \033[37m| \033[36m128.bmp\033[0m\n");
			printf("\033[35m3 \033[37m| \033[36m512.bmp\033[0m\n");
			printf("\033[35m4 \033[37m| \033[36m45565.bmp\033[0m\n");
			printf("\033[35m5 \033[37m| \033[36mbraid2k.png\033[0m\n");
			printf("\033[35m6 \033[37m| \033[36mcombo400.png\033[0m\n");
			printf("\033[35m7 \033[37m| \033[36mperfect2k.png\033[0m\n");
			printf("\033[35m8 \033[37m| \033[36mexit the program\033[0m\n");
			printf("\033[34mYour choice\033[37m: \033[35m");
			scanf("%d", &Option);
			printf("\033[0m");
		}
		if (Option == 5) {
			TheMazeToExplore = Option;

			ExploreTheMaze();
			
			printf("\033[34mDo you wish to explore more mazes? If yes, enter the index.\033[0m\n");
			printf("\033[35mIndex \033[37m| \033[36mName\033[0m\n");
			printf("\033[35m0 \033[37m| \033[36m31.bmp\033[0m\n");
			printf("\033[35m1 \033[37m| \033[36m64.bmp\033[0m\n");
			printf("\033[35m2 \033[37m| \033[36m128.bmp\033[0m\n");
			printf("\033[35m3 \033[37m| \033[36m512.bmp\033[0m\n");
			printf("\033[35m4 \033[37m| \033[36m45565.bmp\033[0m\n");
			printf("\033[35m5 \033[37m| \033[36mbraid2k.png\033[0m\n");
			printf("\033[35m6 \033[37m| \033[36mcombo400.png\033[0m\n");
			printf("\033[35m7 \033[37m| \033[36mperfect2k.png\033[0m\n");
			printf("\033[35m8 \033[37m| \033[36mexit the program\033[0m\n");
			printf("\033[34mYour choice\033[37m: \033[35m");
			scanf("%d", &Option);
			printf("\033[0m");
		}
		if (Option == 6) {
			TheMazeToExplore = Option;

			ExploreTheMaze();
			
			printf("\033[34mDo you wish to explore more mazes? If yes, enter the index.\033[0m\n");
			printf("\033[35mIndex \033[37m| \033[36mName\033[0m\n");
			printf("\033[35m0 \033[37m| \033[36m31.bmp\033[0m\n");
			printf("\033[35m1 \033[37m| \033[36m64.bmp\033[0m\n");
			printf("\033[35m2 \033[37m| \033[36m128.bmp\033[0m\n");
			printf("\033[35m3 \033[37m| \033[36m512.bmp\033[0m\n");
			printf("\033[35m4 \033[37m| \033[36m45565.bmp\033[0m\n");
			printf("\033[35m5 \033[37m| \033[36mbraid2k.png\033[0m\n");
			printf("\033[35m6 \033[37m| \033[36mcombo400.png\033[0m\n");
			printf("\033[35m7 \033[37m| \033[36mperfect2k.png\033[0m\n");
			printf("\033[35m8 \033[37m| \033[36mexit the program\033[0m\n");
			printf("\033[34mYour choice\033[37m: \033[35m");
			scanf("%d", &Option);
			printf("\033[0m");
		}
		if (Option == 7) {
			TheMazeToExplore = Option;

			ExploreTheMaze();
			
			printf("\033[34mDo you wish to explore more mazes? If yes, enter the index.\033[0m\n");
			printf("\033[35mIndex \033[37m| \033[36mName\033[0m\n");
			printf("\033[35m0 \033[37m| \033[36m31.bmp\033[0m\n");
			printf("\033[35m1 \033[37m| \033[36m64.bmp\033[0m\n");
			printf("\033[35m2 \033[37m| \033[36m128.bmp\033[0m\n");
			printf("\033[35m3 \033[37m| \033[36m512.bmp\033[0m\n");
			printf("\033[35m4 \033[37m| \033[36m45565.bmp\033[0m\n");
			printf("\033[35m5 \033[37m| \033[36mbraid2k.png\033[0m\n");
			printf("\033[35m6 \033[37m| \033[36mcombo400.png\033[0m\n");
			printf("\033[35m7 \033[37m| \033[36mperfect2k.png\033[0m\n");
			printf("\033[35m8 \033[37m| \033[36mexit the program\033[0m\n");
			printf("\033[34mYour choice\033[37m: \033[35m");
			scanf("%d", &Option);
			printf("\033[0m");
		}
		if (Option == 8) {
			printf("\033[34mOk, bye\033[0m\n");
			return 0;
		}
	}
}