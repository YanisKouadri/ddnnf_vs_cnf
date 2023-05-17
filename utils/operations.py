import os
import time

def transform():
    '''
    This function transforms a CNF encoding from a temp file to a D-DNNF representation in another temp file. The transformation time is recorded.
    To accomplish this, we use the d4 compiler.
    * The d4 compiler's output is muted using > /dev/null 2>&1
    
    Arguments:
        None

    Returns:
        A float : The recorded transformation time.
    '''
    start_time_transformation=time.time()
    os.system("./executables/d4 ./temp_files/CNF_graph.cnf -out=./temp_files/DDNNF_graph.nnf > /dev/null 2>&1")#execute transformation with the desired software
    transformation_time = time.time()-start_time_transformation
    #print("CNF to DDNNF transformation time :"+str(transformation_time))
    return transformation_time

def query():
    '''
    This function executes queries using the d-dnnf reasoner and records the queries total time. 
    It uses a D-DNNF representation stored in a temp file by the d4 compiler.
    The instructions for the queries are stored in a temp file called commands.txt that is filled by the script calling this function. 
    * The d-dnnf reasoner's output is muted using > /dev/null 2>&1
    
    Arguments:
        None

    Returns:
        A float : The recorded queries time.
    '''
    start_time_query=time.time()
    os.system("./executables/query-dnnf -cmd ./temp_files/commands.txt > /dev/null 2>&1")#execute all queries with the desired software
    query_time = time.time()-start_time_query
    #print("Queries time :"+str(query_time))
    return query_time
    