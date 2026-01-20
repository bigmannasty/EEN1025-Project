
int A[][2] = {{0,1}, {1,2}, {1,0}, {2,1}};
int B[][2] = {{0,2}, {2,0}};
int C[][2] = {{0,4}, {4,0}, {2,3}, {3,2}};
int D[][2] = {{1,4}, {4,1}, {1,3}, {3,1}};
int E[][2] = {{3,4}, {4,3}};

int nodes0[] = {1, 2, 4, 9, 9};
int nodes1[] = {0, 2, 3, 4, 9};
int nodes2[] = {0, 1, 3, 9, 9};
int nodes3[] = {2, 1, 4, 9, 9};
int nodes4[] = {0, 1, 3, 9, 9};

int nodes[][5] = {{0, 1, 1, 0, 1}, {1, 0, 1, 1, 1}, {1, 1, 0, 1, 0}, {0, 1, 1, 0, 1}, {1, 1, 0, 1, 0}};

int route[] = {0, 3};
int startPos = route[0];
int nextPos = route[1];

int weight0[] = {9, 2, 2, 9, 1};
int weight1[] = {2, 9, 2, 4, 4};
int weight2[] = {2, 2, 9, 1, 9};
int weight3[] = {9, 4, 1, 9, 5};
int weight4[] = {1, 4, 9, 5, 9};

int weights[][5] = {{9, 2, 2, 9, 1}, {2, 9, 2, 4, 4}, {2, 2, 9, 1, 9}, {9, 4, 1, 9, 5}, {1, 4, 9, 5, 9}};

bool directCon = true;

int currentNodeIndex;
int currentWeightTotal;
int currentNode = 0;
int weightRankTotal[] = {9, 9, 9};
int routeRank[] = {0, 0, 0};

void routeFind() {
  if (nodes[startPos][nextPos] == 0) {
    for (int i; i < 5; i++) {
      if (nodes[startPos][i] == 1) {

        currentWeightTotal += weights[startPos][i]; //getting weight total for current midpoint
        currentWeightTotal += weights[i][nextPos];

        for (int j; j < 3; i++) {
          
          if (currentWeightTotal < weightRankTotal[j]) {
            int temp = weightRankTotal[j];
            weightRankTotal[j] = currentWeightTotal;
            int rtemp = routeRank[j];
            routeRank[j] = i;
            
            if (j == 0) {
              int temp2 = weightRankTotal[1];
              weightRankTotal[1] = temp;
              weightRankTotal[2] = temp2;
              int rtemp2 = routeRank[1];
              routeRank[1] = rtemp;
              routeRank[2] = rtemp2;
            }

            else if (j == 1) {
              weightRankTotal[2] = weightRankTotal[1];
              weightRankTotal[1] = temp;
              routeRank[2] = routeRank[1];
              routeRank[1] = rtemp;
            }
          }
        }
      }
    }
  }
}




//if we want fastest 0 - 3
//check if not in nodes0 then goes to node3, finds the best weighting, goes to corresponding node then to to 0
//tallies up the weighting then adds the path to a list
//goes onto the next then the next
//so: goes 3, 4+2=6 -> 1+2=3 -> 5+1=6 so rnking goes path1-path0-path2

//check C -> A -> B -> D -> E

//chec


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}



void loop() {
  // put your main code here, to run repeatedly:
  return;

}
