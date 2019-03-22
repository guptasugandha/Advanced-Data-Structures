#include<iostream>
#include<math.h>
#include<vector>
#include<stdlib.h>
#include<fstream>
#include<map>
#include<stdio.h>

using namespace std;
static unsigned long int num_nodes; 			// Keeping count of number of nodes in the Fibonacci heap

class node										// Defining Fibonacci Heap node
{
    public:                
    int frequency;
    string keyword;
    unsigned long int degree;
    bool childcut;
    node* left;
    node* right;
    node* child;
    node* parent;

    node(int value, string word) 				// Constructor to initialize a new node
    {this->left=this;
     this->right=this;
     this->parent=NULL;
     this->child=NULL;
     this->childcut=false;
     this->degree=0;
     this->frequency=value;
     this->keyword=word;
    }
};

map< string, node*> u;   						// Create an unordered hashmap
map< string, node*>::iterator it;   			// Iterator for the hashtable
node* root=NULL;                    			// Pointer to the Fibonacci Heap

void consolidate(node* c, node* p)             	// Parent p, Child c, condition: p>c
{   (c->left)->right=c->right;					// Removing child from root list, editing the left sibling's pointer
    (c->right)->left=c->left;					// Editing right sibling's pointer
    c->parent=p;								// p = c's parent
    c->childcut=false;							// c didn't lose a child since it became p's child
    if(p->child==NULL)							// If p didnt have any children earlier, c is its only child
    {	p->child=c;
        c->left=c;								// The left and right siblings of c don't exist so it points to itself
        c->right=c;
    }
    else                             			// If p already had a children list, c is inserted into this doubly linked circular list
    {   c->right=p->child;						// Insertion into the child list takes place like the insert node insertion
        c->left=(p->child)->left;
        ((p->child)->left)->right=c;
        (p->child)->left=c;
        if(c->frequency<(p->child)->frequency)
        {p->child=c;
        }
    }
    p->degree++;								// The parent's degree is updated
}

void insert(node* new_key, bool existing)		// Insert a new node into the heap
{if(root==NULL)									// If heap is empty, make this as the root node
   {root=new_key;							
    root->parent=NULL;							// The parent of the only node in the heap will be NULL
    if(existing!=true)						
   	 {num_nodes++;
   	 }
    }
else
{   new_key->right=root;                    // If there were one or more nodes already, then insert the new node to the left of the max node
    root->left->right=new_key;				// The previous right sibling of root now points to the new node
    new_key->left=root->left;				// The new node points back to it as left sibling
    root->left=new_key;						// Max node points to new node as left sibling
    new_key->parent=NULL;                   // New node won't have a parent because it is inserted as the root(max node's) sibling
    if(existing!=true)
    { num_nodes++;							// Increase count
    }
if(new_key->frequency>root->frequency)		// If frequency of new node is more than that of the current root, update the pointer to the heap to now point to new node
    { root=new_key;
    }
}
}

void cut(node* cutnode, node* cut_parent)
{if(cutnode->right!=cutnode)             	// Remove the node from childlist of parent if the parent has more than one child
{cutnode->left->right=cutnode->right;
cutnode->right->left=cutnode->left;
}
--cut_parent->degree;                  		// Reduce the degree of parent

if(cutnode==cut_parent->child)         		// If the parent's child pointer is set to this node
{if(cutnode->right!=cutnode)         		// If it has more children change the parent's child pointer to next node in the childlist
{cut_parent->child=cutnode->right;
}
else
{cut_parent->child=NULL;        			// If this node was the only child then the parent does not have any child after the cut
}
}
cutnode->left=cutnode;                  	// Making root point towards itself
cutnode->right=cutnode;
cutnode->childcut=false;
insert(cutnode,true);              			// Insert the subtree rooted at this cut node to root list
}

void meld_fib()
{
unsigned long int maxdegree=num_nodes;		
vector<node*> table;                       	// Table to keep track of trees according to its degree
unsigned long int roots_count=0;      		// Keeping the count of the root nodes for meld operation
if(root==NULL)								// If there are no nodes, it gets returned
{return;
}
else
{node* temp=root;
do
{++roots_count;                				// Counting number of top level trees to be traversed
 temp=temp->right;
}while(temp!=root);
}
node* current=root;
node* cons_child=NULL;          			// Pointers for pointing to same degree trees and pass them to consolidate function
node* cons_parent=NULL;
node* nextnode=NULL;
unsigned long int node_degree=0;
while(roots_count>0)                       	// Iterate through the top level list and for each node the following
{
nextnode=current->right;
node_degree=current->degree;          		
while(node_degree>=table.size())
{
table.push_back(NULL);
}
while(table[node_degree]!=NULL && table[node_degree]!=current)     
{
    if(table[node_degree]->frequency>current->frequency)           	// If tree in the table has a higher frequency
    {
    cons_parent=table[node_degree];                      			// Make it as parent of current node being traversed
    cons_child=current;
    consolidate(cons_child,cons_parent);    						// Combine these two trees
    if(cons_child==root)                                 		
    {root=cons_parent;
    }
    current=cons_parent;
    }
    else
    {cons_parent=current;
     cons_child=table[node_degree];
     consolidate(cons_child,cons_parent);
     if(cons_child==root)
    	 {root=cons_parent;
         }
         current=cons_parent;
    }
    table[node_degree]=NULL;                                       	// Clear table[d] was we just combined the trees to make them of degree d+1
    ++node_degree;
    while(node_degree>=table.size())
    	{table.push_back(NULL);
        }
}
    table[node_degree]=current;                                     // Insert the tree to its degree position in the table
    if(root->frequency<current->frequency)
    {
    root=current;
    }
    current=nextnode;                                               
    --roots_count;
    }

}


