// suffix_tree_ukkonen.c //
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define MAX_CHAR 256
 
struct SuffixTreeNode {
    struct SuffixTreeNode *children[MAX_CHAR];
 
    //pointer to other node via suffix link
    struct SuffixTreeNode *suffixLink;
    int start;
    int *end;
    int suffixIndex;
};
 
typedef struct SuffixTreeNode Node;
// Global variable declaration 
char text[22000]; 
Node *root = NULL; 
 
Node *lastNewNode = NULL;
Node *activeNode = NULL;
 
int activeEdge = -1;
int activeLength = 0;

int remainingSuffixCount = 0;
int leafEnd = -1;
int *rootEnd = NULL;
int *splitEnd = NULL;
int size = -1; 
 // End GLobal Variable Declaration
 
// Structure and Functions for extending tree  
Node *newNode(int start, int *end)
{
    Node *node =(Node*) malloc(sizeof(Node));
    int i;
    for (i = 0; i < MAX_CHAR; i++)
          node->children[i] = NULL;
 
    node->suffixLink = root;
    node->start = start;
    node->end = end;
 
    node->suffixIndex = -1;
    return node;
}
 
int edgeLength(Node *n) {
    return *(n->end) - (n->start) + 1;
}
 
int walkDown(Node *currNode)
{
    if (activeLength >= edgeLength(currNode))
    {
        activeEdge += edgeLength(currNode);
        activeLength -= edgeLength(currNode);
        activeNode = currNode;
        return 1;
    }
    return 0;
}
 
void extendSuffixTree(int pos)
{

    leafEnd = pos;
    remainingSuffixCount++;
    lastNewNode = NULL;
	
    while(remainingSuffixCount > 0) {
 
        if (activeLength == 0)
            activeEdge = pos; 
 //////////////////first character of edge not found////////////////////////
        if (activeNode->children[text[activeEdge]] == NULL)
        {
            activeNode->children[text[activeEdge]] =
                                          newNode(pos, &leafEnd);
            if (lastNewNode != NULL)
            {
                lastNewNode->suffixLink = activeNode;
                lastNewNode = NULL;
            }
        }
///////////////////first character of edge found///////////////////////	
        else
        {
            Node *next = activeNode->children[text[activeEdge]];
            if (walkDown(next))
            {
                continue;
            }
			// Rule 3 found, end phase //
            if (text[next->start + activeLength] == text[pos])
            {
                if(lastNewNode != NULL && activeNode != root)
                {
                    lastNewNode->suffixLink = activeNode;
                    lastNewNode = NULL;
                }
                activeLength++;
                break;
            }
			// Rule 2 found, Create new internal node //
            splitEnd = (int*) malloc(sizeof(int));
            *splitEnd = next->start + activeLength - 1;
 
            Node *split = newNode(next->start, splitEnd);
            activeNode->children[text[activeEdge]] = split;
 
            split->children[text[pos]] = newNode(pos, &leafEnd);
            next->start += activeLength;
            split->children[text[next->start]] = next;
 
            if (lastNewNode != NULL)
            {
                lastNewNode->suffixLink = split;
            }
            lastNewNode = split;
        }
		// decrease remaining leaf nodes to be created //
        remainingSuffixCount--;
		// update activeNode for next extension //
        if (activeNode == root && activeLength > 0)
        {
            activeLength--;
            activeEdge = pos - remainingSuffixCount + 1;
        }
        else if (activeNode != root)
        {
            activeNode = activeNode->suffixLink;
        }
    }
}
// End Structure and Functions for extending tree Definitions  

// Functions for printing Suffix Tree and Labeling Leaf nodes //
/*void print(int i, int j)
{
    int k;
    for (k=i; k<=j; k++)
        printf("%c", text[k]);
}*/

void setSuffixIndexByDFS(Node *n, int labelHeight)
{
    if (n == NULL)  return;
 
    /*if (n->start != -1)
    {
        print(n->start, *(n->end));
    }*/
    int leaf = 1;
    int i;
    for (i = 0; i < MAX_CHAR; i++)
    {
        if (n->children[i] != NULL)
        {
            //if (leaf == 1 && n->start != -1)
                //printf(" [%d]\n", n->suffixIndex);
            leaf = 0;
            setSuffixIndexByDFS(n->children[i], labelHeight +
                                  edgeLength(n->children[i]));
        }
    }
    if (leaf == 1)
    {
        n->suffixIndex = size - labelHeight;
        //printf(" [%d]\n", n->suffixIndex);
    }
}

// free allocated memory where children are NULL// 
void freeSuffixTreeByPostOrder(Node *n)
{
    if (n == NULL)
        return;
    int i;
    for (i = 0; i < MAX_CHAR; i++)
    {
        if (n->children[i] != NULL)
        {
            freeSuffixTreeByPostOrder(n->children[i]);
        }
    }
    if (n->suffixIndex == -1)
        free(n->end);
    free(n);
}
 
void buildSuffixTree()
{
    size = strlen(text);
    int i;
    rootEnd = (int*) malloc(sizeof(int));
    *rootEnd = - 1;

    root = newNode(-1, rootEnd);
 
    activeNode = root; 
    for (i=0; i<size; i++)
        extendSuffixTree(i);
    int labelHeight = 0;
    setSuffixIndexByDFS(root, labelHeight);
 
    //freeSuffixTreeByPostOrder(root);
}
//End of Functions for printing Suffix Tree and Labeling Leaf nodes//

