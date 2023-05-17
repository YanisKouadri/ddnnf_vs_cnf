import sys
from utils import objects
import os
import time
import re

def load(graph_path):
    '''
    This function reads a .tgf graph and encodes it to CNF in a temporary file.

    Arguments:
        graph_path : A string containing the .tgf graph path.

    Returns:
        An integer containing the number of arguments in the graph.
    '''
    file_path = graph_path
    file = open(file_path,"r")
    data = file.read()                  #Parsing the file
    refined_data = (data.split("\n"))
    graph = objects.Graph()
    read_attacks=False
    nb_attacks = 0
    for element in refined_data:        #Creating the graph object with all arguments and their relations
        if(element==""):
            continue
        if(element=="#"):
            read_attacks = True         #Boolean to start reading the attacks instead of the arguments
            continue
        if(read_attacks==False):        #Adding the arguments
            number = int(element)+1     #+1 because we encode in DIMACS, where 0 is reserved, so we make args start by 1
            graph.Nodes.append(objects.Node(str(number))) #The node name takes +1 for the DIMACS encoding but its index will be equal to its real value 
        else:                           #Adding the attacks
                           
            numbers = element.split(" ")                   #(There are no mistakes in the accessing, it has been verified, don't add +1 here)
            node1 = graph.Nodes[int(numbers[0])]           #Using tgf, we can access the arguments directly by index instead of searching for them
            node2 = graph.Nodes[int(numbers[1])]           #The search would be very computationally expensive so the tgf format is very useful
            node1.attacking_list.append(node2)             #All the names are normalized to number (indexes) so no exceptions to handle
            node2.attackers_list.append(node1)
            nb_attacks+=1

    file.close()

                                                           #We start the encoding for the stable semantic
    cnf = "p cnf "+str(len(graph.Nodes))                   #For more informations, check CNF DIMACS format
    formula =""                                            #and the following paper for the formula : http://www.cril.univ-artois.fr/~lagniez/papers/LagniezLM15.pdf
    nb_clause = 0
    for item in graph.Nodes:
        formula+=item.name
        for attacker in item.attackers_list:
            formula+=" "+attacker.name
        formula+=" 0\n"
        nb_clause+=1
        for attacker in item.attackers_list:
            formula+="-"+item.name + " " + "-"+attacker.name+" 0\n"
            nb_clause+=1     
    formula = formula[:-1]
    cnf+=" "+str(nb_clause)
    final_formula = cnf+"\n"+formula

    file2_path = "./temp_files/CNF_graph.cnf"              #The CNF is saved in a temp file for the compiler to use
    file2 = open(file2_path,"w+")
    file2.write(final_formula)
    file2.close()
    
    return len(graph.Nodes)                                #The number of arguments in the graph is returned

