#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <unordered_map>
#include<algorithm>
// Format checker just assumes you have Alarm.bif and Solved_Alarm.bif (your file) in current directory
using namespace std;

// Our graph consists of a list of nodes where each node is represented as follows:
class Graph_Node{

private:
	string Node_Name;  // Variable name 
	vector<int> Children; // Children of a particular node - these are index of nodes in graph.
	vector<string> Parents; // Parents of a particular node- note these are names of parents
	int nvalues;  // Number of categories a variable represented by this node can take
	vector<string> values; // Categories of possible values
	vector<float> CPT; // conditional probability table as a 1-d array . Look for BIF format to understand its meaning
    unordered_map<string, int> considerations;
    vector<string> combinations;

public:
	// Constructor- a node is initialised with its name and its categories
    Graph_Node(string name,int n,vector<string> vals)
	{
		Node_Name=name;
	
		nvalues=n;
		values=vals;
	}
    Graph_Node(){

    }
    unordered_map<string, int> getConsiderations(){
        return considerations;
    }
    void updateConsideration(string str){
        considerations[str] = considerations[str]+1;
    }
    void get_info(){
        cout << Node_Name << ", Parents Are: ";
        for(int i=0; i< Parents.size(); i++){
            cout << Parents[i] << " ";
        }
        cout << "\n\t";
        for(int i=0; i< CPT.size(); i++){
            cout << CPT[i] << " ";
        }
        cout << "\n";
    }
    vector<string> combo(vector<string> combi1, vector<string> combi2){
        vector<string> temp;
        int num = combi1.size()*combi2.size();
        // cout << num << endl;
        for(int i=0; i< combi1.size(); i++){
            for(int j=0; j< combi2.size(); j++){
                string temp1 = combi1[i];
                temp.push_back(temp1.append(combi2[j]));
                // cout << temp[temp.size()-1] << "__"; 
            }
        }
        // cout << endl;
        return temp;
    }
    void updateCPT(){
        int rep = combinations.size()/nvalues;
        for(int i=0; i< rep; i++){
            int sum=0;
            for(int j=i; j< combinations.size(); j=j+rep){
                sum = sum+considerations[combinations[j]];
            }
            if(sum != 0)
                for(int j=i; j< combinations.size(); j=j+rep){
                    CPT[j] = considerations[combinations[j]]/(1.0*sum);
                    // cout << CPT[j] << "___";
                }
            else
                for(int j=i; j< combinations.size(); j=j+rep){
                    CPT[j] = 0;
                }
            // cout << sum << endl;

        }
    }
    void initializeCons(vector<vector<string>> parent_values){
        if(parent_values.size() == 0){
            combinations = values;
            for(int i=0; i< values.size(); i++){
                considerations[values[i]] = 0;
            }
            return;
        }
        vector<string> result = combo(values, parent_values[0]);
        for(int i=1; i<Parents.size(); i++)
        {
            result = combo(result, parent_values[i]);
        }
        for(int i=0; i<result.size(); i++)
        {
            considerations[result[i]] = 0;
        }
        combinations = result;
        // for(int i=0; i< combinations.size(); i++){
        //     cout << combinations[i] << "---";
        // }
        cout << endl;
    }
	string get_name()
	{
		return Node_Name;
	}
	vector<int> get_children()
	{
		return Children;
	}
	vector<string> get_Parents()
	{
		return Parents;
	}
	vector<float> get_CPT()
	{
		return CPT;
	}
	int get_nvalues()
	{
		return nvalues;
	}
	vector<string> get_values()
	{
		return values;
	}
	void set_CPT(vector<float> new_CPT)
	{
		CPT.clear();
		CPT=new_CPT;
	}
    void set_Parents(vector<string> Parent_Nodes)
    {
        Parents.clear();
        Parents=Parent_Nodes;
    }
    // add another node in a graph as a child of this node
    int add_child(int new_child_index )
    {
        for(int i=0;i<Children.size();i++)
        {
            if(Children[i]==new_child_index)
                return 0;
        }
        Children.push_back(new_child_index);
        return 1;
    }



};


 // The whole network represted as a list of nodes