// pattern search  version2.0
Node* pickEdge(Node* node, char* p, int pos) {
	char c = p[pos];
	if(node->children[c] != NULL){
		//printf("node path label: %.*s\n",*(node->children[c]->end) - node->children[c]->start + 1 ,text + node->children[c]->start);
		//printf("node path index: [%d, %d]\n",node->children[c]->start, *(node->children[c]->end));
		return node->children[c];
	}
	return NULL;
}

int traverseEdge(Node* node, char* p, int pos) {
	int i, flag=0;
	for(i = 0; i < edgeLength(node) || p[pos] == '\0'; i++, pos++) {
		//printf("pos_matching_loop= %d\n", pos);
		if(text[(node->start) + i] != p[pos]){
			flag = -1;
			break;
		}
	}
	if(p[pos] == '\0')
		return 0;
	else if(flag == -1)
		return -1;
	return 1;
}

Node* findLocusNode(char* p) {
	Node* u = malloc(sizeof(Node));
	u = root;
	int pos = 0;
	while(p[pos] != '\0') {
		u = pickEdge(u,p,pos); // give next child if exists, NULL other wise
		if (u == NULL)
			break;
		/*if len of p runs out returns 0, if whole edge matches returns 1, if missmatch occurs return -1*/
		int k;
		k = traverseEdge(u, p, pos);
		if(k == 1) {
			pos = pos + edgeLength(u);
			//printf("real_pos=%d\n", pos );
			//printf("node path label: %.*s\n",*(u->end) - u->start + 1 ,text + u->start);
			//printf("node path index: [%d, %d]\n",u->start, *(u->end));
			//puts("going to next node\n");
			continue;
		}
		else if(k == 0) {
			//return u;
			puts("locus node found\n");
			//printf("node path label: %.*s\n",*(u->end) - u->start + 1 ,text + u->start);
			//printf("node path index: [%d, %d]\n",u->start, *(u->end));
			return u;
		}
		else if(k == -1) {
			puts("pattern not found\n");
			//printf("node path label: %.*s\n",*(u->end) - u->start + 1 ,text + u->start);
			//printf("node path index: [%d, %d]\n",u->start, *(u->end));
			return NULL;
		}	
	}
	return NULL;	
}

void subtreeDFS(Node* u) {
	if(u != NULL && u->suffixIndex != -1)
		printf("Index:%d\n", u->suffixIndex);
	if (u == NULL)
		return;
	int i;
	for(i = 0; i < MAX_CHAR; i++) {
		if(u->children[i] != NULL) {
			if(u->children[i]->suffixIndex == -1)
				subtreeDFS(u->children[i]);
			else
				printf("Index:%d\n", u->children[i]->suffixIndex);
			}
		}		
}
// end of pattern search v2.0 //

// memory usage tracking function //
int sizeofTree(Node* u) {		
	if(u == NULL)
		return 0;
	if(u->suffixIndex != -1)
		return sizeof(u);
	int i, size = 0;
	for(i = 0; i < MAX_CHAR; i++) {
		if(u->children[i] != NULL) {
			size += sizeofTree(u->children[i]);
		}
	}
	return size;
}
// end of memory usage tracking function //


