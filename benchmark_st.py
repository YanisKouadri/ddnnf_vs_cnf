from utils import load_graph_cnf
from utils import operations
import os
import random
import sys
import time
import csv


def tests():
    '''
    This function checks the command line arguments to determine which task to run.
    It accepts two possible task flags: '-dc' and '-ds'.
    If '-dc' is provided, it runs the dc() function.
    If '-ds' is provided, it runs the ds() function.
    If any other argument is provided, it prints an error message and exits.

    Arguments:
        None

    Returns:
        None
    '''
    task = sys.argv[1]
    if(task=="-dc"):
       dc()
    elif(task=="-ds"):
       ds()
    else:
       print("Wrong task, accepted tasks : -dc,-ds.")
       exit()
    



def dc():
    '''
    This function executes the tests for the credulous acceptability. 
    These are the steps it goes through :
     - it creates and formats the columns of a csv for the results
     - it parses all the graphs in the folder given by command line (argv[2]), for each .tgf graph :
            * it reads the graph and encode it in CNF by calling utils/load_graph_cnf.py
            * it sets the total number of queries as total args in the graph or user input (argv[3]) (if total < input, then total, else input)
            * it applies the transformation to D-DNNF by calling utils/operations.py (which will use executables/d4) and it records the time taken in the csv
            * it verifies if the D-DNNF transformation was UNSAT (by checking if the file containing the representation is empty) and it records it in the csv
            * it executes the D-DNNF queries by calling utils/operations.py (which will use executables/query-dnnf) and it records the time taken in the csv
            * it executes the CNF queries by calling utils/operations (which will use executables/[...]/mu-toksia), the time is recorded from inside mu-toksia

    The outputs of each executables are muted using > /dev/null 2>&1, get rid of this if you want to be able to debug each executable.
    Arguments:
        None

    Returns:
        None
    '''

    with open('benchmarks_DC.csv','w+') as records :                    #csv creation with columns formatting, nb columns depends on total queries from user
        create_file = csv.writer(records)
        column_names1 = ['Name','Queries number','UNSAT transformation ?','Transformation time','Query time']
        column_names2 = ['Query '+str(x) for x in range(int(sys.argv[3]))]
        column_names_final = column_names1+column_names2
        create_file.writerow(column_names_final)
        
    graphs_directory = sys.argv[2]                                      #folder parsing
    graphs = os.listdir(graphs_directory)
    nb_graphs = len(graphs)
    graph_counter = 1
    for graph in graphs:                                                #loop beginning
       if(graph.endswith(".tgf")==False):
        continue
       graph_path=graphs_directory+"/"+graph
       print("Graph number "+str(graph_counter)+"/"+str(nb_graphs)+" : "+graph_path)
       with open('benchmarks_DC.csv', 'a') as records:
        csv_line=[]
        recorder = csv.writer(records,lineterminator='')
        csv_line.append(graph)
        graph_path=graphs_directory+"/"+graph
        nb_args=load_graph_cnf.load(graph_path)           #writes the cnf in the temp_files folder and returns the graph's number of args
        nb_sample = nb_args
        if(nb_sample>int(sys.argv[3])):                   #nb queries = user input, unless the number of available args is inferior to it, in that case, = nb args
           nb_sample=int(sys.argv[3])
        csv_line.append(nb_sample)
        project_transformation_time = operations.transform()           #executes D-DNNF transformation with d4, returns transformation time
        file_size = os.path.getsize('./temp_files/DDNNF_graph.nnf')
        if(file_size==0):                                              #UNSAT transformation checking
           csv_line.append("YES")
        else:
           csv_line.append("NO")
        csv_line.append(project_transformation_time)
        if(nb_sample>0):                                               #If there are queries to be made, else we only record the transformation time (if user input=0)
           args_to_test=random.sample(range(0,nb_args),int(nb_sample)) #Random sampling of the args to test, we keep the order for mu-toksia
           with open("./temp_files/commands.txt","w+") as queries:     #Instructions for d-dnnf reasoner in a temp file
              queries.write("load ./temp_files/DDNNF_graph.nnf")
              for arg in args_to_test:
                 queries.write("\nmodel "+str(arg+1))            #In DIMACS format, 0 is reserved, so +1 for all args in the D-DNNF approach (handled in the encoding)
           project_query_time=operations.query()
           csv_line.append(project_query_time)
        recorder.writerow(csv_line)                                    #We have to stop the recording here so that mu-toksia can have access to the file
       if(nb_sample>0):                                                #If there are queries to be made (if user input>0)
          for arg in args_to_test:
             os.system("./executables/andreasniskanen-mu-toksia-c44251489a03/build/release/bin/mu-toksia -p DC-ST -f "+graph_path+" -a "+str(arg)+" -fo tgf > /dev/null 2>&1")
       graph_counter+=1
       with open('benchmarks_DC.csv', 'a') as records:                 #Inserting a line break in the csv (done this way for mu-toksia to write on the same line)
        recorder = csv.writer(records)
        recorder.writerow([])