class network{

public:
    vector <Graph_Node> Pres_Graph;
    unordered_map<string, int> variableIndices;
	int addNode(Graph_Node node)
	{
		Pres_Graph.push_back(node);
		return 0;
	}
    
    void makePairs(){
        for(int i=0; i< Pres_Graph.size(); i++){
            variableIndices[Pres_Graph[i].get_name()] = i;
        }
    }
    
	int netSize()
	{
		return Pres_Graph.size();
	}
    // get the index of node with a given name
    int get_index(string val_name)
    {
        if(variableIndices.find(val_name) != variableIndices.end())
            return variableIndices.find(val_name)->second;
        return -1;
    }
    void get_all_info(){
        cout << Pres_Graph.size() << endl;
        for(int i=0; i< Pres_Graph.size(); i++){
            Pres_Graph[i].get_info();
        }
    }
// get the node at nth index
    Graph_Node get_nth_node(int n)
    {
       return Pres_Graph[n];
    }
    //get the iterator of a node with a given name
    Graph_Node search_node(string val_name)
    {
        // Pair temp = variableIndices.find(val_name)
        if(variableIndices.find(val_name) == variableIndices.end()){
            cout<<"node not found\n";
        }
        return Pres_Graph[variableIndices.find(val_name)->second];
    }
	

};

network read_network()
{
	network Alarm;
	string line;
	int find=0;
  	ifstream myfile("alarm.bif"); 
  	string temp;
  	string name;
  	vector<string> values;
    bool stat = false;
  	
    if (myfile.is_open())
    {
    	while (! myfile.eof() )
    	{
    		stringstream ss;
      		getline (myfile,line);
      		ss.str(line);
     		ss>>temp;
     		
     		
     		if(temp.compare("variable")==0)
     		{
                    
     				ss>>name;
     				getline (myfile,line);
                   
     				stringstream ss2;
     				ss2.str(line);
     				for(int i=0;i<4;i++)
     				{
     					
     					ss2>>temp;
     					
     					
     				}
     				values.clear();
     				while(temp.compare("};")!=0)
     				{
     					values.push_back(temp);
     					
     					ss2>>temp;
    				}
     				Graph_Node new_node(name,values.size(),values);
     				int pos=Alarm.addNode(new_node);

     				
     		}
     		else if(temp.compare("probability")==0)
     		{
                    if(!stat){
                        stat=true;
                        Alarm.makePairs();
                    }
     				ss>>temp;
     				ss>>temp;
     				
                    Graph_Node vectorIt;
                    Graph_Node vectorIt1;
     				vectorIt=Alarm.search_node(temp);
                    string nodeName = temp;
                    int index=Alarm.get_index(temp);
                    ss>>temp;
                    values.clear();
     				while(temp.compare(")")!=0)
     				{
                        vectorIt1=Alarm.search_node(temp);
                        vectorIt1.add_child(index);
     					values.push_back(temp);
     					
     					ss>>temp;

    				}
                    vectorIt.set_Parents(values);
    				getline (myfile,line);
     				stringstream ss2;
                    
     				ss2.str(line);
     				ss2>> temp;
                    
     				ss2>> temp;
                    
     				vector<float> curr_CPT;
                    string::size_type sz;
     				while(temp.compare(";")!=0)
     				{
                        
     					curr_CPT.push_back(atof(temp.c_str()));
     					
     					ss2>>temp;
                       
                        

    				}
                    
                    vectorIt.set_CPT(curr_CPT);
                    Alarm.Pres_Graph[Alarm.variableIndices[nodeName]] = vectorIt;

     		}
            else
            {
                
            }
     		
     		

    		
    		
    	}
        for(int i=0; i<Alarm.Pres_Graph.size(); i++)
        {
            vector<vector<string>> parent_values;
            // cout << Alarm.Pres_Graph[i].get_Parents().size() << " jlehrfo" << endl;
            for(int j=0; j<Alarm.Pres_Graph[i].get_Parents().size(); j++)
            {
                string temp = Alarm.Pres_Graph[i].get_Parents()[j];
                parent_values.push_back(Alarm.Pres_Graph[Alarm.variableIndices[temp]].get_values());
                // cout << Alarm.Pres_Graph[Alarm.variableIndices[temp]].get_values().size() << endl;
            }
            // cout << "2" << endl;
            Alarm.Pres_Graph[i].initializeCons(parent_values);
            // cout << "3" << endl;
        }
        myfile.close();
       
  	}
  	return Alarm;
}