void cascade_cut(node* curr)
{node* curr_P=curr->parent;
 if(curr_P!=NULL)                    								// Cascade cut only if there is a parent
 {	if(curr->childcut==false)               						// If childcut of parent is false, set it to true and return
   	{curr->childcut=true;
   	}
 else                                        						// Else cut this subtree rooted at this node and cascade operation upwards to its parent
    {	cut(curr,curr_P);
        cascade_cut(curr_P);
    }
 }
}

node* removemax()
{   node* del=root;
	if(root==NULL)                                              	//If there are no nodes to remove, just return
    {	return root;
    }
    else
    {
	if(del->child!=NULL)                                			//re-insert the subtrees of the max node to top level list
        {node* childnode=del->child;                     			//retrive childlist of the removed node to re-insert them into top level list
        node* nextnode;
        node* firstchild=childnode;                         		//pointer to the firstchild to act as the stopping condition
        do
        {nextnode=childnode->right;
        childnode->left=childnode;
        childnode->right=childnode;
        insert(childnode,true);                    					//inserting nodes into top level list
        childnode=nextnode;
        }while(nextnode!=firstchild);                       		//iterate till you insert all the children
        }

    if(del==del->right && del->child==NULL)    						//checking if this was the only node in the tree
    {root=NULL;
    }
    else
    {del->right->left=del->left;                 					//removing this node from sibling list
    del->left->right=del->right;
    root=del->right;                                				//setting random root as we cant be sure now of which value is greatest, it may not be one of the roots children
    meld_fib();                                 					//call pairwise combine operation
    }
    del->left=del;                                  				//resetting the removed node as this has to be re-inserted to the heap as a fresh node
    del->right=del;
    del->parent=NULL;
    del->child=NULL;
    del->childcut=false;
    del->degree=0;
    return del;                                         			//return the maximum frequency keyword node
    }
}

void increase_frequency(node* changenode)
{node* curr_P=changenode->parent;
if(curr_P!=NULL && changenode->frequency>curr_P->frequency) 		//Perform the cut operation only if the nodes frequency has gone more than its parent or it is not a top level list node
	{cut(changenode,curr_P);                                        //cut the subtree rooted at this node
 	cascade_cut(curr_P);                                            //check cascading cut for its parent
    }
    if(changenode->frequency>root->frequency)                       //any node that was cut was a child of some node,
    {                                                               //only node that could have become more than the root will the node where the frequency is increased                               //
        root=changenode;                                            //If so, assign it to be the root of the heap
    }
    return;
}

int main(int argc, char *argv[])
{
if(argc<2)                                                         	//check if filename was passed as argument
	{return 0;
	}
else
{	ofstream pout;
    ifstream pin;
    pin.open(argv[1]);
    pout.open("output_file.txt");
	string keyword,OPKey;           				        		//a string to read the input
    unsigned long int freq=0,remove_n_maxs=0,flag=0;           		//variables to store frequency to read from file, the count of number of most popular keywords, a variable to iterate respectively
    while(pin>>keyword)                                             //read an input
    {   if(keyword[0]=='s' || keyword[0]=='S')                      //if it says stop, close the input and output file and return
        {
        fclose(stdin);
        fclose(stdout);
        return 0;
        }
        else if(keyword[0]=='$')                       	      		//if the input was a keyword, it starts with '#', it will have a corresponding frequency to be scanned.
        {
        pin>>freq;                                  				//scan the frequency
        it=u.find(keyword);                          				//check if the keyword already exists in the tree
        if(it!=u.end())                              				//if it exists, the iterator will not point to end of the map
        {
        node* incrementnode=it->second;                     		//access the node's pointer from the value to hashtable
        incrementnode->frequency+=freq;                     		//increase the frequency of this node and preform required operations using the increase_frequency function
        increase_frequency(incrementnode);
        }
        else
        {node* new_keyword = new node(freq, keyword);        		//if this is a new keyword, create a new node for this keyword
        u[keyword]=new_keyword;                  					//store its location in the hashtable
        insert(new_keyword,false);                     				//insert it into the heap as a new node
        }
        }
        else if(keyword[0]!='$')                                    //if the input does not match either of the conditions, it will the count of number of remove maxs
        {
        remove_n_maxs= atoi(keyword.c_str());                   	//convert the input to an integer
        vector<node* > del_nodes;
        flag=0;
        while(flag<remove_n_maxs)                          			//remove n top frequency nodes from the heap
        {node* maxhash=removemax();
        del_nodes.push_back(maxhash);
        OPKey=maxhash->keyword;                   					//store the removed nodes in a vector
        OPKey=OPKey.erase(0,1);
		pout<<OPKey;            									//output to file
		if((flag+1)!=remove_n_maxs)
		pout<<",";
        ++flag;
        }
		pout<<endl;
        flag=0;
        while(flag<remove_n_maxs)                          			//iteratively re-insert the removed nodes into the heap
        {
        insert(del_nodes[flag],false);
        ++flag;
        }
        }
        keyword="";
        }
    }
    return 0;
}
