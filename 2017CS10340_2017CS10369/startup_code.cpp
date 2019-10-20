#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <unordered_map>
#include<algorithm>
#include <iomanip>
#include <ctime>
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
        for(int i=0; i< combi1.size(); i++){
            for(int j=0; j< combi2.size(); j++){
                string temp1 = combi1[i];
                temp.push_back(temp1.append(combi2[j]));
            }
        }
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
                }
            else
                for(int j=i; j< combinations.size(); j=j+rep){
                    CPT[j] = 0;
                }

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
    }

    string get_max_value(int offset)
    {
        float max = -1.0;
        int max_index = -1;
        string res = "";
        int rep = CPT.size()/nvalues;
        for(int i=offset;  i<CPT.size(); i+=rep)
        {
            if(CPT[i] > 0.5)
            {
                    return values[i/rep];
            }
            if(CPT[i] > max)
            {
                res = values[i/rep];
                max = CPT[i];
                max_index = i;
            }
        }
        return res;
    }
    void add_consideration(string up)
    {
        updateConsideration(up);
    }
    void del_consideration(string last)
    {
        considerations[last] = considerations[last] - 1;
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
    int get_index(string value_i)
    {
        for(int i=0; i<values.size(); i++)
        {
            if(values[i].compare(value_i) == 0)
                return i;
        }
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

    void update_cons_of_node(int index, string old, string news)
    {
        Graph_Node temp_node = Pres_Graph[index];
        temp_node.add_consideration(news);
        temp_node.del_consideration(old);
        Pres_Graph[index] = temp_node;
    }
	
};

network read_network(ofstream* of, string inpfile)
{
	network Alarm;
	string line;
	int find=0;
  	ifstream myfile(inpfile); 
  	string temp;
  	string name;
  	vector<string> values;
    bool stat = false;

    short out_flag = 0;
  	
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
                    *of << line << endl;
     				ss>>name;
     				getline (myfile,line);
                    *of << line << endl;
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
                    out_flag = 1;
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
               if(out_flag == 0) *of << line << endl;
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


vector<vector<string>> parse_record(int numVariables, string infile){
    string line;
    ifstream myfile(infile);
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
            for(int i=0; i< numVariables; i++){
                ss >> temp;
                temp1.push_back(temp);
            }
            lines.push_back(temp1);
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
    return noData;
}



int main(int argc, const char * argv[]){
    time_t time1 = time(nullptr);
    ofstream* of = new ofstream("solved_alarm.bif");
	network Alarm;
	Alarm=read_network(of, argv[1]);
    int numVariables = Alarm.Pres_Graph.size();
    vector<vector<string>> data = parse_record(numVariables, argv[2]);
    int numLines = data.size();
    vector<int> numOfConsiderations;
    vector<vector<int>> noData = sortData(data, Alarm);
    //initialization
    for(int i=0; i< numVariables; i++){
        Graph_Node presVariable = Alarm.Pres_Graph[i];
        vector<string> parents = presVariable.get_Parents();
        vector<int> parentIndices;
        vector<int> removal = noData[i];
        int index=0;
        parentIndices.push_back(i);
        for(int j=0; j< parents.size(); j++){
            vector<int> temp = noData[Alarm.get_index(parents[j])];
            if(temp.size() !=0)
            {
                vector<int> temperory(removal.size()+temp.size());
                merge(removal.begin(), removal.end(), temp.begin(), temp.end(), temperory.begin());
                removal = temperory;
            }
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
            presVariable.updateConsideration(tempStr);
        }
        presVariable.updateCPT();
        Alarm.Pres_Graph[i] = presVariable;
    }
//iteration
    int iter = 0;
    while(time(nullptr) - time1 < 100)
    {
        iter++;
        for(int i=0; i< numVariables; i++){
            int ctr = 0;
            Graph_Node presVariable = Alarm.Pres_Graph[i];
            vector<int> children = presVariable.get_children();
            vector<int> parentIndices;
            vector<int> removal = noData[i];
            vector<string> parents = presVariable.get_Parents();
            for(int j=0; j<presVariable.get_Parents().size(); j++)
            {
                parentIndices.push_back(Alarm.get_index(parents[j]));
            }
            for(int j=0; j< removal.size(); j++){
                //string parent_values = "";
                vector<string> line = data[removal[j]];
                string current_value = line[i];
                string tempStr="";
                int offset = 0;
                for(int k=0; k< parentIndices.size(); k++){
                    tempStr.append(line[parentIndices[k]]);
                    int siz = Alarm.Pres_Graph[parentIndices[k]].get_nvalues();
                    int index = Alarm.Pres_Graph[parentIndices[k]].get_index(line[parentIndices[k]]);
                    offset = offset*siz + index;
                }
                //updating considerations of current valriable
                string new_value = presVariable.get_max_value(offset);
                if(new_value.compare(current_value)!=0)
                {
                    ctr++;
                    string temp_vs = new_value;
                    presVariable.add_consideration(temp_vs.append(tempStr));
                    temp_vs = current_value;
                    presVariable.del_consideration(temp_vs.append(tempStr));
                    data[removal[j]][i] = new_value;
                    line[i] = new_value;
                                    //updating children considerations
                    for(int k=0; k<children.size(); k++)
                    {
                        vector<int> par_indices_child;
                        vector<string> parents_child = (Alarm.Pres_Graph[children[k]].get_Parents());
                        for(int l=0; l<parents_child.size(); l++)
                        {
                            par_indices_child.push_back(Alarm.get_index(parents_child[l]));
                        } 
                        string temp_vs = "";
                        string current_ch, new_ch;
                        new_ch = line[children[k]];
                        new_ch.append(temp_vs);
                        current_ch = new_ch;
                        for(int l=0; l<par_indices_child.size(); l++)
                        {
                            if(par_indices_child[l]==i)
                            {
                                current_ch.append(current_value);
                            }
                            else
                                current_ch.append(line[par_indices_child[l]]);
                            temp_vs.append(line[par_indices_child[l]]);
                        }

                        Alarm.update_cons_of_node(children[k], current_ch, new_ch);
                        ctr++;
                    }
                }
            }
            Alarm.Pres_Graph[i] = presVariable;
        }
        for(int i=0; i< numVariables; i++){
            Alarm.Pres_Graph[i].updateCPT();
        }
    }

    for(int i=0; i< Alarm.Pres_Graph.size(); i++)
    {
        *of << "probability (  "<< Alarm.Pres_Graph[i].get_name() << " ";
        vector<string> par = Alarm.Pres_Graph[i].get_Parents();
        for(int j=0; j< par.size(); j++){
            *of << " " << par[j] << " ";
        }

        *of <<") { " << "//"<< 1+Alarm.Pres_Graph[i].get_Parents().size() <<" variable(s) and "<< Alarm.Pres_Graph[i].get_CPT().size() <<" values" << endl;
        *of << "\ttable ";
        vector<float> cpt = Alarm.Pres_Graph[i].get_CPT();
        for(int j=0; j< cpt.size(); j++){
            if(cpt[j] > 0.0001 && cpt[j] < 1)
                *of << setprecision(4) << cpt[j] << " ";
            else if(cpt[j] < 0.0001)
                *of << "0.0001" << " ";
            else
                *of << "0.9999" << " ";
        }
        *of << ";\n}\n";
    }
    of->close();
    cout << time(nullptr) - time1 << " " << iter << endl;
}