int main()
{
	//puts("Enter string to build suffix tree on or press enter for default string: ");
	//gets(text);
	//if(strlen(text) == 0)
		//strcpy(text,mississippi$");
	char whole_text[22000] = "ggcttggatgaggctgtactgcgaccatttgccttttgtggtatcatatcctccctgatacaggcgccaatgttttattttaattgggtgattgacagtcaaaattgcctctgtggccggccggtcacgagtaagaatggtgattctcgaagcgaattgtttcggcgccggccggttcattattaaaacaagccgtcggtttaatttgagtaagaccagtggaaaccggaaattgccgcaggttatccaaccggaaattttatagccagtactatccagagcctcaatgtacgctgcggggacgttaccatagtcagtagagggcatgaaacccgattcggtggggattgcttggggataaaaatcccggatgaatattacatatccgcctaatttcaatttcaatcccgccgtaataaatacttcgcctttagatcgatctttgaaaggtgccccagtagtgatattcattagcaagagttcaggggcatattcttcgatccaaaattcggataactttagcgtaaagggcaaatcctgacgcataccattaagatctatgccgctggattctggtttgccctggtgaagaaccaggtttaacttgtgcttatcaccggctcccaaagccatggctactatgatcagccagaatcccaggtggctgaccaggaagcgaatattccggtagcttaggggaactgctcttttgaggattacaaaggttagagtagtcaggaggtatagccaggcaaagaaaaattgccaagagcgccagaggtgcgtaaaatccagtagagaatgttcatgtagattagaagattgaggtaccagtccaataattagtgtcatgacgccgaaatagcttatggcactgagggcggcaggcaaattagtcaaccactgaaccaggggatgatgtcttttaagacgataggcggtaattgccagcgtaagaaaagcgataacaattagtaggttaaaaggaaaaccaatcaggggtgggggaggatcaacaagcacctcaagcaaagtgccaaacaggcacaagcctattgctatcgctaaacttttacaataaccccacgggaattgccagatgatcatgtcatttaaaacttgcttttccattgtggagtttgggatgacatccgagcacactaagttgattcgtaaatattcggtgatgcgatctcacattaagaatatagaatgctctaagataattctggagataattcttgaggtacatataatttttattggatagcttgttcggagaaactgtcccaattatccgcgatctaggttggattgggatattagtagtgcatttcattaattaatcttgtcgttcaggtagtcccgcttgccctttttacgctttttggtgatttaaccgaataaataacaaggcacattaattatagagcaatcaatgccaaacgatttgtcacacctataggggcgataagttaacctttcaagagcgaatcatgactagcaacatcttgaaaggtgagactgcttttacagcgtgtggtttgtgggctggcatgatgagcatttcgccggattttagccggtataccttgcctgagatagtgacatcggcttcaccatctacgcaatacaccagagcatcaaagggggcggtgtgctcgcttagtccttctccctggctaaaggcaaacaaggttaccgatccggtctcacgttcgaagattgtgcggctgaccactgccccaggctgataattgattaaaccactcaaattaaatgcactgtccaagagtgtttccatagctctcctccatttggattagtatttaaaaaacacaattctgagtttattttcagcagattatgcttttatataatccgcgttgcgctttcgattggctttattttgattttttggattacttccaatgtacccaggctttcacctctggggtaaacagcaaattcgcggctataaaggatttgcccctagagcttggctcaaaatatcgcccgatcaagacaattattgccgtagttgagaatttccgcgctgtgacgcaaaccctgcttgaagtgaaggagggtgcttgacaattaataaccacgatagtaagggcagaggaccttttgccgatgcagaagagagaatttccgtttttggtattgacttgctggtatgtatggttgttgtgaatctaaaattgttcaaataatataaaccagcgcggattaaccgccgtatctatttcacgaggagcatttttttatattgggtaaagtcaccagaccggaggcgatataaataaattccgctggagagctgggcgccatcgaattgtagctgataatcaccgacaggcaaaattttattggtaagtacttgaatcaaccggccttgcaaatcaaaaacctcaattgttataggtgcttttgtgaggatgctgaagctaatccaggtgacagaattaaagggattaggataattctgattgagaacccatcgctgcgggatgccggtctcaggtgaattggcaatatctgcaagctgctcgatattggcatcgatccagccgattcgcaaagctagccagttttttaggtaattgatttcttgctggaaagtcgtggcggtgcccatttcaggccagcggaggtagttgcggtattgggcttcggttacaacgttttgcaaggaatcaatcatgcgataaagggtgtccaggtgaaaggtagtagagcgcagggcaaaccagcgctgagctgcatggtaggcgaatgtcgggcgtcggaccaacacttcccaccagaatggaatcttgggaatgtcgaaggttaaacgtttattatggtcgatttcccagtcgccataaaggttgcggtcttgctcgtaccaggcgtggccgaaagtcaggttcatgtcccagatagggccggcttgtagtttgccacctttactgtcgcgatttttatatagaaaggcgctcaggcgataggcgtcgatattacgggccagctcgtttagcaaaaaataatccacgaatgaagcttcgtctataatcccggcataaccactgataggatcgtccacttgcggacttttcatgatggattcaaaagcggcgatgaatttttggatgtactttttctgctgagcggtaatctcgctgggcttggggtaatcatactggtactgaatattgttaggagagtaccaatagcccacgttctcgccctcgatcttatccaacttgatgatgtagccgccagttaatgcatcaccgctcgtatcctgaggctcgcatttggcaatattgacacggtttttgtcccgtttgattttctccatcaggatataaatacctttgtaatcattattaatgatcaattcgaagtagcgggtacggctggcgtaacgtccgatcgcgcgggacaactcgaaaggaagcacattgcggataaaggttttgtcatcataaggaccatagaaaatccaatcggcttccgctggaaagcccagcaaagcaacatctaggtcttctccgttagcatcaatggtttcgacgcggtactgtttcttaggccataacattgagctgctgcctcgcaattcgatgacgatttttccatcgtaattgttatagggatcagtgatgtaattgcgattgcctttaccattgtcgatgacgcccatgtcggctacgatacggctttcatcgacgatggtctgcccgtgggtgtcaatgacgataattggcaggttagatgaggttaggttgacctgagccagtagccaccagggtattaggattagcagcgctactgacagcgagattgttgccagcggtttggtctggtttctgggtttgcttaaatcaatctggaaattactgagcagaccagttttggaatgcgagtctatcatcatgaattaatctatgcagtttaaatcctcgatgaaatgcttttttcctttggtaatgattgtatgcccctctttttcaaggaggtctttatggctggtggcgccgccggggaatttttcgttcagttcgccattgtttttgattatgcgccaatacggcaggtcacttccggcttctgcagcagcgttggcagctatcataacgaaaatagcggtggtcagcggacagcaaaattttgtgccgtattgttgggctaattgcgcgcaaatggtactgagcgtcatcagtttcccgaacggcacggtctggatgattgcgactacagcggaaggaggggccagtacgacactatcaccaggttgggcaccgctttttagtaaggctttcccacaggggaaccggggattaaaggacagaattttagggaagcctttgttatcggctaatttttcagcccaggttttctttttattagccatatcgacctgagttttttacctaacgatttttctcaatttgtgatacccgagccgctttggcgccgggcagtgaacgtgccttgtttagttggtggccagccgaaagttgtaaaagcaatgctccattcaccttgggcttgaccatcatgcgcagtgcctttgacgttaatcgtaaacggtgaggtattatagcctgccgggattgtaggattcggattggtagcgacgccttggccactgacctggatcgaatcgccatttacgatgagagttccctcttcgaccgggcattctactggatacccatcggatataaaaatccagatagccgtagtagtgatgctaccatcagatttcttatcgaaggtagcttcgccatgattggaagaatcattatccataatgaactcccagacttcggtagctacgatcgttggctcatcatcaaccgtggtacaacctaaaatgagcaggccaagccccatagcaattatcagactaattttggtcgacatagctcctccttaaatttatgttgtcagcgattagttctatgaaggacgcgtcatttctattagccctgaatgggtgaagtaatttcgtctccgagtggggagagtgaaaagccaaattcgagtggctcaaagtcgatccttgcgctgggttttttgataatttttcaccattttggagaaggatttttcgcgctttcgcctttcggcgagggtcgtttcaaaatgcgtcctttcccgctctagtttgagataattttcataagcagcccgatcaatcgctccatctgcaatcgctgccaggactgcacagccggcttcgtgggtgtgggtgcaatcgcgatacttacaattttgccccaaggcaagtatttggtcgaaggtcatttccaggccgttatcggtgtcggcaaggccaatttcacgcatcccgggggtgtcaatcaaaagacccccatttcccagcatgatcaactggcggtgagtagtaacatggcgccctttaagcgtgtgctggctgatggtgtcggttttcatcagttgtttgcctgccagattattgagtagagtagatttgcctacaccggaagaacccagcagacagtaggttttgcctttttcgataaagcggtcgatagcctcatagccttgacgagttgcattactaattgccaaaactggcacctggccaatccggactttgagattctcgatcaatcttgggacacaatcagcctcgatcaagtcgattttggtaagcacgattattggcataaccttggaagagtagcagattgtcaaataacgttccaggcggttcaggttgtagtcccgatcaatcgcttggagaataaaggcatagtcaatgttagtggcaatgatttgaacttcaccgtactggccgacggcttgccgtttgattacagaaaagcgtgggaggatgcggtggatcagcgcaaagtcgggttcgtaggtcgtcacgagtacccaatcaccgacgctggggaaatcttcctggctttgcgccgcatagcgcaagctccctgtaatttcggctttgaattcaccttgtcttgtgcggacaagatagagttcgcggtattcagctactacccgtcctacctcaaaagcactcgcatctatgccctgataggcgttttctaattctttattgtaacctaaatcaaataaatccataaatggtttgactaatctccttgtttattcctgattcttagatgctgtcgggtagctggtaggacagtcacgaagacaatcattgtcccgaggagggaatagataaagtggaatagatgaatagagcgaatgaacaggacctgaaaatcaatccagagaattaggattattccgatggcgaaagctagcgaccaagaccaatggtattctttgtctagattcagatactcaaaaaagcgacaatcgggacgcttgatcagggcataggcggtgtaccctggcagtagtcccaggatgcacaacaggaaaagtcccggaattagaaagttgggaaatggtgaatgctccagcagcgagagtggcatctgcagcaacgccccactgggactaatcaccagcgctaagccgccaccgatggcaccaagggattggaaaatgaggagacccaccaagacaaagagcgaaaatttttgaaatgaattcatgctcctattttgcctccaatttttcagttaccttcgtattactgaatcgataaataatgataattacctcgctttttttaccaattttcggtaataatataactcatttagattaagcggttaaaatgttgatgctacgtacggaggggagttttttacctaatattttttaatatcgcggtgatttgaatactggcggtaccatctccgaagatattggtttgcacggaagtaaaattagacgccttcgcgatttcttctggtatacgctgaaaatccgtgccgatgagctttgcccagccattttcaactaactcaacccattccgtctcggtacggagtacaagacacggcactccggcgaaataggcttctttctgaacaccgccggaatcggttagaatgatacgcgcacttttttccagcgccagcatatctagatagccgactggttcaatgaggcgcaggtaggaatttttgatggtcagctgctgttggcggatgaatttcatggttcgggggtggaccgggaagagaataggttcgtctaagtactctaggatgttgatcagtgctttcaaaatggtcggatcgtcagtgttttgagggcgatggatagttgtcaagatgaattttcttggggcaacaccatattttttaagaacggcctcgatgtcaactcgcgagagattgaacaagaacgaatcgtacatcacatcgcctaccaaatagacccccgtgttgatgccctcgttttgcaggtgtttgatcgccgtctgggtcggggcaaataagaactgggcaagggcatcagccacaatacgattaatctcttcaggcatgtcgcgattgaaactacgcagaccagcttcgacatgggcaagtggtagctgcaatttggcggcaactagtgcccccgccatagtagaattggtatccccatacaccatgaccaggtcgggtttgatatgcagcaatactttttcgagctcaatcagcatgcgaccggtttgctcgccatggttgccactaccgataccgaggttgtagtccggttctggaatggcgagctctttgaaaaaggcttgcgacatattatagtcgtaatgctgacctgtatggatcaggatttcgttgaagtgtttgcggatttcccgggaaactggtgcacatttgataaactgcggacgcgcaccgatgatggtggcgatggtgggcatcaattcaaactcctggttttagattggaaataaaatagcgcatcttaccggtggattccggcctaatttctggaactaaaataaaatcaattttgatatcggacgtctgaacggtgcgctcaaatattttgcggaatagttctaaatccgattcaatgaaatcacgactttttacaaaccgcaccgtaaaagcagacggcgcggtctgaacaatctgatactgctcaaatggatgacgccccaaatccaacagcgccaggttcaggtagtcaaagatttcactggaaaaagagcggccgtcactcataatcacaagatcggtggccttgcccgtgtgtaacttcatgattggcagattacatccacagggacatttttcagctacccaatcgccaatgtcgccgatcctataacggataaagggcatatattcattggttaacgatgtcccgatgatttctttggttccgtttttattttcgacaaattctagaaaaacattttcgatggaaacgtgccagttcccaaatgggcattcataggcaaagcccccgatctccgaacagccgtattcattggagatgggcactttgaagactgacgcaatgatttctcgctcgtgtggaaagagggtttcggcggtgacgaaaatcgccttcagccgggacggtactggttggttattatttctgaaataaagtgccagacgataaatgctgttggcgtaaccgtagaaatagcgtgggttaaagcgccgaatctggcggcaatattttgccagaatttcatcggagtattcgaacccggaaattagcagcgtgttgcggatacgacccttgaggacatcgataactctgcgataagtcgaatagagcggtcgcccccaaatggccacttccgggtcgccaatttgtattccgaaccaattccgtgcccgccaacgactagcccaatcccagctgttatattcgccagtaacataaaattctaacggaatgccagtcgagccgctggttttaccgggatatatttttccggcataatgcagattgagaaattgttcgaccttttcggcgattgtctttttcgttagtaacggcaggagtttaaaatcctctacggtgcggatgtcttccggtttaatgccgagccggtcgaaggtttcatgataatagggaatgttgcgataagccactttgagaatatgccagagtttggtcagctggattttctgcaattcttccaacggcagaggttcgttgtttttcaagattctgagatattgggtgattgattcacccttaagacgctgggtgagtggaaagtaaaaatgccggacgaattggctatacatcagcgaatttcctgtaaaaccctttcaatttttcgactagtgatctcccaggtgaaatattgcgtaacgattttgcgagccaattctcccaatgcgatccgccgtttttcgtctagcagcaattcaataagtgtctgggcaaatttttgctgatccttcacatcgacaatctcaacacacctaaaattagtaactaattctctaacggtttcaattggactagcaacgactgttttgccgcatgccaaataatcgaataacttcatgggcgaaccgctaattttattgaaaaattcggtatcccatggcgacacgcaaatatcgaatgaatttatataatacggcgccatctcgaacggcactttgccagtgaaggtgaacgatttactcacatttaattggcgactcagggctttccattcctctaattgggctccatcgccagcaactacgaaacggacctgcggcattttctcaagaatcaaaggcgcggcctgaatcaaatactgcacgccgtgataatgatagcaacttccgatgaaaccgatcgtcggagcatccggtagactggtttgggcaatagcttccgatttgggcattggccggaaaaggtttggattagtgccattcgcgatgggaataattttagcggctttgaccggataattggcgcaaatcaggtttttcaagccttggctgactggaataatgcgcgcgcagtagccgaagttgaggcgttgaatcaggcgcatataggcaatcttccatggtgagtaaccggtctggcgtaattcttccaacacccagccgttgatttcgatagtcagcggtacattgcagagtttcgagactgccagcggaataaatgacgaggtttcccggaaatataaatgatccggccggtgcttcaggatattgtatatcaggtaacagaacgaaaaaatctcgaatattatccagcgcacaagcggaaggttaatgaccggaatcgaatgaatggtgactttaatgtcgaaagtgggcttgacgatgtccggattgaaaaaatgcacctcatgcccaaattttaccaaccagccgatggtttcgcgaatgtgattgttaatgttacccagtcggctaaagccctcgaaacaaaagtaataaattctgaaaggtcgcttcaaagtgatttttaagcctggttaatattagagaaaggataaaggttcatgacttcaaatttaaaatatagttatggtccctatattcacattttgatatttcaggagggtaggtttttatggttttgaatatgatggagttagccattgccaagttggagtttctcgaaaattttaaggtactgttcggcatttacactccaggaagtggtgcgcggcgttatttttccagtattattgagagcatcgtgtatcgccttgattaaagcgttcaagtcattgggttgacatagaaaaccgcctgaataattttccagaatttcaggaataccaccgaccttagtagccacaaccggtttgcccatcgccatggcttccagaatcacattcgggacgccttccgcaagactgggcaaaacaatcacatcggaggctccgtaccagagcggcaactgcgaatgtggaatgttacccggaagaagaacctgcccttgcagtgagttttccgtgatgaaattctcgattaagttccggtcaatgccctgacctattaaaaccaatttcatattcggttggctcatggccaaatatgccgaaagcaacaggtcaatgcctttacggcggctcaaattaccgacaaaaagcagataggtggaatctggagacatatttaattcccggcgtgatcggagttgatcaattggatgaaaaagtgattcatcgacgccattgtagattacggcaattttcgaggcgggatagccacgtcgggttagctctgttttcagagcggctgaaactgttacgagctggtcagcttgagtcagcgttcgatgcatgaagcgcttcaagatcagccgggtgagataggtattgacgtccgagccgcgcagaccgactacggatttcttgccatatttttgcgctaaccggacggccgcatatccacccgggcagcccatttgtcctacaataacatcaaaagcatggatttctggtttcagatccgcttctgccaatctgtaaaattgcccggaacgccgccagatgcggtcgaagcgatcgatcactttaattcgctgaatccaggcaaatcgttgattagtgccgtaatttttacggaaactgactaaatctggatcacagaccaggcggttcctttccctgaaaatcacctggtaaaccgtaaggtcacattttaaggccaggtgaacagcccattgatagataaaaatcccgtggttgggattggttgggtgggggaatagatcggtaaggactaagactttcacgatgtcaattctaaatatacggcttctacatgattcacaaaatttgtaatagaatatttttcagccgttcttttagcgttgctagaaattttctgatataataccggatcgttatataactcagcaatagtttgggctagcgcttgggccgcatccaaccgattcccctggaaggtcttttcccatttttccggaaaagtcagtcgcccatcatacccattatggatgatttcagaaattacaccaacattgagtcccacgaccggtagtccgtaactcattgcttctaataaggcgatcccgaatccttcggtatgaatggagggaaacaggtaacaattcaggctggcgtacaccagactcaagtctttttgaaacggtaggatagtgactgattcttggagtttattctcagcgatcaggcgtttgtgctcggcctggctggcttcatcgcccatgacgacggcgtgtgccggaacgcccatggcgcgcagctttttcagggtcaagtagaagatgttaaagcctttgaaaggatcaaagcggctggtgttgccaatgagaaagcaattggagggtatatgccatcgttttttaaaggctttaataacggcggtgtcggatttgcgcggattttcgaccggattatatattagccggatgcgtttttgcggcgcatattcggtgttaattaaatgcgcagtaacggttttagagattgataaaatccgctcggccagaaagcggttgagcaggatccggccgagtttgaaaaaaagaccagttttaattcgaatattatgtttagtataaatgcaatggcgaacgccattcagcctggcgaaaaacgtcccaaagaattctagtgcgccataatgcgaatgagcgatatcgatgcgatgccgccggatgatttggcgcaattctctaaaagtctctaaatcatgccaggattgcagatgcaggtgatagagcgggatattcagagaacggaaatctttatagaagaaatcatttacaccgtcagaatacatggtaaccacaaccggattgaatttggtacgatcgatattttgcaataggagcaagagcgatttttcggctccgccgacctgtaaactgccgatgatatgcagaacattgatctttttcattttaggattcgggctacttctctaagcggttggtcacgcgtgattattttatcgatgaacagcaagtgccagagttccaaccagaagagcgaccagagccgatgtttgtgctcggcagtgtgggaacgttgttcgctccacagcttattgatgcagtcgtaattgaaataattacgttgccgggcaaatcggctgaaaacaaccgcctcgaaaattccactcaggtcattttgaatccagcggtcgactggaacgccgaatccacgtttcgcggtgtaaaggagctttttgggcagatatttttcggcgattttctttaggagatatttggtctggtggtttttgaatttcagtttaggggggatttgagcgacgtattccagtaaagccgtgtccagaaatggcgaacggcattcgatggagttggccatggttgcccggtcgattttgactaagtaatcgtagaccaagcgggtcattccatcggtgaaaagcacctgatcaattagattaattcctttgactttgttgaaattatcatgaaacgggtgaagtggattgtgttcggataaatgctgtaaatgttccggatgccatagctcatggcggtatttatgaaatcccatcgtattgtaaaagctttcgtacgggaaccctttcaggtagtcgatataaaacagagccttgccgaaaaccgaatcgctttcgggattgaccgacaaatttttcaaaatgttgccgactagcggttggagaaaaggtggaacaatccgggaaaataactgggcattgcgcgggactattgtccgaccataaccgccgaaagcttcatcgccaccatcgccggttagcatcaccgtaatgtattgacgcgcggctttggagatcaaataagtggggatcgctgaagaatcggcgaaaggctcaccgaattgccagacgagttcgagcagattatcagtaacattatgatcgacaatgacctcagtgtgatcggtgtcgtaaagtttggcaacttcccgggcggcaagcacatcttccatcgggtaatctttaaaacccatcgtaaaggtcttgatccgctgggtagagttacggcatagaatggcagtaatgacactggaatctaccccgccactgagcatcgtcccgagtggcacatcgctgatagtgcgtcgcatgactgcttcgacgatcaactgctcgcagtggtgcaagatctctggttcgctaccgaaagtttttaatgaataatcccaatgccaataggtagtggtggcggatgagttctcagaaaacactgtgaatgtacctggctggattttctgcacgccctcgaaaatcgtatgttgttccggaatagcgatgtggtgcaaatagcaatcgattgctgtcaggctgaccggtggttgtgacggcagcgccgcgatgatggctttaatgtccgaggcgaaataaactaggccggattgttcggcatagtagagaggctttttgccgacgcggtctcggccgagaattagttgatttttacgcgcatcgtaaatcgcgatggcaaacatgcccgtgatttttcggaataattctgttccccattcggcgtaaccatgcacgagcacttcggtatcagaattagattgaaattggtaaccagcagcgagtaaggcttgacgcagttcttgaaaattataaatctcaccgttgtaaactgtccaaacggtacggtctgcattagtcattggctgattggcggcgtctgacaggtcaatgatcttcaatcgtcggtgaccgagagctattttcggcagaattgcataaccggcggaatccggaccacgttcgatcatacaatcccgcatccgacagatttcagctggagtgacctcgattctaccctgatgaaaaatgccacaaatgccgcacatgttactcgcctaatttattaagcgtcaattgataaagattcactaggtgccggatattaggatggcccaattaaccacagaataattttttgataggcgttttcttccagaccagtgatttgaattgccctggtgcctcctatgaggcaaggattatggtcttactatcggcactgctcgcaaccaacctctacatgagcgatactattattgagacgctcacaacagtgttactcgcatccgctacaccccgcacagttattatcgattagcgaatattcatccttatgttttagataaaactccagtaaagccggtttgaatatatccttctcatcagctgaaaggtcgttatataaattcacctgctcccacatcatttcggcgatatcgtcttcgtccacggggaaagatatcaacgttcccagtacgcattcatccaaccccagcagacgacgctctgcggtgttgattttcaatataaattcctttttggtacagctacaacccatacaattctccctgttcgattaaaagatctggcggtcatcccggaaatatttgtttaccccaaaaatgataccaccgaagattagcaccgccaccggcacgacgatccagtgctggacccccaaggcctcgaatagattgccattaatggatctggcaagcagatgggctttgaaaaactggacattcagcgcatacaccaggattcccagcaccatgccgagtaatcccagccaggcgtcctttttgccttcgccaatgcgtgatagagcagtgccggggcaatagcccagtaaacccatgccgacgccaaaaatcagcccgccgactaatttgccccagtccagcggcttgggattgaactgaatcaagccaaggtcgctcataagcgtaaaacctaccagggaaatggtcacggcagtgagcatgaacttgagaattttgaaatcttttaacagcagtaagcccgtcacgcgcggatacttggtgatgccagtacgttgcagaatcaatccgaaagccaagcccatcaacagaccaatgactattttcaggatcatttcatacctccggtttttggatagagcagtttggcggtgatgatcccggtggcaaaaaacgaggctccggcgaccatactgcccagcgacagatgcgcccagcccatcaggatattaccggtcgtacagcctgatgccagaatcgccccgaagccaagacaaaatccgccgatcagcacaacgatcagccgtttccaaatattgggaccgtggtattttacccacaccgcgggaatggtttcaggtgaatactgacgtgatgatctagcaccgacatagccgccaatgatgattccgatcagcgcgaagaattccacaaaggcagctggtgaagagcttaggcttttgatgagcggattggttgccagggacgggaagatgtggcggaactgagcgctgatgtagccgacgccagcagtgatcccgaaaggataatccttgatgctcagcgtctgaaacagccagtaactgaacattcccagcagcgacgccagcaaaccgccggtgaaccagtgccattcgccgttttgattgaaaagttttttcatgtgctctccattttactaactatggtttgaatgtttcgtttcatctgtttttcagctattccggctcgatcatttttcatcgtttggtaatttaattgatgggttgttcgccgaggatgattcctggtgaagctttgatcccgaatttttcgatgatttcttcggcgcccggatcatccacatatttcaccacgtatggcaatcctgattccttcagaaagcgctcgacgtggttgctcttgcaacatttttgcgaacggattaagagaatttgcattttcactgagacttagctatttatgatctcaaccgatttaaaaaattaccatgcctattaaaccacagccaatgagcaaatagattggattcaactgtttaaactttaaaaaaacaaagaatgccactatggccagtagatacggacgaaaatccctgaaaccacccgctgccatgagaaaaacagtccgcataagcaggacgagtacgatgaccgtgatagcgcttaagaaatttttatagattttgttctctttgattttaactaaggcgatgatgacaatcagggctaaaaggaacggcaaaatatcgtaacctattgtagcaataaccgcccctaaaaatcccttgagcctgaaaccgataaatgtcgacagattcatcccgattggacccggagtggtttcggcgatcgagaccatatagagaaaatcctgttcggtgatccattgtagattttcaacgatttcacggtgtaaaatcgggaaaacggcattgccgccgccaaaagcaatgatgccaatcttgcagaattcaaggaaaattttaactaacatcgcttctcatcatcctactaatcggcatgttaatgagttttctgacatcaacggtaatatcaaatttgctgggaaagcaaagtggcattttcgggattctgctgacggctttacttggtgcggtcatgcatatcccttcgattgtcgcctttccgctggcagcgtcattgctggaaaaaggggcgtcggtcatggcaattgctactttcatcactaccctgacaatgattgggattgtcactttgccgttggaaatcaaagagttgggaaagaaattcgcgttctggcgcaacggactcagtttcattttggcaattctggttggcctcctgatgggagttgtcctgtgaaaccgccatcaaatcaaaaatcgaaactgaccaaacgtaccgactggctgatcctggcaattgcagttcttacgggcgttgtcctcttactaattttcccagagaaacaggaaatcgctgggataaccgcccggcgtattgcgacggagatgtttaccatttttccagcagtactaattttaatgggattattcgccgtctgggtgtcaaaagagacagtgatgcaatttctgggcgaaaactccgggttaaaaggcattgcgctggcgctgtttttcggggcattacccaccggaccgctctacatcgcttttccgttggccgccggtttgcaccaaaaaggagcaagtcttggtaatatcgccattttcctgactgcctgggcttgtatcaaactgccgcaggaattggtggagctgcagtttctgggagttcggtttatgcttgcccgtctgggcttgacaatcctggccgcagtgattatgggtatgattatcgataaaattgtccaggcggaccattttccggacaataaatcaagggagaatattctatgaaagtatttgatctcaaggccatgcaagccgctgactatgccgaacgcggtaaaaatgtgttctacagtacgccagaatttaaaacccggattatcgaactgccggctggaggtacgatgccacaatgtgaaatggcgtccgatgtcgtctttgtcgttatagatggcacagcaactgtcactgttaatcaacaggaagagcagttgaaagctggtcagtgcctgatcaccgaacccgccatgctctcaatgaaaacggaaaaaggcgttaaaattcttggtattcagattcaaaaacaaaaatagattagaattaaaaattataaggtacagaattatgataagaaaaatggctctgataactacgctcagtttatctgtcggattatttctcggttgccaggcaaaaaaggagaccgctccagtctctgaaaaagctgtagcaaaattagtgcaggttgaagtcgttaaaacgcgcaaaatggtggaaaccctggacctgactggcacattgcgggcggagaatgtcgccaatattctctcgaccgtcgagggtaaaatctcctgcctgttggtacgggaaggtgatcaggtagaacccaaccaagtcgtggcgatgatcagttcactggtgcgtgaagatattatcaacgcggcccggctccgcatggaagccgctaaaagacaattaaacgacaatcctgaaaatccccaattcaaagtcgcgtatgaacaggcgcaacaggattatgaatttgccgtgcaacaatacaaagaaattccggtaacatccccaatgcaagggctggtttcccggcgttgggtggatttgggggatatggtcccttccaaagctaaactctttgaaattcaaagcagcgccaaactgattgtggatgtaccggtatcagagcttgatcttagaaaactgaaaaatgagcagaaggctaaaatattcgttgatgcctgtccagaaaaacaattccaaggcgctattcagcgcattcatccccaggttgatgcccagacccgcaatggattggtggaaatcatccttcttgacccctgccctaatctgaaatccggcatgttcgtgcgggtcaccttcgatgtccgcaccattgaaaatgcaatcgcagttccggtgcaagccatcatcgaacgtcctcaatacaaaacctgttttatagtcaacgatgacaaggctcaagaaaaaataatcacaacgggtctggaaagcggcggctgggttgaaattctttccggtctttctgtcggagaaaaaatcgtgatcgaaggtcagcaacaactcaaaaccggttcgcctgttaaaataaaagggaagaagtaaccacaaaaacacaaagaacacagaaaaaaatatgttttaaaattttaaaataagtaatctgttttgtatcttctatgcctttgtggcaaaaagtaaagaaataatgtccaccgagacacttcagcaagagtctccaatgggacgaggaacccagagaattcattatgattactaaaaatttgaaaataaaaaaagtcttcgtgccctgcgtgtccaaaaaggaatcttaaatgaagctaacaactatttccattcatcgtccggtagcaaccattgtgttaattattaccgcggttgtacttggatttttcgggtacagccgcatgccgatcgatttcttgccggaaatcacttacccgatgatcaaggtttatgtctattggcgtggtgccacaccggaagaaattaacgacaacatcgccgacccgattgaaagaaccctcagcacggttgacgatctcgactacctggaatcgtcgtcaattgaaggcctctataccttattggtgaatttcgaatacggcaccgacgtagacgtcgcctatcaggatgttgtcgctaaaatgggactagcaacccgtaaattgccacctgatgtcgatccgccgataatttttaaagctgatccttctcaattgcccgttgttcagctaattgttcaatcggaaggccgtgatttggttaaactgcgcgaatggatcgaaaatgttgtgcaggatcagtttctcgccgtcaagggcgtcgccgggactgaaattctcggcggattgaaacgtgaaatccgcattcacctcgatcccaaacgaactacagcctacaatttaaccttaaatacggtgattaaacgcctccaagaagagaacatcgagcgtttagctggacgtgtcaccgaaagcgggcgtgaatttatcgtccgtactaatgcagaatttcgtaacctagatgacattcgcaatgtcattattttcaacgacggcaaatccatggtgcgcctgaaagatctggcaaccgttgaagacagtcacgaagaacaacgggtcattacccggtttaacagcaaaccagccatcaaactcaacattttaaaacaagccgatgctaacactgtcgatattgccgagcaggtcaacgcgcttattgcaaaactagatgatactgcgccgccggacatcacttttaatacagtcgaaaaccaggcggactatatcaagggcgccattgccggggtacgggattccgctattattgccgtcttactggtaatccttgtcatttatatttttctgggacattggcggcaggtagttatcatgctgattgccctcccggtaactattttattcaatttctttttgatgcaactgcttgggttttcaatcaatatcttttccttgggcggattagtggtcgcgatgggtgtggtgctagataattccatcgttgttattgagaatatcacccgcctgcatcttgaaaaaggcagccgtaccgatacagtcgcagacactgccaccagtgaggtcgcgacggcagtactggcttccactttaacttttatggcgatatttctgccgtttttgct";
    time_t start, end;
	double cpu_time[22];
	int size[22];
	int i  = 0;
	const char dollar[] = "$";
	for(i = 0; i < 22; i++) {
		strncpy(text, whole_text, i*1000);
		strcat(text, dollar);
		start = clock();
		buildSuffixTree();
		size[i] = sizeofTree(root);
		freeSuffixTreeByPostOrder(root);
		end = clock();
		cpu_time[i] = ((double) (end - start))/CLOCKS_PER_SEC;		
	}
	
	for(i = 0; i < 22; i++) {
	//printf("Time taken, #of characters:[%f sec, %d]\n", cpu_time[i], i*200);
	printf("%d,%f,%d\n", i*1000, cpu_time[i], size[i]);
	}
	
	buildSuffixTree();
	
	char pattern[1000];
	while(1) {
		puts("Enter pattern to search: ");
		gets(pattern);
		if(strlen(pattern) == 0)
			break;
		printf("Pattern: %s\n", pattern);
		subtreeDFS(findLocusNode(pattern));	
		
	}
	freeSuffixTreeByPostOrder(root);
	//checkForSubString("abc");
    return 0;
}