def ds():

    '''
    This function executes the tests for the skeptical acceptability. 
    These are the steps it goes through :
     - it creates and formats the columns of a csv for the results
     - it parses all the graphs in the folder given by command line (argv[2]), for each .tgf graph :
            * it reads the graph and encode it in CNF by calling utils/load_graph_cnf.py
            * it sets the total number of queries as total args in the graph or user input (argv[3]) (if total < input, then total, else input)
            * it applies the transformation to D-DNNF by calling utils/operations.py (which will use executables/d4) and it records the time taken in the csv
            * it verifies if the D-DNNF transformation was UNSAT (by checking if the file containing the representation is empty) and it records it in the csv
            * it executes the D-DNNF queries by calling utils/operations.py (which will use executables/query-dnnf) and it records the time taken in the csv
            * it executes the CNF queries by calling utils/operations (which will use executables/[...]/mu-toksia), the time is recorded from inside mu-toksia

    The outputs of each executables are muted using > /dev/null 2>&1, get rid of this if you want to be able to debug each executable.
    Arguments:
        None

    Returns:
        None
    '''

    with open('benchmarks_DS.csv','w+') as records :                        #csv creation with columns formatting, nb columns depends on total queries from user
        create_file = csv.writer(records)
        column_names1 = ['Name','Queries number','UNSAT transformation ?','Transformation time','Query time']
        column_names2 = ['Query '+str(x) for x in range(int(sys.argv[3]))]
        column_names_final = column_names1+column_names2
        create_file.writerow(column_names_final)
    graphs_directory = sys.argv[2]                                          #folder parsing
    graphs = os.listdir(graphs_directory)
    nb_graphs = len(graphs)
    graph_counter = 1
    for graph in graphs:                                                    #loop beginning
       if(graph.endswith(".tgf")==False):
        continue
       graph_path=graphs_directory+"/"+graph
       print("Graph number "+str(graph_counter)+"/"+str(nb_graphs)+" : "+graph_path)
       with open('benchmarks_DS.csv', 'a') as records:
        csv_line=[]
        recorder = csv.writer(records,lineterminator='')
        csv_line.append(graph)
        graph_path=graphs_directory+"/"+graph
        nb_args=load_graph_cnf.load(graph_path)               #writes the cnf in the temp_files folder and returns the graph's number of args
        nb_sample = nb_args                                   
        if(nb_sample>int(sys.argv[3])):                       #nb queries = user input, unless the number of available args is inferior to it, in that case, = nb args
           nb_sample=int(sys.argv[3])
        csv_line.append(nb_sample)
        project_transformation_time = operations.transform()                    #executes D-DNNF transformation with d4, returns transformation time
        file_size = os.path.getsize('./temp_files/DDNNF_graph.nnf')
        if(file_size==0):                                                       #UNSAT transformation checking
           csv_line.append("YES")
        else:
           csv_line.append("NO")
        csv_line.append(project_transformation_time)
        if(nb_sample>0):                                               #If there are queries to be made, else we only record the transformation time (if user input=0)
            args_to_test=random.sample(range(0,nb_args),int(nb_sample))#Random sampling of the args to test, we keep the order for mu-toksia
            with open("./temp_files/commands.txt","w+") as queries:    #Instructions for d-dnnf reasoner in a temp file
               queries.write("load ./temp_files/DDNNF_graph.nnf")
               for arg in args_to_test:
                  queries.write("\nmodel -"+str(arg+1))         #In DIMACS format, 0 is reserved, so +1 for all args in the D-DNNF approach (handled in the encoding)
            project_query_time=operations.query()
            csv_line.append(project_query_time)
        recorder.writerow(csv_line)                                    #We have to stop the recording here so that mu-toksia can have access to the file
       if(nb_sample>0):                                                #If there are queries to be made (if user input>0)
          for arg in args_to_test:
             os.system("./executables/andreasniskanen-mu-toksia-c44251489a03/build/release/bin/mu-toksia -p DS-ST -f "+graph_path+" -a "+str(arg)+" -fo tgf > /dev/null 2>&1")
       graph_counter+=1
       with open('benchmarks_DS.csv', 'a') as records:                 #Inserting a line break in the csv (done this way for mu-toksia to write on the same line)
        recorder = csv.writer(records)
        recorder.writerow([])        


#Checking for input exceptions before launching the tests method
#The checking for -ds or -dc is done directly in the method

file_existence = os.path.isdir(sys.argv[2])

if(len(sys.argv)<4 or file_existence==False or int(sys.argv[3])<1):
    if(len(sys.argv)<4):
        print("Missing arguments in the command line, command's syntax : \npython3 benchmark_st.py -task folder_path queries_number\ntask being dc or ds, folder_path being the folder containing your .tgf graphs, and queries_number being the total number of queries you want (it will be equal to the total number of args in the graph if your input is superior to it).")
        exit()
    elif(file_existence==False):
        print("The provided graphs\' folder do not exist or isn't a folder")
        exit()
    elif(int(sys.argv[3])<0):
        print("Error : Number of queries must be superior or equal to 0 (if equal to 0, only the transformation time will be recorded).")
        exit()

tests()