vector<vector<string>> parse_record(int numVariables){
    string line;
    ifstream myfile("records.dat");
    string temp;
    string name;
    vector<vector<string>> lines;
    //int line_number =0;
    if(myfile.is_open()){
        while(!myfile.eof()){
            stringstream ss;
            vector<string> temp1;
            getline (myfile,line);
            ss.str(line);
            // ss >> temp;
            // cout << temp << " ";
            // temp1.push_back(temp);
            for(int i=0; i< numVariables; i++){
                ss >> temp;
                // cout << temp << " ";
                temp1.push_back(temp);
            }
            lines.push_back(temp1);
            // cout << temp1.size() << endl;
            //line_number++;
        }
    }
    myfile.close();
    return lines;
}
vector<vector<int>> sortData(vector<vector<string>> data, network alarm){
    vector<vector<int>> noData;
    for(int i=0; i< alarm.Pres_Graph.size(); i++){
        vector<int> temp;
        noData.push_back(temp);
    }
    for(int i=0; i< data.size(); i++){
        for(int j=0; j< data[i].size(); j++){
            if(data[i][j].compare("\"?\"") == 0)
                noData[j].push_back(i);
        }
    }
    cout << "no data size " << noData.size() << endl;
    return noData;
}



int main(){
	network Alarm;
    cout << "1" << endl;
	Alarm=read_network();
    cout << "2" << endl;
    int numVariables = Alarm.Pres_Graph.size();
    vector<vector<string>> data = parse_record(numVariables);
    int numLines = data.size();
    cout << numLines << endl;
    vector<int> numOfConsiderations;
    // for(int i=0; i< data.size(); i++){
    //     // cout << "hello" << endl;
    //     for(int j=0; j< data[i].size(); j++){
    //         cout << data[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    vector<vector<int>> noData = sortData(data, Alarm);
    for(int i=0; i< noData.size(); i++){
        cout << Alarm.get_nth_node(i).get_name() << " : ";
        // cout << "hello" << endl;
        for(int j=0; j< noData[i].size(); j++){
            cout << noData[i][j] << " ";
        }
        cout << endl;
    }
    Alarm.get_all_info();
    for(int i=0; i< numVariables; i++){
        Graph_Node presVariable = Alarm.Pres_Graph[i];
        vector<string> parents = presVariable.get_Parents();
        vector<int> parentIndices;
        vector<int> removal = noData[i];
        int index=0;
        parentIndices.push_back(i);
        // cout << removal.size() << " " << i << endl;
        for(int j=0; j< parents.size(); j++){
            vector<int> temp = noData[Alarm.get_index(parents[j])];
            // cout<< "hello " << j << endl;
            if(temp.size() !=0)
            {
                vector<int> temperory(removal.size()+temp.size());
                merge(removal.begin(), removal.end(), temp.begin(), temp.end(), temperory.begin());
                removal = temperory;
                // cout<< "hello 6e" << j << endl;
            }
            // cout<< "hello " << j << endl;
            parentIndices.push_back(Alarm.get_index(parents[j]));
        }
        for(int j=0; j< numLines; j++){
            if(j==removal[index]){
                index++;
                continue;
            }
            vector<string> line = data[j];
            string tempStr="";
            for(int k=0; k< parentIndices.size(); k++){
                tempStr = tempStr.append(line[parentIndices[k]]);
            }
            // cout << tempStr << endl;
            presVariable.updateConsideration(tempStr);
            // cout << "updated" << presVariable.getConsiderations()[tempStr] << endl;
        }
        presVariable.updateCPT();
        Alarm.Pres_Graph[i] = presVariable;
    }
    Alarm.get_all_info();

}